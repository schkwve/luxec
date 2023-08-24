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

#include <ast.h>
#include <interp.h>

int interpret_ast(struct ast_node *node)
{
	int left_val;
	int right_val;

	if (node->left) {
		left_val = interpret_ast(node->left);
	}
	if (node->right) {
		right_val = interpret_ast(node->right);
	}

	switch (node->op) {
	case A_ADD:
		return (left_val + right_val);
		break;
	case A_SUBTRACT:
		return (left_val - right_val);
		break;
	case A_MULTIPLY:
		return (left_val * right_val);
		break;
	case A_DIVIDE:
		return (left_val / right_val);
		break;
	case A_INTLIT:
		return node->int_val;
		break;
	default:
		fprintf(stderr, "Unknown AST oprator %d\n", node->op);
		exit(1);
	}
}