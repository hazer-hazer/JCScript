#ifndef LEXER_H
#define LEXER_H

#include <vector>

#include "object.h"

class Lexer {

	public:
		Lexer();
		virtual ~Lexer() = default;

		ObjectBuffer & lex(const std::string & code);

	private:
		ObjectBuffer objects;
		
		// Stream
		std::string code;

		unsigned int index;
		char current;
		char peek();
		char advance();

		unsigned int line;
		unsigned int column;

		// Determinators
		bool is_skipable(const char & c);
		bool is_digit(const char & c);
		bool is_identifier_first(const char & c);
		bool is_identifier(const char & c);
		bool is_punct(const char & c);
		bool is_quote(const char & c);

	// Errors
	private:
		void unexpected_token();
		void unexpected_token(const std::string & val);
		void expected_token(const std::string & val);
};

#endif