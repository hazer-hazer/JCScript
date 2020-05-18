#ifndef ERROR_H
#define ERROR_H

struct Exception : std::exception {
	std::string msg;

	ParsingException(const std::string & msg){
		this->msg = msg;
	}

	const char * what () const throw () {
		return msg.c_str();
    }
};

inline void error(const std::string & msg, unsigned int line, unsigned int column){
	std::stringstream output;
	output << "\e[0;31m" << message << " at " << line << ":" << column;

	throw Exception(output.str());
}

#endif