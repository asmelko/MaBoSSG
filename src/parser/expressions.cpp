#include "expressions.h"

#include <algorithm>
#include <stdexcept>

#include "driver.h"

unary_expression::unary_expression(operation op, expr_ptr expr) : op(op), expr(std::move(expr)) {}

float unary_expression::evaluate(const driver& drv) const
{
	switch (op)
	{
		case operation::PLUS:
			return expr->evaluate(drv);
		case operation::MINUS:
			return -expr->evaluate(drv);
		case operation::NOT:
			return !expr->evaluate(drv);
		default:
			throw std::runtime_error("Unknown unary operator");
	}
}

void unary_expression::generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const
{
	switch (op)
	{
		case operation::PLUS:
			os << "+";
			expr->generate_code(drv, current_node, os);
			break;
		case operation::MINUS:
			os << "-";
			expr->generate_code(drv, current_node, os);
			break;
		case operation::NOT:
			os << "!";
			expr->generate_code(drv, current_node, os);
			break;
		default:
			throw std::runtime_error("Unknown unary operator");
	}
}

expr_ptr unary_expression::simplify() const
{
	auto simplified = expr->simplify();

	if (op == operation::PLUS)
		return simplified;

	if (auto simplified_unary = dynamic_cast<unary_expression*>(simplified.get()))
	{
		if (simplified_unary->op == op)
		{
			return std::move(simplified_unary->expr);
		}
	}

	return std::make_unique<unary_expression>(op, std::move(simplified));
}

binary_expression::binary_expression(operation op, expr_ptr left, expr_ptr right)
	: op(op), left(std::move(left)), right(std::move(right))
{}

float binary_expression::evaluate(const driver& drv) const
{
	switch (op)
	{
		case operation::PLUS:
			return left->evaluate(drv) + right->evaluate(drv);
		case operation::MINUS:
			return left->evaluate(drv) - right->evaluate(drv);
		case operation::STAR:
			return left->evaluate(drv) * right->evaluate(drv);
		case operation::SLASH:
			return left->evaluate(drv) / right->evaluate(drv);
		case operation::AND:
			return left->evaluate(drv) && right->evaluate(drv);
		case operation::OR:
			return left->evaluate(drv) || right->evaluate(drv);
		case operation::EQ:
			return left->evaluate(drv) == right->evaluate(drv);
		case operation::NE:
			return left->evaluate(drv) != right->evaluate(drv);
		case operation::LE:
			return left->evaluate(drv) <= right->evaluate(drv);
		case operation::LT:
			return left->evaluate(drv) < right->evaluate(drv);
		case operation::GE:
			return left->evaluate(drv) >= right->evaluate(drv);
		case operation::GT:
			return left->evaluate(drv) > right->evaluate(drv);
		default:
			throw std::runtime_error("Unknown binary operator " + std::to_string(static_cast<int>(op)));
	}
}

void binary_expression::generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const
{
	switch (op)
	{
		case operation::PLUS:
			left->generate_code(drv, current_node, os);
			os << " + ";
			right->generate_code(drv, current_node, os);
			break;
		case operation::MINUS:
			left->generate_code(drv, current_node, os);
			os << " - ";
			right->generate_code(drv, current_node, os);
			break;
		case operation::STAR:
			left->generate_code(drv, current_node, os);
			os << " * ";
			right->generate_code(drv, current_node, os);
			break;
		case operation::SLASH:
			left->generate_code(drv, current_node, os);
			os << " / ";
			right->generate_code(drv, current_node, os);
			break;
		case operation::AND:
			left->generate_code(drv, current_node, os);
			os << " && ";
			right->generate_code(drv, current_node, os);
			break;
		case operation::OR:
			left->generate_code(drv, current_node, os);
			os << " || ";
			right->generate_code(drv, current_node, os);
			break;
		case operation::EQ:
			left->generate_code(drv, current_node, os);
			os << " == ";
			right->generate_code(drv, current_node, os);
			break;
		case operation::NE:
			left->generate_code(drv, current_node, os);
			os << " != ";
			right->generate_code(drv, current_node, os);
			break;
		case operation::LE:
			left->generate_code(drv, current_node, os);
			os << " <= ";
			right->generate_code(drv, current_node, os);
			break;
		case operation::LT:
			left->generate_code(drv, current_node, os);
			os << " < ";
			right->generate_code(drv, current_node, os);
			break;
		case operation::GE:
			left->generate_code(drv, current_node, os);
			os << " >= ";
			right->generate_code(drv, current_node, os);
			break;
		case operation::GT:
			left->generate_code(drv, current_node, os);
			os << " > ";
			right->generate_code(drv, current_node, os);
			break;
		default:
			throw std::runtime_error("Unknown binary operator " + std::to_string(static_cast<int>(op)));
	}
}

