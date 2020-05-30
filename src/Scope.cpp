#include "Scope.h"

Scope::Scope(Scope * parent, const std::string & name){
	this->parent = parent;
	this->name = name;
}
/**
 * Notes:
 * - Set local: Set something actually in scope locals
 * - Define: defines something(var/func and etc.) in current scope and sets value if it's not nullptr
 * - Set: Set value to something that was defined
 * - Get: Get something from current scope
 * - Lookup: Looks up for the first appearance of given something
 *
 * Variables and functions have different behaviour in search, because variable just points to
 * identifier, while function is something more complex with return type, argument list with types
 * and etc.
 */

///////////////
// Variables //
///////////////

VVariable * Scope::set_local_var(const std::string & name, VVariable * val){
	if(!locals.contains(name)){
		val->error("Variable " + name + " is not defined");
	}else if(!dynamic_cast<VVariable>(locals[name])){
		val->error(name + " is not a variable");
	}
	return locals[name] = val;
}


VVariable * Scope::define_var(const std::string & name, VVariable * val){
	if(locals.contains(name)){
		val->error(name + " has been already declared");
	}
	return locals[name] = val;
}

VVariable * Scope::set_var(const std::string & name, VVariable * val){
	Scope * lookup_scope = lookup_var(name);

	if(!lookup_scope){
		val->error("Variable " + name + " is not defined");
	}

	lookup_scope->set_local_var(name, val);

	return val;
}

VVariable * Scope::get_local_var(const std::string & name){
	for(const auto & local : locals){
		if(local.first == name && VVariable * var = dynamic_cast<VVariable*>(local.second)){
			return var;
		}
	}
	return nullptr;
}

Scope * Scope::lookup_var(const sts::string & name){
	for(const auto & local : locals){
		if(local.first == name){
			return this;
		}
	}
	if(parent){
		return parent->lookup(name);
	}
	return nullptr;
}

///////////
// Types //
///////////

// Note: Type cannot be reasigned or defined without value

// VType * Scope::get_local_type(const std::string & name){
// 	for(const auto & local : locals){
// 		if(local.first == name && VType * type = dynamic_cast <VType*>(local.second)){
// 			return type;
// 		}
// 	}
// 	return nullptr;
// }

// VType * Scope::define_type(const std::string & name){
// 	if(locals.contains(name)){
// 		val->error("Variable " + name + " has been already declared");
// 	}
// 	return locals[name] = val;
// }