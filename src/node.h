#ifndef NODE_H
#define NODE_H

#include <vector>

#include "Token.h"
#include "Object.h"

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

	// For errors in Node, sometimes position is the position of the last Token in statement or expression
	Position pos;

	virtual void error(const std::string msg){
		err(msg + " at " + pos.line + ":" + pos.column);
	}

	virtual std::string to_string(){
		return "[NODE]";
	}

	virtual Object * eval(Scope * scope);
};

struct NExpression : Node {
	virtual std::string to_string() override {
		return "[NExpression]";
	}

	virtual Object * eval(Scope * scope) override;
};

inline std::string expression_list_to_string(const ExpressionList & expression_list, const std::string & sep){
	std::string str;

	for(int i = 0; i < expression_list.size(); i++){
		str += expression_list[i]->to_string();
		if(i != expression_list.size() - 1){
			str += sep;
		}	
	}

	return str;
}

struct NStatement : Node {
	virtual std::string to_string() override {
		return "[NStatement]";
	}

	virtual Object * eval(Scope * scope) override;
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

	NExpressionStatement(NExpression & expr, const Position & pos)
						: expression(expr), pos(pos) {}

	virtual std::string to_string() override {
		return expression.to_string();
	}
	
	virtual Object * eval(Scope * scope) override;
};

/////////////////
// Value nodes //
/////////////////

struct NInt : NExpression {
	int value;
	NInt(const int & value, const Position & pos) : value(value), pos(pos) {}

	virtual std::string to_string() override {
		return std::to_string(value);
	}
	
	virtual Object * eval(Scope * scope) override;
};

// Note: NFloat contains 64-bit precision number like double does
struct NFloat : NExpression {
	double value;
	NFloat(const double & value, const Position & pos) : value(value), pos(pos) {}

	virtual std::string to_string() override {
		return std::to_string(value);
	}
	
	virtual Object * eval(Scope * scope) override;
};

struct NBool : NExpression {
	bool value;
	NBool(const bool & value, const Position & pos) : value(value), pos(pos) {}

	virtual std::string to_string() override {
		return (value ? "true" : "false");
	}
	
	virtual Object * eval(Scope * scope) override;
};

struct NString : NExpression {
	std::string value;
	NString(const std::string & value, const Position & pos) : value(value), pos(pos) {}

	virtual std::string to_string() override {
		return "'" + value + "'";
	}
	
	virtual Object * eval(Scope * scope) override;
};

//////////////////
// Common nodes //
//////////////////

struct NIdentifier : NExpression {
	std::string name;

	NIdentifier(const std::string & name, const Position & pos){
		this->name = name;
	}

	virtual std::string to_string() override {
		return "[NIdentifier] " + name;
	}

	virtual bool compare(const NIdentifier & id){
		return name == id.name;
	}
	
	virtual Object * eval(Scope * scope) override;
};

struct NBlock : NExpression {
	StatementList statements;

	NBlock(const Position & pos) : pos(pos) {}

	virtual std::string to_string() override {
		return "[NBlock] " + statement_list_to_string(statements);
	}
	
	virtual Object * eval(Scope * scope) override;
};

////////////////////
// Operator nodes //
////////////////////

struct NInfixOp : NExpression {
	NExpression & left;
	Operator op;
	NExpression & right;

	NInfixOp(NExpression & left, const Operator & op, NExpression & right, const Position & pos)
			: left(left), op(op), right(right), pos(pos) {}

	virtual std::string to_string() override {
		return "[NInfixOp] " + left.to_string() + " " + op_to_str(op) + " " + right.to_string();
	}
	
	virtual Object * eval(Scope * scope) override;
};

struct NPrefixOp : NExpression {
	Operator op;
	NExpression & right;

	NPrefixOp(const Operator & op, NExpression & right, const Position & pos)
			 : op(op), right(right), pos(pos) {}

	virtual std::string to_string() override {
		return "[NPrefixOp] " + op_to_str(op) + " " + right.to_string();
	}
	
	virtual Object * eval(Scope * scope) override;
};

struct NPostfixOp : NExpression {
	NExpression & left;
	Operator op;

	NPostfixOp(NExpression & left, const Operator & op, const Position & pos)
			  : left(left), op(op), pos(pos) {}

	virtual std::string to_string() override {
		return "[NPostfixOp] " + left.to_string() + " " + op_to_str(op);
	}
	
	virtual Object * eval(Scope * scope) override;
};

// NType contains special type syntax
// It can be array of primitives/user-defined types
// Or it can be list of allowed types ([string, int])
// TODO: Add special types like array
struct NType : NExpression {
	bool nullable = false;

