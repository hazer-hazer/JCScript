#include "Lexer.h"

#include <iostream>

Lexer::Lexer(){
	index = 0;
	tokens.clear();
	line = 1;
	column = 1;
}

const std::string Lexer::operators[] = {
	"+", "-", "*", "/", "%",
	"="
};

const std::string Lexer::keywords[] = {
	"var"
};

char Lexer::peek(){
	current = code[index];
	return current;
}

char Lexer::advance(){
	index++;
	if(peek() == '\n'){
		line++;
		column = 1;
	}else{
		column++;
	}
	return current;
}

// Determinators
bool Lexer::is_skipable(const char & c){
	return c == ' '  | c == '\t' | c == '\n' |
		   c == '\v' | c == '\f' | c == '\r' ;
}

bool Lexer::is_digit(const char & c){
	return c >= 48 & c <= 57;
}

bool Lexer::is_identifier_first(const char & c){
	return c >= 65  & c >= 90  | // Large alpha
		   c >= 97  & c <= 122 | // Small alpha
	       c == '$' | c == '_';
}
bool Lexer::is_identifier(const char & c){
	return is_identifier_first(c) | is_digit(c);
}

bool Lexer::is_punct(const char & c){
	return c == 33 | c == 35 | c == 37  | c == 38 |
		   c >= 40 & c <= 47 | c >= 58  & c <= 64 |
		   c >= 91 & c <= 95 | c >= 123 & c <= 126;
}

bool Lexer::is_quote(const char & c){
	return c == '"' | c == '\'';
}

// Post Determinators
bool Lexer::is_operator(const std::string & s){
	for(int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++){
		if(operators[i] == s) return true;
	}
	return false;
}

bool Lexer::is_keyword(const std::string & s){
	for(int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++){
		if(keywords[i] == s) return true;
	}
	return false;
}

// Errors
void Lexer::error(const std::string & message){
	Object::error(message +" at "+ std::to_string(line) +":"+ std::to_string(column));
}

void Lexer::unexpected_token(){
	unexpected_token(std::to_string((char)current));
}
void Lexer::unexpected_token(const std::string & token){
	error("Unexpected token `" + token + "`");
}

Tokens Lexer::lex(const std::string & code){
	this->code = code;

	peek();

	while(current){
		Token token;

		token.line = line;
		token.column = column;

		if(is_skipable(current)){
			advance();
			continue;
		}else if(is_identifier_first(current)){
			std::string iden;
			do{
				iden += peek();
			}while(is_identifier(advance()));

			if(is_keyword(iden)){
				token.type = Token::Type::KEYWORD;
			}else{
				token.type = Token::Type::IDENTIFIER;				
			}
			token.value = iden;
		}else if(is_digit(current)){
			std::string number;
			bool floating = false;
			do{
				number += current;
			}while(is_digit(advance()));

			if(current == '.'){
				number += current;
				if(!is_digit(advance())){
					unexpected_token();
				}
				while(is_digit(current)){
					number += current;
					advance();
				}
			}

			token.type = Token::Type::NUMBER;
			token.value = number;
		}else if(is_punct(current)){
			std::string op;
			do{
				op += peek();
			}while(is_operator(op + advance()));

			if(!is_operator(op)){
				unexpected_token(op);
			}

			token.type = Token::Type::OPERATOR;
			token.value = op;
		}else{
			unexpected_token();
		}

		tokens.push_back(token);
	}

	return tokens;
}