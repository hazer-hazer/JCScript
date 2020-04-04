#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <sstream>

class Object {
	
	#define OBJ_CLASS(m_class) \
	public: \
		virtual std::string get_class(){ return #m_class; }

	OBJ_CLASS(Object);

	public:
		Object(){};
		virtual ~Object() = default;

	public:
		virtual void print(const std::string & message){
			std::cout << get_class() + ": " + message << std::endl;
		}

		virtual void error(const std::string & message){
			throw get_class() + ": " + "\e[0;31m" + message;
		}
};

#endif