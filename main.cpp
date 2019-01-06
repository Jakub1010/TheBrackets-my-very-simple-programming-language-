#include <iostream>
#include "Program.h"
#include <cstdlib>
#include <memory>
#include <string>
#include <fstream>
#include "Parser.h"

int main(int argc, char* argv[]) {

	if (argc != 2) {
		std::cout << "no argument for the program!\n";
		return EXIT_FAILURE;
	}
	else {

		std::fstream file;

		file.open(argv[1]);
		if (!file.is_open()) {
			std::cout << "the file was not found!\n";
			std::cin.get();
			std::cin.get();
			return EXIT_FAILURE;
		}

		std::string input;
		char c;

		while (file.get(c)) {
			input.push_back(c);
		}

		file.close();

		try {
			Parser parser(input);
			std::unique_ptr<AST_Program> exp = parser.parse_program();
			Memory m;
			exp->Make(m);
		}

		catch (const std::invalid_argument& inv) {

			std::cout << inv.what() << std::endl;
			return EXIT_FAILURE;
		}
		catch (const std::logic_error& inv) {
			std::cout << inv.what() << std::endl;
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}
 
}
