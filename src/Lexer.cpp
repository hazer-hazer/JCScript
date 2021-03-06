#include "Lexer.h"

#include <iostream>

Lexer::Lexer(){
	// Note: When using line and column to show tokens in error output, use (column-1)
	tokens.clear();

	index = 0;
	line = 1;
	column = 0;
}

char Lexer::peek(){
	return code[index];
}

char Lexer::advance(){
	index++;
	if(peek() == '\n'){
		line++;
		column = 0;
	}else{
		column++;
	}
	return peek();
}

void Lexer::add_token(const TokenType & type, const std::string & val){
	Token t(type, val);
	t.pos.line = line;
	t.pos.column = column;
	tokens.push_back(t);
}
void Lexer::add_token(const Operator & op){
	Token t(op);
	t.pos.line = line;
	t.pos.column = column;
	tokens.push_back(t);
}

// Determinators
bool Lexer::eof(){
	return index >= code.length();
}
bool Lexer::is_skipable(const char & c){
	return c == ' '  || c == '\t' || c == '\v'
		|| c == '\f' || c == '\r';
}
bool Lexer::is_endl(const char & c){
	return c == '\n';
}
bool Lexer::is_digit(const char & c){
	return c >= '0' && c <= '9';
}
bool Lexer::is_hex(const char & c){
	return is_digit(c)
		|| (c >= 'a' && c <= 'f')
		|| (c >= 'A' && c <= 'F');
}
bool Lexer::is_identifier_first(const char & c){
	return (c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z')
		||  c == '$' || c == '_';
}
bool Lexer::is_identifier(const char & c){
	return is_identifier_first(c) || is_digit(c);
}
bool Lexer::is_quote(const char & c){
	return c == '"' || c == '\'' || c == '`';
}

void Lexer::lex_number(){	
	std::string num;
	enum {
		INT, BIN, HEX, FLOAT
	} number_type;

	bool negative = false;

	number_type = INT;

	if(peek() == '-'){
		negative = true;
		advance();
	}

	if(peek() == '0'){
		if(negative){
			error("Only integer numbers can be negative");
		}
		advance();
		if(peek() == 'x' || peek() == 'X'){
			number_type = HEX;
			advance();
			num = "0x";
		}else if(peek() == 'b' || peek() == 'B'){
			number_type = BIN;
			advance();
			num = "0b";
		}else{
			num = "0";
		}
	}

	if(number_type == HEX){
		while(is_hex(peek())){
			num += peek();
			advance();
		}
	}else if(number_type == BIN){
		if(peek() != '0' || peek() != '1'){
			unexpected_token();
		}
		do{
			num += peek();
		}while(peek() == '0' || peek() == '1');
	}else{
		while(is_digit(peek())){
			num += peek();
			advance();
		}
	}

	// Note: only decimal numbers can be floating
	if(number_type == INT && peek() == '.'){
		num += peek();
		if(!is_digit(advance())){
			unexpected_token("");
		}
		while(is_digit(peek())){
			num += peek();
			advance();
		}
		number_type = FLOAT;
	}

	TokenType token_type;

	switch(number_type){
		case INT:
		case HEX:
		case BIN:{
			token_type = T_INT;
			break;
		}
		case FLOAT:{
			token_type = T_FLOAT;
			break;
		}
	}

	add_token(token_type, num);
}

std::string Lexer::lex_identifier(){
	std::string id(1, peek());
	while(is_identifier(advance())){
		id += peek();
	}
	return id;
}

// Errors
void Lexer::error(const std::string & msg){
	// Debug
	std::cout << "Tokens:\n";
	for(auto & t : tokens){
		std::cout << t.to_string(true) << std::endl;
	}

	err("Lexer [ERROR]: " + msg, line, column);
}
void Lexer::unexpected_token(const std::string & token){
	error("Unexpected token `" + (token.empty() ? std::string(1, peek()) : token) + "`");
}

std::vector <Token> Lexer::lex(const char * path){
	// Read code (Maybe move to separate stream class)
	std::ifstream file(path);
	std::stringstream buffer;
	buffer << file.rdbuf();
	code = buffer.str();

	// Fix first endl line++
	// TODO: Move to Stream
	// if(peek() == '\n'){
	// 	line++;
	// }

	while(!eof()){
		// TODO: Add comments

		// Think about replacing determinator functions and arrays with switch-case
		// Maybe it will increase efficiency

		if(is_skipable(peek())){
			advance();
			continue;
		}else if(is_endl(peek())){
			// TODO: Skip case "endl [white-space] endl"
			while(is_endl(peek())){
				advance();
			}
			// Do not add endl token if previous was endl
			if(!(tokens.size() > 0 && tokens.back().type == T_ENDL)){
				add_token(T_ENDL, "");
			}
		}else if(is_identifier_first(peek())){
			std::string id = lex_identifier();
			if(is_kw(id)){
				if(id == "true" || id == "false"){
					add_token(T_BOOL, id);
				}else{
					add_token(T_KW, id);
				}
			}else if(id == "as" && peek() == '?'){
				// Check for `as?` operator
				add_token(OP_AS_NULLABLE);
				advance();
			}else if(str_operator(id) < operators.size()){
				add_token(str_operator(id));
			}else{
				add_token(T_ID, id);
			}
		}else if(is_digit(peek())){
			lex_number();
		}else if(is_quote(peek())){
			std::string str = "";
			const char quote = peek();
			advance();
			while(peek() != quote && !eof() && peek() != '\n'){
				str += peek();
				advance();
			}
			if(eof() || peek() != quote){
				error("Expected ending quote `" + std::string(1, quote) + "`");
			}
			add_token(T_STR, str);
			advance();
		}else{
			// Lex operators
			switch(peek()){
				case '+':{
					advance();
					switch(peek()){
						case '=':{
							add_token(OP_ASSIGN_ADD);
							advance();
							break;
						}
						case '+':{
							add_token(OP_INC);
							advance();
							break;
						}
						default: add_token(OP_ADD); break;
					}
					break;
				}
				case '-':{
					advance();
					if(is_digit(peek())){
						lex_number();
					}else if(peek() == '='){
						add_token(OP_ASSIGN_SUB);
						advance();
					}else if(peek() == '-'){
						add_token(OP_DEC);
						advance();
					}else{
						add_token(OP_SUB);
					}
					break;
				}
				case '*':{
					advance();
					switch(peek()){
						case '=':{
							add_token(OP_ASSIGN_MUL);
							advance();
							break;
						}
						case '*':{
							advance();
							if(peek() == '='){
								add_token(OP_ASSIGN_EXP);
								advance();
							}else{
								add_token(OP_EXP);
							}
							break;
						}
						// case '/':{
						// 	add_token(OP_CLOSE_COMMENT);
						// 	advance();
						// 	break;
						// }
						default: add_token(OP_MUL); break;
					}
					break;
				}
				case '/':{
					advance();
					switch(peek()){
						case '=':{
							add_token(OP_ASSIGN_DIV);
							advance();
							break;
						}
						// Note: One-line comment
						case '/':{
							while(!is_endl(advance())){}
							break;
						}
						case '*':{
							// Start multiline comment
							advance();
							std::string comment_operator(1, peek());
							comment_operator += advance();
							std::cout << "comment_operator: " << comment_operator << std::endl;
							while(!eof()){
								std::cout << "comment_operator: " << comment_operator << std::endl;
								if(comment_operator == "*/"){
									break;
								}
								comment_operator = peek();
								comment_operator += advance();
							}
							advance();
							break;
						}
						// TODO: Think about '//' as int div
						default: add_token(OP_DIV); break;
					}
					break;
				}
				case '%':{
					advance();
					switch(peek()){
						case '=':{
							add_token(OP_ASSIGN_MOD);
							advance();
							break;
						}
						default: add_token(OP_MOD); break;
					}
					break;
				}
				case '=':{
					advance();
					if(peek() == '='){
						add_token(OP_EQUAL);
						advance();
					}else if(peek() == '>'){
						add_token(OP_ARROW);
						advance();
					}else{
						add_token(OP_ASSIGN);
					}
					break;
				}
				case '&':{
					advance();
					switch(peek()){
						case '&':{
							add_token(OP_AND);
							advance();
							break;
						}
						case '=':{
							add_token(OP_ASSIGN_BIT_AND);
							advance();
							break;
						}
						default: add_token(OP_BIT_AND); break;
					}
					break;
				}
				case '|':{
					advance();
					switch(peek()){
						case '|':{
							add_token(OP_OR);
							advance();
							break;
						}
						case '=':{
							add_token(OP_ASSIGN_BIT_OR);
							advance();
							break;
						}
						case '>':{
							add_token(OP_PIPELINE);
							advance();
							break;
						}
						default: add_token(OP_BIT_OR); break;
					}
					break;
				}
				case '^':{
					advance();
					switch(peek()){
						case '=':{
							add_token(OP_ASSIGN_BIT_XOR);
							advance();
							break;
						}
						default: add_token(OP_BIT_XOR); break;
					}
					break;
				}
				case '!':{
					advance();
					if(is_identifier_first(peek())){
						std::string id = lex_identifier();
						if(id == "in"){
							add_token(OP_NOT_IN);
							advance();
						}else if(id == "is"){
							add_token(OP_NOT_IS);
						}else{
							add_token(T_ID, id);
						}
					}else if(peek() == '='){
						add_token(OP_NOT_EQUAL);
						advance();
					}else{
						add_token(OP_NOT);
					}
					break;
				}
				case '<':{
					advance();
					switch(peek()){
						case '=':{
							advance();
							if(peek() == '>'){
								add_token(OP_SPACESHIP);
								advance();
							}else{
								add_token(OP_LESS_EQUAL);
							}
							break;
						}
						case '<':{
							advance();
							if(peek() == '='){
								add_token(OP_ASSIGN_SHIFT_LEFT);
								advance();
							}else{
								add_token(OP_SHIFT_LEFT);
							}
							break;
						}
						default: add_token(OP_LESS); break;
					}
					break;
				}
				case '>':{
					advance();
					switch(peek()){
						case '=':{
							add_token(OP_GREATER_EQUAL);
							advance();
							break;
						}
						case '>':{
							advance();
							if(peek() == '='){
								add_token(OP_ASSIGN_SHIFT_RIGHT);
								advance();
							}else{
								add_token(OP_SHIFT_RIGHT);
							}
							break;
						}
						default: add_token(OP_GREATER); break;
					}
					break;
				}
				case '~':{
					advance();
					add_token(OP_BIT_INVERT);
					break;
				}
				// Punctuations
				case '(':{
					add_token(OP_PAREN_L);
					advance();
					break;
				}
				case ')':{
					add_token(OP_PAREN_R);
					advance();
					break;
				}
				case '[':{
					add_token(OP_BRACKET_L);
					advance();
					break;
				}
				case ']':{
					add_token(OP_BRACKET_R);
					advance();
					break;
				}
				case '{':{
					add_token(OP_BRACE_L);
					advance();
					break;
				}
				case '}':{
					add_token(OP_BRACE_R);
					advance();
					break;
				}
				case ':':{
					add_token(OP_COLON);
					advance();
					break;
				}
				case ',':{
					add_token(OP_COMMA);
					advance();
					break;
				}
				case '.':{
					advance();
					if(peek() == '.'){
						advance();
						if(peek() == '.'){
							add_token(OP_SPREAD);
							advance();
						}else if(peek() == '='){
							add_token(OP_RANGE_INCL);
							advance();
						}else{
							add_token(OP_RANGE);
						}
					}else{
						add_token(OP_MEMBER_ACCESS);
					}
					break;
				}
				case '?':{
					advance();
					if(peek() == ':'){
						add_token(OP_ELVIS);
						advance();
					}else{
						add_token(OP_QUESTION_MARK);
					}
					break;
				}
				case ';':{
					add_token(OP_SEMICOLON);
					// Note: Skip all repeating semicolons, also delimited with endls!
					while(advance() == ';' || is_endl(peek())){}
					break;
				}

				default: unexpected_token("");
			}
		}
	}

	add_token(T_PROG_END, "");

	return tokens;
}