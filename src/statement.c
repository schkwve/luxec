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
#include <statement.h>
#include <sym.h>
#include <misc.h>

void statements(void)
{
	while (1) {
		switch (Token.token) {
		case T_PRINT:
			print_statement();
			break;
		case T_INT:
			var_declar();
			break;
		case T_IDENT:
			assign_statement();
			break;
		case T_EOF:
			return;
		default:
			fatald("Syntax error: %s", Token.token);
		}
	}
}

void assign_statement(void)
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
	tree = make_ast_node(A_ASSIGN, left, right, 0);

	gen_ast(tree, -1);
	gen_freeregs();

	semi();
}

void print_statement(void)
{
	struct ast_node *tree;
	int reg;

	match(T_PRINT, "print");

	tree = binexpr(0);
	reg = gen_ast(tree, -1);
	gen_printint(reg);
	gen_freeregs();

	semi();
}

void semi(void)
{
	match(T_SEMI, ";");
}

void ident(void)
{
	match(T_IDENT, "identifier");
}