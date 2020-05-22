#ifndef NODE_H
#define NODE_H

#include <vector>

#include "Token.h"

struct NStatement;
struct NExpression;
struct NVarDecl;
struct NArgDecl;
struct NBlock;
typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVarDecl*> VarList;
typedef std::vector<NArgDecl*> ArgList;
typedef std::pair<NExpression*, NBlock*> ConditionBlock;

struct Node {
	Node(){}
	virtual ~Node() = default;
};

struct NExpression : Node {};

struct NStatement : Node {};

struct NExpressionStatement : NStatement {
	NExpression & expression;

	NExpressionStatement(NExpression & expr) : expression(expr) {}
};

// Value nodes

struct NFloat : NExpression {
	float value;
	NFloat(const float & value) : value(value) {}
};

struct NString : NExpression {
	std::string value;
	NString(const std::string & value) : value(value) {}
};

// 

struct NIdentifier : NExpression {
	std::string name;

	NIdentifier(const std::string & name){
		this->name = name;
	}
};

struct NBlock : NExpression {
	StatementList statements;
	NBlock(){}
};

// Operator nodes
struct NBinOp : NExpression {
	const Operator op;
	NExpression & left;
	NExpression & right;
	NBinOp(NExpression & left, const Operator & op, NExpression & right)
		: left(left), op(op), right(right) {}
};

// NType contains special type syntax
// It can be array of primitives/user-defined types
// Or it can be list of allowed types ([string, int])
// TODO: Add special types like array
struct NType : NExpression {
	const NIdentifier & name;
	const bool nullable;

	NType(const NIdentifier & name, const bool & nullable) : name(name), nullable(nullable) {}
};

// In type declaration there cannot be case when it's declared but not assigned
// So NTypeDecl always contain name of type and right-hand assignment expression
// that must be NType
struct NTypeDecl : NStatement {
	const NIdentifier & name;
	const NType & type;

	NTypeDecl(const NIdentifier & name, const NType & type) : name(name), type(type) {}
};

struct NVarDecl : NStatement {
	const bool is_val;
	const NIdentifier & id;
	NType * type;
	NExpression * assignment_expr;

	// TODO: Add default `any` type

	// NVarDecl(const bool & is_val,
	// 		 const NIdentifier & id)
	// 		: is_val(is_val), id(id) {}

	// NVarDecl(const bool & is_val,
	// 		 const NIdentifier & id,
	// 		 NType * type)
	// 		: is_val(is_val), id(id), type(type) {}

	NVarDecl(const bool & is_val,
			 const NIdentifier & id,
			 NType * type,
			 NExpression * assignment_expr)
			: is_val(is_val), id(id), type(type), assignment_expr(assignment_expr) {}
};

struct NFuncCall : NExpression {
	const NExpression & left;
	ExpressionList args;

	NFuncCall(const NExpression & left, const ExpressionList & args)
			: left(left), args(args) {}
};

struct NArgDecl : NExpression {
	const NIdentifier & id;
	NType * type;
	NExpression * default_value;

	NArgDecl(const NIdentifier & id, NType * type, NExpression * default_value)
			: id(id), type(type), default_value(default_value) {}
};

struct NFuncDecl : NStatement {
	const NIdentifier & id;
	ArgList args;
	NType * return_type;
	NBlock & block;

	NFuncDecl(const NIdentifier & id,
			  const ArgList & args,
			  NType * return_type,
			  NBlock & block)
			: id(id), args(args), return_type(return_type), block(block) {}
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
};

struct NWhile : NStatement {
	NExpression & condition;
	NBlock & block;

	NWhile(NExpression & condition, NBlock & block)
		: condition(condition), block(block) {}
};

struct NFor : NStatement {
	NIdentifier & For;
	NExpression & In;

	NFor(NIdentifier & For, NExpression & In) : For(For), In(In) {}
};

#endif