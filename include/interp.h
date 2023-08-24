/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __INTERP_H_
#define __INTERP_H_

#include <ast.h>

int interpret_ast(struct ast_node *node);

#endif /* __INTERP_H_ */