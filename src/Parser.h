#ifndef PARSER_H
#define PARSER_H

#include "object.h"

class Parser {
	public:
		Parser();
		virtual ~Parser() = default;

	private:
		Tree tree;

		unsigned int index;
		Node * peek();
		Node * advance();

		ObjectBuffer objects;
		Tree parse(ObjectBuffer & objects);

		Node * interpret_object(const Object & object);

		// Parsers
		Node * parse_expression(unsigned int prec);

};

#endif