#ifndef NODE_H
#define NODE_H

#include <sstream>
#include <string>
#include <iostream>
#include <variant>

#include <iomanip>
#include "Token.h"

struct Node;
// Tree is just an array of `Node`
// It can be used anythere, mostly to store list of expressions, argument list and etc.
typedef std::vector <Node*> Tree;

/**
 * Note:
 * No so long ago, I thought that idea of using operators and keywords as enums is good.
 * But I realised that I will need to convert strings to enums and back.
 * As I read, C++ std::string `operator==` is optimized enough to use it instead, so...
 */

// TODO: Change NodeVal string type from std::string to char * to improve performance
// TODO: Replace variant with union after performance tests

typedef float Real;

// union NodeVal {
// 	Real r;
// 	std::string s;

// 	NodeVal(){
// 		// r = 0;
// 	}
// 	~NodeVal(){}
// };

struct Node {
private:
	// NodeVal val;
	std::variant <Real, std::string> val{0.0};

public:

	TokenType type = T_NULL;

	uint32_t line = 0;
	uint32_t column = 0;

	void interpret(const Token & token){
		type = token.type;

		line = token.line;
		column = token.column;

		switch(token.type){
			case T_NUM:{
				// TODO: Add parsing errors catching
				val = std::stof(token.val);
				break;
			}
			case T_KW:{
				// Check for bool and store it in Real
				// If I'm right it won't increase complexity
				if(token.val == "true" || token.val == "false"){
					val = (Real)(token.val == "true");
				}else{
					val = token.val;
				}
				break;
			}
			default:{
				val = token.val;
			}
		}
	}

	// Value getters
	std::string str(){
		return std::get<std::string>(val);
	}

	Real real(){
		return std::get<Real>(val);
	}

	virtual void print(){
		std::cout << "[Node]: type: `" + token_type_to_str(type) + "`";
		std::string str_val;
		if(type == T_NULL || type == T_PROG_END || type == T_ENDL){
			str_val = "";
		}else if(type == T_NUM){
			str_val = "`" + std::to_string(real()) + "`";
		}else{
			str_val = "`" + str() + "`";
		}
		std::cout << " val: " + str_val;
	}
};

// For debug
inline void print_tree(const Tree & tree){
	for(Node * n : tree){
		n->print();
		std::cout << "\n";
	}
}

// TODO: Think about `type` type
// `type` type is used to define user-defined types
// syntax: `type my_type = string` for example

// Node that stores Variable declaration (not a value)
// Note: It doesn't store right-hand assignment
struct VarNode : Node {
	std::string var_type;
	std::string var_name;

	bool is_val;

	VarNode(){}
	VarNode(const std::string & var_type,
			const std::string & var_name,
			const bool & is_val = false)
	: Node() {
		this->var_type = var_type;
		this->var_name = var_name;
		this->is_val = is_val;
	}

	void print() override {
		std::cout << "[VarNode]: var_type: `" + var_type +
					 "` var_name: `" + var_name +
					 "` is_val: `" + std::to_string(is_val) + "`";
	}
};

struct ScopeNode : Node {
	Tree expressions;

	ScopeNode(){}
	ScopeNode(const Tree & expressions) : Node() {
		this->expressions = expressions;
	}

	void print() override {
		std::cout << "[ScopeNode]:\n";
		print_tree(expressions);
	}
};

// Node that stores function (`func`) declaration
struct FuncNode : Node {
	std::string func_name;
	Tree args;
	ScopeNode * body = nullptr;

	FuncNode(){}
	FuncNode(const std::string & func_name,
			 const Tree & args,
			 ScopeNode * body)
	: Node() {
		this->func_name = func_name;
		this->args = args;
		this->body = body;
	}

	void print() override {
		std::cout << "[FuncNode]: func_name: `" + func_name + "`\nargs: \n";
		print_tree(args);
		std::cout << "\nbody:\n";
		body->print();
	}
};

struct ArrayNode : Node {
	Tree array;

	ArrayNode(){}
	ArrayNode(const Tree & array) : Node() {
		this->array = array;
	}

	void print() override {
		std::cout << "[ArrayNode]: array:\n";
		print_tree(array);
	}
};

typedef std::pair <Node*, ScopeNode*> Condition;

// For debug
inline void print_condition(const Condition & condition){
	if(!condition.first || !condition.second){
		err("Unable to print condition, condition Node or ScopeNode is undefined",
			condition.first->line, condition.first->column);
	}
	condition.first->print();
	std::cout << "\n";
	condition.second->print();
}

struct ConditionNode : Node {
	Condition If;
	std::vector <Condition> Elifs;
	ScopeNode * Else;

	ConditionNode(){
		If = std::make_pair(nullptr, nullptr);
		Else = nullptr;
	}
	ConditionNode(const Condition & If,
				  const std::vector <Condition> & Elifs,
				  ScopeNode * Else)
	: Node() {
		this->If = If;
		this->Elifs = Elifs;
		this->Else = Else;
	}

	void print() override {
		std::cout << "[ConditionNode]: if: ";
		print_condition(If);
		std::cout << "\n";
		for(const Condition & c : Elifs){
			print_condition(c);
			std::cout << "\n";
		}
		if(Else){
			std::cout << "Else: ";
			Else->print();
		}
	}
};

struct WhileNode : Node {
	Node * condition = nullptr;
	ScopeNode * body = nullptr;

	WhileNode(){}
	WhileNode(Node * condition, ScopeNode * body) : Node() {
		this->condition = condition;
		this->body = body;
	}

	void print() override {
		std::cout << "[WhileNode]: Condition: ";
		condition->print();
		std::cout << "\nBody:";
		body->print();
	}
};

struct ForNode : Node {
	Node * For = nullptr;
	Node * In = nullptr;
	ScopeNode * body = nullptr;

	ForNode(){}
	ForNode(Node * For, Node * In, ScopeNode * body) : Node() {
		this->For = For;
		this->In = In;
		this->body = body;
	}

	void print() override {
		std::cout << "[ForNode]: For: ";
		For->print();
		std::cout << ", In: ";
		In->print();
		std::cout << "\nbody:\n";
		body->print();
	}
};

struct CallNode : Node {
	Node * func_name = nullptr;
	Tree args;

	CallNode(){}
	CallNode(Node * func_name, const Tree & args) : Node() {
		this->func_name = func_name;
		this->args = args;
	}

	void print() override {
		std::cout << "[CallNode]: func_name: ";
		func_name->print();
		std::cout << ", args: ";
		print_tree(args);
	}
};

struct BinNode : Node {
	std::string op;

	Node * left = nullptr;
	Node * right = nullptr;

	BinNode(){}
	BinNode(const std::string & op, Node * left, Node * right) : Node() {
		this->op = op;

		this->left = left;
		this->right = right;
	}

	void print() override {
		std::cout << "[BinNode]: left: ";
		left->print();
		std::cout << ", op: `" + op + "`, right: ";
		right->print();
	}
};

#endif