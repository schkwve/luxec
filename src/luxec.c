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
#include <stdlib.h>

#include <def.h>

#define extern_
#include <data.h>
#undef extern_

#include <ast.h>
#include <expr.h>
#include <gen.h>
#include <interp.h>
#include <scanner.h>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s file.lc\n", argv[0]);
		exit(-1);
	}

	Line = 1;
	Putback = '\n';

	InFile = fopen(argv[1], "r");

	OutFile = fopen("out.asm", "w");

	struct ast_node *node;

	scan(&Token);
	node = binexpr(0);
	printf("%d\n", interpret_ast(node));
	generate_code(node);

	return 0;
}