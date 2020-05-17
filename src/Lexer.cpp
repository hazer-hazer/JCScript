#include "Lexer.h"

#include <iostream>

Lexer::Lexer(){
	index = 0;

	// Note: When using line and column to show tokens in error output, use (column-1)
	line = 1;
	column = 1;
	tokens.clear();
}

char Lexer::peek(){
	return current = code[index];
}

char Lexer::advance(){
	index++;
	if(peek() == '\n'){
		line++;
		column = 1;
	}else{
		column++;
	}
	return peek();
}

void Lexer::add_token(const Token & t){
	this->tokens.push_back(t);
}

// Determinators
bool Lexer::is_skipable(const char & c){
	return c == ' '  || c == '\t' || c == '\v'
		|| c == '\f' || c == '\r';
}

bool Lexer::is_digit(const char & c){
	return c >= 48 & c <= 57;
}
bool Lexer::is_identifier_first(const char & c){
	return c >= 65  && c <= 90  || // Large alpha
		   c >= 97  && c <= 122 || // Small alpha
	       c == '$' || c == '_';
}
bool Lexer::is_identifier(const char & c){
	return is_identifier_first(c) | is_digit(c);
}
bool Lexer::is_punct(const char & c){
	return c == 33 || c == 35 || c == 37  || c == 38
		|| c >= 40 && c <= 47 || c >= 58  && c <= 64
		|| c >= 91 && c <= 95 || c >= 123 && c <= 126;
}
bool Lexer::is_quote(const char & c){
	return c == '"' || c == '\'' || c == '`';
}
bool Lexer::is_endl(const char & c){
	return c == '\n';
}

// Errors
void Lexer::error(const std::string & msg){
	err("Lexer [ERROR]: " + msg, line, column);
}
void Lexer::unexpected_token(const std::string & token){
	error("Unexpected token `" + (token.empty() ? std::string(1, current) : token) + "`");
}

std::vector <Token> Lexer::lex(const std::string & code){
	this->code = code;

	Token token;

	TokenType type;
	std::string val;

	// Fix first endl line++
	if(peek() == '\n'){
		line++;
	}

	while(peek()){

		token.line = line;
		token.column = column;

		val = "";

		if(is_skipable(current)){
			advance();
			continue;
		}else if(is_endl(current)){
			while(is_endl(advance())){}

			type = T_ENDL;
		}else if(is_identifier_first(current)){
			do{
				val += current;
			}while(is_identifier(advance()));

			type = is_kw(val) ? T_KW : T_ID;
		}else if(is_digit(current)){
			do{
				val += current;
			}while(is_digit(advance()));

			if(current == '.'){
				val += current;
				if(!is_digit(advance())){
					unexpected_token("");
				}
				while(is_digit(current)){
					val += current;
					advance();
				}
			}
			type = T_NUM;
		}else if(is_quote(current)){
			const char quote = current;
			while(advance() != quote && current && current != '\n'){
				val += current;
			}
			if(!current || current != quote){
				error("Expected ending quote `" + std::string(1, quote) + "`");
			}
			advance();			
			type = T_STR;
		}else if(is_punct(current)){
			do{
				val += current;
			}while(is_op(val + advance()));

			if(!is_op(val)){
				unexpected_token("");
			}
			type = T_OP;
		}else{
			unexpected_token("");
		}

		token.type = type;
		token.val = val;

		add_token(token);
	}

	token.type = T_PROG_END;
	token.val = "";

	add_token(token);

	return tokens;
}