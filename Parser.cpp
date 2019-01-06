#include <string>
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include "Program.h"
#include "Parser.h"
#include <memory>
#include "GlobalProjectInfo.h"

///--------------------------------------------------------------------

Parser::Parser(const std::string& input)
	: pointer(0), m_input(std::move(input)) {
	m_input.push_back(EOS);
}

///--------------------------------------------------------------------

void Parser::skip_whitespace() {
	while (isspace(m_input[pointer]))
		++pointer;
}

///--------------------------------------------------------------------

char Parser::look_ahead() {
	skip_whitespace();
	return m_input[pointer];
}

//////////////////////////// EXPRESSION //////////////////////////////////////////

///--------------------------------------------------------------------

AST_Exp_Ptr Parser::parse_logic() {
	auto  res = parse_sum();
	char c = look_ahead();
	while (c == '=' || c == '>' || c == '<' || c == '!') {
		++pointer;
		res = std::make_unique<BinaryOperator>(c, std::move(res), parse_sum());
		c = look_ahead();
	}
	return res;
}

///--------------------------------------------------------------------

AST_Exp_Ptr Parser::parse_sum() {
	auto res = parse_mult();
	char c = look_ahead();
	while (c == '+' || c == '-') {
		++pointer;
		res = std::make_unique<BinaryOperator>(c, std::move(res), parse_mult());
		c = look_ahead();
	}
	return res;
}

///--------------------------------------------------------------------

AST_Exp_Ptr Parser::parse_mult() {
	auto res = parse_term();
	char c = look_ahead();
	while (c == '*' || c == '/' || c == '%' || c == '^') {
		++pointer;
		res = std::make_unique<BinaryOperator>(c, std::move(res), parse_term());
		c = look_ahead();
	}
	return res;
}

///--------------------------------------------------------------------

AST_Exp_Ptr Parser::parse_term() {
	char c = look_ahead();
	if (std::isdigit(c))
		return parse_constant();
	else if (std::isalpha(c))
		return parse_variable();
	else if (c == '(')
		return parse_paren();
	else {
		std::ostringstream oss;
		oss << "['" << c << "'=?] -> unknown operator";
		throw std::logic_error(oss.str());
	}
}

///--------------------------------------------------------------------

AST_Exp_Ptr Parser::parse_constant() {
	std::string num;
	while (std::isdigit(m_input[pointer]) || m_input[pointer] == '.') {
		num.push_back(m_input[pointer]);
		++pointer;
	}

	double temp = 0.0;
	std::stringstream ss;
	ss << num;
	ss >> temp;

	return std::make_unique<Constant>(temp);
}

///--------------------------------------------------------------------

AST_Exp_Ptr Parser::parse_variable() {
	std::string temp;
	while (std::isalnum(m_input[pointer])) {
		temp.push_back(m_input[pointer]);
		++pointer;
	}

	return std::make_unique<Variable>(temp);
}

///--------------------------------------------------------------------

AST_Exp_Ptr Parser::parse_paren() {
	++pointer;

	auto res = parse_logic();
	if (look_ahead() == ')') {
		++pointer;
		return res;
	}
	else
		throw std::logic_error("odd number of ')'");
}


//////////////////////////// PROGRAM /////////////////////////////////////////////

bool Parser::is_instruction(char c) {
	if (c == '[' || c == '@' || c == '$' || c == '#' || c == '~' || c=='?' || c=='`')
		return true;
	return false;
}

///--------------------------------------------------------------------

std::string Parser::read_word() {
	std::string temp;
	while (std::isalnum(m_input[pointer])) {
		temp.push_back(m_input[pointer]);
		++pointer;
	}
	return temp;
}

///--------------------------------------------------------------------

AST_Program_Ptr Parser::parse_program() {
	auto res = parse_block();
	if (look_ahead() == EOS)
		return res;
	else
		throw std::logic_error("{program}['EOS'=?]} -> undefined error"); 
}

///--------------------------------------------------------------------

AST_Program_Ptr Parser::parse_block() {

	auto res = parse_instruction();

	char c = look_ahead();

	while (c != '}' &&  c != EOS) {
		auto res2 = parse_instruction();
		res = std::make_unique<Composition>(std::move(res), std::move(res2));
		c = look_ahead();
	}

	return res;
}

///--------------------------------------------------------------------

