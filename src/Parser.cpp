#include "Parser.h"
#include "error.h"

#include <iostream>

Parser::Parser(){
	index = 0;
	tokens.clear();
	tree.clear();
}

Node * Parser::peek(){
	if(!advanced){
		return current;
	}
	advanced = false;
	current = new Node();
	current->interpret(tokens[index]);
	return current;
}

Node * Parser::advance(){
	index++;
	advanced = true;
	return peek();
}

bool Parser::prog_end(){
	return peek()->type == T_PROG_END;
}

bool Parser::is_op(const std::string & op){
	return peek()->type == T_OP && peek()->str() == op;
}
bool Parser::is_kw(const std::string & kw){
	return peek()->type == T_KW && peek()->str() == kw;
}

// Checks if current node equals given binary operator Node
bool Parser::is_bin_op(){
	return peek()->type == T_OP && PREC_BIN.find(peek()->str()) != PREC_BIN.end();
}

// Do not use `skip` for numbers skipping (!)
void Parser::skip(const TokenType & type,
				  const std::string & val,
				  const bool & optional,
				  const bool & infinite)
{
	if(peek()->type == type && peek()->str() == val){
		advance();
		if(infinite){
			while(peek()->type == type && peek()->str() == val){
				advance();
			}
		}
	}else if(!optional){
		expected_error(token_type_to_str(type) + " `" + val + "`");
	}
}
void Parser::skip_op(const std::string & op, const bool & optional){
	skip_endl(true);
	skip(T_OP, op, optional, false);
	skip_endl(true);
}
void Parser::skip_kw(const std::string & kw, const bool & optional){
	skip_endl(true);
	skip(T_KW, kw, optional, false);
	skip_endl(true);
}
// TODO: Rename to `skip_endls`, because of semantic (skip_endl skips all endls)
void Parser::skip_endl(const bool & optional){
	skip(T_ENDL, "", optional, true);
}

void Parser::error(const std::string & msg){
	err("Parser [ERROR]: " + msg, peek()->line, peek()->column);
}

void Parser::expected_error(const std::string & expected, const std::string & given){
	error("Expected " + expected + ", " + given + " given");
}

void Parser::expected_error(const std::string & expected){
	std::string given = peek()->type == T_NUM ? std::to_string(peek()->real()) : peek()->str();
	expected_error(expected, token_type_to_str(peek()->type) + " `" + given + "`");
}

void Parser::unexpected_error(){
	tokens[index].unexpected_error();
}

Tree Parser::parse(const std::vector <Token> & tokens){
	this->tokens = tokens;

	while(!prog_end()){
		tree.push_back(parse_expression());
		if(!prog_end()){
			skip_endl();
		}
	}

	return tree;
}

Node * Parser::parse_expression(){
	return maybe_call([&]() -> void* {
		return maybe_binary(parse_atom(), 0);
	});
}

Tree Parser::parse_delimited(
	const std::string & begin,
	const std::string & end,
	const std::string & sep)
{
	Tree nodes;
	bool first = true;
	skip_op(begin);
	while(!prog_end()){
		if(is_op(end)){
			break;
		}
		if(first){
			first = false;
		}else if(!sep.empty()){
			skip_op(sep);
		}else{
			skip_endl();
		}
		if(is_op(end)){
			break;
		}
		// Think about `parser` function, maybe can be not only parse_expression
		nodes.push_back(parse_expression());
	}
	// Note: Could be replaced with `advance()`
	skip_op(end);
	return nodes;
}

