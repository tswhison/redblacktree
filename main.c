#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "rbt.h"
#include "rbt_util.h"

#if 0

void print_tree_item(avl_tree_node *node, void *context, int level)
{
	int *last_level = (int *) context;

	if (level > *last_level) {
		printf("\n");
		*last_level = level;
	}

	printf("%d ", (int) (int64_t) node->item);
}
#endif

redblack_tree_node * my_allocate_redblack_node(void *item)
{
	redblack_tree_node *node = (redblack_tree_node *)
				calloc(1, sizeof(redblack_tree_node));
	if (node)
		node->item = item;

	return node;
}

void my_free_redblack_node(redblack_tree_node *node)
{
	free(node);
}

int my_int_compare(void *a, void *b)
{
	int ia = (int) (int64_t) a;
	int ib = (int) (int64_t) b;
	return ia - ib;
}


typedef struct _int_randomizer {
	int *array;
	int num_items;
	int num_to_consider;
} int_randomizer;

int * allocate_items(int num_items)
{
	int *arr = (int *) malloc(num_items * sizeof(int));
	int i;
	
	for (i = 0 ; i < num_items ; ++i)
		arr[i] = i;

	return arr;
}

int_randomizer * allocate_randomizer(int num_items)
{
	int_randomizer *r;

	r = (int_randomizer *) malloc(sizeof(int_randomizer));
	r->array = allocate_items(num_items);
	r->num_items = num_items;
	r->num_to_consider = num_items;

	return r;
}

void reset_randomizer(int_randomizer *r)
{
	r->num_to_consider = r->num_items;
}

int get_random(int_randomizer *r)
{
	int temp;
	int index = rand() % r->num_to_consider;
	--r->num_to_consider;

	temp = r->array[index];
	r->array[index] = r->array[r->num_to_consider];
	r->array[r->num_to_consider] = temp;

	return temp;
}

void free_randomizer(int_randomizer *r)
{
	free(r->array);
	free(r);
}

void verify_red_rule(redblack_tree_node *n, void *context)
{
	int *red_rule = (int *) context;

	if (n->parent && n->color == RBT_RED)
		if (n->parent->color != RBT_BLACK)
			*red_rule = 0;

}

int rbt_max_black_nodes(redblack_tree_node *node)
{
	int is_black = 0;

	if (!node) // leaves are black
		return 1;

	if (node->color == RBT_BLACK)
		is_black = 1;

	return is_black + rbt_max(rbt_max_black_nodes(node->left),
				  rbt_max_black_nodes(node->right));
}

int rbt_min_black_nodes(redblack_tree_node *node)
{
	int is_black = 0;

	if (!node) // leaves are black
		return 1;

	if (node->color == RBT_BLACK)
		is_black = 1;

	return is_black + rbt_min(rbt_min_black_nodes(node->left),
				  rbt_min_black_nodes(node->right));
}

int is_redblack_tree(redblack_tree *t)
{
/*
** Red Rule: all non-root red nodes have a black parent.
** Black Rule: each root-to-frontier path contains the same
**             number of black nodes.
** External Rule: external (NULL) nodes are black.
*/
	int red_rule = 1;
	int black_rule = 1;

	if (!t->root)
		return 1;

	redblack_tree_in_order(t, verify_red_rule, &red_rule);

	if (rbt_max_black_nodes(t->root) !=
	    rbt_min_black_nodes(t->root))
		black_rule = 0;

	return red_rule && black_rule;
}

void visualize_calc_widths(redblack_tree_node *node, void *context)
{
	int64_t width = 0;

	if (node->left)
		width += (int64_t) node->left->context;

	if (node->right)
		width += (int64_t) node->right->context;

	node->context = (void *) (12 + width/2);
}

void visualize_print_tree(redblack_tree_node *node, void *context, int level)
{
	char fmt[32];
	int *last_level = (int *) context;

	if (level != *last_level) {
		printf("\n");
		*last_level = level;
	}

	sprintf(fmt, "%%%du%%c[%%p]", (int) (int64_t) node->context);

	printf(fmt, (unsigned) (uint64_t) node->item,
		    node->color == RBT_RED ? 'r' : 'b',
		    node);
}

void visualize_tree(redblack_tree *t)
{
	int last_level = 0;
	redblack_tree_post_order(t, visualize_calc_widths, NULL);
	redblack_tree_level_order(t, visualize_print_tree, &last_level);
	printf("\n\n");
}

