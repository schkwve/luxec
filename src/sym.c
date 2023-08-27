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

static int newglob(void)
{
	int p = Globs++;

	if (p >= SYM_COUNT)
		fatal("Too many global symbols");
	return p;
}

int addglob(char *name, int type, int stype, int end_label)
{
	int y = findglob(name);

	if (y != -1)
		return y;

	y = newglob();
	Gsym[y].name = __strdup(name);
	Gsym[y].type = type;
	Gsym[y].stype = stype;
	Gsym[y].end_label = end_label;
	return y;
}

int findglob(char *s)
{
	int i;

	for (i = 0; i < Globs; i++) {
		if (*s == *Gsym[i].name && !strcmp(s, Gsym[i].name))
			return i;
	}
	return -1;
}