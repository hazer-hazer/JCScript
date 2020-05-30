#ifndef SCOPE_H
#define SCOPE_H

#include <map>
#include "Value.h"
#include "err.h"

class Scope {
	public:
		Scope(Scope * parent, const std::string & name);
		virtual ~Scope() = default;

		Scope * get_parent();

		///////////////
		// Variables //
		///////////////
		VVariable * set_local_var(const std::string & name, VVariable * val);
		VVariable * get_local_var(const std::string & name);
		VVariable * define_var(const std::string & name, VVariable * val);
		VVariable * set_var(const std::string & name, VVariable * val);
		Scope * lookup_var(const std::string & name);

		///////////
		// Types //
		///////////
		VType * get_local_type(const std::string & name);
		VType * define_type(const std::string & name, VType & val);
		Scope * lookup_type(const std::string & name);



	private:
		std::string name;
		Scope * parent;

		std::map <std::string, Value*> locals;
};

#endif