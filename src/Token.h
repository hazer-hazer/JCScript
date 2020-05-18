#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
#include <algorithm>

#include "err.h"

// TODO: Think about null-safety

enum TokenType {
	T_NULL,
	T_NUM, // Number
	T_STR, // String
	T_ID, // Identifier
	T_KW, // Keyword - ID before
	T_OP, // Operator,
	T_ENDL,
	T_PROG_END
};

enum Operator {
	OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
	OP_ASSIGN,
	OP_ASSIGN_ADD, OP_ASSIGN_SUB, OP_ASSIGN_MUL, OP_ASSIGN_DIV, OP_ASSIGN_MOD
};

enum Keyword {

};

typedef float Real;

// TODO: Add null
typedef std::variant<Real, char*, Operator, Keyword> TokenVal;

struct Token {
	TokenType type;
	TokenVal val;

	// Trace
	unsigned int line = 0;
	unsigned int column = 0;

	Real real(){
		return std::get<int>(val);
	}
	char * str(){
		return std::get<char*>(val);
	}
	Operator op(){
		return std::get<Operator>(val);
	}
	Keyword kw(){
		return std::get<Keyword>(val);
	}

	// Errors
	void error(const std::string & msg){
		err(msg + " " + val + " type of " + token_type_to_str(type), line, column);
	}
	void unexpected_error(){
		error("Unexpected token");
	}

	std::string to_string(){
		std::string str;
		switch (type) {
			case T_NULL: str = "null"; break;
			case T_NUM: str = "number"; break;
			case T_STR: str = "string"; break;
			case T_ID: str = "identifier"; break;
			case T_KW: str = "keyword"; break;
			case T_OP: str = "operator"; break;
			case T_ENDL: str = "end of line"; break;
			case T_PROG_END: str = "end of program"; break;
			default: str = "Unrecognized token type";
		}
		switch (type) {
			case T_NUM: str += real(); break;
			case T_STR:
			case T_ID:
				str += str();
				break;
			case T_KW:
				str += kw();
				break;
			case T_OP:
				str += op();
				break;
		}
		// TODO: Add op and kw to string

		return str;
	}
};

#endif