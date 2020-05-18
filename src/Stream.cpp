#include "Stream.h"

#include <iostream>
#include <fstream>
#include <sstream>

Stream::Stream(const std::string & path){
	std::ifstream file(path);
	std::stringstream buffer;
	buffer << file.rdbuf();
	code = buffer.str();
}

char Stream::peek(){
	return code[index];
}

char Stream::advance(){
	index++;
	if(peek() == '\n'){
		line++;
		column = 1;
	}else{
		column++;
	}
	return peek();
}

unsigned int Stream::get_line(){
	return line;
}

unsigned int Stream::get_column(){
	return column;
}