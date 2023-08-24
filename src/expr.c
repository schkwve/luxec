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

#include <expr.h>
#include <data.h>
#include <def.h>

int arith_op(int tok)
{
	switch (tok) {
	case T_PLUS:
		return A_ADD;
	case T_MINUS:
		return A_SUBTRACT;
	case T_STAR:
		return A_MULTIPLY;
	case T_SLASH:
		return A_DIVIDE;
	default:
		fprintf(stderr, "arithop(): Unknown token on line '%d'\n", Line);
		exit(1);
	}
}