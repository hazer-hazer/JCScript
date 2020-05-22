#ifndef LEXER_H
#define LEXER_H

#include <sstream>
#include <fstream>
#include <vector>

#include "Token.h"

// TODO: Think about nextToken configuration instead of Lexer->Parser

// Note: One-line comments skipping is implemented on Lexer level, but
// multi-line comments skipping is implemented on Parser level, because of complex operator problem

class Lexer {
	public:
		Lexer();
		virtual ~Lexer() = default;

		std::vector <Token> lex(const char * path);

	private:
		std::string code;
		uint32_t index;
		uint32_t line;
		uint32_t column;
		char peek();
		char advance();

		std::vector <Token> tokens;
		void add_token(const TokenType & type, const std::string & val);
		void add_token(const Operator & op);

		bool eof();
		// Determinators
		bool is_skipable(const char & c);
		bool is_digit(const char & c);
		bool is_hex(const char & c);
		bool is_identifier_first(const char & c);
		bool is_identifier(const char & c);
		bool is_punct(const char & c);
		bool is_quote(const char & c);
		bool is_endl(const char & c);

		std::string lex_number();
		std::string lex_identifier();

	// Errors
	private:
		void error(const std::string & msg);
		void unexpected_token(const std::string & val = "");
};

#endif