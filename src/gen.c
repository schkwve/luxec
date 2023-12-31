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
#include <misc.h>

static int gen_if_ast(struct ast_node *node)
{
	int lfalse;
	int lend;

	lfalse = label();
	if (node->right) {
		lend = label();
	}

	gen_ast(node->left, lfalse, node->op);
	gen_freeregs();

	gen_ast(node->mid, NOREG, node->op);
	gen_freeregs();

	if (node->right) {
		cgjump(lend);
	}

	cglabel(lfalse);

	if (node->right) {
		gen_ast(node->right, NOREG, node->op);
		gen_freeregs();
		cglabel(lend);
	}

	return NOREG;
}

static int gen_while_ast(struct ast_node *node)
{
	int lstart;
	int lend;

	lstart = label();
	lend = label();
	cglabel(lstart);

	gen_ast(node->left, lend, node->op);
	gen_freeregs();

	gen_ast(node->right, NOREG, node->op);
	gen_freeregs();

	cgjump(lstart);
	cglabel(lend);

	return NOREG;
}

int label(void)
{
	static int id = 1;
	return id++;
}

int gen_ast(struct ast_node *node, int reg, int parent_ast_op)
{
	int left_reg;
	int right_reg;

	switch (node->op) {
	case A_IF:
		return gen_if_ast(node);
	case A_WHILE:
		return gen_while_ast(node);
	case A_GLUE:
		gen_ast(node->left, NOREG, node->op);
		gen_freeregs();
		gen_ast(node->right, NOREG, node->op);
		gen_freeregs();
		return NOREG;
	case A_FUNC:
		cgfuncpreamble(node->v.id);
		gen_ast(node->left, NOREG, node->op);
		cgfuncpostamble(node->v.id);
		return NOREG;
	}

	if (node->left)
		left_reg = gen_ast(node->left, NOREG, node->op);
	if (node->right)
		right_reg = gen_ast(node->right, left_reg, node->op);

	switch (node->op) {
	case A_ADD:
		return cgadd(left_reg, right_reg);
	case A_SUBTRACT:
		return cgsub(left_reg, right_reg);
	case A_MULTIPLY:
		return cgmul(left_reg, right_reg);
	case A_DIVIDE:
		return cgdiv(left_reg, right_reg);
	case A_EQ:
	case A_NE:
	case A_LT:
	case A_GT:
	case A_LE:
	case A_GE:
		if (parent_ast_op == A_IF || parent_ast_op == A_WHILE)
			return cgcompare_and_jump(node->op, left_reg, right_reg, reg);
		else
			return cgcompare_and_set(node->op, left_reg, right_reg);
	case A_INTLIT:
		return cgloadint(node->v.int_val, node->type);
	case A_IDENT:
		return cgloadglob(node->v.id);
	case A_LVIDENT:
		return cgstoreglob(reg, node->v.id);
	case A_ASSIGN:
		return right_reg;
	case A_PRINT:
		gen_printint(left_reg);
		gen_freeregs();
		return NOREG;
	case A_WIDEN:
		return cgwiden(left_reg, node->left->type, node->type);
	case A_SCALE:
		switch (node->v.size) {
		case 2:
			return cgshlconst(left_reg, 1);
		case 4:
			return cgshlconst(left_reg, 2);
		case 8:
			return cgshlconst(left_reg, 3);
		default:
			right_reg = cgloadint(node->v.size, P_INT);
			return cgmul(left_reg, right_reg);
		}
	case A_RETURN:
		cgreturn(left_reg, FuncId);
		return NOREG;
	case A_FUNCCALL:
		return cgcall(left_reg, node->v.id);
	case A_ADDR:
		return cgaddress(node->v.id);
	case A_DEREF:
		return cgderef(left_reg, node->left->type);
	default:
		fatald("Unknown AST operator", node->op);
	}

	return NOREG;
}