#pragma once

#include <memory>
#include <string>
#include <vector>

class expression;
class driver;

using expr_ptr = std::unique_ptr<expression>;

enum class operation
{
	PLUS,
	MINUS,
	STAR,
	SLASH,
	AND,
	OR,
	XOR,
	NOT,
	LE,
	LT,
	GE,
	GT,
	EQ,
	NE
};

class expression
{
public:
	virtual ~expression() noexcept = default;
	virtual float evaluate(const driver& drv) const = 0;
	virtual void generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const = 0;
	virtual expr_ptr simplify() const = 0;
};

class unary_expression : public expression
{
public:
	unary_expression(operation op, expr_ptr expr);
	float evaluate(const driver& drv) const override;
	void generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const override;
	virtual expr_ptr simplify() const override;

	operation op;
	expr_ptr expr;
};

class binary_expression : public expression
{
public:
	binary_expression(operation op, expr_ptr left, expr_ptr right);
	float evaluate(const driver& drv) const override;
	void generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const override;
	virtual expr_ptr simplify() const override;

	operation op;
	expr_ptr left;
	expr_ptr right;
};

class ternary_expression : public expression
{
public:
	ternary_expression(expr_ptr left, expr_ptr middle, expr_ptr right);
	float evaluate(const driver& drv) const override;
	void generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const override;
	virtual expr_ptr simplify() const override;

	expr_ptr left;
	expr_ptr middle;
	expr_ptr right;
};

class parenthesis_expression : public expression
{
public:
	parenthesis_expression(expr_ptr expr);
	float evaluate(const driver& drv) const override;
	void generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const override;
	virtual expr_ptr simplify() const override;

	expr_ptr expr;
};

class literal_expression : public expression
{
public:
	literal_expression(float value);
	float evaluate(const driver& drv) const override;
	void generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const override;
	virtual expr_ptr simplify() const override;

	float value;
};

class identifier_expression : public expression
{
public:
	identifier_expression(std::string name);
	float evaluate(const driver& drv) const override;
	void generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const override;
	virtual expr_ptr simplify() const override;

	std::string name;
};

class variable_expression : public expression
{
public:
	variable_expression(std::string name);
	float evaluate(const driver& drv) const override;
	void generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const override;
	virtual expr_ptr simplify() const override;

	std::string name;
};

class alias_expression : public expression
{
public:
	alias_expression(std::string name);
	float evaluate(const driver& drv) const override;
	void generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const override;
	virtual expr_ptr simplify() const override;

	std::string name;
};

class flat_expression : public expression
{
	void generate_code_default(const driver& drv, const std::string& current_node, std::ostream& os) const;
	void generate_code_binary(const driver& drv, const std::string& current_node, std::ostream& os) const;

public:
	flat_expression(operation op, std::vector<expr_ptr> exprs);
	float evaluate(const driver& drv) const override;
	void generate_code(const driver& drv, const std::string& current_node, std::ostream& os) const override;
	virtual expr_ptr simplify() const override;

	operation op;
	std::vector<expr_ptr> exprs;
};
