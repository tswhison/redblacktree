/*
** rbt.h : definitions for Red-Black Trees
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

// for level-order traverse
typedef struct _redblack_queue_entry {
	redblack_tree_node *node;
	struct _redblack_queue_entry *next;
} redblack_queue_entry;

typedef struct _redblack_tree {
	redblack_tree_node *root;
	redblack_tree_node * (*allocate_node)(void *item);
	void (*free_node)(redblack_tree_node * );
	int64_t (*compare_items)(void * , void * );
	redblack_queue_entry * (*allocate_entry)(redblack_tree_node * );
	void (*free_entry)(redblack_queue_entry * );
} redblack_tree;

void redblack_tree_init(redblack_tree *t,
		redblack_tree_node * (*allocate_node)(void *item),
		void (*free_node)(redblack_tree_node * ),
		int64_t (*compare_items)(void * , void * ),
		redblack_queue_entry * (*allocate_entry)(redblack_tree_node * ),
		void (*free_entry)(redblack_queue_entry * ));

void redblack_tree_destroy(redblack_tree *t);

// 0 if insertion failed
int redblack_tree_insert(redblack_tree *t, void *item);

// 0 if removal failed
int redblack_tree_remove(redblack_tree *t, void *item);

uint32_t redblack_tree_num_items(redblack_tree *t);

// NULL if not found
redblack_tree_node * redblack_tree_find(redblack_tree *t,
					void *item);

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
