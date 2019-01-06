#ifndef PARSER_H_
#define PARSER_H_

#include "Program.h"
#include "GlobalProjectInfo.h"
#include <string>

///--------------------------------------------------------------------

class Parser {
private:
	const char EOS = 0;
	std::string m_input;
	unsigned pointer;

private:
	void skip_whitespace();
	char look_ahead();

public:
	Parser(const std::string& input);
	//AST_Exp_Ptr parse_expression();
	AST_Program_Ptr parse_program();

private:

	AST_Exp_Ptr parse_logic();
	AST_Exp_Ptr parse_sum();
	AST_Exp_Ptr parse_mult();
	AST_Exp_Ptr parse_term();		   
	AST_Exp_Ptr parse_constant();
	AST_Exp_Ptr parse_variable();
	AST_Exp_Ptr parse_paren();

	std::string read_word();
	bool is_instruction(char c); 

	AST_Program_Ptr parse_block();
	AST_Program_Ptr parse_instruction();
	AST_Program_Ptr parse_read();
	AST_Program_Ptr parse_write();
	AST_Program_Ptr parse_if();
	AST_Program_Ptr parse_while();
	AST_Program_Ptr parse_assign(std::string varNam); 
	AST_Program_Ptr parse_writeText(); 
};

///--------------------------------------------------------------------

#endif 






