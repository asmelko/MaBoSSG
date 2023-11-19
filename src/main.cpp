#include <fstream>
#include <iostream>

#include "generator.h"
#include "kernel_compiler.h"
#include "simulation_runner.h"
#include "state_word.h"
#include "statistics/final_states.h"
#include "statistics/fixed_states.h"
#include "statistics/stats_composite.h"
#include "statistics/window_average_small.h"

state_t create_noninternals_mask(driver& drv)
{
	state_t mask(drv.nodes.size());
	for (size_t i = 0; i < drv.nodes.size(); ++i)
	{
		if (!drv.nodes[i].is_internal(drv))
			mask.set(i);
	}

	return mask;
}

void add_fixed_states_stats(stats_composite& stats_runner, int state_words)
{
	switch (state_words)
	{
		case 1:
			stats_runner.add(std::make_unique<fixed_states_stats<1>>());
			break;
		case 2:
			stats_runner.add(std::make_unique<fixed_states_stats<2>>());
			break;
		case 3:
			stats_runner.add(std::make_unique<fixed_states_stats<3>>());
			break;
		case 4:
			stats_runner.add(std::make_unique<fixed_states_stats<4>>());
			break;
		case 5:
			stats_runner.add(std::make_unique<fixed_states_stats<5>>());
			break;
		case 6:
			stats_runner.add(std::make_unique<fixed_states_stats<6>>());
			break;
		case 7:
			stats_runner.add(std::make_unique<fixed_states_stats<7>>());
			break;
		case 8:
			stats_runner.add(std::make_unique<fixed_states_stats<8>>());
			break;
		default:
			assert(false);
			break;
	}
}

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv + argc);

	if (!(args.size() == 2 || (args.size() == 4 || args[0] == "-o")))
	{
		std::cout << "Usage: MaBoSSG [-o prefix] bnd_file cfg_file" << std::endl;
		return 1;
	}

	std::string output_prefix = "";
	std::string bnd_path = args[0];
	std::string cfg_path = args[1];
	if (args[0] == "-o")
	{
		output_prefix = args[1];
		bnd_path = args[2];
		cfg_path = args[3];
	}

	driver drv;
	if (drv.parse(bnd_path, cfg_path))
		return 1;

	bool discrete_time = drv.constants["discrete_time"] != 0;
	float max_time = drv.constants["max_time"];
	float time_tick = drv.constants["time_tick"];
	int sample_count = drv.constants["sample_count"];
	auto noninternals_mask = create_noninternals_mask(drv);
	int noninternals_count =
		std::count_if(drv.nodes.begin(), drv.nodes.end(), [&](const auto& node) { return !node.is_internal(drv); });

	if (noninternals_count > 20)
	{
		std::cerr << "This executable supports a maximum of 20 non-internal nodes." << std::endl;
		return 1;
	}

	if (drv.nodes.size() > 256)
	{
		std::cerr << "This executable supports a maximum of 256 nodes." << std::endl;
		return 1;
	}

	std::vector<std::string> node_names;
	for (auto&& node : drv.nodes)
		node_names.push_back(node.name);

	generator gen(drv);

	auto s = gen.generate_code();

	kernel_compiler compiler;

	if (compiler.compile_simulation(s, discrete_time))
		return 1;

	simulation_runner r(sample_count, noninternals_mask.words_n());

	stats_composite stats_runner;

	// for final states
	stats_runner.add(
		std::make_unique<final_states_stats>(noninternals_mask, noninternals_count, compiler.final_states));

	// for fixed states
	add_fixed_states_stats(stats_runner, noninternals_mask.words_n());

	// for window averages
	stats_runner.add(std::make_unique<window_average_small_stats>(
		time_tick, max_time, discrete_time, noninternals_mask, noninternals_count, r.trajectory_len_limit,
		r.trajectory_batch_limit, compiler.window_average_small));

	// // run
	r.run_simulation(stats_runner, compiler.initialize_random, compiler.initialize_initial_state, compiler.simulate);

	// // finalize
	stats_runner.finalize();

	// visualize
	if (output_prefix.size() > 0)
	{
		stats_runner.write_csv(sample_count, node_names, output_prefix);
	}
	else
	{
		stats_runner.visualize(sample_count, node_names);
	}

	return 0;
}
