/* Copyright (c) 2012, Matthew E. Cross <matt.cross@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* test-bst.c - Unit tests for bst's. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mec-lib/bst.h"



#define TEST(_expr)                             \
        do {                                    \
                if (!(_expr)) {                 \
                        fprintf(stderr, "TEST FAILED @ %s:%d '%s' not true\n",  \
                                __FILE__, __LINE__, #_expr );                   \
                        abort();                                                \
                }                                                               \
        } while (0)

void assert_bst_subtree_valid(struct bst *bst, struct bst_node *n)
{
        void *my_key = bst->ops->get_key(n);

        /* Check levels - left link must not be horizontal; right link may be horizontal or vertical. */
        TEST ((n->left->level + 1) == n->level);
        TEST (((n->right->level + 1) == n->level) ||
              ((n->right->level == n->level) &&
               ((n->right->right->level + 1) == n->level) ) );

        if (n->left != bst_nil) {
                void *l_key = bst->ops->get_key(n->left);

                TEST (bst->ops->compare(l_key, my_key) < 0);
                TEST (n->left->parent == n);
                assert_bst_subtree_valid(bst, n->left);
        }

        if (n->right != bst_nil) {
                void *r_key = bst->ops->get_key(n->right);

                TEST (bst->ops->compare(my_key, r_key) < 0);
                TEST (n->right->parent == n);
                assert_bst_subtree_valid(bst, n->right);
        }
}

void assert_bst_valid(struct bst *bst)
{
        if (bst->root != bst_nil) {
                TEST(bst->root->parent == NULL);

                assert_bst_subtree_valid(bst, bst->root);
        }
}

struct thing {
        int a;
        char name[40];
        struct bst_node bstn;
        struct bst_node name_bstn;
};

void *thing_get_int_key(struct bst_node *n)
{
        struct thing *thing;

        thing = BST_ITEM(n, struct thing, bstn);

        return &thing->a;
}

int compare_ints(void *key_a, void *key_b)
{
        int *int_a = (int *)key_a;
        int *int_b = (int *)key_b;

        return *int_a - *int_b;
}

struct bst_ops thing_int_bst_ops = {
        .get_key = thing_get_int_key,
        .compare = compare_ints,
};

void *thing_get_string_key(struct bst_node *n)
{
        struct thing *thing = BST_ITEM(n, struct thing, name_bstn);

        return &thing->name;
}

int compare_strings(void *key_a, void *key_b)
{
        return strcmp(key_a, key_b);
}

struct bst_ops thing_string_bst_ops = {
        .get_key = thing_get_string_key,
        .compare = compare_strings,
};

struct bst_node *bruteforce_find_smallest_gte(struct bst *bst, void *key)
{
        struct bst_node *n;

        for (n = bst_next(bst, NULL);
             n && (bst->ops->compare(key, bst->ops->get_key(n)) > 0);
             n = bst_next(bst, n) )
                continue;

        return n;
}

struct bst_node *bruteforce_find_largest_lte(struct bst *bst, void *key)
{
        struct bst_node *n;

        for (n = bst_prev(bst, NULL);
             n && (bst->ops->compare(key, bst->ops->get_key(n)) < 0);
             n = bst_prev(bst, n) )
                continue;

        return n;
}

int main(void)
{
        struct bst tree, name_tree;
        struct thing *thing_array;
        struct thing *thingp, *last_thingp;
        struct bst_node *n, *next_n;
        unsigned num_things = 10000;
        unsigned i;
        int key;

        thing_array = malloc(sizeof(struct thing) * num_things);
        TEST(thing_array);

        bst_init(&tree, &thing_int_bst_ops);
        bst_init(&name_tree, &thing_string_bst_ops);

        printf("Adding %u random items to bst...\n", num_things);
        for (i = num_things; i>0; i--) {
                thingp = &thing_array[i-1];

                thingp->a = (int)random();
                snprintf(thingp->name, sizeof(thingp->name), "thing %u", i);

                TEST(bst_insert(&tree, &thingp->bstn) == 0);
                assert_bst_valid(&tree);

                TEST(bst_insert(&name_tree, &thingp->name_bstn) == 0);
                assert_bst_valid(&name_tree);
        }

        printf("Checking bst_find() for every item in tree...\n");
        for (i=0; i<num_things; i++) {
                TEST(bst_find(&tree, &thing_array[i].a) == &thing_array[i].bstn);
                TEST(bst_find(&name_tree, &thing_array[i].name) == &thing_array[i].name_bstn);
        }

        printf("Checking bst_find_smallest_gte() and bst_find_largest_lte() with %u random items\n", num_things/10);
        for (i=0; i<num_things/10; i++) {
                char name_key[40];
                key = (int)random();

                TEST(bst_find_smallest_gte(&tree, &key) == bruteforce_find_smallest_gte(&tree, &key));
                TEST(bst_find_largest_lte(&tree, &key) == bruteforce_find_largest_lte(&tree, &key));

                sprintf(name_key, "thing %d", key % num_things);

                TEST(bst_find_smallest_gte(&name_tree, &name_key) == bruteforce_find_smallest_gte(&name_tree, &name_key));
                TEST(bst_find_largest_lte(&name_tree, &name_key) == bruteforce_find_largest_lte(&name_tree, &name_key));
        }

        printf("Walking bst with bst_next (and deleting every other item)...\n");
        last_thingp = NULL;
        for (i=0, n = bst_next(&tree, NULL), next_n = bst_next(&tree, n);
             n;
             i++, n = next_n, next_n = bst_next(&tree, n)) {
                thingp = BST_ITEM(n, struct thing, bstn);

                if (last_thingp)
                        TEST(thingp->a > last_thingp->a);
                last_thingp = thingp;

                if ((i % 2) == 0) {
                        TEST(bst_delete(&tree, &thingp->bstn) == 0);
                        assert_bst_valid(&tree);
                }
        }

        printf("Removing remaining items with bst_delete...\n");
        i = 0;
        while (tree.root != bst_nil) {
                i++;

                thingp = BST_ITEM(tree.root, struct thing, bstn);
                TEST(bst_delete(&tree, &thingp->bstn) == 0);

                assert_bst_valid(&tree);
        }
        printf("  (Popped %u items)\n", i);
        TEST(i == (num_things / 2));


        printf("Walking name bst with bst_next (and deleting every other item)...\n");
        last_thingp = NULL;
        for (i=0, n = bst_next(&name_tree, NULL), next_n = bst_next(&name_tree, n);
             n;
             i++, n = next_n, next_n = bst_next(&name_tree, n)) {
                thingp = BST_ITEM(n, struct thing, name_bstn);

                if (last_thingp)
                        TEST(strcmp(thingp->name, last_thingp->name) > 0);
                last_thingp = thingp;

                if ((i % 2) == 0) {
                        TEST(bst_delete(&name_tree, &thingp->name_bstn) == 0);
                        assert_bst_valid(&name_tree);
                }
        }

        printf("Removing remaining items from name tree with bst_delete...\n");
        i = 0;
        while (name_tree.root != bst_nil) {
                i++;

                thingp = BST_ITEM(name_tree.root, struct thing, name_bstn);
                TEST(bst_delete(&name_tree, &thingp->name_bstn) == 0);

                assert_bst_valid(&name_tree);
        }
        printf("  (Popped %u items)\n", i);
        TEST(i == (num_things / 2));


        return 0;
}



/* Local Variables:            */
/* mode: c                     */
/* c-basic-offset: 8           */
/* indent-tabs-mode: nil       */
/* fill-column: 120            */
/* c-backslash-max-column: 120 */
/* End:                        */