expr_ptr binary_expression::simplify() const
{
	auto simplified_left = left->simplify();
	auto simplified_right = right->simplify();

	// remove parentheses
	{
		auto get_binary_in_parentheses = [](auto&& expr) -> binary_expression* {
			if (auto* paren = dynamic_cast<parenthesis_expression*>(expr.get()))
			{
				if (auto* bin = dynamic_cast<binary_expression*>(paren->expr.get()))
				{
					return bin;
				}
			}
			return nullptr;
		};

		auto get_flat_in_parentheses = [](auto&& expr) -> flat_expression* {
			if (auto* paren = dynamic_cast<parenthesis_expression*>(expr.get()))
			{
				if (auto* flat = dynamic_cast<flat_expression*>(paren->expr.get()))
				{
					return flat;
				}
			}
			return nullptr;
		};

		auto remove_parenthesis = [&](auto& expr) {
			auto* bin_expr = get_binary_in_parentheses(expr);

			if (bin_expr)
			{
				if (bin_expr->op == op)
				{
					simplified_left = std::move(dynamic_cast<parenthesis_expression*>(expr.get())->expr);
				}
			}

			auto* flat_expr = get_flat_in_parentheses(expr);

			if (flat_expr)
			{
				if (flat_expr->op == op)
				{
					simplified_left = std::move(dynamic_cast<parenthesis_expression*>(expr.get())->expr);
				}
			}
		};

		remove_parenthesis(simplified_left);
		remove_parenthesis(simplified_right);
	}

	if (dynamic_cast<flat_expression*>(simplified_left.get()) && dynamic_cast<flat_expression*>(simplified_right.get()))
	{
		auto* left_flat = dynamic_cast<flat_expression*>(simplified_left.get());
		auto* right_flat = dynamic_cast<flat_expression*>(simplified_right.get());

		if (left_flat->op == right_flat->op && left_flat->op == op)
		{
			left_flat->exprs.insert(left_flat->exprs.end(), std::move_iterator(right_flat->exprs.begin()),
									std::move_iterator(right_flat->exprs.end()));

			return simplified_left;
		}
	}
	else if (dynamic_cast<flat_expression*>(simplified_left.get()))
	{
		auto* left_flat = dynamic_cast<flat_expression*>(simplified_left.get());

		if (left_flat->op == op)
		{
			left_flat->exprs.push_back(std::move(simplified_right));

			return simplified_left;
		}
	}
	else if (dynamic_cast<flat_expression*>(simplified_right.get()))
	{
		auto* right_flat = dynamic_cast<flat_expression*>(simplified_right.get());

		if (right_flat->op == op)
		{
			right_flat->exprs.insert(right_flat->exprs.begin(), std::move(simplified_left));

			return simplified_right;
		}
	}

	std::vector<expr_ptr> exprs;
	exprs.push_back(std::move(simplified_left));
	exprs.push_back(std::move(simplified_right));

	return std::make_unique<flat_expression>(op, std::move(exprs));
}

ternary_expression::ternary_expression(expr_ptr left, expr_ptr middle, expr_ptr right)
	: left(std::move(left)), middle(std::move(middle)), right(std::move(right))
{}

float ternary_expression::evaluate(const driver& drv) const
{
	return left->evaluate(drv) ? middle->evaluate(drv) : right->evaluate(drv);
}

void ternary_expression::generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const
{
	left->generate_code(drv, current_node, os);
	os << " ? ";
	middle->generate_code(drv, current_node, os);
	os << " : ";
	right->generate_code(drv, current_node, os);
}

expr_ptr ternary_expression::simplify() const
{
	auto simplified_left = left->simplify();
	auto simplified_middle = middle->simplify();
	auto simplified_right = right->simplify();

	if (dynamic_cast<literal_expression*>(simplified_left.get()))
	{
		if (dynamic_cast<literal_expression*>(simplified_left.get())->value)
			return simplified_middle;
		else
			return simplified_right;
	}

	return std::make_unique<ternary_expression>(std::move(simplified_left), std::move(simplified_middle),
												std::move(simplified_right));
}

