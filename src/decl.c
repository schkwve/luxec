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

#include <statement.h>
#include <codegen.h>
#include <misc.h>
#include <sym.h>

void var_declar(void)
{
	match(T_INT, "int");

	ident();
	addglob(Text);
	gen_globsym(Text);
	semi();
}