#include "rbt.h"
#include "rbt_util.h"

static inline void redblack_tree_remove_case6(redblack_tree_node **root,
					      redblack_tree_node *node)
{
	redblack_tree_node *s = sibling(node);

	if (s) {
		s->color = node->parent->color;
		node->parent->color = RBT_BLACK;

		if (node == node->parent->left) {
			if (s->right)
				s->right->color = RBT_BLACK;
			rol(root, node->parent);
		} else {
			if (s->left)
				s->left->color = RBT_BLACK;
			ror(root, node->parent);
		}
	}
}

static inline void redblack_tree_remove_case5(redblack_tree_node **root,
					      redblack_tree_node *node)
{
	redblack_tree_node *s = sibling(node);

	if (s && s->color == RBT_BLACK) {
		if ((node == node->parent->left) &&
		    (color(s->right) == RBT_BLACK) &&
		    (s->left && s->left->color == RBT_RED)) {
			s->color = RBT_RED;
			s->left->color = RBT_BLACK;
			ror(root, s);
		} else if ((node == node->parent->right) &&
			   (color(s->left) == RBT_BLACK) &&
			   (s->right && s->right->color == RBT_RED)) {
			s->color = RBT_RED;
			s->right->color = RBT_BLACK;
			rol(root, s);
		}
	}

	redblack_tree_remove_case6(root, node);
}

static inline void redblack_tree_remove_case4(redblack_tree_node **root,
					      redblack_tree_node *node)
{
	redblack_tree_node *s = sibling(node);

	if (s &&
	    (color(node->parent) == RBT_RED) &&
	    (color(s) == RBT_BLACK) &&
	    (color(s->left) == RBT_BLACK) &&
	    (color(s->right) == RBT_BLACK)) {
		s->color = RBT_RED;
		node->parent->color = RBT_BLACK;
	} else
		redblack_tree_remove_case5(root, node);
}

static inline void redblack_tree_remove_case1(redblack_tree_node **root,
					      redblack_tree_node *n);

static inline void redblack_tree_remove_case3(redblack_tree_node **root,
					      redblack_tree_node *node)
{
	redblack_tree_node *s = sibling(node);

	if (s &&
 	    (color(node->parent) == RBT_BLACK) &&
	    (color(s) == RBT_BLACK) &&
	    (color(s->left) == RBT_BLACK) &&
	    (color(s->right) == RBT_BLACK)) {
		s->color = RBT_RED;
		redblack_tree_remove_case1(root, node->parent);
	} else
		redblack_tree_remove_case4(root, node);
}

static inline void redblack_tree_remove_case2(redblack_tree_node **root,
					      redblack_tree_node *node)
{
	redblack_tree_node *s = sibling(node);

	if (s && s->color == RBT_RED) {
		node->parent->color = RBT_RED;
		s->color = RBT_BLACK;
		if (node == node->parent->left)
			rol(root, node->parent);
		else
			ror(root, node->parent);
	}
	redblack_tree_remove_case3(root, node);
}

static inline void redblack_tree_remove_case1(redblack_tree_node **root,
					      redblack_tree_node *node)
{
	if (node->parent)
		redblack_tree_remove_case2(root, node);
}

static void redblack_tree_remove_node(redblack_tree *t,
				      void *item,
				      redblack_tree_node *node,
				      int *removed)
{
	int64_t res;
	redblack_tree_node *child;

	while (node) {
		res = t->compare_items(item, node->item);
		if (res < 0)
			node = node->left;
		else if (res > 0)
			node = node->right;
		else // found item
			break;
	}

	if (!node) // item not found
		return;

	if (is_internal(node)) {
		redblack_tree_node *succ = successor(node);
		node->item = succ->item;
		node->context = succ->context;
		node = succ;
	}

	child = !node->left ? node->right : node->left;

	if (node->color == RBT_BLACK) {
		node->color = color(child);
		redblack_tree_remove_case1(&t->root, node);
	}

	if (!node->parent)
		t->root = child;
	else {
		if (node == node->parent->left)
			node->parent->left = child;
		else
			node->parent->right = child;
	}

	if (child)
		child->parent = node->parent;

	t->free_node(node);
	*removed = 1;
}

int redblack_tree_remove(redblack_tree *t, void *item)
{
	int removed = 0;

	redblack_tree_remove_node(t, item, t->root, &removed);

	return removed;
}
