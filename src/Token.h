#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
#include <algorithm>
#include <variant>

#include <iostream>

#include "err.h"

// TODO: Think about null-safety

enum TokenType {
	T_NUM, // Number
	T_STR, // String
	T_ID, // Identifier
	T_KW, // Keyword - ID before
	T_OP, // Operator,
	T_ENDL,
	T_PROG_END
};

enum Operator {
	OP_ASSIGN,
	OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_EXP,
	OP_ASSIGN_ADD, OP_ASSIGN_SUB, OP_ASSIGN_MUL, OP_ASSIGN_DIV, OP_ASSIGN_MOD, OP_ASSIGN_EXP,
	
	OP_BIT_AND, OP_BIT_OR, OP_BIT_XOR, OP_BIT_INVERT,
	OP_ASSIGN_BIT_AND, OP_ASSIGN_BIT_OR, OP_ASSIGN_BIT_XOR,
	
	OP_INC, OP_DEC,

	OP_AND, OP_OR, OP_NOT,
	OP_EQUAL, OP_NOT_EQUAL,
	OP_LESS, OP_GREATER, OP_LESS_EQUAL, OP_GREATER_EQUAL, OP_SPACESHIP,

	OP_SHIFT_LEFT, OP_SHIFT_RIGHT,
	OP_ASSIGN_SHIFT_LEFT, OP_ASSIGN_SHIFT_RIGHT,

	OP_PIPELINE,

	OP_OPEN_COMMENT, OP_CLOSE_COMMENT,

	// Punctuactions
	// TODO: Think about creating separate type for puncts
	OP_PAREN_L, OP_PAREN_R,
	OP_BRACKET_L, OP_BRACKET_R,
	OP_BRACE_L, OP_BRACE_R,
	OP_COLON, OP_COMMA,
	OP_MEMBER_ACCESS, OP_RANGE, OP_SPREAD,
	OP_QUESTION_MARK, OP_NULL_COALESCING, OP_ELVIS,
	OP_SEMICOLON,

	// Start of string operators
	OP_START_OF_STRING_OPS,

	OP_IN, OP_NOT_IN,
	OP_AS, OP_AS_NULLABLE,

	OP_MAX
};

const static char * operator_names[OP_MAX] = {
	"=",
	"+", "-", "*", "/", "%", "**",
	"+=", "-=", "*=", "/=", "%=", "**=",
	"&", "|", "^", "~",
	"&=", "|=", "^=",
	"++", "--",
	"&&", "||", "!",
	"==", "!=",
	"<", ">", "<=", ">=", "<=>",
	"<<", ">>",
	"<<=", ">>=",
	"|>",
	"/*", "*/",
	"(", ")",
	"[", "]",
	"{", "}",
	":", ",",
	".", "..", "...",
	"?", "??", "?:"
	";",

	// String operators
	"[String Operators]",

	"in", "!in",
	"as", "as?"
};

inline Operator str_operator(const std::string & str){
	return static_cast<Operator>(std::distance(operator_names, std::find(std::begin(operator_names) + OP_START_OF_STRING_OPS, std::end(operator_names), str.c_str())));
}

enum Keyword {
	KW_VAR, KW_VAL, KW_FUNC,
	KW_IF, KW_ELIF, KW_ELSE,
	KW_FOR, KW_WHILE, KW_REPEAT,
	KW_IN,
	KW_MAX
};

const std::vector <std::string> keywords {
	"var", "val", "func",
	"if", "elif", "else",
	"for", "while", "repeat",
	"in"
};

inline Keyword str_to_kw(const std::string & str){
	return static_cast<Keyword>(std::distance(keywords.begin(), std::find(keywords.begin(), keywords.end(), str)));
}
inline bool is_kw(const std::string & str){
	return str_to_kw(str) < KW_MAX;
}

inline std::string kw_to_str(const Keyword & kw){
	return keywords[static_cast<int>(kw)];
}
inline std::string op_to_str(const Operator & op){
	return operator_names[static_cast<int>(op)];
}

// Typedefs here are used to simplify changing value types for Tokens and Nodes
// For example, in the future std::string will be replaced with std::wstring
typedef float Real;
typedef std::string String;

// TODO: Add more number type: int, uint, short, double and etc.
// TODO: Think about std::string vs char*
// Note: As I tested, char* won't work properly in variant, because of setting by ref
typedef std::variant<Real, String, Operator, Keyword> TokenVal;

struct Token {
	TokenType type;
	TokenVal val;

	// Trace
	uint32_t line = 0;
	uint32_t column = 0;

	Token(const TokenType & t, const std::string & v){
		type = t;

		switch(t){
			case T_ENDL:
			case T_PROG_END:{
				val = 0.0;
				break;
			}
			case T_NUM:{
				// TODO: Add parsing errors catching
				val = std::stof(v);
				break;
			}
			case T_STR:
			case T_ID:{
				val = v;
				break;
			}
			case T_KW:{
				// Check for bool and store it in Real
				// If I'm right it won't increase complexity
				if(v == "true" || v == "false"){
					type = T_NUM;
					val = static_cast<Real>(v == "true");
				}else{
					val = str_to_kw(v);
				}
				break;
			}
		}
	}
	// Setting operator is differert
	Token(const Operator & op){
		type = T_OP;
		val = op;
	}

	Real real(){
		return std::get<Real>(val);
	}
	String str(){
		return std::get<String>(val);
	}
	Operator op(){
		return std::get<Operator>(val);
	}
	Keyword kw(){
		return std::get<Keyword>(val);
	}

	// Errors
	void error(const std::string & msg){
		err(msg + " " + to_string(), line, column);
	}
	void unexpected_error(){
		error("Unexpected token");
	}

	std::string to_string(const bool & with_pos = false){
		std::string token_str;
		switch (type) {
			case T_NUM: token_str = "number"; break;
			case T_STR: token_str = "string"; break;
			case T_ID: token_str = "identifier"; break;
			case T_KW: token_str = "keyword"; break;
			case T_OP: token_str = "operator"; break;
			case T_ENDL: token_str = "end of line"; break;
			case T_PROG_END: token_str = "end of program"; break;
			default: token_str = "Unrecognized token type";
		}
		token_str += " `";
		switch (type) {
			case T_NUM:
				token_str += std::to_string(real());
				break;
			case T_STR:
			case T_ID:
				token_str += str();
				break;
			case T_KW:
				token_str += kw_to_str(kw());
				break;
			case T_OP:
				token_str += op_to_str(op());
				break;
		}
		token_str += "`";

		if(with_pos){
			token_str += " at " + std::to_string(line) + ":" + std::to_string(column);
		}
		// TODO: Add op and kw to string

		return token_str;
	}
};

#endif