parenthesis_expression::parenthesis_expression(expr_ptr expr) : expr(std::move(expr)) {}

float parenthesis_expression::evaluate(const driver& drv) const { return expr->evaluate(drv); }

void parenthesis_expression::generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const
{
	os << "(";
	expr->generate_code(drv, current_node, os);
	os << ")";
}

expr_ptr parenthesis_expression::simplify() const
{
	auto simplified = expr->simplify();

	if (dynamic_cast<literal_expression*>(simplified.get()))
		return simplified;
	if (dynamic_cast<identifier_expression*>(simplified.get()))
		return simplified;
	if (dynamic_cast<variable_expression*>(simplified.get()))
		return simplified;
	if (dynamic_cast<alias_expression*>(simplified.get()))
		return simplified;
	if (dynamic_cast<parenthesis_expression*>(simplified.get()))
		return simplified;
	if (dynamic_cast<unary_expression*>(simplified.get()))
		return simplified;

	return std::make_unique<parenthesis_expression>(std::move(simplified));
}

literal_expression::literal_expression(float value) : value(value) {}

float literal_expression::evaluate(const driver&) const { return value; }

void literal_expression::generate_code(const driver&, const std::string&, std::ostream& os) const { os << value; }

expr_ptr literal_expression::simplify() const { return std::make_unique<literal_expression>(*this); }

identifier_expression::identifier_expression(std::string name) : name(std::move(name)) {}

float identifier_expression::evaluate(const driver&) const
{
	throw std::runtime_error("identifier " + name + "in expression which needs to be evaluated");
}

void identifier_expression::generate_code(const driver& drv, const std::string&, std::ostream& os) const
{
	auto it = std::find_if(drv.nodes.begin(), drv.nodes.end(), [this](auto&& node) { return node.name == name; });
	if (it == drv.nodes.end())
	{
		throw std::runtime_error("unknown node name: " + name);
	}
	int i = it - drv.nodes.begin();
	int word = i / 32;
	int bit = i % 32;
	os << "((state[" << word << "] & " << (1u << bit) << "u) != 0)";
}

expr_ptr identifier_expression::simplify() const { return std::make_unique<identifier_expression>(*this); }

variable_expression::variable_expression(std::string name) : name(std::move(name)) {}

float variable_expression::evaluate(const driver& drv) const { return drv.variables.at(name); }

void variable_expression::generate_code(const driver& drv, const std::string&, std::ostream& os) const
{
	os << drv.variables.at(name);
}

expr_ptr variable_expression::simplify() const { return std::make_unique<variable_expression>(*this); }

alias_expression::alias_expression(std::string name) : name(std::move(name)) {}

float alias_expression::evaluate(const driver&) const
{
	throw std::runtime_error("alias " + name + "in expression which needs to be evaluated");
}

void alias_expression::generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const
{
	auto it = std::find_if(drv.nodes.begin(), drv.nodes.end(), [&](auto&& node) { return node.name == current_node; });
	assert(it != drv.nodes.end());

	auto&& attr = it->get_attr(name.substr(1));

	attr.second->generate_code(drv, current_node, os);
}

expr_ptr alias_expression::simplify() const { return std::make_unique<alias_expression>(*this); }

flat_expression::flat_expression(operation op, std::vector<expr_ptr> exprs) : op(op), exprs(std::move(exprs)) {}

float flat_expression::evaluate(const driver&) const
{
	throw std::runtime_error("a flat expression which needs to be evaluated");
}

void flat_expression::generate_code_default(const driver& drv, const std::string& current_node, std::ostream& os) const
{
	std::string op_str;
	switch (op)
	{
		case operation::PLUS:
			op_str = " + ";
			break;
		case operation::MINUS:
			op_str = " - ";
			break;
		case operation::STAR:
			op_str = " * ";
			break;
		case operation::SLASH:
			op_str = " / ";
			break;
		case operation::AND:
			op_str = " && ";
			break;
		case operation::OR:
			op_str = " || ";
			break;
		case operation::EQ:
			op_str = " == ";
			break;
		case operation::NE:
			op_str = " != ";
			break;
		case operation::LE:
			op_str = " <= ";
			break;
		case operation::LT:
			op_str = " < ";
			break;
		case operation::GE:
			op_str = " >= ";
			break;
		case operation::GT:
			op_str = " > ";
			break;
		default:
			throw std::runtime_error("Unknown binary operator " + std::to_string(static_cast<int>(op)));
	}

	bool first = true;

	for (auto&& expr : exprs)
	{
		if (first)
			first = false;
		else
			os << op_str;

		expr->generate_code(drv, current_node, os);
	}
}

