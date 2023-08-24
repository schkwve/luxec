/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __DEF_H_
#define __DEF_H_

enum {
	T_PLUS,
	T_MINUS,
	T_STAR,
	T_SLASH,
	T_INTLIT
};

struct token {
	int token;
	int int_val;
};

#endif /* __DEF_H_ */