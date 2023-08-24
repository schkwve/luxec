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

#include <data.h>

#include <misc.h>
#include <scanner.h>

void match(int t, char *what)
{
	if (Token.token == t) {
		scan(&Token);
	} else {
		printf("%s expected on line %d\n", what, Line);
		exit(1);
	}
}

void semi(void)
{
	match(T_SEMI, ";");
}