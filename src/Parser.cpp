#include "Parser.h"

Parser::Parser(){
	index = 0;
}

bool Parser::eof(){
	return peek().type == T_PROG_END;
}

Token Parser::peek(){
	return tokens[index];
}

Token Parser::advance(){
	return tokens[++index];
}

// Recognizers
bool Parser::is_typeof(const TokenType & t){
	return peek().type == t;
}
bool Parser::is_id(){
	return is_typeof(T_ID);
}
bool Parser::is_str(){
	return is_typeof(T_STR);
}
bool Parser::is_endl(){
	return is_typeof(T_ENDL);
}
bool Parser::is_op(){
	return is_typeof(T_OP);
}
bool Parser::is_kw(){
	return is_typeof(T_KW);
}
bool Parser::is_expr_end(){
	return is_endl() || is_op(OP_SEMICOLON);
}

bool Parser::is_op(const Operator & op){
	return is_op() && peek().op() == op;
}
bool Parser::is_kw(const Keyword & kw){
	return is_kw() && peek().kw() == kw;
}

bool Parser::is_infix_op(){
	return is_op() && OP_INFIX_PREC.find(peek().op()) != OP_INFIX_PREC.end();
}
bool Parser::is_prefix_op(){
	return is_op() && OP_PREFIX_PREC.find(peek().op()) != OP_PREFIX_PREC.end();
}
bool Parser::is_postfix_op(){
	return is_op() && OP_POSTFIX_PREC.find(peek().op()) != OP_POSTFIX_PREC.end();
}

// Skippers
void Parser::skip_endl(const bool & optional){
	if(is_endl()){
		advance();
	}else if(!optional){
		expected_error("end of line");
	}
}
void Parser::skip_expr_end(const bool & optional){
	// Expression end can be endl or ';'
	if(is_expr_end()){
		advance();
	}else if(!optional){
		expected_error("end of expression (end of line or ';')");
	}
}
void Parser::skip_op(const Operator & op, const bool & skip_left_endl, const bool & skip_right_endl){
	if(skip_left_endl){
		skip_endl(true);
	}
	if(is_op(op)){
		advance();
	}else{
		expected_error("operator `" + op_to_str(op) + "`");
	}
	if(skip_right_endl){
		skip_endl(true);
	}
}

void Parser::skip_kw(const Keyword & kw, const bool & skip_left_endl, const bool & skip_right_endl){
	if(skip_left_endl){
		skip_endl(true);
	}
	if(is_kw(kw)){
		advance();
	}else{
		expected_error("keyword `" + kw_to_str(kw) + "`");
	}
	if(skip_right_endl){
		skip_endl(true);
	}
}

// Errors
void Parser::error(const std::string & msg){
	err("Parser [ERROR]: " + msg, peek().pos.line, peek().pos.line);
}
void Parser::expected_error(const std::string & expected, const std::string & given){
	error("Expected " + expected + ", " + given + " given");
}
void Parser::expected_error(const std::string & expected){
	// Note: set `given` as current token and print without position
	std::string given;
	if(eof()){
		given = "end of file";
	}else{
		given = peek().to_string();
	}
	expected_error(expected, given);
}
void Parser::unexpected_error(){
	peek().unexpected_error();
}

StatementList Parser::parse(const std::vector <Token> & tokens){
	this->tokens = tokens;

	while(!eof()){
		NStatement * statement = parse_statement();
		if(statement != nullptr){
			tree.push_back(statement);
		}
		if(!eof()){
			// Note: some expressions end with for example `}` can be escaped optionaly
			skip_expr_end(optional_expr_end);
			optional_expr_end = false;
		}
	}

	return tree;
}

