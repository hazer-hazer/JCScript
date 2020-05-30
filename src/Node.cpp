////////////////
// Base nodes //
////////////////

Value * NExpressionStatement(const Scope & scope){
	return expression.eval(scope);
}

/////////////////
// Value nodes //
/////////////////

Int * NInt::eval(const Scope & scope){
	return new Int(*this);
}

Float * NFloat::eval(const Scope & scope){
	return new Float(*this);
}

Bool * NBool::eval(const Scope & scope){
	return new Bool(*this);
}

String * NString::eval(const Scope & scope){
	return new String(*this);
}

//////////////////
// Common nodes //
//////////////////

// Note: Maybe useless because of wide usage
// Identifier * NIdentifier::eval(const Scope & scope){
// 	return new VIdentifier(name);
// }


// TODO: Solve problems with universal block
// Value * NBlock::eval(const Scope & scope){
// 	Value * val = nullptr;
// 	for(NStatement * stmt : statements){
// 		if(stmt != nullptr){
// 			val = stmt.eval();
// 		}
// 	}
// 	return val;
// }

////////////////////
// Operator nodes //
////////////////////

Value * NInfixOp::eval(const Scope & scope){
	Value * left_value = left.eval(scope);
	if(left_value.has_infix_operator(op)){
		return left_value.apply_infix(op, right.eval(scope));
	}else{
		left_value->error("Infix operator " + op_to_str(op) + " does not exist");
		return nullptr;
	}
}

// Value * NPrefixOp::eval(const Scope & scope){
// 	return scope.eval_prefix_operator(op, right);
// }

// Value * NPostfixOp::eval(const Scope & scope){
// 	return scope.eval_postfix_operator(left, op);
// }

/////////////////
// Types nodes //
/////////////////

Value * NType::eval(const Scope & scope){
	return new VType(*this);
}

Value * NTypeDecl::eval(const Scope & scope){
	return scope.set_local_type(id.name, new VType(type));
}

Value * NVarDecl::eval(const Scope & scope){
	return scope.set_local_var(id.name, new VVariable(*this));
}

Value * NArgDecl::eval(const Scope & scope){
	if(VFunc * func = dynamic_cast<VFunc*>(*scope)){
		func->declare_argument(*this);
	}else{
		error("WTF? Why argument declaration is not inside a func declaration??");
	}
}

Value * NType::eval(const Scope & scope){
	return new VType(name, nullable);
}

Value * NType::eval(const Scope & scope){
	return new VType(name, nullable);
}
