#include "src/Lexer.h"
#include "src/Parser.h"

#include <iostream>
#include <fstream>
#include <sstream>

// TODO: Make to_string for every node for debug!!!

int main(int argc, char const * argv[]) {

	if(argc < 2){
		std::cout << "Please, specify path to input file" << std::endl;
		return -1;
	}

	std::ifstream file(argv[1]);

	std::stringstream buffer;
	buffer << file.rdbuf();

	std::string code = buffer.str();

	try{
		// Lexing
		Lexer lexer;
		
		std::vector <Token> tokens = lexer.lex(code);

		std::cout << "Tokens:\n";
		for(const auto & t : tokens){
			std::cout << token_type_to_str(t.type) << " : `" << t.val << "` pos = " << t.line << ":" << t.column << std::endl;
		}

		Parser parser;

		Tree tree = parser.parse(tokens);

		std::cout << "\nTree[size=" << tree.size() << "]:\n";

		print_tree(tree);


	}/*catch(const char * message){
		std::cout << message << std::endl;
	}catch(const std::string & message){
		std::cout << message << std::endl;
	}*/catch (std::exception & e){
	    std::cerr << e.what() << std::endl;
	}

    return 0;
}