NStatement * Parser::parse_statement(){
	if(is_expr_end()){
		// Skip lines with expr_end only
		advance();
		return parse_statement();
	}
	if(is_kw()){
		switch(peek().kw()){
			case KW_VAR:
			case KW_VAL:{
				return parse_var_decl();
				break;
			}
			case KW_TYPE:{
				return parse_type_decl();
			}
			case KW_FUNC:{
				return parse_func_decl();
				break;
			}
			case KW_MATCH:{
				return parse_match();
				break;
			}
			case KW_RETURN:{
				Position pos = peek().pos;
				advance();
				NExpression * return_expr = nullptr;
				if(!is_expr_end()){
					return_expr = parse_expression();
				}
				return new NReturn(return_expr, pos);
			}
		}
	}else{
		return new NExpressionStatement(*parse_expression(), peek().pos);
	}

	return nullptr;
}

NExpression * Parser::parse_expression(){
	NExpression * expression = parse_atom();
	
	while(!eof()){
		// Check for chain of function call or list access or infix operator
		if(is_op(OP_PAREN_L)){
			expression = parse_func_call(expression);
		}else if(is_op(OP_BRACKET_L)){
			expression = parse_list_access(expression);
		}else if(is_infix_op()){
			expression = maybe_infix(expression, 0);
		}else{
			break;
		}
	}

	return expression;
}

NExpression * Parser::maybe_infix(NExpression * left, int prec){
	if(is_infix_op()){
		Operator op = peek().op();
		int right_prec = OP_INFIX_PREC.at(op);
		if(right_prec > prec){
			Position pos;
			advance();
			return maybe_infix(new NInfixOp(*left, op, *maybe_infix(parse_atom(), right_prec), pos), prec);
		}
	}
	return left;
}

NExpression * Parser::maybe_call(NExpression * left){
	// if(allow_func_call && is_op(OP_PAREN_L)){
	// 	allow_func_call = false;
	// 	return parse_func_call(left);
	// return left;
	
	return left;
}

NExpression * Parser::maybe_list_access(NExpression * left){
	// if(is_op(OP_BRACKET_L)){
	// 	NExpression * access = parse_expression();
	// 	return new NListAccess(*left, *access);
	// }

	return left;
}

NListAccess * Parser::parse_list_access(NExpression * left){
	skip_op(OP_BRACKET_L, false, true);
	NExpression * access = parse_expression();
	skip_op(OP_BRACKET_R, true, false);

	return new NListAccess(*left, *access);
}

NExpression * Parser::parse_atom(){

	/*if(is_endl()){
		// Skip end_of_line
		advance();
		return parse_atom();
	}else */

	// Numbers
	if(is_typeof(T_INT)){
		NInt * num = new NInt(peek().Int());
		advance();
		return num;
	}
	if(is_typeof(T_FLOAT)){
		NFloat * num = new NFloat(peek().Float());
		advance();
		return num;
	}
	if(is_typeof(T_BOOL)){
		NBool * num = new NBool(peek().Bool());
		advance();
		return num;
	}

	if(is_op(OP_PAREN_L)){
		// Parse subexpression
		skip_op(OP_PAREN_L, true, true);
		NExpression * expression = parse_expression();
		skip_op(OP_PAREN_R, true, false);
		
		// TODO: Think about allow_func_call after subexpression
		
		return expression;
	}
	if(is_op(OP_BRACKET_L)){
		skip_op(OP_BRACKET_L, false, true);
		ExpressionList expressions;
		bool first = true;
		while(!eof()){
			if(is_op(OP_BRACKET_R)){
				break;
			}
			if(first){
				first = false;
			}else{
				skip_op(OP_COMMA, true, true);
			}
			expressions.push_back(parse_expression());
		}

		skip_op(OP_BRACKET_R, true, false);

		return new NList(expressions);
	}
	if(is_str()){
		NString * str = new NString(peek().String());
		advance();
		return str;
	}
	if(is_prefix_op()){
		// Parse prefix operator
		Operator op = peek().op();
		advance();
		return new NPrefixOp(op, *parse_expression());
	}
	if(is_kw()){
		switch(peek().kw()){
			case KW_IF:{
				return parse_condition();
				break;
			}
		}
	}
	if(is_id()){
		NIdentifier * id = new NIdentifier(peek().String());
		allow_func_call = true;
		advance();
		return id;
	}

	unexpected_error();

	return nullptr;
}

