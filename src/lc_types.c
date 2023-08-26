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

#include <def.h>
#include <data.h>

#include <lc_types.h>

int type_compat(int *left, int *right, int only_right)
{
	if ((*left == P_VOID) || (*right == P_VOID)) {
		return 0;
	}

	if (*left == *right) {
		*left = 0;
		*right = 0;
		return 1;
	}

	if ((*left == P_CHAR) && (*right == P_INT)) {
		*left = A_WIDEN;
		*right = 0;
		return 1;
	}
	if ((*left == P_INT) && (*right == P_CHAR)) {
		if (only_right) {
			return 0;
		}
		*left = 0;
		*right = A_WIDEN;
		return 1;
	}

	*left = 0;
	*right = 0;
	return 1;
}