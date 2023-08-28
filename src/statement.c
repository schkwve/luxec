/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <stdio.h>

#include <def.h>
#include <data.h>

#include <ast.h>
#include <expr.h>
#include <decl.h>
#include <codegen.h>
#include <gen.h>
#include <scanner.h>
#include <statement.h>
#include <sym.h>
#include <misc.h>
#include <lc_types.h>

struct ast_node *single_statement(void)
{
	int type;
	switch (Token.token) {
	case T_PRINT:
		return print_statement();
	case T_CHAR:
	case T_INT:
	case T_LONG:
		type = parse_type();
		ident();
		var_declar(type);
		return NULL;
	case T_IDENT:
		return assign_statement();
	case T_IF:
		return if_statement();
	case T_WHILE:
		return while_statement();
	case T_FOR:
		return for_statement();
	case T_RETURN:
		return return_statement();
	default:
		fatald("Syntax error, token", Token.token);
		break;
	}

	return NULL;
}

struct ast_node *compound_statement(void)
{
	struct ast_node *left = NULL;
	struct ast_node *tree;

	lbrace();

	while (1) {
		tree = single_statement();

		if (tree != NULL && (tree->op == A_PRINT || tree->op == A_ASSIGN ||
							 tree->op == A_RETURN || tree->op == A_FUNCCALL)) {
			semi();
		}

		if (tree != NULL) {
			if (left == NULL) {
				left = tree;
			} else {
				left = make_ast_node(A_GLUE, P_NONE, left, NULL, tree, 0);
			}
		}

		if (Token.token == T_RBRACE) {
			rbrace();
			return left;
		}
	}
}

struct ast_node *assign_statement(void)
{
	struct ast_node *left;
	struct ast_node *right;
	struct ast_node *tree;
	int id;

	ident();

	if (Token.token == T_LPAREN) {
		return func_call();
	}

	id = findglob(Text);
	if (id == -1) {
		fatals("Undeclared variable", Text);
	}

	right = make_ast_leaf(A_LVIDENT, Gsym[id].type, id);

	match(T_ASSIGN, "=");

	left = binexpr(0);
	left = modify_type(left, right->type, 0);
	if (left == NULL) {
		fatal("Incompatible expression in assignment");
	}

	tree = make_ast_node(A_ASSIGN, P_INT, left, NULL, right, 0);

	return tree;
}

struct ast_node *if_statement(void)
{
	struct ast_node *cond_ast;
	struct ast_node *true_ast;
	struct ast_node *false_ast = NULL;

	match(T_IF, "if");
	lparen();

	cond_ast = binexpr(0);
	if (cond_ast->op < A_EQ || cond_ast->op > A_GE) {
		fatal("Bad comparison operator");
	}

	rparen();

	true_ast = compound_statement();

	if (Token.token == T_ELSE) {
		scan(&Token);
		false_ast = compound_statement();
	}

	return make_ast_node(A_IF, P_NONE, cond_ast, true_ast, false_ast, 0);
}

struct ast_node *while_statement(void)
{
	struct ast_node *cond_ast;
	struct ast_node *body_ast;

	match(T_WHILE, "while");

	lparen();

	cond_ast = binexpr(0);
	if (cond_ast->op < A_EQ || cond_ast->op > A_GE) {
		fatal("Bad comparison operator");
	}

	rparen();

	body_ast = compound_statement();

	return make_ast_node(A_WHILE, P_NONE, cond_ast, NULL, body_ast, 0);
}

struct ast_node *for_statement(void)
{
	struct ast_node *cond_ast;
	struct ast_node *body_ast;
	struct ast_node *preop_ast;
	struct ast_node *postop_ast;
	struct ast_node *tree;

	match(T_FOR, "for");
	lparen();

	preop_ast = single_statement();
	semi();

	cond_ast = binexpr(0);
	if (cond_ast->op < A_EQ || cond_ast->op > A_GE) {
		fatal("Bad comparison operator");
	}

	semi();

	postop_ast = single_statement();
	rparen();

	body_ast = compound_statement();

	tree = make_ast_node(A_GLUE, P_NONE, body_ast, NULL, postop_ast, 0);
	tree = make_ast_node(A_WHILE, P_NONE, cond_ast, NULL, tree, 0);

	return make_ast_node(A_GLUE, P_NONE, preop_ast, NULL, tree, 0);
}

struct ast_node *print_statement(void)
{
	struct ast_node *tree;

	match(T_PRINT, "print");

	tree = binexpr(0);
	tree = modify_type(tree, P_INT, 0);
	if (tree == NULL) {
		fatal("Incompatible type to print");
	}

	tree = make_ast_unary(A_PRINT, P_NONE, tree, 0);

	return tree;
}

struct ast_node *return_statement(void)
{
	struct ast_node *tree;

	if (Gsym[FuncId].type == P_VOID) {
		fatal("Can't return from a void function");
	}

	match(T_RETURN, "return");
	lparen();

	tree = binexpr(0);
	tree = modify_type(tree, Gsym[FuncId].type, 0);
	if (tree == NULL) {
		fatal("Incompatible type to return");
	}

	tree = make_ast_unary(A_RETURN, P_NONE, tree, 0);
	rparen();

	return tree;
}