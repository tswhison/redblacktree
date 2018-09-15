/*
** rbt_insert.c : implementation of Red-Black Tree insert
** Copyright (C) 2018  Tim Whisonant
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "rbt.h"
#include "rbt_util.h"

static inline void redblack_tree_insert_repair_case_4_2(redblack_tree_node **root,
							redblack_tree_node *n)
{
/*
** Case 4.2:
**
** Left Case:
**    Gb         Pb
**   /  \       /  \
**  Pr  Ub     Nr  Gr
**  /               \
** Nr               Ub
**
** Right Case:
**    Gb         Pb
**   /  \       /  \
**  Ub  Pr     Gr  Nr
**       \     /
**       Nr   Ub
*/
	redblack_tree_node *p = parent(n);
	redblack_tree_node *gp = parent(p);

	redblack_tree_assert(p);
	redblack_tree_assert(gp);

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
**         (nothing to do)
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
** Case 4.1: the parent is red, the uncle is black.
**
** Left-Right Case:
**      Gb          Gb
**     /  \        /  \
**    Pr  Ub      Nr  Ub
**     \          /
**     Nr        Pr
**
** Right-Left Case:
**      Gb          Gb
**     /  \        /  \
**    Ub  Pr      Ub  Nr
**        /            \
**       Nr            Pr
*/
		redblack_tree_node *gp = parent(p);

		redblack_tree_assert(gp);

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
	int64_t res;
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
