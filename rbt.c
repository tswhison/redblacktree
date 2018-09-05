#include "rbt.h"
#include "rbt_util.h"

void redblack_tree_init(redblack_tree *t,
		redblack_tree_node * (*allocate_node)(void *item),
		void (*free_node)(redblack_tree_node * ),
		int (*compare_items)(void * , void * ))
{
	t->root = NULL;
	t->allocate_node = allocate_node;
	t->free_node = free_node;
	t->compare_items = compare_items;
}

static void redblack_tree_destroy_node(redblack_tree *t, redblack_tree_node *node)
{
	if (!node)
		return;

	redblack_tree_destroy_node(t, node->left);
	redblack_tree_destroy_node(t, node->right);

	t->free_node(node);
}

void redblack_tree_destroy(redblack_tree *t)
{
	redblack_tree_destroy_node(t, t->root);
	t->root = NULL;
}

static uint32_t redblack_tree_num_items_node(redblack_tree_node *node)
{
	if (!node)
		return 0;
	return 1 + redblack_tree_num_items_node(node->left) +
		   redblack_tree_num_items_node(node->right);
}

uint32_t redblack_tree_num_items(redblack_tree *t)
{
	return redblack_tree_num_items_node(t->root);
}

int redblack_tree_find(redblack_tree *t,
		       void *item,
		       void (*visitor)(redblack_tree_node *node, void *context),
		       void *context)
{
	redblack_tree_node *node;
	int res;

	node = t->root;

	while (node) {

		res = t->compare_items(item, node->item);

		if (res < 0) {
			node = node->left;
		} else if (res > 0) {
			node = node->right;
		} else {
			visitor(node, context);
			return 1;
		}

	}

	return 0;
}

static void redblack_tree_pre_order_node(redblack_tree *t,
					 void (*visitor)(redblack_tree_node *node, void *context),
					 void *context,
					 redblack_tree_node *node)
{
	if (!node)
		return;

	visitor(node, context);

	redblack_tree_pre_order_node(t, visitor, context, node->left);
	redblack_tree_pre_order_node(t, visitor, context, node->right);
}

void redblack_tree_pre_order(redblack_tree *t,
			     void (*visitor)(redblack_tree_node *node, void *context),
			     void *context)
{
	redblack_tree_pre_order_node(t, visitor, context, t->root);
}

static void redblack_tree_in_order_node(redblack_tree *t,
					void (*visitor)(redblack_tree_node *node, void *context),
					void *context,
					redblack_tree_node *node)
{
	if (!node)
		return;

	redblack_tree_in_order_node(t, visitor, context, node->left);

	visitor(node, context);

	redblack_tree_in_order_node(t, visitor, context, node->right);
}

void redblack_tree_in_order(redblack_tree *t,
			    void (*visitor)(redblack_tree_node *node, void *context),
			    void *context)
{
	redblack_tree_in_order_node(t, visitor, context, t->root);
}

static void redblack_tree_post_order_node(redblack_tree *t,
					  void (*visitor)(redblack_tree_node *node, void *context),
					  void *context,
					  redblack_tree_node *node)
{
	if (!node)
		return;

	redblack_tree_post_order_node(t, visitor, context, node->left);

	redblack_tree_post_order_node(t, visitor, context, node->right);

	visitor(node, context);
}

void redblack_tree_post_order(redblack_tree *t,
			      void (*visitor)(redblack_tree_node *node, void *context),
			      void *context)
{
	redblack_tree_post_order_node(t, visitor, context, t->root);
}

typedef struct _redblack_queue_entry {
	redblack_tree_node *node;
	struct _redblack_queue_entry *next;
} redblack_queue_entry;

static void redblack_add_queue_entry(redblack_tree_node *node, redblack_queue_entry **queue_head)
{
	redblack_queue_entry *entry;

	entry = (redblack_queue_entry *) malloc(sizeof(redblack_queue_entry));

	entry->node = node;
	entry->next = NULL;

	if (*queue_head == NULL) {
		*queue_head = entry;
	} else {
		redblack_queue_entry *last = *queue_head;

		while (last->next)
			last = last->next;

		last->next = entry;
	}
}

static void redblack_tree_level_order_node(redblack_tree *t,
					   redblack_tree_node *node,
					   redblack_queue_entry **queue_array,
					   int level)
{
	if (!node)
		return;

	redblack_add_queue_entry(node, &queue_array[level]);

	redblack_tree_level_order_node(t, node->left, queue_array, level+1);
	redblack_tree_level_order_node(t, node->right, queue_array, level+1);
}

void redblack_tree_level_order(redblack_tree *t,
			       void (*visitor)(redblack_tree_node *node, void *context, int level),
			       void *context)
{
	redblack_queue_entry **queue_array;
	uint32_t height;
	uint32_t i;

	// Allocate an array of queues, one for each level of the tree.
	height = redblack_tree_height(t);

	if (!height)
		return;

	queue_array = (redblack_queue_entry **)
			calloc(height, sizeof(redblack_queue_entry *));

	// Place each tree item in one of the queues, based on the tree level.
	redblack_tree_level_order_node(t, t->root, queue_array, 0);

	// Iterate over each queue.
	for (i = 0 ; i < height ; ++i) {
		redblack_queue_entry *entry = queue_array[i];

		while (entry) {
			redblack_queue_entry *trash;

			visitor(entry->node, context, i);

			trash = entry;
			entry = entry->next;
			free(trash);
		}
	}

	free(queue_array);
}

static uint32_t redblack_tree_height_node(redblack_tree_node *node)
{
	if (!node)
		return 0;
	return 1 + rbt_max(redblack_tree_height_node(node->left),
			   redblack_tree_height_node(node->right));
}

uint32_t redblack_tree_height(redblack_tree *t)
{
	return redblack_tree_height_node(t->root);
}
