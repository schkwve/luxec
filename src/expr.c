/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <stdlib.h>
#include <stdio.h>

#include <def.h>
#include <data.h>

#include <ast.h>
#include <expr.h>
#include <scanner.h>
#include <misc.h>
#include <sym.h>
#include <lc_types.h>

static int op_prec[] = { 0, 30, 30, 40, 40, 10, 10, 20, 20, 20, 20 };

static struct ast_node *primary(void)
{
	struct ast_node *node = NULL;
	int id;

	switch (Token.token) {
	case T_INTLIT:
		if (Token.int_val >= 0 && Token.int_val < 256) {
			node = make_ast_leaf(A_INTLIT, P_CHAR, Token.int_val);
		} else {
			node = make_ast_leaf(A_INTLIT, P_INT, Token.int_val);
		}
		break;
	case T_IDENT:
		scan(&Token);
		if (Token.token == T_LPAREN) {
			return func_call();
		}
		reject_token(&Token);

		id = findglob(Text);
		if (id == -1) {
			fatals("Unknown variable", Text);
		}

		node = make_ast_leaf(A_IDENT, Gsym[id].type, id);
		break;
	default:
		fatald("Syntax error, token", Token.token);
	}

	scan(&Token);
	return node;
}

static int op_precedence(int token_type)
{
	int prec = op_prec[token_type];
	if (prec == 0) {
		fatald("Syntax error, token", token_type);
	}

	return prec;
}

struct ast_node *prefix(void)
{
	struct ast_node *tree;

	switch (Token.token) {
	case T_AMPER:
		scan(&Token);
		tree = prefix();
		if (tree->op != A_IDENT) {
			fatal("& operator must be followed by an identifier");
		}

		tree->op = A_ADDR;
		tree->type = pointer_to(tree->type);
		break;
	case T_STAR:
		scan(&Token);
		tree = prefix();

		if (tree->op != A_IDENT && tree->op != A_DEREF) {
			fatal("* operator must be followed by an identifier or *");
		}

		tree = make_ast_unary(A_DEREF, val_at(tree->type), tree, 0);
		break;
	default:
		tree = primary();
	}
	return tree;
}

int arith_op(int tok)
{
	if (tok > T_EOF && tok < T_INTLIT) {
		return tok;
	}

	fatald("Syntax error, token", tok);
	return 0;
}

struct ast_node *func_call(void)
{
	struct ast_node *tree;
	int id = findglob(Text);

	if (id == -1) {
		fatals("Undeclared function", Text);
	}

	lparen();
	tree = binexpr(0);
	tree = make_ast_unary(A_FUNCCALL, Gsym[id].type, tree, id);
	rparen();

	return tree;
}

struct ast_node *binexpr(int ptp)
{
	struct ast_node *left;
	struct ast_node *right;
	struct ast_node *left_tmp;
	struct ast_node *right_tmp;
	int ast_op;
	int token_type;

	left = prefix();

	token_type = Token.token;
	if (token_type == T_SEMI || token_type == T_RPAREN) {
		return left;
	}

	while (op_precedence(token_type) > ptp) {
		scan(&Token);

		right = binexpr(op_prec[token_type]);

		ast_op = arith_op(token_type);
		left_tmp = modify_type(left, right->type, ast_op);
		right_tmp = modify_type(right, left->type, ast_op);
		if (left_tmp == NULL && right_tmp == NULL) {
			fatal("Incompatible types in binary expression");
		}
		if (left_tmp != NULL) {
			left = left_tmp;
		}
		if (right_tmp != NULL) {
			right = right_tmp;
		}

		left = make_ast_node(arith_op(token_type), left->type, left, NULL,
							 right, 0);

		token_type = Token.token;
		if (token_type == T_SEMI || token_type == T_RPAREN) {
			return left;
		}
	}

	return left;
}