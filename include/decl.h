/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __DECL_H_
#define __DECL_H_

int parse_type(void);

void global_declar(void);
void var_declar(int type);
struct ast_node *func_declar(int type);

#endif /* __DECL_H_ */