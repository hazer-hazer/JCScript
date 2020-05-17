#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
#include <algorithm>

#include "error.h"

// TokenType
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

inline std::string token_type_to_str(const TokenType & type){
	switch (type) {
		case T_NULL: return "null"; break;
		case T_NUM: return "number"; break;
		case T_STR: return "string"; break;
		case T_ID: return "identifier"; break;
		case T_KW: return "keyword"; break;
		case T_OP: return "operator"; break;
		case T_ENDL: return "end of line"; break;
		case T_PROG_END: return "end of program"; break;
		default: return "Unrecognized token type";
	}
}

const std::vector <std::string> operators {
	"+", "-", "*", "/", "%",
	"=",
	"(", ")", "{", "}", "[", "]",
	",", ":",
	"!", "&&", "||"
};

inline bool is_op(const std::string & str){
	return std::find(operators.begin(), operators.end(), str) != operators.end();
}

const std::vector <std::string> keywords {
	"type",
	"var", "val", "func",
	"num", "str", "bool",
	"false", "true",
	"if", "else", "elif",
	"while", "for",
	"in"
};

inline bool is_kw(const std::string & str){
	return std::find(keywords.begin(), keywords.end(), str) != keywords.end();
}

struct Token {
	TokenType type;
	std::string val;

	// Trace
	uint32_t line = 0;
	uint32_t column = 0;

	// Errors
	void error(const std::string & msg){
		err(msg + " " + val + " type of " + token_type_to_str(type), line, column);
	}
	void unexpected_error(){
		error("Unexpected token");
	}
};

#endif