AST_Program_Ptr Parser::parse_instruction() {
	char c = look_ahead();
	if (c == '{') {
		++pointer;
		auto res = parse_block();
		if (look_ahead() == '}') {
			++pointer;
			return res;
		}
		else
			throw std::logic_error("odd number of '}'");
	}
	else if (is_instruction(c)) {

		++pointer;
		if (c == '$')
			return parse_read();
		else if (c == '#')
			return parse_write();
		else if (c == '@')
			return parse_if();
		else if (c == '[')
			return parse_while();
		else if (c == '~')
			return std::make_unique<Skip>();
		else if (c == '`')
			return std::make_unique<Break>();
		else if (c == '?')
			return parse_writeText();

	}
	else if (std::isalpha(c)) {
		return parse_assign(read_word());
	}
	else {
		std::ostringstream oss;
		oss <<"['"<< c << "'=? ] -> unknown operator";
		throw std::logic_error(oss.str());
	}
}

///--------------------------------------------------------------------

AST_Program_Ptr Parser::parse_read() {
	char c = look_ahead();
	if (std::isalpha(c)) {
		std::string temp = read_word();
		return std::make_unique<Read>(temp);
	}
	else
		throw std::logic_error("$['var'=?] -> the variable is missing");

}

///--------------------------------------------------------------------

AST_Program_Ptr Parser::parse_write() {
	char c = look_ahead();
	if (std::isalpha(c)) {
		std::string temp = read_word();
		return std::make_unique<Write>(temp);
	}
	else
		throw std::logic_error("#['var'=?] -> the variable is missing");
}

///--------------------------------------------------------------------

AST_Program_Ptr Parser::parse_if() {

	auto cond = parse_logic();

	char c = look_ahead();

	if (c == ',') {

		++pointer;
		auto b1 = parse_instruction();
		c = look_ahead();

		if (c == ',') {

			++pointer;
			auto b2 = parse_instruction();
			c = look_ahead(); 

			if(c!='&')
				throw std::logic_error("@(),{},{}['&'=?] -> there is no end instruction");

			else { 
				++pointer;
				return std::make_unique<If>(std::move(cond), std::move(b1), std::move(b2));
			}

		}
		else {
			throw std::logic_error("@(),{},['{}'=?]& -> instruction is missing");
		}
	}
	else {
		throw std::logic_error("@(),['{}'=?],{}& -> instruction is missing");
	}

}

///--------------------------------------------------------------------

AST_Program_Ptr Parser::parse_while() {


	auto cond = parse_logic();
	char c = look_ahead();

	if (c == ',') {
		++pointer;
		auto b = parse_instruction();
		c = look_ahead();

		if(c!=']')
			throw std::logic_error("[(),{}[']'=?] -> there is no end instruction");
		else {
			++pointer;
			return std::make_unique<While>(std::move(cond), std::move(b));
		}

	}
	else {
		throw std::logic_error("[(),['{}'=?]] -> instruction is missing");
	}
}

///--------------------------------------------------------------------

AST_Program_Ptr Parser::parse_assign(std::string varNam) {

	char c = look_ahead();
	if (c == ':') {
		++pointer;
		auto val = parse_logic();
		return std::make_unique<Assign>(varNam, std::move(val));
	}
	else {
		std::ostringstream oss;
		oss << c << ":['exp'=?] -> the expression is missing";
		throw std::logic_error(oss.str());
	}
}

///--------------------------------------------------------------------

AST_Program_Ptr Parser::parse_writeText() {

	std::string temp;
	while(m_input[pointer]!='?') {

		if (m_input[pointer] == '\\') {

			if (m_input[pointer + 1] == '?') {
				++pointer;
				temp.push_back(m_input[pointer]);
			}
			else if (m_input[pointer + 1] == '\\') {
				++pointer;
				temp.push_back(m_input[pointer]);
			}
			else {
				temp.push_back('\\');
			}
		}
		else {
			temp.push_back(m_input[pointer]);
		}

		++pointer;
		if (pointer >= m_input.size()) {
			throw std::logic_error("?\"txt\"['?'=?]<|,;> -> the end of the text is missing");
		}
	}
	++pointer;
	char c = look_ahead();
	
	if(c==';'){
		++pointer;
		return std::make_unique<WriteText>(temp,true);
	}	
	else if(c=='|'){
		++pointer;
		return std::make_unique<WriteText>(temp,false);
	}
	else {
		throw std::logic_error("?\"txt\"?[?] -> text manipulator is missing");
	}
}

///--------------------------------------------------------------------


