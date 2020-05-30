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

bool Parser::is_infix_op(const Operator & op){
	return OP_INFIX_PREC.find(op) != OP_INFIX_PREC.end();
}
bool Parser::is_prefix_op(const Operator & op){
	return OP_PREFIX_PREC.find(op) != OP_PREFIX_PREC.end();
}
bool Parser::is_postfix_op(const Operator & op){
	return OP_POSTFIX_PREC.find(op) != OP_POSTFIX_PREC.end();
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
		std::cout << "Parse statement: " << peek().to_string() << std::endl;
		NStatement * statement = parse_statement();
		std::cout << "pushing statement:" << statement->to_string() << " " << peek().to_string() << std::endl;
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
	}else{
		if(is_expr_end()){
			// Skip statement that is just only a semicolon
			advance();
			return parse_statement();
		}
		NExpressionStatement * expression_statement = new NExpressionStatement(*parse_expression());
		std::cout << "Parsed expression_statement: " << expression_statement->to_string() << std::endl;
		return expression_statement;
	}

	return nullptr;
}
NExpression * Parser::parse_expression(){
	NExpression * expression = maybe_infix(parse_atom(), 0);

	return expression;
}

NExpression * Parser::maybe_infix(NExpression * left, int prec){
	if(!is_op()){
		return left;
	}
	Operator op = peek().op();
	if(is_infix_op(op)){
		int right_prec = OP_INFIX_PREC.at(op);
		if(right_prec > prec){
			advance();
			return maybe_infix(new NInfixOp(*left, op, *maybe_infix(parse_atom(), right_prec)), prec);
		}
	}
	return left;
}

NExpression * Parser::parse_atom(){
	std::cout << "parse_atom: " << peek().to_string() << std::endl;

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
		return expression;
	}
	if(is_str()){
		NString * str = new NString(peek().String());
		advance();
		return str;
	}
	if(is_op()){
		Operator op = peek().op();
		// Prefix operator
		if(is_prefix_op(op)){
			advance();
			return new NPrefixOp(op, *parse_expression());
		}
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
		std::cout << "push statement" << std::endl;
		block->statements.push_back(parse_statement());
		std::cout << "pushed statement: " << block->statements.back()->to_string() << std::endl;
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
	// TODO: Rewrite for special types
	if(!is_id()){
		expected_error("type identifier");
	}

	NIdentifier * type_id = parse_identifier();

	bool nullable = false;
	if(is_op(OP_QUESTION_MARK)){
		nullable = true;
		advance();
	}

	return new NType(*type_id, nullable);
}

NVarDecl * Parser::parse_var_decl(){
	const bool is_val = is_kw(KW_VAL);
	advance();

	NIdentifier * id = parse_identifier();

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

	skip_op(OP_PAREN_R, true, true);

	skip_kw(KW_IN, true, true);

	NExpression * In = parse_expression();

	return new NFor(*For, *In);
}
