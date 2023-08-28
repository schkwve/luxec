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

#define NOREG -1

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
	A_ASSIGN,
	A_PRINT,
	A_GLUE,
	A_IF,
	A_WHILE,

	A_ADDR,
	A_DEREF,

	A_FUNC,
	A_FUNCCALL,
	A_RETURN,

	A_WIDEN
};

// token types
enum {
	T_EOF,

	// Operators
	T_PLUS,
	T_MINUS,
	T_STAR,
	T_SLASH,
	T_AMPER,
	T_LOGAND,

	// Equality
	T_EQ,
	T_NE,
	T_LT,
	T_GT,
	T_LE,
	T_GE,

	// Misc
	T_INTLIT,
	T_SEMI,
	T_ASSIGN,
	T_IDENT,
	T_LBRACE,
	T_RBRACE,
	T_LPAREN,
	T_RPAREN,
	T_COMMA,

	// Keywords
	T_PRINT,

	T_CHAR,
	T_INT,
	T_LONG,
	T_VOID,

	T_RETURN,

	T_IF,
	T_ELSE,
	T_WHILE,
	T_FOR
};

// Primitive types
enum {
	P_NONE,
	P_VOID,
	P_CHAR,
	P_INT,
	P_LONG,
	P_VOIDPTR,
	P_CHARPTR,
	P_INTPTR,
	P_LONGPTR
};

// Structural types
enum { S_VAR, S_FUNC };

// AST structure
struct ast_node {
	int op;
	int type;

	struct ast_node *left;
	struct ast_node *mid;
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
	int type;
	int stype;
	int end_label;
};

#endif /* __DEF_H_ */