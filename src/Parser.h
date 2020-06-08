#ifndef PARSER_H
#define PARSER_H

#include <map>

#include "Node.h"

const std::map <Operator, int> OP_INFIX_PREC {
	{OP_PIPELINE, 2},

	{OP_ASSIGN, 3},
	{OP_ASSIGN_ADD, 3}, {OP_ASSIGN_SUB, 3}, {OP_ASSIGN_MUL, 3},
	{OP_ASSIGN_DIV, 3}, {OP_ASSIGN_MOD, 3}, {OP_ASSIGN_EXP, 3},

	{OP_ASSIGN_BIT_AND, 3}, {OP_ASSIGN_BIT_OR, 3},
	{OP_ASSIGN_BIT_XOR, 3}, {OP_ASSIGN_SHIFT_LEFT, 3}, {OP_ASSIGN_SHIFT_RIGHT, 3},

	{OP_OR, 5},
	{OP_AND, 6},
	{OP_BIT_OR, 8},
	{OP_BIT_XOR, 9},
	{OP_BIT_AND, 10},
	{OP_EQUAL, 11}, {OP_NOT_EQUAL, 11},

	{OP_LESS, 12}, {OP_LESS_EQUAL, 12}, {OP_GREATER, 12},
	{OP_GREATER_EQUAL, 12}, {OP_SPACESHIP, 12},

	{OP_IN, 13}, {OP_NOT_IN, 13}, {OP_IS, 13, OP_NOT_IS, 13},

	{OP_ELVIS, 14},

	{OP_SHIFT_LEFT, 15}, {OP_SHIFT_RIGHT, 15},

	{OP_RANGE, 16}, {OP_RANGE_INCL, 16},
	
	{OP_ADD, 17}, {OP_SUB, 17},
	{OP_MUL, 18}, {OP_DIV, 18}, {OP_MOD, 18},

	{OP_EXP, 19},

	{OP_AS, 20}, {OP_AS_NULLABLE, 20},

	{OP_MEMBER_ACCESS, 25}
};

const std::map <Operator, int> OP_PREFIX_PREC {
	{OP_SPREAD, 1},
	{OP_INC, 21}, {OP_DEC, 21}, {OP_ADD, 21}, {OP_SUB, 21},
	{OP_BIT_INVERT, 21}, {OP_NOT, 21}
};

const std::map <Operator, int> OP_POSTFIX_PREC {
	{OP_INC, 22}, {OP_DEC, 22}
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
		bool is_typeof(const TokenType & t);
		bool is_id();
		bool is_num();
		bool is_str();
		bool is_endl();
		bool is_op();
		bool is_kw();
		bool is_expr_end();

		bool is_op(const Operator & op);
		bool is_kw(const Keyword & kw);

		bool is_infix_op();
		bool is_prefix_op();
		bool is_postfix_op();

		// Skippers
		void skip_endl(const bool & optional = false);
		void skip_op(const Operator & op, const bool & skip_left_endl, const bool & skip_right_endl);
		void skip_kw(const Keyword & kw, const bool & skip_left_endl, const bool & skip_right_endl);

		bool optional_expr_end = false;
		void skip_expr_end(const bool & optional = false);

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
		NExpression * parse_atom();

		// TODO: Add `one_line` parameter that will set if block can be one line
		NBlock * parse_block();

		// Maybe's
		bool allow_func_call = false;
		// bool allow_list_access = false; // ????
		NExpression * maybe_infix(NExpression * left, int prec);
		NExpression * maybe_call(NExpression * left);
		NExpression * maybe_list_access(NExpression * left);

		// Value
		NIdentifier * parse_identifier();

		// List
		NListAccess * parse_list_access(NExpression * left);
		
		// Type
		NType * parse_type();
		NListType * parse_list_type();
		NTupleType * parse_tuple_type();

		// Type declaration
		NTypeDecl * parse_type_decl();

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

		// Match
		NMatch * parse_match();
};

#endif