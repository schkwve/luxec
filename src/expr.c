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

#include <data.h>
#include <def.h>

#include <ast.h>
#include <expr.h>

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

struct ast_node *additive_expr(void)
{
	struct ast_node *left;
	struct ast_node *right;
	int token_type;

	left = multiplicative_expr();

	token_type = Token.token;
	if (token_type == T_EOF) {
		return left;
	}

	while (1) {
		scan(&Token);

		right = multiplicative_expr();
		left = make_ast_node(arith_op(token_type), left, right, 0);

		token_type = Token.token;
		if (token_type == T_EOF) {
			break;
		}

		return left;
	}
}

struct ast_node *multiplicative_expr(void)
{
	struct ast_node *left;
	struct ast_node *right;
	int token_type;

	left = primary();

	token_type = Token.token;
	if (token_type == T_EOF) {
		return left;
	}

	while ((token_type == T_STAR) || (token_type == T_SLASH)) {
		scan(&Token);
		right = primary();

		left = make_ast_node(arith_op(token_type), left, right, 0);

		token_type = Token.token;
		if (token_type == T_EOF) {
			break;
		}
	}

	return left;
}