#include "Node.h"

Object * Node::eval(Scope * scope) {
	return nullptr;
}

Object * NExpression::eval(Scope * scope) {
	return nullptr;
}

Object * NStatement::eval(Scope * scope) {
	return nullptr;
}

Object * NExpressionStatement::eval(Scope * scope) {
	return expression.eval();
}

/////////////////
// Value nodes //
/////////////////

Object * NInt::eval(Scope * scope) {
	return new Int(*this);
}

Object * NFloat::eval(Scope * scope) {
	return new Float(*this);
}

Object * NBool::eval(Scope * scope) {
	return new Bool(*this);
}

Object * NString::eval(Scope * scope) {
	return new String(*this);
}

//////////////////
// Common nodes //
//////////////////

Object * NIdentifier::eval(Scope * scope) {
	// Note: NIdentifier is general for types, functions and variables
	return scope->lookup(name);
}

Object * NBlock::eval(Scope * scope) {
	// TODO: Think about return statement and the value of block
	Object * last_stmt_value = nullptr;
	for(NStatement * stmt : statements){
		if(stmt){
			last_stmt_value = stmt->eval(scope);
		}
	}
	return last_stmt_value;
}

////////////////////
// Operator nodes //
////////////////////

// Note: Operators like &&, || are not overloadable
const std::map <Operator, std::string> infix_operator_functions {
	{OP_BIT_OR, "__or"},
	{OP_BIT_XOR, "__xor"},
	{OP_BIT_AND, "__and"},
	{OP_EQUAL, "__eq"},
	{OP_NOT_EQUAL, "__noteq"},
	{OP_LESS, "__lt"},
	{OP_LESS_EQUAL, "__le"},
	{OP_GREATER, "__gt"},
	{OP_GREATER_EQUAL, "__ge"},
	{OP_SPACESHIP, "__cmp"},
	{OP_SHIFT_LEFT, "__shl"},
	{OP_SHIFT_RIGHT, "__shr"},
	{OP_RANGE, "__rangeto"},
	{OP_RANGE_INCL, "__rangeto_incl"},
	{OP_ADD, "__add"},
	{OP_SUB, "__sub"},
	{OP_MUL, "__mul"},
	{OP_DIV, "__div"},
	{OP_MOD, "__mod"},
	{OP_EXP, "__pow"},
	{OP_IN, "__contains"}
};

const std::map <Operator, std::string> postfix_operator_functions {
	{OP_INC, "__inc"},
	{OP_DEC, "__dec"}
};

const std::map <Operator, std::string> prefix_operator_functions {
	{OP_INC, "__inc"},
	{OP_DEC, "__dec"},
	{OP_ADD, "__uplus"},
	{OP_SUB, "__uminus"},
	{OP_NOT, "__not"},
	{OP_BIT_INVERT, "__invert"}
};

Object * NInfixOp::eval(Scope * scope) {
	Object * lho = left.eval(scope);

	// Note: Augmented assignment operators work as: a += b -> a = a + b
	// and cannot be overloaded (automatically overload when augment operator overloaded)

	switch(op){
		case OP_ASSIGN:{
			return scope->set_var(right.eval(scope));
			break;
		}
		// TODO: Add other augmented assignment operators
		case OP_ASSIGN_ADD:{
			return lho = lho->call_method(infix_operator_functions.at(OP_ADD), right.eval(scope));
			break;
		}
		case OP_ELVIS:{
			if(lho){
				return lho;
			}else{
				return right.eval(scope);
			}
			break;
		}
		case OP_NOT_IN:{
			return lho->call_method()
			break;
		}
		default:{
			return lho->call_method(infix_operator_functions.at(op), right.eval(scope));
		}
	}
}

Object * NPrefixOp::eval(Scope * scope) {
	return right.eval(scope)->call_method(prefix_operator_functions.at(op));
}

Object * NPostfixOp::eval(Scope * scope) {
	return left.eval(scope)->call_method(postfix_operator_functions.at(op));
}

////////////////
// Type nodes //
////////////////

Object * NType::eval(Scope * scope) {
	// Note: There must be no case when is base NType Node used, so just return nullptr,
	// but maybe catch an error in the future on Parser level or on this level
	return nullptr;
}

Object * NIdentifierType::eval(Scope * scope) {
	// TODO: ??? WHATT ??? How to use it???
}

Object * NTypeDecl::eval(Scope * scope) {
	return scope->define_type(*this);
}


Object * NVarDecl::eval(Scope * scope) {
	// Note: Think about definitions and maybe it must be one define for everything
	return scope->define_var(*this);
}

Object * NFuncCall::eval(Scope * scope) {
	Func * func = dynamic_cast<Func*>(left.eval(scope));
	if(!func){
		// TODO: Add error catching `left cannot be used as function`
		return nullptr;
	}
	return func->call(args);
}

Object * NReturn::eval(Scope * scope) {
	// TODO: Return function in function scope
}

Object * NFuncDecl::eval(Scope * scope) {
	return scope->define_func(*this);
}

Object * NListAccess::eval(Scope * scope) {
	List * list = dynamic_cast<List*>(left.eval(scope));
	if(!list){
		// TODO: Add error catching `left cannot be accessed as list`
		return nullptr;
	}
	// TODO: Think about __getitem__ or equivalent instead
	return list->access(access.eval(scope));
}

Object * NList::eval(Scope * scope) {
	return new List(*this);
}

Object * NCondition::eval(Scope * scope) {
	if(If.first->eval(scope)->toBool()){
		return If.second->eval(scope);
	}

	for(const auto & Elif : Elifs){
		if(Elif.first->eval(scope)->toBool()){
			return Elif.second->eval(scope);
		}
	}

	return Else->eval(scope);
}

Object * NWhile::eval(Scope * scope) {
	while(condition.eval(scope)->toBool()){
		block.eval(scope);
	}

	return nullptr;
}


// aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa..........

Object * NFor::eval(Scope * scope) {

}

Object * NMatch::eval(Scope * scope) {

}