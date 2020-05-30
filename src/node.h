#ifndef NODE_H
#define NODE_H

#include <vector>

#include "Token.h"
#include "Value.h"
#include "Scope.h"

struct NStatement;
struct NExpression;
struct NArgDecl;
struct NBlock;
struct NVarDecl;
struct NFuncDecl;
struct NTypeDecl;
typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NArgDecl*> ArgList;
typedef std::pair<NExpression*, NBlock*> ConditionBlock;
typedef std::vector<NVarDecl*> VarList;
typedef std::vector<NFuncDecl*> FuncList;
typedef std::vector<NTypeDecl*> TypeList;

////////////////
// Base nodes //
////////////////

struct Node {
	Node(){}
	virtual ~Node() = default;

	virtual Value * eval(const Scope & scope) {}

	virtual std::string to_string(){
		return "[NODE]";
	}
};

struct NExpression : Node {
	virtual std::string to_string() override {
		return "[NExpression]";
	}
};

inline std::string expression_list_to_string(const ExpressionList & expression_list){
	std::string str;

	for(NExpression * e : expression_list){
		str += e->to_string() + '\n';
	}

	return str;
}

struct NStatement : Node {
	virtual std::string to_string() override {
		return "[NStatement]";
	}
};

inline std::string statement_list_to_string(const StatementList & statements){
	std::string str;

	for(NStatement * s : statements){
		str += s->to_string() + '\n';
	}

	return str;
}

struct NExpressionStatement : NStatement {
	NExpression & expression;

	NExpressionStatement(NExpression & expr) : expression(expr) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NExpressionStatement]: " + expression.to_string();
	}
};

/////////////////
// Value nodes //
/////////////////

struct NInt : NExpression {
	int value;
	NInt(const int & value) : value(value) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NInt]: " + std::to_string(value);
	}
};

// Note: NFloat contains 64-bit precision number like double does
struct NFloat : NExpression {
	double value;
	NFloat(const double & value) : value(value) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NFloat]: " + std::to_string(value);
	}
};

struct NBool : NExpression {
	bool value;
	NBool(const bool & value) : value(value) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NBool]: " + std::to_string(value);
	}
};

struct NString : NExpression {
	std::string value;
	NString(const std::string & value) : value(value) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NString]: " + value;
	}
};

//////////////////
// Common nodes //
//////////////////

struct NIdentifier : NExpression {
	std::string name;

	NIdentifier(const std::string & name){
		this->name = name;
	}

	bool operator == (const NIdentifier & id1, const NIdentifier & id2){
		return id1.name == id2.name;
	}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NIdentifier]: " + name;
	}
};

struct NBlock : NExpression {
	StatementList statements;
	NBlock(){}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NBlock]: " + statement_list_to_string(statements);
	}
};

inline std::string condition_block_to_string(const ConditionBlock & cb){
	return cb.first->to_string() + " " + cb.second->to_string();
}

////////////////////
// Operator nodes //
////////////////////

struct NInfixOp : NExpression {
	NExpression & left;
	Operator op;
	NExpression & right;

	NInfixOp(NExpression & left, const Operator & op, NExpression & right)
		: left(left), op(op), right(right) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NInfixOp]: " + left.to_string() + " " + op_to_str(op) + " " + right.to_string();
	}
};

struct NPrefixOp : NExpression {
	Operator op;
	NExpression & right;

	NPrefixOp(const Operator & op, NExpression & right) : op(op), right(right) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NPrefixOp]: " + op_to_str(op) + " " + right.to_string();
	}
};

struct NPostfixOp : NExpression {
	NExpression & left;
	Operator op;

	NPostfixOp(NExpression & left, const Operator & op) : left(left), op(op) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NPostfixOp]: " + left.to_string() + " " + op_to_str(op);
	}
};

// NType contains special type syntax
// It can be array of primitives/user-defined types
// Or it can be list of allowed types ([string, int])
// TODO: Add special types like array
struct NType : NExpression {
	NIdentifier & id;
	bool nullable;

	NType(NIdentifier & id, bool & nullable) : id(id), nullable(nullable) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NType]: " + id.to_string() + (nullable ? "?" : "");
	}
};

