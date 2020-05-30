#include "src/Lexer.h"
#include "src/Parser.h"

#include <iostream>
#include <chrono>

//////
// TODO: Add special numbers like hex and exp and etc. //
//////

int main(int argc, char const * argv[]){

	if(argc < 2){
		std::cout << "Please, specify path to input file" << std::endl;
		return -1;
	}

	try{
		Lexer lexer;
		Parser parser;
		
		
		// Lexing
		auto lexer_start = std::chrono::high_resolution_clock::now();
		std::vector <Token> tokens = lexer.lex(argv[1]);
		auto lexer_finish = std::chrono::high_resolution_clock::now();

		std::cout << "Tokens:\n";
		for(auto & t : tokens){
			std::cout << t.to_string(true) << std::endl;
		}

		// Parsing
		
		std::cout << "\nParsing...\n";

		auto parser_start = std::chrono::high_resolution_clock::now();
		StatementList tree = parser.parse(tokens);
		auto parser_finish = std::chrono::high_resolution_clock::now();

		std::cout << "\nTree:\n";
		for(NStatement * t : tree){
			std::cout << t->to_string() << std::endl;
		}

		Interpreter I;

		I.interpret(tree);

		std::chrono::duration<double> lexer_elapsed = lexer_finish - lexer_start;
		std::chrono::duration<double> parser_elapsed = parser_finish - parser_start;
		std::chrono::duration<double> eval_ms = eval_finish - eval_start;

		int lexer_ms = lexer_elapsed.count() * 10e3;
		int parser_ms = parser_elapsed.count() * 10e3;
		std::cout << "Lexing was done in: " << lexer_ms << "ms" << std::endl;
		std::cout << "Parsing was done in: " << parser_ms << "ms" << std::endl;
		std::cout << "Evaluation was done in: " << eval_ms << "ms" << std::endl;

	}catch (std::exception & e){
	    std::cerr << e.what() << std::endl;
	    return 1;
	}

    return 0;
}
