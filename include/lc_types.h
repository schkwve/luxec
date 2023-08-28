/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __LC_TYPES_H_
#define __LC_TYPES_H_

int type_compat(int *left, int *right, int only_right);
struct ast_node *modify_type(struct ast_node *tree, int rtype, int op);

int val_at(int type);
int pointer_to(int type);

#endif /* __LC_TYPES_H_ */