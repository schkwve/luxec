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
#include <gen.h>
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
	if (t == T_LONG) {
		return P_LONG;
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
	id = addglob(Text, type, S_VAR, 0);
	gen_globsym(id);
	semi();
}

struct ast_node *func_declar(void)
{
	struct ast_node *tree;
	struct ast_node *final_statement;
	int name_slot;
	int type;
	int end_label;

	type = parse_type(Token.token);
	scan(&Token);
	ident();

	end_label = label();
	name_slot = addglob(Text, type, S_FUNC, end_label);
	FuncId = name_slot;

	lparen();
	rparen();

	tree = compound_statement();

	if (type != P_VOID) {
		final_statement = (tree->op == A_GLUE) ? tree->right : tree;
		if (final_statement == NULL || final_statement->op != A_RETURN) {
			fatal("No return for function with non-void type");
		}
	}

	return make_ast_unary(A_FUNC, type, tree, name_slot);
}