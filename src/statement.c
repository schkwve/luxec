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

struct ast_node *compound_statement(void)
{
	struct ast_node *left = NULL;
	struct ast_node *tree;

	lbrace();

	while (1) {
		switch (Token.token) {
		case T_PRINT:
			tree = print_statement();
			break;
		case T_INT:
			var_declar();
			tree = NULL;
			break;
		case T_IDENT:
			tree = assign_statement();
			break;
		case T_IF:
			tree = if_statement();
			break;
		case T_RBRACE:
			rbrace();
			return left;
		default:
			fatald("Syntax error: %s", Token.token);
		}

		if (tree) {
			if (left == NULL) {
				left = tree;
			} else {
				left = make_ast_node(A_GLUE, left, NULL, tree, 0);
			}
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

	if ((id = findglob(Text)) == -1) {
		fatals("Undeclared variable", Text);
	}
	right = make_ast_leaf(A_LVIDENT, id);

	match(T_ASSIGN, "=");

	left = binexpr(0);
	tree = make_ast_node(A_ASSIGN, left, NULL, right, 0);

	semi();

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

	return make_ast_node(A_IF, cond_ast, true_ast, false_ast, 0);
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

	return make_ast_node(A_WHILE, cond_ast, NULL, body_ast, 0);
}

struct ast_node *print_statement(void)
{
	struct ast_node *tree;

	match(T_PRINT, "print");

	tree = binexpr(0);
	tree = make_ast_unary(A_PRINT, tree, 0);

	semi();

	return tree;
}