#ifndef LEXER_H
#define LEXER_H

#include <vector>

#include "token.h"

class Lexer {
	public:
		Lexer();
		virtual ~Lexer() = default;

	private:
		std::vector tokens;
		std::vector::iterator index;

		char peek();
		char advance();
};

#endif