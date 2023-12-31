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
#include <string.h>
#include <errno.h>

#include <def.h>

#define extern_
#include <data.h>
#undef extern_

#include <ast.h>
#include <expr.h>
#include <codegen.h>
#include <decl.h>
#include <gen.h>
#include <interp.h>
#include <scanner.h>
#include <statement.h>
#include <sym.h>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s file.lc\n", argv[0]);
		exit(-1);
	}

	InFile = fopen(argv[1], "r");
	if (InFile == NULL) {
		fprintf(stderr, "Unable to open '%s': %s\n", argv[1], strerror(errno));
		exit(1);
	}

	OutFile = fopen("out.s", "w");
	if (OutFile == NULL) {
		fprintf(stderr, "Unable to open 'out.s': %s\n", strerror(errno));
		exit(1);
	}

	Line = 1;
	Putback = '\n';

	// ensure printint() is defined
	addglob("printint", P_CHAR, S_FUNC, 0);

	scan(&Token);
	gen_preamble();
	global_declar();
	gen_postamble();

	fclose(InFile);
	fclose(OutFile);

	return 0;
}