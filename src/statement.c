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

#include <data.h>
#include <def.h>

#include <ast.h>
#include <expr.h>
#include <codegen.h>
#include <gen.h>
#include <misc.h>

void statements(void)
{
	struct ast_node *tree;
	int reg;

	while (1) {
		match(T_PRINT, "print");

		tree = binexpr(0);
		reg = gen_ast(tree);

		gen_printint(reg);
		gen_freeregs();

		semi();
		if (Token.token == T_EOF) {
			return;
		}
	}
}