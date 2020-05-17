#ifndef ERROR_H
#define ERROR_H

#include <sstream>
#include <exception>

// TODO: Add specified custom exception
struct Exception : std::exception {
	std::string msg;

	Exception(const std::string & msg){
		this->msg = msg;
	}

	const char * what () const throw () {
		return msg.c_str();
    }
};

inline void err(const std::string & message, uint32_t line, uint32_t column){
	std::stringstream output;
	output << "\e[0;31m" << message << " at " << line << ":" << column;

	throw Exception(output.str());
}

#endif