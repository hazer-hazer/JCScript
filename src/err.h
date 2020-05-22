#ifndef ERROR_H
#define ERROR_H

#include <sstream>

// TODO: Think about CursorErrorHandler class as Lexer, Token, Parser parent
// It will contain line, column

struct Exception : std::exception {
	std::string msg;

	Exception(const std::string & msg){
		this->msg = msg;
	}

	const char * what () const throw () {
		return msg.c_str();
    }
};

inline void err(const std::string & msg, uint32_t line, uint32_t column){
	std::stringstream output;
	output << "\e[0;31m" << msg << " at " << line << ":" << column;

	throw Exception(output.str());
}

#endif