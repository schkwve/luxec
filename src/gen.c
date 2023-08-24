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
#include <codegen.h>
#include <gen.h>

int gen_ast(struct ast_node *node, int reg)
{
	int left_reg;
	int right_reg;

	if (node->left) {
		left_reg = gen_ast(node->left, -1);
	}
	if (node->right) {
		right_reg = gen_ast(node->right, left_reg);
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
		return cgloadint(node->v.int_val);
	case A_IDENT:
		return cgloadglob(Gsym[node->v.id].name);
	case A_LVIDENT:
		return cgstoreglob(reg, Gsym[node->v.id].name);
	case A_ASSIGN:
		return right_reg;
	default:
		fprintf(stderr, "Unknown AST operator %d\n", node->op);
		exit(1);
	}
}