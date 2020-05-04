#ifndef NODE_H
#define NODE_H

#include <vector>
#include "object.h"

struct Node {
	Object object;
	unsigned int prec;
	
	Node(const Object & object){
		this->object = object;
	}
};

typedef std::vector <Node*> Tree;

#endif