	NType(const Position & pos) : pos(pos) {}

	virtual std::string to_string() override {
		return "[NType]";
	}

	virtual bool compare(NType * type) {
		return type->nullable == nullable;
	}
	
	virtual Object * eval(Scope * scope) override;
};

struct NIdentifierType : NType {
	NIdentifier & id;

	NIdentifierType(NIdentifier & id, const Position & pos) : id(id), pos(pos) {}

	virtual std::string to_string() override {
		return id.to_string() + (nullable ? "?" : "");
	}

	virtual bool compare(NType * type) override {
		NIdentifierType * id_type = dynamic_cast<NIdentifierType*>(type);
		return id_type && NType::compare(type) && id.compare(id_type->id);
	}
	
	virtual Object * eval(Scope * scope) override;
};

struct NListType : NType {
	NType & wrapped_type;
	
	NListType(NType & wrapped_type, const Position & pos)
			 : wrapped_type(wrapped_type), pos(pos) {}

	virtual std::string to_string() override {
		return "[" + wrapped_type.to_string() + "]" + (nullable ? "?" : "");
	}

	virtual bool compare(NType * type) override {
		NListType * list_type = dynamic_cast<NListType*>(type);
		return list_type && NType::compare(type) && wrapped_type.compare(&list_type->wrapped_type);
	}
	
	virtual Object * eval(Scope * scope) override;
};

struct NTupleType : NType {
	std::vector <NType*> types;

	// TODO: Add default values

	NTupleType(const std::vector <NType*> types, const Position & pos)
			  : types(types), pos(pos) {}

	virtual std::string to_string() override {
		std::string str = "(";
		for(int i = 0; i < types.size(); i++){
			str += types[i]->to_string();
			if(i < types.size() - 1){
				str += ", ";
			}
		}
		return str + ")";
	}

	virtual bool compare(NType * type) override {
		NTupleType * tuple_type = dynamic_cast<NTupleType*>(type);

		bool types_comparison = true;
		if(types.size() != tuple_type->types.size()){
			types_comparison = false;
		}else{
			for(int i = 0; i < types.size(); i++){
				if(types[i]->compare(tuple_type->types[i])){
					types_comparison = false;
					break;
				}
			}
		}

		return tuple_type && NType::compare(type) && types_comparison; 
	}
	
	virtual Object * eval(Scope * scope) override;
};

// In type declaration there cannot be any case when it's declared but not defined
// So NTypeDecl always contain id of type and right-hand assignment expression
// that must be NType
struct NTypeDecl : NStatement {
	NIdentifier & id;
	NType & type;

	NTypeDecl(NIdentifier & id, NType & type, const Position & pos)
			 : id(id), type(type), pos(pos) {}

	virtual std::string to_string() override {
		return "type "+ id.to_string() +" = "+ type.to_string();
	}
	
	virtual Object * eval(Scope * scope) override;
};

struct NVarDecl : NStatement {
	bool is_val;
	NIdentifier & id;
	NType * type;
	NExpression * assignment_expr;

	// Note: Type automatically set to `any` by default in Parser
	NVarDecl(const bool & is_val,
			 NIdentifier & id,
			 NType * type,
			 NExpression * assignment_expr,
			 const Position & pos)
			: is_val(is_val), id(id), type(type),
			  assignment_expr(assignment_expr), pos(pos) {}


	virtual std::string to_string() override {
		return std::string(is_val ? "val" : "var") + " " + id.to_string() + ": " +
			   (type ? type->to_string() : "any") +
			   (assignment_expr ? " = " + assignment_expr->to_string() : "");
	}
	
	virtual Object * eval(Scope * scope) override;
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

	NArgDecl(NIdentifier & id, NType * type, NExpression * default_value, const Position & pos)
			: id(id), type(type), default_value(default_value), pos(pos) {}

	virtual std::string to_string() override {
		return id.to_string() + 
			   (type != nullptr ? ": " + type->to_string() : "") +
			   (default_value != nullptr ? " = " + default_value->to_string() : "");
	}
	
	virtual Object * eval(Scope * scope) override;
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

	NFuncCall(NExpression & left, const ExpressionList & args, const Position & pos)
			: left(left), args(args), pos(pos) {}

	virtual std::string to_string() override {
		return "[NFuncCall] " + left.to_string() + "(" + expression_list_to_string(args, ", ") + ")";
	}
	
	virtual Object * eval(Scope * scope) override;
};

struct NReturn : NStatement {
	NExpression * right;

	NReturn(NExpression * right, const Position & pos) : right(right), pos(pos) {}

