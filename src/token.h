#ifndef TOKEN_H
#define TOKEN_H

#include <vector>
#include <string>

struct Token {

	static const std::string operators[];
	static const std::string keywords[];

	enum Type {
		NUMBER,
		OPERATOR,
		IDENTIFIER,
		KEYWORD,
		STRING
	};

	Type type;
	std::string value;

	unsigned int line;
	unsigned int column;
};

#endif