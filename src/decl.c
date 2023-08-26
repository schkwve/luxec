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
#include <decl.h>
#include <statement.h>
#include <codegen.h>
#include <misc.h>
#include <scanner.h>
#include <sym.h>

int parse_type(int t)
{
	if (t == T_CHAR) {
		return P_CHAR;
	}
	if (t == T_INT) {
		return P_INT;
	}
	if (t == T_VOID) {
		return P_VOID;
	}

	fatald("Illegal type, token", t);
	return 0;
}

void var_declar(void)
{
	int id;
	int type;

	type = parse_type(Token.token);
	scan(&Token);
	ident();
	id = addglob(Text, type, S_VAR);
	gen_globsym(id);
	semi();
}

struct ast_node *func_declar(void)
{
	struct ast_node *tree;
	int name_slot;

	match(T_VOID, "void");

	ident();
	name_slot = addglob(Text, P_VOID, S_FUNC);
	lparen();
	rparen();

	tree = compound_statement();

	return make_ast_unary(A_FUNC, P_VOID, tree, name_slot);
}