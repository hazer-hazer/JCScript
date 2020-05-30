#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Value.h"

class Interpreter {
	public:
		Interpreter();
		virtual ~Interpreter() = default;

		void interpret(const StatementList & statements);

	private:


};

#endif