// In type declaration there cannot be any case when it's declared but not defined
// So NTypeDecl always contain id of type and right-hand assignment expression
// that must be NType
struct NTypeDecl : NStatement {
	NIdentifier & id;
	NType & type;

	NTypeDecl(NIdentifier & id, NType & type) : id(id), type(type) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NTypeDecl]: " + id.to_string() + " = " + type.to_string();
	}
};

struct NVarDecl : NStatement {
	bool is_val;
	NIdentifier & id;
	NType * type;
	NExpression * assignment_expr;

	// Note: Type automatically set to `any` by default in Parser
	NVarDecl(const bool & is_val,
			 NIdentifier & id,
			 NType & type,
			 NExpression * assignment_expr)
			: is_val(is_val), id(id), type(type), assignment_expr(assignment_expr) {}


	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return std::string("[NVarDecl]: ") +
				(is_val ? "val" : "var") + " " + id.to_string() +
				(type != nullptr ? ": " + type->to_string() : "") +
				(assignment_expr != nullptr ? " = " + assignment_expr->to_string() : "");
	}
};

inline std::string var_list_to_string(const VarList & var_list){
	std::string str;

	for(NVarDecl * v : var_list){
		str += v->to_string() + '\n';
	}

	return str;
}

struct NArgDecl : NStatement {
	NIdentifier & id;
	NType * type;
	NExpression * default_value;

	NArgDecl(NIdentifier & id, NType * type, NExpression * default_value)
			: id(id), type(type), default_value(default_value) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NArgDecl]: " + id.to_string() + 
				(type != nullptr ? ": " + type->to_string() : "") +
				(default_value != nullptr ? " = " + default_value->to_string() : "");
	}
};

inline std::string arg_list_to_string(const ArgList & arg_list){
	std::string str;
	for(NArgDecl * a : arg_list){
		str += a->to_string() + ", ";
	}
	return str;
}

struct NFuncCall : NExpression {
	NExpression & left;
	ExpressionList args;

	NFuncCall(NExpression & left, const ExpressionList & args)
			: left(left), args(args) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NFuncCall]: " + left.to_string() + "(" + expression_list_to_string(args) + ")";
	}
};

struct NFuncDecl : NStatement {
	NIdentifier & id;
	ArgList args;
	NType * return_type;
	NBlock & block;

	NFuncDecl(NIdentifier & id,
			  const ArgList & args,
			  NType * return_type,
			  NBlock & block)
			: id(id), args(args), return_type(return_type), block(block) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string() override {
		return "[NFuncCall]: " + id.to_string() + "(" + arg_list_to_string(args) + ")" +
				(return_type != nullptr ? ": " + return_type->to_string() : "") +
				"{\n" + block.to_string() + "\n}";
	}
};
	
// Note: !Important! `if` is an expression
// and can be used as statement by `NExpressionStatement`
struct NCondition : NExpression {
	ConditionBlock If;
	std::vector <ConditionBlock> Elifs;
	NBlock * Else;

	NCondition(ConditionBlock & If,
			   const std::vector <ConditionBlock> & Elifs,
			   NBlock * Else)
			: If(If), Elifs(Elifs), Else(Else) {}

	virtual Value * eval(const Scope & scope);

	virtual std::string to_string(){
		std::string elifs_str;
		for(const ConditionBlock & cb : Elifs){
			elifs_str += condition_block_to_string(cb) + '\n';
		}
		return "[NCondition]: " + condition_block_to_string(If) + " " +
				elifs_str + " " + (Else != nullptr ? Else->to_string() : "");
	}
};

struct NWhile : NStatement {
	NExpression & condition;
	NBlock & block;

	NWhile(NExpression & condition, NBlock & block)
		: condition(condition), block(block) {}

	virtual Value * eval(const Scope & scope);
};

struct NFor : NStatement {
	NIdentifier & For;
	NExpression & In;

	NFor(NIdentifier & For, NExpression & In) : For(For), In(In) {}
	
	virtual Value * eval(const Scope & scope);
};

#endif