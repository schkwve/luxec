/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __STATEMENT_H_
#define __STATEMENT_H_

#include <ast.h>

struct ast_node *compound_statement(void);

struct ast_node *assign_statement(void);
struct ast_node *if_statement(void);
struct ast_node *print_statement(void);

void semi(void);
void ident(void);

#endif /* __STATEMENT_H_ */