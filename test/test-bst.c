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

int is_bst_subtree_valid(struct bst *bst, struct bst_node *n)
{
        void *my_key = bst->ops->get_key(n);

        /* Check levels - left link must not be horizontal; right link may be horizontal or vertical. */
        if ((n->left->level + 1) != n->level)
                return 0;
        if (((n->right->level + 1) != n->level) &&
            (n->right->level != n->level) )
                return 0;

        if (n->left != bst_nil) {
                void *l_key = bst->ops->get_key(n->left);

                if (bst->ops->compare(l_key, my_key) >= 0)
                        return 0;
                if (n->left->parent != n)
                        return 0;
                if (!is_bst_subtree_valid(bst, n->left))
                        return 0;
        }

        if (n->right != bst_nil) {
                void *r_key = bst->ops->get_key(n->right);

                if (bst->ops->compare(my_key, r_key) >= 0)
                        return 0;
                if (n->right->parent != n)
                        return 0;
                if (!is_bst_subtree_valid(bst, n->right))
                        return 0;
        }

        return 1;
}

int is_bst_valid(struct bst *bst)
{
        if (bst->root != bst_nil) {
                if (bst->root->parent != NULL)
                        return 0;

                return is_bst_subtree_valid(bst, bst->root);
        }

        return 1;
}

struct thing {
        int a;
        char name[40];
        struct bst_node bstn;
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
        struct thing *thing = BST_ITEM(n, struct thing, bstn);

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

int main(void)
{
        struct bst tree;
        struct thing *thing_array;
        struct thing *thingp, *last_thingp;
        struct bst_node *n;
        unsigned num_things = 1000;
        unsigned i;

        thing_array = malloc(sizeof(struct thing) * num_things);
        TEST(thing_array);

        bst_init(&tree, &thing_int_bst_ops);

        printf("Adding %u random items to bst...\n", num_things);
        for (i = num_things; i>0; i--) {
                thingp = &thing_array[i-1];

                thingp->a = (int)random();
                snprintf(thingp->name, sizeof(thingp->name), "thing %u", i);

                bst_insert(&tree, &thingp->bstn);
                TEST(is_bst_valid(&tree));
        }

        printf("Walking dlist with dlist_for_each_safe (and deleting every other item)...\n");
        last_thingp = NULL;
        for (i=0, n = bst_next(&tree, NULL);
             n;
             i++, n = bst_next(&tree, n)) {
                thingp = BST_ITEM(n, struct thing, bstn);

                if (last_thingp)
                        TEST(thingp->a > last_thingp->a);
                last_thingp = thingp;

                if ((i % 2) == 0) {
                        bst_del(&tree, &thingp->bstn);
                        TEST(is_bst_valid(&tree));
                }
        }

        printf("Removing remaining items with bst_del...\n");
        i = 0;
        while (tree.root != bst_nil) {
                i++;

                thingp = BST_ITEM(tree.root, struct thing, bstn);
                bst_del(&tree, &thingp->bstn);

                TEST(is_bst_valid(&tree));
        }
        printf("  (Popped %u items)\n", i);
        TEST(i == (num_things / 2));


        /* TBD: test with strings. */


        return 0;
}



/* Local Variables:            */
/* mode: c                     */
/* c-basic-offset: 8           */
/* indent-tabs-mode: nil       */
/* fill-column: 120            */
/* c-backslash-max-column: 120 */
/* End:                        */
