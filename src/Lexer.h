#ifndef LEXER_H
#define LEXER_H

#include <vector>

#include "Stream.h"
#include "Token.h"

class Lexer {
	public:
		Lexer();
		virtual ~Lexer() = default;

		std::vector <Token> lex(const std::string & code);

	private:
		// Output tokens
		std::vector <Token> tokens;

		void add_token(const Token & t);

		// Determinators
		bool is_skipable(const char & c);
		bool is_digit(const char & c);
		bool is_identifier_first(const char & c);
		bool is_identifier(const char & c);
		bool is_punct(const char & c);
		bool is_quote(const char & c);
		bool is_endl(const char & c);

	// Errors
	private:
		void error(const std::string & msg);
		void unexpected_token(const std::string & val = "");
};

#endif