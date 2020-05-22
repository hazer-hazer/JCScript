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
bool Parser::is_id(){
	return peek().type == T_ID;
}
bool Parser::is_num(){
	// TODO: Rewrite for over number types
	return peek().type == T_NUM;
}
bool Parser::is_str(){
	return peek().type == T_STR;
}
bool Parser::is_endl(){
	return peek().type == T_ENDL;
}
bool Parser::is_op(){
	return peek().type == T_OP;
}
bool Parser::is_kw(){
	return peek().type == T_KW;
}

bool Parser::is_op(const Operator & op){
	return is_op() && peek().op() == op;
}
bool Parser::is_kw(const Keyword & kw){
	return is_kw() && peek().kw() == kw;
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
	if(is_endl() || is_op(OP_SEMICOLON)){
		advance();
	}else if(!optional){
		expected_error("end of expression (end of line or ';')");
	}
}
void Parser::skip_op(const Operator & op, const bool & skip_side_endl){
	if(skip_side_endl){
		skip_endl();
	}
	if(is_op(op)){
		advance();
	}else{
		expected_error("operator `" + op_to_str(op) + "`");
	}
	if(skip_side_endl){
		skip_endl(true);
	}
}

void Parser::skip_kw(const Keyword & kw, const bool & skip_side_endl){
	if(skip_side_endl){
		skip_endl(true);
	}
	if(is_kw(kw)){
		advance();
	}else{
		expected_error("keyword `" + kw_to_str(kw) + "`");
	}
	if(skip_side_endl){
		skip_endl(true);
	}
}

// Errors
void Parser::error(const std::string & msg){
	err("Parser [ERROR]: " + msg, peek().line, peek().line);
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
		tree.push_back(parse_statement());
		if(!eof()){
			skip_expr_end();
		}
	}

	return tree;
}

NStatement * Parser::parse_statement(){
	if(is_kw()){
		switch(peek().kw()){
			case KW_VAR:
			case KW_VAL:{
				return parse_var_decl();
				break;
			}
			case KW_FUNC:{
				return parse_func_decl();
				break;
			}
		}
	}

	unexpected_error();
	return nullptr;
}

NExpression * Parser::parse_expression(){
	// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	// aaAAAAAAAaAAAaaaaaAAAAAAAAAAAAaaaaaaaaaaaAAAAAAAAAA
	
	
	// if(peek().type == T_ID){
	// 	NIdentifier * left = parse_identifier();
	// 	advance();
	// 	if(peek().type == T_OP){
	// 		return parse_operator(left);
	// 	}
	// }else if(peek().type == T)

	return nullptr;
}

NBlock * Parser::parse_block(){
	skip_op(OP_BRACE_L);
	NBlock * block;
	while(!is_op(OP_BRACE_R) && !eof()){
		block->statements.push_back(parse_statement());
		if(!eof()){
			skip_expr_end();
		}else{
			expected_error("closing brace `}`");
		}
	}
	skip_op(OP_BRACE_R);
	return block;
}

NIdentifier * Parser::parse_identifier(){
	if(!is_id()){
		expected_error("identifier");
	}
	NIdentifier * id = new NIdentifier(peek().str());
	advance();
	return id;
}

NType * Parser::parse_type(){
	// TODO: Rewrite for special types
	if(!is_id()){
		expected_error("identifier");
	}
	NIdentifier * type_id = parse_identifier();

	advance();

	bool nullable = false;
	if(is_op(OP_QUESTION_MARK)){
		nullable = true;
		advance();
	}

	return new NType(type_id, nullable);
}

NVarDecl * Parser::parse_var_decl(){
	const bool is_val = is_kw(KW_VAL);
	advance();
	NIdentifier * id = parse_identifier();
	advance();
	
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
	skip_op(OP_PAREN_L);
	ExpressionList args;
	while(!is_op(OP_PAREN_R) && !eof()){
		args.push_back(parse_expression());
		if(eof()){
			skip_expr_end();
		}
		if(!is_op(OP_PAREN_R)){
			skip_op(OP_COMMA);
		}
	}
	skip_op(OP_PAREN_R);
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
	skip_op(OP_PAREN_L);
	ArgList args;
	while(!is_op(OP_PAREN_R) && !eof()){
		args.push_back(parse_arg_declaration());
		if(eof()){
			expected_error("closing parenthesis `)`");
		}
		if(!is_op(OP_PAREN_R)){
			skip_op(OP_COMMA);
		}
	}
	skip_op(OP_PAREN_R);
	return args;
}

NFuncDecl * Parser::parse_func_decl(){
	skip_kw(KW_FUNC);
	NIdentifier * id = parse_identifier();
	advance();
	ArgList args = parse_arg_declaration_list();

	NType * return_type = nullptr;
	if(is_op(OP_COLON)){
		return_type = parse_type();
	}

	NBlock * block = parse_block();

	return new NFuncDecl(*id, args, return_type, *block);
}

// 
// Condition
// 
NCondition * Parser::parse_condition(){
	skip_kw(KW_IF);
	
	ConditionBlock If;
	skip_op(OP_PAREN_L);
	If.first = parse_expression();
	skip_op(OP_PAREN_R);
	If.second = parse_block();

	std::vector <ConditionBlock> Elifs;
	while(is_kw(KW_ELIF)){
		skip_kw(KW_ELIF);

		ConditionBlock Elif;
		skip_op(OP_PAREN_L);
		Elif.first = parse_expression();
		skip_op(OP_PAREN_R);

		Elif.second = parse_block();

		Elifs.push_back(Elif);
	}

	NBlock * Else = nullptr;
	if(is_kw(KW_ELSE)){
		advance();
		Else = parse_block();
	}

	return new NCondition(If, Elifs, Else);
}

NWhile * Parser::parse_while(){
	skip_kw(KW_WHILE);

	skip_op(OP_PAREN_L);
	NExpression * condition = parse_expression();
	skip_op(OP_PAREN_R);

	NBlock * block = parse_block();

	return new NWhile(*condition, *block);
}

NFor * Parser::parse_for(){
	skip_kw(KW_FOR);

	skip_op(OP_PAREN_L);

	// TODO: Add (key, val)
	NIdentifier * For;

	if(is_op(OP_PAREN_L)){
		// Parse key, val
	}else{
		For = parse_identifier();
	}

	skip_op(OP_PAREN_R);

	skip_kw(KW_IN);

	NExpression * In = parse_expression();

	return new NFor(*For, *In);
}
