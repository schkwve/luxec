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

static int op_prec[] = { 10, 10, 20, 20, 0, 0 };
// +, -, *, /, INTLIT, EOF

static struct ast_node *primary(void)
{
	struct ast_node *node = NULL;
	int id;

	switch (Token.token) {
	case T_INTLIT:
		node = make_ast_leaf(A_INTLIT, Token.int_val);
		break;
	case T_IDENT:
		id = findglob(Text);
		if (id == -1) {
			fatals("Unknown variable", Text);
		}

		node = make_ast_leaf(A_IDENT, id);
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
		fprintf(stderr, "Syntax error on line %d: %d\n", Line, token_type);
		exit(1);
	}

	return prec;
}

int arith_op(int tok)
{
	switch (tok) {
	case T_PLUS:
		return A_ADD;
	case T_MINUS:
		return A_SUBTRACT;
	case T_STAR:
		return A_MULTIPLY;
	case T_SLASH:
		return A_DIVIDE;
	default:
		fprintf(stderr, "arithop(): Unknown token on line '%d'\n", Line);
		exit(1);
	}
}

struct ast_node *binexpr(int ptp)
{
	struct ast_node *left;
	struct ast_node *right;
	int token_type;

	left = primary();

	token_type = Token.token;
	if (token_type == T_SEMI) {
		return left;
	}

	while (op_precedence(token_type) > ptp) {
		scan(&Token);

		right = binexpr(op_prec[token_type]);
		left = make_ast_node(arith_op(token_type), left, right, 0);

		token_type = Token.token;
		if (token_type == T_SEMI) {
			return left;
		}
	}

	token_type = Token.token;
	if (token_type == T_SEMI) {
		return left;
	}

	return left;
}