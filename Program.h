#ifndef VALUE_H_
#define VALUE_H_

#include <string>
#include <stdexcept>
#include <map>
#include <memory>

///--------------------------------------------------------------------

class AST_Exp;
class AST_Program;

using Memory = std::map<std::string, double>;
using AST_Exp_Ptr = std::unique_ptr<AST_Exp>;
using AST_Program_Ptr = std::unique_ptr<AST_Program>;

//////////////////////////// EXPRESSION //////////////////////////////////////////

class AST_Exp {
public:
	virtual double Solve(Memory& vars) = 0;
	virtual ~AST_Exp() {}
};

///--------------------------------------------------------------------

class Constant : public AST_Exp {
private:
	const double m_constant;
public:
	Constant(double constant) : m_constant(constant) {}
	virtual double Solve(Memory& vars);
};

///--------------------------------------------------------------------

class BinaryOperator : public AST_Exp {
private:
	char m_operator;
	AST_Exp_Ptr m_left;
	AST_Exp_Ptr m_right;
public:
	BinaryOperator(char operat, AST_Exp_Ptr left, AST_Exp_Ptr right)
		: m_operator(operat), m_left(std::move(left)), m_right(std::move(right)) {}
	virtual double Solve(Memory& vars);
};

///--------------------------------------------------------------------

class Variable : public AST_Exp {
private:
	std::string m_name;
public:
	Variable(const std::string& variable) : m_name(variable) {}
	virtual double Solve(Memory& vars);
};

//////////////////////////// PROGRAM /////////////////////////////////////////////

class AST_Program {
public:
	virtual bool Make(Memory& vars) = 0;
	virtual ~AST_Program() {}
};

///--------------------------------------------------------------------

class Skip : public AST_Program {
public:
	Skip() {}
	virtual bool Make(Memory& vars) { return true; }
};

///--------------------------------------------------------------------

class Assign : public AST_Program {
private:
	std::string m_var;
	AST_Exp_Ptr m_exp;
public:
	Assign(const std::string& var, AST_Exp_Ptr exp)
		: m_var(var), m_exp(std::move(exp)) {}
	virtual bool Make(Memory& vars);
};

///--------------------------------------------------------------------

class Read : public AST_Program {
private:
	std::string m_var;
public:
	Read(std::string var) : m_var(var) {}
	virtual bool Make(Memory& vars);
};

///--------------------------------------------------------------------

class Write : public AST_Program {
private:
	std::string m_var;
public:
	Write(std::string var) : m_var(var) {}
	virtual bool Make(Memory& vars);
};

///--------------------------------------------------------------------

class Composition : public AST_Program {
private:
	AST_Program_Ptr m_left;
	AST_Program_Ptr m_right;
public:
	Composition(AST_Program_Ptr left, AST_Program_Ptr right)
		: m_left(std::move(left)), m_right(std::move(right)) {}
	virtual bool Make(Memory& vars);
};

///--------------------------------------------------------------------

class If : public AST_Program {
private:
	AST_Program_Ptr m_then;
	AST_Program_Ptr m_else;
	AST_Exp_Ptr m_condition;
public:
	If(AST_Exp_Ptr condition, AST_Program_Ptr then, AST_Program_Ptr els)
		: m_then(std::move(then)), m_else(std::move(els)), m_condition(std::move(condition)) {}
	virtual bool Make(Memory& vars);
};

///--------------------------------------------------------------------

class While : public AST_Program {
private:
	AST_Program_Ptr m_body;
	AST_Exp_Ptr m_condition;
public:
	While(AST_Exp_Ptr condition, AST_Program_Ptr body)
		: m_body(std::move(body)), m_condition(std::move(condition)) {}
	virtual bool Make(Memory& vars);
};

///--------------------------------------------------------------------

class WriteText : public AST_Program {
private:
	std::string m_text;
	bool m_newLine;
public:
	WriteText(std::string text, bool newLine)
		: m_text(text), m_newLine(newLine) {}
	virtual bool Make(Memory& vars);
};

///--------------------------------------------------------------------

class Break : public AST_Program {
public:
	Break() {}
	virtual bool Make(Memory& vars) { return false; }
};

///--------------------------------------------------------------------

#endif





