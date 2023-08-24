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

#include <ast.h>
#include <expr.h>
#include <scanner.h>

struct ast_node *make_ast_node(int op, struct ast_node *left,
							   struct ast_node *right, int int_val)
{
	struct ast_node *new;
	new = (struct ast_node *)malloc(sizeof(struct ast_node));
	if (new == NULL) {
		fprintf(stderr, "malloc() failed in make_ast_node()\n");
		exit(1);
	}

	new->op = op;
	new->left = left;
	new->right = right;
	new->v.int_val = int_val;

	return new;
}

struct ast_node *make_ast_leaf(int op, int int_val)
{
	return make_ast_node(op, NULL, NULL, int_val);
}

struct ast_node *make_ast_unary(int op, struct ast_node *left, int int_val)
{
	return make_ast_node(op, left, NULL, int_val);
}