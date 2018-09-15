/*
** rbt_util.h : implementation of Red-Black Tree helpers
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
#ifndef __RBT_UTIL_H__
#define __RBT_UTIL_H__
#include <stdio.h>

#if 1
#define redblack_tree_assert(__expr)
#else
#define redblack_tree_assert(__expr)                                         \
do                                                                           \
{                                                                            \
	if (!(__expr)) {                                                     \
		fprintf(stderr, "%s:%d redblack_tree_assert( %s ) failed\n", \
				__FILE__, __LINE__, #__expr);                \
	}                                                                    \
}while(0)
#endif

#define redblack_tree_max(__a, __b) \
({                                  \
	typeof(__a) ___a = __a;     \
	typeof(__b) ___b = __b;     \
	___a > ___b ? ___a : ___b;  \
})

#define redblack_tree_min(__a, __b) \
({                                  \
	typeof(__a) ___a = __a;     \
	typeof(__b) ___b = __b;     \
	___a < ___b ? ___a : ___b;  \
})

static inline redblack_tree_color color(redblack_tree_node *n)
{
	if (!n) // leaves are black
		return RBT_BLACK;
	return n->color;
}

static inline redblack_tree_node * parent(redblack_tree_node *n)
{
	if (!n)
		return NULL;
	return n->parent;
}

static inline redblack_tree_node * grandparent(redblack_tree_node *n)
{
	return parent(parent(n));
}

static inline redblack_tree_node * sibling(redblack_tree_node *n)
{
	redblack_tree_node *p = parent(n);

	if (!p)
		return NULL;

	if (n == p->left)
		return p->right;
	else
		return p->left;
}

static inline redblack_tree_node * uncle(redblack_tree_node *n)
{
	return sibling(parent(n));
}

/*
**      n                r
**    /   \            /   \
**   l     r          n    T4
**  / \   / \        / \
** T1 T2 T3 T4      l  T3
**                 / \
**                T1 T2
*/

static inline redblack_tree_node * rol(redblack_tree_node **root,
				       redblack_tree_node *n)
{
	redblack_tree_node *nnew;
	redblack_tree_node *p;

	if (!n)
		return NULL;

	nnew = n->right;
	redblack_tree_assert(nnew != NULL);
	p = parent(n);
	n->right = nnew->left;
	nnew->left = n;
	n->parent = nnew;
	if (n->right)
		n->right->parent = n;
	if (p) {
		if (n == p->left)
			p->left = nnew;
		else {
			redblack_tree_assert(n == p->right);
			p->right = nnew;
		}
	}
	nnew->parent = p;
	if (n == *root)
		*root = nnew;
	return nnew;
}

/*
**      n                l
**    /   \            /   \
**   l     r          T1    n
**  / \   / \              / \
** T1 T2 T3 T4            T2  r
**                           / \
**                          T3 T4
*/

static inline redblack_tree_node * ror(redblack_tree_node **root,
				       redblack_tree_node *n)
{
	redblack_tree_node *nnew;
	redblack_tree_node *p;

	if (!n)
		return NULL;

	nnew = n->left;
	redblack_tree_assert(nnew != NULL);
	p = parent(n);
	n->left = nnew->right;
	nnew->right = n;
	n->parent = nnew;
	if (n->left)
		n->left->parent = n;
	if (p) {
		if (n == p->left)
			p->left = nnew;
		else {
			redblack_tree_assert(n == p->right);
			p->right = nnew;
		}
	}
	nnew->parent = p;
	if (n == *root)
		*root = nnew;
	return nnew;
}

static inline int is_leaf(redblack_tree_node *n)
{
	redblack_tree_assert(n);
	return (n->left == NULL) && (n->right == NULL);
}

static inline int is_internal(redblack_tree_node *n)
{
	redblack_tree_assert(n);
	return (n->left != NULL) && (n->right != NULL);
}

static inline redblack_tree_node * successor(redblack_tree_node *n)
{
	redblack_tree_assert(n);
	redblack_tree_assert(n->right);
	n = n->right;
	while (n->left)
		n = n->left;
	return n;
}

#endif // __RBT_UTIL_H__
