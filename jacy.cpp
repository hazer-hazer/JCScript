#include "src/Lexer.h"

#include <iostream>

int main(int argc, char const *argv[]) {

	// for(char c = 32; c < 127; c++){
	// 	std::cout << (int)c << " " << c << std::endl;
	// }

	Lexer lexer;

	const char * code = "var a = 0";
						// "print(a)";

	Tokens tokens;

	try{
		tokens = lexer.lex(code);
	}catch(const char * message){
		std::cout << message << std::endl;
	}catch(const std::string & message){
		std::cout << message << std::endl;
	}

	for(auto t : tokens){
		std::cout << t.type << " `" << t.value << "`; pos = " << t.line << ":" << t.column << std::endl;
	}

    return 0;
}
