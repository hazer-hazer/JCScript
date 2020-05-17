#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <map>
#include <functional>
#include "Node.h"

const std::map <std::string, uint8_t> PREC_BIN {
	{"=", 3},
	{":", 4},
	{"||", 5},
	{"&&", 6},
	{"==", 10}, {"!=", 10},
	{"<", 11}, {">", 11}, {"<=", 11}, {">=", 11},
	{"+", 13}, {"-", 13},
	{"*", 14}, {"/", 14}
};

const std::map <std::string, uint8_t> PREC_PRE {};
const std::map <std::string, uint8_t> PREC_POST {};

class Parser {
	public:
		Parser();
		virtual ~Parser() = default;

		Tree parse(const std::vector <Token> & tokens);

	private:
		Tree tree;

		std::vector <Token> tokens;

		uint32_t index = 0;
		Node * current;
		Node * peek();
		Node * advance();
		bool advanced = true;
		bool prog_end();

		// Check if current token is type of op/kw and val equals given
		bool is_op(const std::string & op);
		bool is_kw(const std::string & kw);

		bool is_bin_op();

		void skip(const TokenType & type,
				  const std::string & val,
				  const bool & optional = false,
				  const bool & infinite = false);
		void skip_op(const std::string & op, const bool & optional = false);
		void skip_kw(const std::string & kw, const bool & optional = false);
		void skip_endl(const bool & endl = false);

		// Errors
		void error(const std::string & msg);
		void expected_error(const std::string & expected, const std::string & given);
		void expected_error(const std::string & expected);
		void unexpected_error();

		// Parsers
		Node * parse_expression();
		Tree parse_delimited(const std::string & begin,
							 const std::string & end,
							 const std::string & sep);
		Node * parse_atom();
		
		VarNode * parse_var();

		FuncNode * parse_func();
		ScopeNode * parse_scope();
		CallNode * parse_call(Node * func_name);
		Tree parse_args();
		ArrayNode * parse_array();
		ConditionNode * parse_condition();
		WhileNode * parse_while();
		ForNode * parse_for();

		Node * maybe_call(const std::function <void*()> & expression);
		Node * maybe_binary(Node * left, const uint8_t & prec);
};

#endif