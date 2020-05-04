#include "src/Lexer.h"

#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char const * argv[]) {

	if(argc < 2){
		std::cout << "Please, specify path to input file" << std::endl;
	}

	std::ifstream file(argv[1]);

	std::stringstream buffer;
	buffer << file.rdbuf();

	std::string code = buffer.str();

	Lexer lexer;

	ObjectBuffer objects;

	try{
		objects = lexer.lex(code);
	}catch(const char * message){
		std::cout << message << std::endl;
	}catch(const std::string & message){
		std::cout << message << std::endl;
	}

	for(auto & t : objects){
		std::cout << t.type << " pos = " << t.line << ":" << t.column << std::endl;
	}

    return 0;
}
