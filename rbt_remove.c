#include "rbt.h"
#include "rbt_util.h"

static redblack_tree_node * redblack_tree_remove_node(redblack_tree *t,
						      void *item,
						      redblack_tree_node *node,
						      int *removed)
{
	int res;

	if (!node)
		return node;

	res = t->compare_items(item, node->item);

	if (res < 0)
		node->left = redblack_tree_remove_node(t, item, node->left, removed);
	else if (res > 0)
		node->right = redblack_tree_remove_node(t, item, node->right, removed);
	else {

		if (!node->left || !node->right) {
			// one or both children empty.
			redblack_tree_node *trash = node->left ? node->left : node->right;

			if (!trash) {
				// no children
				trash = node;
				node = NULL;
			} else // copy the contents of the non-empty child
				*node = *trash;

			*removed = 1;
			t->free_node(trash);

		} else {
			// both children present.
			// find the successor's value, place it in node,
			// and continue to delete the successor.
			redblack_tree_node *successor = redblack_tree_successor_node(node);

			node->item = successor->item;

			node->right = redblack_tree_remove_node(t, successor->item, node->right, removed);
		}

	}

	if (!node)
		return node;


	return node;
}

int redblack_tree_remove(redblack_tree *t, void *item)
{
	int removed = 0;
	t->root = redblack_tree_remove_node(t, item, t->root, &removed);
	return removed;
}
