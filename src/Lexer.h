#ifndef LEXER_H
#define LEXER_H

#include <vector>

#include "Object.h"
#include "Token.h"

typedef std::vector <Token> Tokens;

class Lexer : public Object {

	OBJ_CLASS(Lexer);

	public:
		Lexer();
		virtual ~Lexer() = default;

		Tokens lex(const std::string & code);

		static const std::string operators[];

		static const std::string keywords[];


	private:
		Tokens tokens;
		
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

		// Post Determinators
		bool is_keyword(const std::string & s);
		bool is_operator(const std::string & s);

	// Errors
	public:
		virtual void error(const std::string & message);

	private:
		void unexpected_token();
		void unexpected_token(const std::string & token);
};

#endif