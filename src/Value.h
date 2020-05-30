#ifndef VALUE_H
#define VALUE_H

#include "Node.h"

struct Value {
	Node & node;

	Value(Node & node) : node(node) {}

	virtual void error(const std::string & msg){
		node.error(msg);
	}
};

/////////////////////
// Constant values //
/////////////////////

struct Int : Value {
	Int(NInt & node) : Value(node) {}
};

struct Float : Value {
	Float(NFloat & node) : Value(node) {}
};

struct Bool : Value {
	Bool(NBool & node) : Value(node) {}
};

struct String : Value {
	String(NString & node) : Value(node) {}
};

///////////
// Types //
///////////

struct Type : Value {
	Type(NType & node) : Value(node) {}
};

struct Variable : Value {
	
};

#endif