#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <string>
#include <sstream>

/**
 * Object Type
 */
typedef enum {
	NUMBER,
	OPERATOR,
	IDENTIFIER,
	KEYWORD,
	STRING,

	PROG_END

} ObjectType;

/**
 * Operators
 */
typedef enum {
	PLUS, MINUS, MULT, DIV, MOD,
	ASSIGN,
	PAREN_L, PAREN_R
} Operator;

const std::string operators[] = {
	"+", "-", "*", "/", "%",
	"=",
	"(", ")"
};

inline Operator str_to_op(const std::string & str){
	for(int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++){
		if(operators[i] == str){
			return static_cast<Operator>(i);
		}
	}
	throw "Cannot recohize operator " + str;
}

inline bool is_operator(const std::string & str){
	for(int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++){
		if(operators[i] == str){
			return true;
		}
	}
	return false;
}

/**
 * Keywords
 */
typedef enum {
	VAR
} Keyword;

const std::string keywords[] = {
	"var"
};

inline Keyword str_to_kw(const std::string & str){
	for(int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++){
		if(keywords[i] == str){
			return static_cast<Keyword>(i);
		}
	}
	throw "Cannot recohize keyword " + str;
}

inline bool is_keyword(const std::string & str){
	for(int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++){
		if(keywords[i] == str){
			return true;
		}
	}
	return false;
}

/**
 * Object Value
 */

// Real number (float)
typedef float Real;

// String 
typedef const char * String;

typedef union {
	Real real;
	String str;
	Operator op;
	Keyword kw;
} ObjectValue;

/**
 * Object
 */

inline void object_error(const std::string & message, unsigned int line, unsigned int column){
	std::stringstream mes;
	mes << "\e[0;31m" << message << " at " << line << ":" << column;
	throw mes.str();
}

typedef struct {
	ObjectType type;
	ObjectValue val;

	unsigned int line;
	unsigned int column;

	void set_val(const char * val){
		switch(type){
			case NUMBER:{
				this->val.real = std::stof(val);
				break;
			}
			case OPERATOR:{
				this->val.op = str_to_op(val);
				break;
			}
			case IDENTIFIER:{
				if(is_keyword(val)){
					type = KEYWORD;
					this->val.kw = str_to_kw(val);
				}else{
					this->val.str = val;
				}
				break;
			}
			case STRING:{
				this->val.str = val;
				break;
			}
		}
	};

} Object;

typedef std::vector <Object> ObjectBuffer;

#endif