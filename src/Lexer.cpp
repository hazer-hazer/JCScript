#include "Lexer.h"

#include <iostream>

Lexer::Lexer(){
	index = 0;
	objects.clear();
	line = 1;
	column = 1;
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

// Errors
void Lexer::unexpected_token(){
	unexpected_token(std::to_string((char)current));
}
void Lexer::unexpected_token(const std::string & val){
	object_error("Unexpected token `" + val + "`", line, column);
}

void Lexer::expected_token(const std::string & val){
	object_error("Unexpected end of expression, `" + val + "` expected", line, column);
}

ObjectBuffer & Lexer::lex(const std::string & code){
	this->code = code;

	Object obj;

	std::string val;

	while(peek()){

		obj.line = line;
		obj.column = column;

		val = "";

		if(is_skipable(current)){
			advance();
			continue;
		}else if(is_identifier_first(current)){
			do{
				val += current;
			}while(is_identifier(advance()));

			obj.type = IDENTIFIER;
		}else if(is_digit(current)){
			do{
				val += current;
			}while(is_digit(advance()));

			if(current == '.'){
				val += current;
				if(!is_digit(advance())){
					unexpected_token();
				}
				while(is_digit(current)){
					val += current;
					advance();
				}
			}
			obj.type = NUMBER;
		}else if(is_quote(current)){
			const char quote = current;
			while(advance() != quote && current && current != '\n'){
				val += current;
			}
			if(!current || current != quote){
				expected_token(std::string(1, quote));
			}
			advance();
			obj.type = STRING;
		}else if(is_punct(current)){
			do{
				val += current;
			}while(is_operator(val + advance()));

			if(!is_operator(val)){
				unexpected_token(val);
			}
			obj.type = OPERATOR;
		}else{
			unexpected_token();
		}

		obj.set_val(val.c_str());

		objects.push_back(obj);
	}

	obj.type = PROG_END;
	objects.push_back(obj);

	return objects;
}