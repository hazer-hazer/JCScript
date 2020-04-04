#include "src/Lexer.h"

#include <iostream>

int main(int argc, char const *argv[]) {

	// for(char c = 32; c < 127; c++){
	// 	std::cout << (int)c << " " << c << std::endl;
	// }

	Lexer lexer;

	const char * code = "var a = 'my_string\n"
						"print(a)";

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