Node * Parser::parse_atom(){
	return maybe_call([&]() -> void* {
		// Skip optional endl
		skip_endl(true);
		
		if(is_kw("if")){
			return parse_condition();
		}
		if(is_kw("while")){
			return parse_while();
		}
		if(is_kw("for")){
			return parse_for();
		}
		if(is_kw("var") || is_kw("val")){
			return parse_var();
		}
		if(is_kw("func")){
			return parse_func();
		}
		if(is_op("{")){
			return parse_scope();
		}

		if(is_op("(")){
			advance();
			// Note: Maybe, could be parse_delimited with endls
			Node * node = parse_expression();
			skip_op(")");
			return node;
		}
		if(is_op("[")){
			return parse_array();
		}

		Node * node = peek();
		if(node->type == T_ID || node->type == T_STR || node->type == T_NUM){
			advance();
			return node;
		}

		unexpected_error();

		return new Node();
	});
}

VarNode * Parser::parse_var(){
	const bool is_val = peek()->str() == "val";

	if(advance()->type != T_ID){
		expected_error("variable name (identifier)");
	}

	const std::string var_name = peek()->str();

	advance();
	
	// By default variable type is any
	std::string var_type = "any";

	if(is_op(":")){
		// TODO: Add array type
		// Note: Now only works with primitive types
		// Note: Does not work with endls
		if(advance()->type != T_ID){
			expected_error("variable type");
		}
		var_type = peek()->str();
		advance();
	}

	return new VarNode(var_type, var_name, is_val);
}

FuncNode * Parser::parse_func(){
	if(advance()->type != T_ID){
		expected_error("function name (identifier)");
	}

	const std::string func_name = peek()->str();

	advance();

	Tree args = parse_args();

	return new FuncNode(func_name, args, parse_scope());
}

// TODO: parse_tail -- parse scope or one expression for loops or ifs and etc.

ScopeNode * Parser::parse_scope(){
	return new ScopeNode(parse_delimited("{", "}", ""));
}

// TODO: Think about different parsers for call args and func args
Tree Parser::parse_args(){
	return parse_delimited("(", ")", ",");
}

ArrayNode * Parser::parse_array(){
	return new ArrayNode(parse_delimited("[", "]", ","));
}

ConditionNode * Parser::parse_condition(){
	skip_kw("if");

	Condition If;

	skip_op("(");
	If.first = parse_expression();
	skip_op(")");

	// TODO: Add one line scope
	If.second = parse_scope();

	std::vector <Condition> Elifs;
	ScopeNode * Else = nullptr;

	while(is_kw("elif")){
		skip_kw("elif");

		Condition Elif;

		skip_op("(");
		Elif.first = parse_expression();
		skip_op(")");

		Elif.second = parse_scope();

		Elifs.push_back(Elif);
	}

	if(is_kw("else")){
		skip_kw("else");
		Else = parse_scope();
	}

	return new ConditionNode(If, Elifs, Else);
}

WhileNode * Parser::parse_while(){
	skip_kw("while");

	Condition While;

	skip_op("(");
	While.first = parse_expression();
	skip_op(")");

	While.second = parse_scope();

	return new WhileNode(While.first, While.second);
}

ForNode * Parser::parse_for(){
	skip_kw("for");

	Node * For;
	Node * In;
	ScopeNode body;

	skip_op("(");
	// TODO: Add special construction support. Like for(var [key, val] in map)
	// Parse For-var construction
	skip_kw("var");
	For = peek();
	advance();

	skip_kw("in");
	In = parse_expression();

	skip_op(")");

	return new ForNode(For, In, parse_scope());
}

CallNode * Parser::parse_call(Node * func_name){
	return new CallNode(func_name, parse_args());
}

Node * Parser::maybe_call(const std::function <void*()> & expression){
	Node * left = static_cast <Node*> (expression());
	return is_op("(") ? parse_call(left) : left;
}

Node * Parser::maybe_binary(Node * left, const uint8_t & prec){
	Node * node = peek();
	if(is_bin_op()){
		// is_bin_op ensures that PREC_BIN[peek] exists, so no checking otherwise
		uint8_t left_prec = PREC_BIN.at(peek()->str());
		if(left_prec > prec){
			advance();
			return maybe_binary(new BinNode(node->str(), left, maybe_binary(parse_atom(), left_prec)), prec);
		}
	}

	return left;
}