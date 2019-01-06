#include <string>
#include <stdexcept>
#include <iostream>
#include <map>
#include "Program.h"
#include <cmath>
#include <memory>

///--------------------------------------------------------------------

double Constant::Solve(Memory& m) {
	return m_constant;
}

///--------------------------------------------------------------------

double BinaryOperator::Solve(Memory& m) {
	if (m_operator == '+')
		return m_left->Solve(m) + m_right->Solve(m);
	else if (m_operator == '-')
		return m_left->Solve(m) - m_right->Solve(m);
	else if (m_operator == '*')
		return m_left->Solve(m) * m_right->Solve(m);
	else if (m_operator == '/')
		return m_left->Solve(m) / m_right->Solve(m);
	else if (m_operator == '^')
		return std::pow(m_left->Solve(m), m_right->Solve(m));
	else if (m_operator =='>')
		return static_cast<double>(m_left->Solve(m) > m_right->Solve(m));
	else if (m_operator == '<')
		return static_cast<double>(m_left->Solve(m) < m_right->Solve(m));
	else if (m_operator == '=')
		return static_cast<double>(m_left->Solve(m) == m_right->Solve(m));
	else if (m_operator == '!')
		return static_cast<double>(m_left->Solve(m) != m_right->Solve(m));
	else if (m_operator == '%')
		return static_cast<double>(int(m_left->Solve(m)) % int(m_right->Solve(m)));
}

///--------------------------------------------------------------------

double Variable::Solve(Memory& m) {
	Memory::iterator it;
	it = m.find(m_name);
	if (it == m.end())
		throw std::invalid_argument("Such name of variable not exist");
	return it->second;
}

//////////////////////////// PROGRAM /////////////////////////////////////////////

bool Assign::Make(Memory& vars) {
	vars[m_var] = m_exp->Solve(vars);
	return true;
}

///--------------------------------------------------------------------

bool Read::Make(Memory& vars) {
	double temp;
	std::cin >> temp;
	vars[m_var] = temp;
	return true;
}

///--------------------------------------------------------------------

bool Write::Make(Memory& vars) {
	Memory::iterator it = vars.find(m_var);
	if (it == vars.end())
		throw std::invalid_argument("Such name of variable not exist");
	else std::cout << vars[m_var];
	return true;
}

///--------------------------------------------------------------------

bool Composition::Make(Memory& vars) {
	if (!(m_left->Make(vars)))
		return false;
	if (!(m_right->Make(vars)))
		return false;
	return true;
}

///--------------------------------------------------------------------

bool If::Make(Memory& vars) {
	if (m_condition->Solve(vars) != 0)
		return m_then->Make(vars);
	else return m_else->Make(vars);
}

///--------------------------------------------------------------------

bool While::Make(Memory& vars) {
	if (m_condition->Solve(vars) != 0) {
		if (!(m_body->Make(vars))) return true;
		this->Make(vars);
	}
	return true;
}

///--------------------------------------------------------------------

bool WriteText::Make(Memory& vars) {
	std::cout<<m_text;
	if(m_newLine)
		std::cout<<"\n";
	return true; 
}

///--------------------------------------------------------------------




