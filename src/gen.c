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

#include <ast.h>
#include <codegen.h>
#include <gen.h>

static int gen_ast(struct ast_node *node)
{
	int left_reg;
	int right_reg;

	if (node->left) {
		left_reg = gen_ast(node->left);
	}
	if (node->right) {
		right_reg = gen_ast(node->right);
	}

	switch (node->op) {
	case A_ADD:
		return cgadd(left_reg, right_reg);
	case A_SUBTRACT:
		return cgsub(left_reg, right_reg);
	case A_MULTIPLY:
		return cgmul(left_reg, right_reg);
	case A_DIVIDE:
		return cgdiv(left_reg, right_reg);
	case A_INTLIT:
		return cgload(node->int_val);
	default:
		fprintf(stderr, "Unknown AST operator %d\n", node->op);
		exit(1);
	}
}

void generate_code(struct ast_node *node)
{
	int reg;

	cg_preamble();
	reg = gen_ast(node);
	cg_printint(reg);
	cg_postamble();
}