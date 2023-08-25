/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __DEF_H_
#define __DEF_H_

// AST node types
enum {
	A_ADD = 1,
	A_SUBTRACT,
	A_MULTIPLY,
	A_DIVIDE,

	A_EQ,
	A_NE,
	A_LT,
	A_GT,
	A_LE,
	A_GE,
	
	A_INTLIT,
	A_IDENT,
	A_LVIDENT,
	A_ASSIGN
};

// token types
enum {
	T_EOF,
	
	// Operators
	T_PLUS,
	T_MINUS,
	T_STAR,
	T_SLASH,

	// Equality
	T_EQ,
	T_NE,
	T_LT,
	T_GT,
	T_LE,
	T_GE,

	// Keywords
	T_PRINT,

	// Variable types
	T_IDENT,
	T_LVIDENT,
	T_ASSIGN,

	T_INT,

	// Misc
	T_SEMI,
	T_INTLIT
};

// AST structure
struct ast_node {
	int op;
	struct ast_node *left;
	struct ast_node *right;
	union {
		int int_val;
		int id;
	} v;
};

// token structure
struct token {
	int token;
	int int_val;
};

struct symtable {
	char *name;
};

#endif /* __DEF_H_ */