NBlock * Parser::parse_block(){
	NBlock * block = new NBlock();

	bool one_line = false;
	bool first = true;

	skip_endl(true);

	if(is_op(OP_BRACE_L)){
		skip_op(OP_BRACE_L, true, true);
	}else{
		one_line = true;
	}

	if(one_line){
		block->statements.push_back(parse_statement());
	}else{
		while(!eof()){
			if(is_op(OP_BRACE_R)) break;
			if(first){
				first = false;
			}else{
				skip_expr_end();
			}
			if(is_op(OP_BRACE_R)) break;
			block->statements.push_back(parse_statement());
		}
		skip_op(OP_BRACE_R, true, false);
	}
	optional_expr_end = true;

	return block;
}

NIdentifier * Parser::parse_identifier(){
	if(!is_id()){
		expected_error("identifier");
	}
	NIdentifier * id = new NIdentifier(peek().String());
	advance();
	return id;
}

NType * Parser::parse_type(){

	NType * type = nullptr;

	if(is_op(OP_BRACKET_L)){
		type = parse_list_type();
	}else if(is_op(OP_PAREN_L)){
		type = parse_tuple_type();
	}else if(is_id()){
		type = new NIdentifierType(*parse_identifier());
	}else{
		unexpected_error();
		return nullptr;
	}

	if(is_op(OP_QUESTION_MARK)){
		type->nullable = true;
		advance();
	}

	return type;
}

NListType * Parser::parse_list_type(){
	skip_op(OP_BRACKET_L, false, false);
	NType * wrapped_type = parse_type();
	skip_op(OP_BRACKET_R, false, false);

	return new NListType(*wrapped_type);
}

NTupleType * Parser::parse_tuple_type(){
	// TODO: Add tuple var names and maybe default values

	skip_op(OP_PAREN_L, false, false);

	std::vector <NType*> types;

	bool first = true;
	while(!eof()){
		if(is_op(OP_PAREN_R)){
			break;
		}
		if(first){
			first = false;
		}else{
			skip_op(OP_COMMA, false, false);
		}
		types.push_back(parse_type());
	}

	skip_op(OP_PAREN_R, false, false);

	return new NTupleType(types);
}

NTypeDecl * Parser::parse_type_decl(){
	skip_kw(KW_TYPE, false, false);

	NIdentifier * id = parse_identifier();

	skip_op(OP_ASSIGN, false, false);

	NType * type = parse_type();

	return new NTypeDecl(*id, *type);
}

NVarDecl * Parser::parse_var_decl(){
	const bool is_val = is_kw(KW_VAL);
	advance();

	NIdentifier * id = parse_identifier();

	// Note: Default type is any (not nullable)
	NType * type = nullptr;
	if(is_op(OP_COLON)){
		advance();
		type = parse_type();
	}

	NExpression * assignment_expr = nullptr;
	if(is_op(OP_ASSIGN)){
		advance();
		assignment_expr = parse_expression();
	}

	return new NVarDecl(is_val, *id, type, assignment_expr);
}

NFuncCall * Parser::parse_func_call(NExpression * left){
	skip_op(OP_PAREN_L, false, true);
	ExpressionList args;
	while(!is_op(OP_PAREN_R) && !eof()){
		args.push_back(parse_expression());
		if(eof()){
			skip_expr_end();
		}
		if(!is_op(OP_PAREN_R)){
			skip_op(OP_COMMA, true, true);
		}
	}
	skip_op(OP_PAREN_R, true, false);
	return new NFuncCall(*left, args);
}

// 
// Function
// 
NArgDecl * Parser::parse_arg_declaration(){
	NIdentifier * id = parse_identifier();
	// TODO: Add default `any` type
	NType * type = nullptr;
	if(is_op(OP_COLON)){
		advance();
		type = parse_type();
	}

	NExpression * default_value = nullptr;
	if(is_op(OP_ASSIGN)){
		advance();
		default_value = parse_expression();
	}

	return new NArgDecl(*id, type, default_value);
}

