#ifndef __RBT_H__
#define __RBT_H__
#include <stdlib.h>
#include <stdint.h>

typedef enum _redblack_tree_color
{
	RBT_BLACK,
	RBT_RED
} redblack_tree_color;

typedef struct _redblack_tree_node {
	void *item;
	void *context;
	struct _redblack_tree_node *parent;
	struct _redblack_tree_node *left;
	struct _redblack_tree_node *right;
	int8_t color;
} redblack_tree_node;

typedef struct _redblack_tree {
	redblack_tree_node *root;
	redblack_tree_node * (*allocate_node)(void *item);
	void (*free_node)(redblack_tree_node * );
	int (*compare_items)(void * , void * );
} redblack_tree;

void redblack_tree_init(redblack_tree *t,
		redblack_tree_node * (*allocate_node)(void *item),
		void (*free_node)(redblack_tree_node * ),
		int (*compare_items)(void * , void * ));

void redblack_tree_destroy(redblack_tree *t);

// 0 if insertion failed
int redblack_tree_insert(redblack_tree *t, void *item);

// 0 if removal failed
int redblack_tree_remove(redblack_tree *t, void *item);

uint32_t redblack_tree_num_items(redblack_tree *t);

// 1 if found, 0 if not
int redblack_tree_find(redblack_tree *t,
		       void *item,
		       void (*visitor)(redblack_tree_node *node, void *context),
		       void *context);

void redblack_tree_pre_order(redblack_tree *t,
			     void (*visitor)(redblack_tree_node *node, void *context),
			     void *context);

void redblack_tree_in_order(redblack_tree *t,
			    void (*visitor)(redblack_tree_node *node, void *context),
			    void *context);

void redblack_tree_post_order(redblack_tree *t,
			      void (*visitor)(redblack_tree_node *node, void *context),
			      void *context);

void redblack_tree_level_order(redblack_tree *t,
			       void (*visitor)(redblack_tree_node *node, void *context, int level),
			       void *context);

uint32_t redblack_tree_height(redblack_tree *t);

#endif // __RBT_H__