	virtual std::string to_string() override {
		return "return " + (right ? right->to_string() : "");
	}

	virtual Object * eval(Scope * scope) override;
};

struct NFuncDecl : NStatement {
	NIdentifier & id;
	ArgList args;
	NType * return_type;
	NBlock & block;

	NFuncDecl(NIdentifier & id,
			  const ArgList & args,
			  NType * return_type,
			  NBlock & block,
			  const Position & pos)
			: id(id), args(args), return_type(return_type), block(block), pos(pos) {}

	virtual std::string to_string() override {
		return  "func " + id.to_string() + "(" + arg_list_to_string(args) + ")" +
				(return_type != nullptr ? ": " + return_type->to_string() : "") +
				"{\n" + block.to_string() + "\n}";
	}
	
	virtual Object * eval(Scope * scope) override;
};


inline std::string condition_block_to_string(const std::string & cond_name, const ConditionBlock & cb){
	return cond_name +"("+ cb.first->to_string() +"){\n"+ cb.second->to_string() + "\n}";
}

struct NListAccess : NExpression {
	NExpression & left;
	NExpression & access;

	NListAccess(NExpression & left, NExpression & access, const Position & pos)
			   : left(left), access(access), pos(pos) {}


	virtual std::string to_string() override {
		return "[NListAccess] " + left.to_string() + "[" + access.to_string() + "]";
	}
	
	virtual Object * eval(Scope * scope) override;
};

struct NList : NExpression {
	ExpressionList expressions;

	NList(const ExpressionList & expressions, const Position & pos)
		 : expressions(expressions), pos(pos) {}

	virtual std::string to_string() override {
		std::string str = "[";
		for(int i = 0; i < expressions.size(); i++){
			str += expressions[i]->to_string();
			if(i < expressions.size() - 1){
				str += ", ";
			}
		}
		return str + "]";
	}
	
	virtual Object * eval(Scope * scope) override;
};

// Note: !Important! `if` is an expression
// and can be used as statement by `NExpressionStatement`
struct NCondition : NExpression {
	ConditionBlock If;
	std::vector <ConditionBlock> Elifs;
	NBlock * Else;

	NCondition(ConditionBlock & If,
			   const std::vector <ConditionBlock> & Elifs,
			   NBlock * Else,
			   const Position & pos)
			  : If(If), Elifs(Elifs), Else(Else), pos(pos) {}

	virtual std::string to_string() override {
		std::string elifs_str;
		for(const ConditionBlock & cb : Elifs){
			elifs_str += condition_block_to_string("elif", cb) + '\n';
		}
		return condition_block_to_string("if", If) +"\n"+
			   elifs_str + "else{" + (Else != nullptr ? Else->to_string() : "") + "}";
	}
	
	virtual Object * eval(Scope * scope) override;
};

struct NWhile : NStatement {
	NExpression & condition;
	NBlock & block;

	NWhile(NExpression & condition, NBlock & block, const Position & pos)
		  : condition(condition), block(block), pos(pos) {}

	virtual std::string to_string() override {
		return "while("+ condition.to_string() +"){\n"+ block.to_string() +"\n}";
	}
	
	virtual Object * eval(Scope * scope) override;
};

// TODO: Think about for `For`
// For must be any expression like (key, val) or val and etc.
struct NFor : NStatement {
	NIdentifier & For;
	NExpression & In;
	NBlock & block;

	NFor(NIdentifier & For, NExpression & In, NBlock & block, const Position & pos)
		: For(For), In(In), block(block), pos(pos) {}

	virtual std::string to_string() override {
		return "for("+ For.to_string() +" in "+ In.to_string() +"){\n"+ block.to_string() +"\n}";
	}
	
	virtual Object * eval(Scope * scope) override;
};

typedef std::pair<ExpressionList, NBlock*> MatchCase;

struct NMatch : NStatement {
	NExpression & expression;
	std::vector <MatchCase> Cases;
	NBlock * Else;

	NMatch(NExpression & expression,
		   const std::vector <MatchCase> Cases,
		   NBlock * Else,
		   const Position & pos)
		 : expression(expression), Cases(Cases), Else(Else), pos(pos) {}

	virtual std::string to_string() override {
		std::string cases_string;
		for(const auto & Case : Cases){
			cases_string += expression_list_to_string(Case.first, ", ") +" => "+ Case.second->to_string() +"";
		}
		return "match("+ expression.to_string() +"){\n" + cases_string + "else => " + (Else ? Else->to_string() : "") +"}";
	}
	
	virtual Object * eval(Scope * scope) override;
};

#endif // NODE_H