ArgList Parser::parse_arg_declaration_list(){
	skip_op(OP_PAREN_L, false, true);
	ArgList args;
	while(!is_op(OP_PAREN_R) && !eof()){
		args.push_back(parse_arg_declaration());
		if(eof()){
			expected_error("closing parenthesis `)`");
		}
		if(!is_op(OP_PAREN_R)){
			skip_op(OP_COMMA, true, true);
		}
	}
	skip_op(OP_PAREN_R, true, false);
	return args;
}

NFuncDecl * Parser::parse_func_decl(){
	skip_kw(KW_FUNC, false, false);
	NIdentifier * id = parse_identifier();

	ArgList args = parse_arg_declaration_list();

	NType * return_type = nullptr;
	if(is_op(OP_COLON)){
		advance();
		return_type = parse_type();
	}

	NBlock * block = parse_block();

	return new NFuncDecl(*id, args, return_type, *block);
}

// 
// Condition
// 
NCondition * Parser::parse_condition(){
	skip_kw(KW_IF, false, true);

	ConditionBlock If;
	skip_op(OP_PAREN_L, true, true);
	If.first = parse_expression();
	skip_op(OP_PAREN_R, true, true);
	If.second = parse_block();

	std::vector <ConditionBlock> Elifs;
	while(is_kw(KW_ELIF)){
		skip_kw(KW_ELIF, true, true);

		ConditionBlock Elif;
		skip_op(OP_PAREN_L, true, true);
		Elif.first = parse_expression();
		skip_op(OP_PAREN_R, true, true);

		Elif.second = parse_block();

		Elifs.push_back(Elif);
	}

	skip_endl(true);

	NBlock * Else = nullptr;
	if(is_kw(KW_ELSE)){
		skip_kw(KW_ELSE, true, true);
		Else = parse_block();
	}

	return new NCondition(If, Elifs, Else);
}

NWhile * Parser::parse_while(){
	skip_kw(KW_WHILE, false, true);

	skip_op(OP_PAREN_L, true, true);
	NExpression * condition = parse_expression();
	skip_op(OP_PAREN_R, true, true);

	NBlock * block = parse_block();

	return new NWhile(*condition, *block);
}

NFor * Parser::parse_for(){
	skip_kw(KW_FOR, false, true);

	skip_op(OP_PAREN_L, true, true);

	// TODO: Add (key, val)
	NIdentifier * For;

	if(is_op(OP_PAREN_L)){
		// Parse key, val
	}else{
		For = parse_identifier();
	}

	skip_op(OP_IN, true, true);

	NExpression * In = parse_expression();

	skip_op(OP_PAREN_R, true, true);

	NBlock * block = parse_block();

	return new NFor(*For, *In, *block);
}

NMatch * Parser::parse_match(){
	skip_kw(KW_MATCH, false, true);

	skip_op(OP_PAREN_L, true, true);
	NExpression * expression = parse_expression();
	skip_op(OP_PAREN_R, true, true);

	skip_op(OP_BRACE_L, true, true);

	std::vector <MatchCase> Cases;

	NBlock * Else;

	while(!eof()){
		if(is_op(OP_BRACE_R)){
			break;
		}
		// Parse cases
		MatchCase Case;
		bool first = true;
		bool is_else = false;
		while(!eof()){
			// Parse left hand expressions
			if(is_op(OP_ARROW)){
				break;
			}
			if(first){
				first = false;
			}else{
				skip_op(OP_COMMA, true, true);
			}
			if(is_kw(KW_ELSE)){
				is_else = true;
				advance();
				break;
			}
			Case.first.push_back(parse_expression());
		}

		if(!is_else && Case.first.size() == 0){
			expected_error("Left hand expression");
		}

		skip_op(OP_ARROW, false, true);

		if(is_else){
			Else = parse_block();
			break;
		}else{
			Case.second = parse_block();
			skip_expr_end();
			Cases.push_back(Case);
		}
	}

	skip_op(OP_BRACE_R, true, true);

	return new NMatch(*expression, Cases, Else);
}