void insert_and_remove_stress(void)
{
	redblack_tree t;
	int repetitions = 10000;
//	int repetitions = 5;
	int num_items;
	int max_items = 128;
//	int max_items = 16;
	int j;
	int item;
	int i;

	redblack_tree_init(&t, 
		      my_allocate_redblack_node,
		      my_free_redblack_node,
		      my_int_compare);

	for (j = 0 ; j < repetitions ; ++j) {
		for (num_items = 1 ; num_items < max_items ; ++num_items) {
			int_randomizer *r;

			r = allocate_randomizer(num_items);

			// add each item, randomly
			for (i = 0 ; i < num_items ; ++i) {
				item = get_random(r);
				redblack_tree_insert(&t, (void *) (int64_t) item);
				assert(is_redblack_tree(&t));
				assert(redblack_tree_num_items(&t) == (uint32_t)i+1);
			}

			reset_randomizer(r);
#if 0
//			visualize_tree(&t);
			redblack_tree_destroy(&t);
#else
			// remove each item, randomly
			for (i = 0 ; i < num_items ; ++i) {
				item = get_random(r);
				redblack_tree_remove(&t, (void *) (int64_t) item);
				assert(is_redblack_tree(&t));
				assert(redblack_tree_num_items(&t) == ((uint32_t)num_items - ((uint32_t)i+1)));
			}
#endif
			free_randomizer(r);
		}

		if (!(j % 100)) {
			printf(".");
			fflush(stdout);
		}
	}

	redblack_tree_destroy(&t);
}

void test_rbt_util(void)
{
	redblack_tree_node A;
	redblack_tree_node B;
	redblack_tree_node C;
	redblack_tree_node D;
	redblack_tree_node E;
	redblack_tree_node F;
	redblack_tree_node G;
	redblack_tree_node *root;

	root = &D;

	A.left = A.right = NULL;
	A.parent = &B;
	A.color = RBT_RED;

	B.left = &A;
	B.right = &C;
	B.parent = &D;
	B.color = RBT_BLACK;

	C.left = C.right = NULL;
	C.parent = &B;
	C.color = RBT_RED;

	D.left = &B;
	D.right = &F;
	D.parent = NULL;
	D.color = RBT_RED;

	E.left = E.right = NULL;
	E.parent = &F;
	E.color = RBT_RED;

	F.left = &E;
	F.right = &G;
	F.parent = &D;
	F.color = RBT_BLACK;

	G.left = G.right = NULL;
	G.parent = &F;
	G.color = RBT_RED;
/*
**       Dr
**    /     \
**   Bb      Fb
**  / \     /  \
** Ar  Cr  Er   Gr
*/

	assert(rbt_max(1, 2) == 2);

	assert(color(&D) == RBT_RED);
	assert(color(A.left) == RBT_BLACK);

	assert(parent(&A) == &B);
	assert(parent(&B) == &D);
	assert(parent(&C) == &B);
	assert(parent(&D) == NULL);

	assert(grandparent(&A) == &D);
	assert(grandparent(&B) == NULL);
	assert(grandparent(&C) == &D);
	assert(grandparent(&D) == NULL);

	assert(sibling(&A) == &C);
	assert(sibling(&B) == &F);
	assert(sibling(&C) == &A);
	assert(sibling(&D) == NULL);

	assert(uncle(&A) == &F);
	assert(uncle(&B) == NULL);
	assert(uncle(&C) == &F);
	assert(uncle(&D) == NULL);

/*
**       Dr                 Fb
**    /     \              /  \
**   Bb      Fb           Dr   Gr
**  / \     /  \         / \
** Ar  Cr  Er   Gr      Bb  Er
**                     / \
**                    Ar  Cr
*/
	assert(rol(&root, &D) == &F);
	assert(E.parent == &D);
	assert(D.right == &E);
	assert(D.parent == &F);
	assert(F.left == &D);
	assert(F.right == &G);
	assert(F.parent == NULL);
	assert(root == &F);

/*
**       Fb                 Dr
**      /  \             /     \
**     Dr   Gr          Bb      Fb
**    / \              /  \    /  \
**   Bb  Er           Ar  Cr  Er   Gr
**  / \
** Ar  Cr
*/
	assert(ror(&root, &F) == &D);
	assert(E.parent == &F);
	assert(D.right == &F);
	assert(D.left == &B);
	assert(D.parent == NULL);
	assert(F.left == &E);
	assert(F.right == &G);
	assert(F.parent == &D);
	assert(root == &D);
}

int main(int argc, char *argv[])
{
	test_rbt_util();
	insert_and_remove_stress();
	return 0;
}
