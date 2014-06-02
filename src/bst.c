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

/* bst.c - Binary search tree - currently implemented as an AA Tree
 * (or Andersson Tree).  See: http://en.wikipedia.org/wiki/AA_tree
 */

#include "mec-lib/bst.h"
#include "mec-lib/util.h"



/* This is used internally by the implementation, but may be useful for external code that wants to walk the tree
   manually. */
const struct bst_node bst_nil_node = {
        .parent = NULL,
        .left = (struct bst_node *)&bst_nil_node,
        .right = (struct bst_node *)&bst_nil_node,
        .level = 0,
};

struct bst_node *bst_nil = (struct bst_node *)&bst_nil_node;



/* Initalize a BST. */
void bst_init(struct bst *bst, struct bst_ops *ops)
{
        bst->root = bst_nil;
        bst->ops = ops;
}

/* The AA tree skew operation - repair a left horizontal link. */
static struct bst_node *bst_skew(struct bst *bst, struct bst_node *n)
{
        if ((n == bst_nil) || (n->left == bst_nil)) {
                return n;
        } else if (n->left->level == n->level) {
                /* Horizontal left link - do a left rotate to eliminate it. */
                struct bst_node *l = n->left;

                l->parent = n->parent;
                if (n->parent == NULL)
                        bst->root = l;
                else if (n->parent->left == n)
                        l->parent->left = l;
                else
                        l->parent->right = l;

                n->left = l->right;
                if (n->left != bst_nil)
                        n->left->parent = n;
                l->right = n;
                n->parent = l;

                return l;
        } else {
                return n;
        }
}

/* The AA tree split operation - repair a dual horizontal right link. */
static struct bst_node *bst_split(struct bst *bst, struct bst_node *n)
{
        if ((n == bst_nil) ||
            (n->right == bst_nil) ||
            (n->right->right == bst_nil) ) {
                return n;
        } else if (n->level == n->right->right->level) {
                /* We have two horizontal right links - repair it by popping the middle node up a level. */
                struct bst_node *r = n->right;

                r->parent = n->parent;
                if (n->parent == NULL)
                        bst->root = r;
                else if (n->parent->left == n)
                        r->parent->left = r;
                else
                        r->parent->right = r;

                n->right = r->left;
                if (n->right != bst_nil)
                        n->right->parent = n;
                r->left = n;
                n->parent = r;
                r->level++;

                return r;
        } else {
                return n;
        }
}

/* Insert an item into a BST.  Returns 0 on success, non-zero on error. */
int bst_insert(struct bst *bst, struct bst_node *n)
{
        void *k = bst->ops->get_key(n);
        struct bst_node *cur;

        /* Initialize n as a leaf node. */
        n->level = 1;
        n->left = n->right = bst_nil;

        /* Handle insertion into an empty tree. */
        if (bst->root == bst_nil) {
                n->parent = NULL;
                bst->root = n;
                return 0;
        }

        /* Find the proper place in the tree to insert this node as a leaf. */
        cur = bst->root;
        while (1) {
                void *cur_key = bst->ops->get_key(cur);
                int comparison = bst->ops->compare(k, cur_key);

                if (comparison < 0) {
                        if (cur->left == bst_nil) {
                                cur->left = n;
                                n->parent = cur;
                                break;
                        } else {
                                cur = cur->left;
                        }
                } else if (comparison > 0) {
                        if (cur->right == bst_nil) {
                                cur->right = n;
                                n->parent = cur;
                                break;
                        } else {
                                cur = cur->right;
                        }
                } else {
                        /* Two items with the same key not allowed! */
                        return 1;
                }
        }

        /* Now walk back up the tree to repair any temporary damage. */
        while (n) {
                n = bst_skew(bst, n);
                n = bst_split(bst, n);
                n = n->parent;
        }

        return 0;
}

/* Remove an item from a BST.  Returns 0 on success, non-zero on error. */
int bst_delete(struct bst *bst, struct bst_node *n)
{
        struct bst_node *r;
        struct bst_node *cur;

        /* If the node to be deleted is a leaf node, then just remove it. */
        if ((n->left == bst_nil) && (n->right == bst_nil)) {
                if (n->parent == NULL)
                        bst->root = bst_nil;
                else if (n->parent->left == n)
                        n->parent->left = bst_nil;
                else
                        n->parent->right = bst_nil;
                cur = n->parent;
        } else {
                /* This is a non-leaf node.  Find a leaf node below this one and unlink it from the tree so that when we
                   walk back up we can replace 'n' with 'r'. */
                if (n->left == bst_nil)
                        r = bst_next(bst, n);
                else
                        r = bst_prev(bst, n);

                if (r->parent->left == r)
                        r->parent->left = bst_nil;
                else
                        r->parent->right = bst_nil;
                cur = r->parent;
        }

        /* Walk back up the tree rebalancing as we go.  If we find 'n', then substitute 'r' in its place. */
        while (cur) {
                if (cur == n) {
                        /* Put the node 'r' here. */
                        r->parent = cur->parent;
                        if (cur->parent == NULL)
                                bst->root = r;
                        else if (cur->parent->left == cur)
                                r->parent->left = r;
                        else
                                r->parent->right = r;
                        r->left = cur->left;
                        if (r->left != bst_nil)
                                r->left->parent = r;
                        r->right = cur->right;
                        if (r->right != bst_nil)
                                r->right->parent = r;
                        r->level = cur->level;

                        cur = r;
                }

                /* Fix up the level of this node if necessary. */
                if (cur->level > (MEC_MIN(cur->left->level, cur->right->level) + 1)) {
                        cur->level = MEC_MIN(cur->left->level, cur->right->level) + 1;
                        if (cur->right->level > cur->level)
                                cur->right->level = cur->level;
                }

                /* Handle rebalancing. */
                cur = bst_skew(bst, cur);
                bst_skew(bst, cur->right);
                bst_skew(bst, cur->right->right);
                cur = bst_split(bst, cur);
                bst_split(bst, cur->right);

                cur = cur->parent;
        }

        /* Clean up the node we just deleted. */
        n->level = 0;
        n->parent = n->left = n->right = NULL;

        return 0;
}

