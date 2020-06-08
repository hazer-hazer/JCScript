#ifndef VALUE_H
#define VALUE_H

/**
 * The objects storing in scope rules
 *
 * Functions:
 * example: func print(str: string): void {}
 * converted: "func_print(string)"
 * Note: function return type is cannot be overloaded so it doesn't matter for storing
 */


/**
 * get - get from THIS scope
 * define - var/val/func and etc. that defines new name in the scope
 * set - assignes (but not defines) the value to something
 * lookup - get the nearest scope where something exists
 */

class Object;
class Var;
class Func;

class Scope {
	public:
		Scope();
		virtual ~Scope() = default;

		Scope * get_parent();

		Var * get_var(const std::string & var_name){
			if(locals.contains(var_name)){
				return locals.at(var_name);
			}else{
				return nullptr;
			}
		}

		Var * define_var(const NVarDecl & var_decl){
			// Note: Variable only needs a name of var, but not like a func where structure has sense

			if(locals.contains(var_decl.id.name)){
			}else{
				// Note: Variables stores with "var_" prefix for var and val together
				return locals[var_decl.id.name] = new Var(var_decl);
			}
		}

		// Func * get_func(const std::string & func_structure){
		// 	if(locals.contains(func_structure)){
		// 		return locals.at(func_structure);
		// 	}else{
		// 		return nullptr;
		// 	}
		// }

		// Func * define_func(const NFuncDecl & func_decl){
		// 	std::string func_structure = "func_";
		// 	func_structure += func_decl.id.name;

		// 	func_structure += "(";
		// 	for(NArgDecl * arg : func_decl.args){
		// 		func_structure += arg.type->to_storage_structure();
		// 	}
		// 	func_structure += ")";
		// }

	private:
		Scope * parent;

		std::map <std::string, Object*> locals;

};

/**
 * Thoughts
 *
 * All primitives are objects (classes)
 *
 * Classes must have:
 * - constructors
 * - fields:
 *   * var/val
 *   * functions
 * All classes inherit `any`
 * All primitives are lower-case (not like in Kotlin)
 *
 * The problems:
 * - How to implement functions extensions? How to store them and etc.
 */

// Note: Object is not something like Object in Java
// It's just a main class to project everything (var/val, func, objects and etc.)

class Object : Scope {
	public:
		Object();

		virtual ~Object() = default;

		//////////////////////
		// Object functions //
		//////////////////////

		virtual bool to_bool(){
			return true;
		}

	private:

};

class Func : Object {

};

class Int : Object {
	public:
		Int(NInt & value) : value(value) {}

		virtual ~Int() = default;

	private:
		NInt & value;
};

class Float : Object {
	public:
		Float(NFloat & value) : value(value) {}

		virtual ~Float() = default;

	private:
		NFloat & value;
};

class Bool : Object {
	public:
		Bool(NBool & value) : value(value) {}

		virtual ~Bool() = default;

	private:
		NBool & value;
};

class String : Object {
	public:
		String(NString & value) : value(value) {}

		virtual ~String() = default;

	private:
		NString & value;
};

#endif