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

#include <codegen.h>
#include <lc_types.h>
#include <misc.h>

int type_compat(int *left, int *right, int only_right)
{
	int left_size;
	int right_size;

	if (*left == *right) {
		*left = 0;
		*right = 0;
		return 1;
	}

	left_size = gen_primesize(*left);
	right_size = gen_primesize(*right);

	if ((left_size == 0) || (right_size == 0)) {
		return 0;
	}

	if (left_size < right_size) {
		*left = A_WIDEN;
		*right = 0;
		return 1;
	}

	if (right_size < left_size) {
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

int val_at(int type)
{
	int new_type = -1;
	switch (type) {
	case P_VOIDPTR:
		new_type = P_VOID;
		break;
	case P_CHARPTR:
		new_type = P_CHAR;
		break;
	case P_INTPTR:
		new_type = P_INT;
		break;
	case P_LONGPTR:
		new_type = P_LONG;
		break;
	default:
		fatald("Unrecognized in val_at: type", type);
		break;
	}

	return new_type;
}

int pointer_to(int type)
{
	int new_type = -1;
	switch (type) {
	case P_VOID:
		new_type = P_VOIDPTR;
		break;
	case P_CHAR:
		new_type = P_CHARPTR;
		break;
	case P_INT:
		new_type = P_INTPTR;
		break;
	case P_LONG:
		new_type = P_LONGPTR;
		break;
	default:
		fatald("Unrecognized in pointer_to: type", type);
	}

	return new_type;
}