/* Find an item in a BST.  Returns a pointer to the node, or NULL if item was not found. */
struct bst_node *bst_find(struct bst *bst, void *key)
{
        struct bst_node *cur;

        cur = bst->root;
        while (cur != bst_nil) {
                void *cur_key = bst->ops->get_key(cur);
                int comparison = bst->ops->compare(key, cur_key);

                if (comparison < 0)
                        cur = cur->left;
                else if (comparison > 0)
                        cur = cur->right;
                else
                        return cur;
        }

        return NULL;
}

/* Find the smallest item in a BST whose key is greater than or equal to 'key'.  Returns NULL if no such item is
   found. */
struct bst_node *bst_find_smallest_gte(struct bst *bst, void *key)
{
        struct bst_node *cur;

        cur = bst->root;
        while (cur != bst_nil) {
                void *cur_key = bst->ops->get_key(cur);
                int comparison = bst->ops->compare(key, cur_key);

                if (comparison < 0) {
                        if (cur->left == bst_nil)
                                return cur;
                        cur = cur->left;
                } else if (comparison > 0) {
                        if (cur->right == bst_nil)
                                return bst_next(bst, cur);
                        cur = cur->right;
                } else
                        return cur;
        }

        return NULL;
}

/* Find the largest item in a BST whose key is less than or equal to 'key'.  Returns NULL if no such item is found. */
struct bst_node *bst_find_largest_lte(struct bst *bst, void *key)
{
        struct bst_node *cur;

        cur = bst->root;
        while (cur != bst_nil) {
                void *cur_key = bst->ops->get_key(cur);
                int comparison = bst->ops->compare(key, cur_key);

                if (comparison < 0) {
                        if (cur->left == bst_nil)
                                return bst_prev(bst, cur);
                        cur = cur->left;
                } else if (comparison > 0) {
                        if (cur->right == bst_nil)
                                return cur;
                        cur = cur->right;
                } else
                        return cur;
        }

        return NULL;
}

/* Given a node, return a pointer to the node in the tree with the next highest key.  If NULL is passed in, returns a
   pointer to the node in the tree with the smallest key.  If no more nodes exist, returns NULL. */
struct bst_node *bst_next(struct bst *bst, struct bst_node *n)
{
        struct bst_node *cur;

        if (n == NULL) {
                if (bst->root == bst_nil)
                        return NULL;
                cur = bst->root;
                while (cur->left != bst_nil)
                        cur = cur->left;
                return cur;
        } else {
                if (n->right != bst_nil) {
                        /* The next node is the left most child of our right subtree. */
                        cur = n->right;
                        while (cur->left != bst_nil)
                                cur = cur->left;
                        return cur;
                } else {
                        /* Walk up the tree until we walk up a left link; when we do that is the next node. */
                        cur = n;
                        while (cur->parent) {
                                if (cur->parent->left == cur)
                                        return cur->parent;
                                cur = cur->parent;
                        }
                        return NULL;
                }
        }
}

/* Given a node, return a pointer to the node in the tree with the next lowest key.  If NULL is passed in, returns a
   pointer to the node in the tree with the largest key.  If no more nodes exist, returns NULL. */
struct bst_node *bst_prev(struct bst *bst, struct bst_node *n)
{
        struct bst_node *cur;

        if (n == NULL) {
                if (bst->root == bst_nil)
                        return NULL;
                cur = bst->root;
                while (cur->right != bst_nil)
                        cur = cur->right;
                return cur;
        } else {
                if (n->left != bst_nil) {
                        /* The next node is the right most child of our left subtree. */
                        cur = n->left;
                        while (cur->right != bst_nil)
                                cur = cur->right;
                        return cur;
                } else {
                        /* Walk up the tree until we walk up a right link; when we do that is the next node. */
                        cur = n;
                        while (cur->parent) {
                                if (cur->parent->right == cur)
                                        return cur->parent;
                                cur = cur->parent;
                        }
                        return NULL;
                }
        }
}



/* Local Variables:            */
/* mode: c                     */
/* c-basic-offset: 8           */
/* indent-tabs-mode: nil       */
/* fill-column: 120            */
/* c-backslash-max-column: 120 */
/* End:                        */
