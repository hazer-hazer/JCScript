#include "Parser.h"

Parser::Parser(){
	objects.clear();
}

Node * Parser::interpret_object(const Object & object){
	Node * n = new Node(object);
	return n;
}

Node * Parser::peek(){
	return interpret_object(object[index]);
}

Node * Parser::advance(){
	index++;
	return peek();
}

Node * Parser::parse_expression(unsigned int prec){
	switch(peek()->object.type){
		case NUMBER:
		case STRING:{
			return Node(peek());
			break;
		}
		case IDENTIFIER:{
			switch
			if(advance()->object.type == PAREN_L){
				// Parse function call
			}else
		}
	}
	while(peek()->prec){

	}
	return node;
}

void Parser::parse(ObjectBuffer objects){
	this->objects = objects;

	while(peek().type != PROG_END){
		tree.push_back( parse_expression(0) );
	}
}