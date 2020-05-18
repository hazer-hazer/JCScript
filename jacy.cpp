#include "src/Lexer.h"
#include "src/Parser.h"

// TODO: Make to_string for every node for debug!!!

int main(int argc, char const * argv[]) {

	if(argc < 2){
		std::cout << "Please, specify path to input file" << std::endl;
		return -1;
	}

	try{
		// Lexer lexer;
		
		// std::vector <Token> tokens = lexer.lex(code);

		// std::cout << "Tokens:\n";
		// for(const auto & t : tokens){
		// 	std::cout << token_type_to_str(t.type) << " : `" << t.val << "` pos = " << t.line << ":" << t.column << std::endl;
		// }

	}catch (Exception & e){
	    std::cerr << e.what() << std::endl;
	    return EXIT_FAILURE;
	}

    return 0;
}
