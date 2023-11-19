#pragma once
#include <string>
#include <vector>

#include "state.cuh"
#include "utils.h"

struct state_t
{
	static constexpr int word_size = 32;

	std::size_t state_size;
	std::vector<state_word_t> data;

	state_t(std::size_t state_size) : state_size(state_size), data(DIV_UP(state_size, word_size), 0) {}

	state_t(std::size_t state_size, const state_word_t* data)
		: state_size(state_size), data(data, data + DIV_UP(state_size, word_size))
	{}

	std::size_t words_n() const { return data.size(); }

	bool is_set(int bit) const
	{
		auto word_idx = bit / word_size;
		auto bit_idx = bit % word_size;

		return data[word_idx] & (1 << bit_idx);
	}

	void set(int bit)
	{
		auto word_idx = bit / word_size;
		auto bit_idx = bit % word_size;

		data[word_idx] |= (1 << bit_idx);
	}

	std::string to_string(const std::vector<std::string>& names) const
	{
		bool first = true;
		std::string name;
		for (std::size_t i = 0; i < state_size; i++)
		{
			if (is_set(i))
			{
				if (!first)
					name += " -- ";
				first = false;
				name += names[i];
			}
		}

		if (name.empty())
			name = "<nil>";

		return name;
	}
};

template <int state_words>
struct static_state_t
{
	uint32_t data[state_words] = { 0 };

	constexpr static_state_t() {}

	constexpr bool operator==(const static_state_t<state_words>& other) const
	{
		bool same = true;
		for (int i = 0; i < state_words; i++)
			same &= data[i] == other.data[i];

		return same;
	}

	constexpr bool operator<(const static_state_t<state_words>& other) const
	{
		for (int i = state_words - 1; i >= 0; i--)
			if (data[i] != other.data[i])
				return data[i] < other.data[i];
		return false;
	}
};

// template <int bits>
// struct state_t_template
// {
// 	static constexpr int word_size = 32;
// 	static constexpr int words_n = (bits + word_size - 1) / word_size;

// 	uint32_t data[words_n] = { 0 };

// 	constexpr state_t_template() {}

// 	explicit constexpr state_t_template(int set_bit)
// 	{
// 		auto word_idx = set_bit / word_size;
// 		auto bit_idx = set_bit % word_size;

// 		data[word_idx] = 1 << bit_idx;
// 	}

// 	constexpr bool operator==(const state_t_template<bits>& other) const
// 	{
// 		bool same = true;
// 		for (int i = 0; i < words_n; i++)
// 			same &= data[i] == other.data[i];

// 		return same;
// 	}

// 	constexpr bool operator<(const state_t_template<bits>& other) const
// 	{
// 		for (int i = words_n - 1; i >= 0; i--)
// 			if (data[i] != other.data[i])
// 				return data[i] < other.data[i];
// 		return false;
// 	}

// 	constexpr bool is_set(int bit) const
// 	{
// 		auto word_idx = bit / word_size;
// 		auto bit_idx = bit % word_size;

// 		return data[word_idx] & (1 << bit_idx);
// 	}

// 	constexpr void set(int bit)
// 	{
// 		auto word_idx = bit / word_size;
// 		auto bit_idx = bit % word_size;

// 		data[word_idx] |= (1 << bit_idx);
// 	}

// 	constexpr void unset(int bit)
// 	{
// 		auto word_idx = bit / word_size;
// 		auto bit_idx = bit % word_size;

// 		data[word_idx] &= ~(1 << bit_idx);
// 	}

// 	constexpr void flip(int bit)
// 	{
// 		auto word_idx = bit / word_size;
// 		auto bit_idx = bit % word_size;

// 		data[word_idx] ^= (1 << bit_idx);
// 	}

// 	constexpr void operator&=(const state_t_template<bits>& other)
// 	{
// 		for (int i = 0; i < words_n; i++)
// 			data[i] &= other.data[i];
// 	}

// 	constexpr state_t_template<bits> operator&(const state_t_template<bits>& other) const
// 	{
// 		state_t_template<bits> ret = *this;


// 		for (int i = 0; i < words_n; i++)
// 			ret.data[i] &= other.data[i];

// 		return ret;
// 	}

// 	constexpr void operator|=(const state_t_template<bits>& other)
// 	{
// 		for (int i = 0; i < words_n; i++)
// 			data[i] |= other.data[i];
// 	}

// 	constexpr state_t_template<bits> operator|(const state_t_template<bits>& other) const
// 	{
// 		state_t_template<bits> ret = *this;


// 		for (int i = 0; i < words_n; i++)
// 			ret.data[i] |= other.data[i];

// 		return ret;
// 	}

// 	constexpr void operator^=(const state_t_template<bits>& other)
// 	{
// 		for (int i = 0; i < words_n; i++)
// 			data[i] ^= other.data[i];
// 	}

// 	constexpr state_t_template<bits> operator^(const state_t_template<bits>& other) const
// 	{
// 		state_t_template<bits> ret = *this;


// 		for (int i = 0; i < words_n; i++)
// 			ret.data[i] ^= other.data[i];

// 		return ret;
// 	}

// 	constexpr state_t_template<bits> operator~() const
// 	{
// 		state_t_template<bits> ret = *this;


// 		for (int i = 0; i < words_n; i++)
// 			ret.data[i] = ~ret.data[i];

// 		return ret;
// 	}
// };

// using unit_state_t = state_t_template<32>;

// template <int bits>
// std::string to_string(const state_t_template<bits>& s, const std::vector<std::string>& names)
// {
// 	bool first = true;
// 	std::string name;
// 	for (int i = 0; i < bits; i++)
// 	{
// 		if (s.is_set(i))
// 		{
// 			if (!first)
// 				name += " -- ";
// 			first = false;
// 			name += names[i];
// 		}
// 	}

// 	if (name.empty())
// 		name = "<nil>";

// 	return name;
// }
