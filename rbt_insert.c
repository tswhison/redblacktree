#include "rbt.h"
#include "rbt_util.h"

static inline void redblack_tree_insert_repair_case_4_2(redblack_tree_node **root,
							redblack_tree_node *n)
{
	redblack_tree_node *p = parent(n);
	redblack_tree_node *gp = parent(p);

	rbt_assert(p);
	rbt_assert(gp);

	if (n == p->left)
		ror(root, gp);
	else
		rol(root, gp);

	p->color = RBT_BLACK;
	gp->color = RBT_RED;
}

static void redblack_tree_insert_repair(redblack_tree_node **root,
					redblack_tree_node *n)
{
	redblack_tree_node *p = parent(n);
	redblack_tree_node *u = uncle(n);

	if (!p) {
/*
** Case 1: if the new node is the root, then color it black.
*/
		n->color = RBT_BLACK;
	} else if (p->color == RBT_BLACK) {
/*
** Case 2: the parent is black, so there is no color violation.
*/
		return;
	} else if (u && u->color == RBT_RED) {
/*
** Case 3: the parent is red, the uncle is red.
**         Color the parent and uncle black.
**         Color the grandparent red.
**         Continue the repair process with the grandparent.
**
**      Gb          Gr
**     /  \        /  \
**    Pr  Ur      Pb  Ub
**   /           /
**  Nr          Nr
*/
		redblack_tree_node *gp = parent(p);
		p->color = u->color = RBT_BLACK;
		gp->color = RBT_RED;
		redblack_tree_insert_repair(root, gp);
	} else {
/*
** Case 4: the parent is red, the uncle is black.
**
**      Gb          Gr
**     /  \        /  \
**    Pr  Ur      Pb  Ub
**   /           /
**  Nr          Nr
*/
		redblack_tree_node *gp = parent(p);

		rbt_assert(gp);

		if (gp->left && n == gp->left->right) {
			rol(root, p);
			n = n->left;
		} else if (gp->right && n == gp->right->left) {
			ror(root, p);
			n = n->right;
		}
		redblack_tree_insert_repair_case_4_2(root, n);
	}
}

int redblack_tree_insert(redblack_tree *t, void *item)
{
	int res;
	int inserted = 0;
	redblack_tree_node **node;
	redblack_tree_node *parent;

	node = &t->root;
	parent = NULL;

	while (*node) {
		parent = *node;
		res = t->compare_items(item, (*node)->item);
		if (res < 0)
			node = &(*node)->left;
		else if (res > 0)
			node = &(*node)->right;
		else // collision - item not inserted
			return inserted;
	}

	// New item inserted at *node

	*node = t->allocate_node(item);
	if (!*node)
		return inserted;

	(*node)->parent = parent;
	(*node)->color = RBT_RED;
	inserted = 1;

	redblack_tree_insert_repair(&t->root, *node);

	return inserted;
}
