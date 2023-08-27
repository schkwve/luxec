/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __EXPR_H_
#define __EXPR_H_

int arith_op(int tok);
struct ast_node *func_call(void);
struct ast_node *binexpr(int ptp);

#endif /* __EXPR_H_ */