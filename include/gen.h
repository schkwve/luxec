/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __GEN_H_
#define __GEN_H_

#include <ast.h>

int gen_ast(struct ast_node *node, int reg, int parent_ast_op);

#endif /* __GEN_H_ */