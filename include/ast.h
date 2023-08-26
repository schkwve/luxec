/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __AST_H_
#define __AST_H_

#include <def.h>

struct ast_node *make_ast_node(int op, int type, struct ast_node *left,
							   struct ast_node *mid, struct ast_node *right,
							   int int_val);
struct ast_node *make_ast_leaf(int op, int type, int int_val);
struct ast_node *make_ast_unary(int op, int type, struct ast_node *left,
								int int_val);

#endif /* __AST_H_ */