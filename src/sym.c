/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <string.h>
#include <stdio.h>

#include <def.h>
#include <data.h>

#include <decl.h>
#include <misc.h>
#include <sym.h>

static int Globs = 0;

static int newglob(void)
{
	int p;

	if ((p = Globs++) >= SYM_COUNT)
		fatal("Too many global symbols");
	return (p);
}

int addglob(char *name)
{
	int y;

	if ((y = findglob(name)) != -1)
		return (y);

	y = newglob();
	Gsym[y].name = strdup(name);
	return (y);
}

int findglob(char *s)
{
	int i;

	for (i = 0; i < Globs; i++) {
		if (*s == *Gsym[i].name && !strcmp(s, Gsym[i].name))
			return (i);
	}
	return (-1);
}