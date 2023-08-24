/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __SCANNER_H_
#define __SCANNER_H_

#include <def.h>

int scan_int(int c);
int scan(struct token *t);

#endif /* __SCANNER_H_ */