void flat_expression::generate_code_binary(const driver& drv, const std::string& current_node, std::ostream& os) const
{
	assert(op == operation::AND || op == operation::OR);

	// check if all expressions are literals or identifiers
	{
		bool is_suitable = std::all_of(exprs.begin(), exprs.end(), [](auto&& expr) {
			if (dynamic_cast<literal_expression*>(expr.get()))
				return true;
			if (auto un_ex = dynamic_cast<unary_expression*>(expr.get()))
			{
				if (un_ex->op == operation::NOT && dynamic_cast<identifier_expression*>(un_ex->expr.get()))
					return true;
			}

			return false;
		});

		if (!is_suitable)
		{
			generate_code_default(drv, current_node, os);
			return;
		}
	}

	std::vector<int> positive_indices;
	std::vector<int> negative_indices;

	// get the indices of the identifiers and divide them into positive and negative groups
	{
		auto get_index = [&](auto&& name) {
			auto it = std::find_if(drv.nodes.begin(), drv.nodes.end(), [&](auto&& node) { return node.name == name; });
			if (it == drv.nodes.end())
			{
				throw std::runtime_error("unknown node name: " + name);
			}
			return it - drv.nodes.begin();
		};

		for (auto&& expr : exprs)
		{
			if (auto id_expr = dynamic_cast<identifier_expression*>(expr.get()))
			{
				positive_indices.push_back(get_index(id_expr->name));
			}
			else
			{
				auto un_expr = dynamic_cast<unary_expression*>(expr.get());
				assert(un_expr);
				assert(un_expr->op == operation::NOT);
				auto id_expr2 = dynamic_cast<identifier_expression*>(un_expr->expr.get());
				assert(id_expr2);
				negative_indices.push_back(get_index(id_expr2->name));
			}
		}
	}

	std::sort(positive_indices.begin(), positive_indices.end());
	std::sort(negative_indices.begin(), negative_indices.end());

	// check for the intersection of positive_indices and negative_indices
	{
		std::vector<int> intersection;
		std::set_intersection(positive_indices.begin(), positive_indices.end(), negative_indices.begin(),
							  negative_indices.end(), std::back_inserter(intersection));

		if (!intersection.empty())
		{
			if (op == operation::AND)
				os << "false";
			else
				os << "true";
		}
	}

	// generate the code
	{
		constexpr int word_size = 32;
		int iteration = 0;
		bool first = true;
		auto pit = positive_indices.begin();
		auto nit = negative_indices.begin();

		os << "(";

		while (true)
		{
			++iteration;
			auto pnext = std::lower_bound(pit, positive_indices.end(), word_size * iteration);
			auto nnext = std::lower_bound(nit, negative_indices.end(), word_size * iteration);

			if (pnext == positive_indices.end() && nnext == negative_indices.end())
				break;

			if (pit == pnext && nit == nnext)
				continue;

			uint32_t xor_mask = 0;
			uint32_t and_mask = 0;

			for (; pit != pnext; ++pit)
			{
				int bit = *pit % word_size;
				and_mask |= 1u << bit;
			}

			for (; nit != nnext; ++nit)
			{
				int bit = *nit % word_size;
				and_mask |= 1u << bit;
				xor_mask |= 1u << bit;
			}

			if (first)
				first = false;
			else
				os << " | ";

			if (op == operation::AND)
				os << "(state[" << iteration - 1 << "] & " << and_mask << "u ^ " << (xor_mask ^ and_mask) << "u)";
			else
				os << "(state[" << iteration - 1 << "] ^ " << xor_mask << "u & " << and_mask << "u)";
		}

		if (op == operation::AND)
			os << "== 0";
		else
			os << "!= 0";

		os << ")";
	}
}

void flat_expression::generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const
{
	if (op != operation::AND && op != operation::OR)
	{
		generate_code_default(drv, current_node, os);
	}
	else
	{
		generate_code_binary(drv, current_node, os);
	}
}

expr_ptr flat_expression::simplify() const
{
	throw std::runtime_error("a flat expression which needs to be simplified");
}
