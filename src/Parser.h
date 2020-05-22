#ifndef PARSER_H
#define PARSER_H

#include <map>

#include "Node.h"

const std::map <Operator, int> OP_INFIX_PREC {
	{OP_ASSIGN, 3}, {OP_ASSIGN_ADD, 3}, {OP_ASSIGN_SUB, 3},
	{OP_ASSIGN_MUL, 3}, {OP_ASSIGN_DIV, 3}, {OP_ASSIGN_MOD, 3},
	{OP_ASSIGN_EXP, 3}, {OP_ASSIGN_BIT_OR, 3}, {OP_ASSIGN_BIT_AND, 3},
	{OP_ASSIGN_BIT_XOR, 3}, {OP_ASSIGN_SHIFT_LEFT, 3}, {OP_ASSIGN_SHIFT_RIGHT, 3},

	{OP_OR, 5},
	{OP_AND, 6},
	{OP_NULL_COALESCING, 7},
	{OP_BIT_OR, 8},
	{OP_BIT_XOR, 9},
	{OP_BIT_AND, 10},
	{OP_EQUAL, 11}, {OP_NOT_EQUAL, 11},

	{OP_LESS, 12}, {OP_LESS_EQUAL, 12}, {OP_GREATER, 12},
	{OP_GREATER_EQUAL, 12}, {OP_SPACESHIP, 12},
	
	{OP_SHIFT_LEFT, 13}, {OP_SHIFT_RIGHT, 13},
	{OP_ADD, 14}, {OP_SUB, 14},
	{OP_MUL, 15}, {OP_DIV, 15}, {OP_MOD, 15},
	{OP_EXP, 16},

	{OP_MEMBER_ACCESS, 18}
};

const std::map <Operator, int> OP_PREFIX_PREC {
	{OP_SPREAD, 1},
	{OP_INC, 17}, {OP_DEC, 17}, {OP_ADD, 17}, {OP_SUB, 17},
	{OP_BIT_INVERT, 17}, {OP_NOT, 17},
};

const std::map <Operator, int> OP_POSTFIX_PREC {
	{OP_INC, 18}, {OP_DEC, 18}
};

class Parser {
	public:
		Parser();
		virtual ~Parser() = default;

		StatementList parse(const std::vector <Token> & tokens);

	private:
		StatementList tree;

		std::vector <Token> tokens;
		uint32_t index;
		bool eof();
		Token peek();
		Token advance();

		// Recognizers
		bool is_id();
		bool is_num();
		bool is_str();
		bool is_endl();
		bool is_op();
		bool is_kw();

		bool is_op(const Operator & op);
		bool is_kw(const Keyword & kw);

		// Skippers
		void skip_endl(const bool & optional = false);
		void skip_expr_end(const bool & optional = false);
		void skip_op(const Operator & op, const bool & skip_endl = true);
		void skip_kw(const Keyword & kw, const bool & skip_endl = true);

		// Errors
		void error(const std::string & msg);
		void expected_error(const std::string & expected, const std::string & given);
		void expected_error(const std::string & expected);
		void unexpected_error();

		// 
		// Parsers
		// 
		
		// Common
		NStatement * parse_statement();
		NExpression * parse_expression();
		NBlock * parse_block();

		// Value
		NIdentifier * parse_identifier();
		
		// Type
		NType * parse_type();

		// Variable
		NVarDecl * parse_var_decl();

		// Function
		NArgDecl * parse_arg_declaration();
		ArgList parse_arg_declaration_list();
		NFuncDecl * parse_func_decl();

		// Function call
		NFuncCall * parse_func_call(NExpression * left);

		// Condition
		NCondition * parse_condition();

		// While
		NWhile * parse_while();

		// For
		NFor * parse_for();
};

#endif