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

/* bst.h - Binary search tree. */

#ifndef _BST_H
#define _BST_H

#include <stddef.h>

/* Currently implemented as an AA Tree (or Andersson Tree).  See: http://en.wikipedia.org/wiki/AA_tree
 */

struct bst_node {
        struct bst_node *parent;
        struct bst_node *left;
        struct bst_node *right;
        unsigned level;
};

/* Extract pointer to an item that contains a bst node. */
#define BST_ITEM(d,type,field)                                                  \
        ({                                                                      \
                typeof(d) _dl = (d);                                            \
                                                                                \
                (_dl) ?                                                         \
                        (type *) ((char *)_dl - offsetof(type, field))          \
                        :                                                       \
                        (type *)NULL;                                           \
        })

struct bst_ops {
        void *(*get_key)(struct bst_node *n);
        int (*compare)(void *key_a, void *key_b);
};

struct bst {
        struct bst_ops *ops;
        struct bst_node *root;
};

/* This is used internally by the implementation, but may be useful for external code that wants to walk the tree
   manually. */
extern struct bst_node *bst_nil;



/* Initalize a BST. */
extern void bst_init(struct bst *bst, struct bst_ops *ops);

/* Insert an item into a BST.  Returns 0 on success, non-zero on error. */
extern int bst_insert(struct bst *bst, struct bst_node *n);

/* Remove an item from a BST.  Returns 0 on success, non-zero on error. */
extern int bst_delete(struct bst *bst, struct bst_node *n);

/* Find an item in a BST.  Returns a pointer to the node, or NULL if item was not found. */
extern struct bst_node *bst_find(struct bst *bst, void *key);

/* Find the smallest item in a BST whose key is greater than or equal to 'key'.  Returns NULL if no such item is
   found. */
extern struct bst_node *bst_find_smallest_gte(struct bst *bst, void *key);

/* Find the largest item in a BST whose key is less than or equal to 'key'.  Returns NULL if no such item is found. */
extern struct bst_node *bst_find_largest_lte(struct bst *bst, void *key);

/* Given a node, return a pointer to the node in the tree with the next highest key.  If NULL is passed in, returns a
   pointer to the node in the tree with the smallest key.  If no more nodes exist, returns NULL. */
extern struct bst_node *bst_next(struct bst *bst, struct bst_node *n);

/* Given a node, return a pointer to the node in the tree with the next lowest key.  If NULL is passed in, returns a
   pointer to the node in the tree with the largest key.  If no more nodes exist, returns NULL. */
extern struct bst_node *bst_prev(struct bst *bst, struct bst_node *n);



#endif /* _BST_H */



/* Local Variables:            */
/* mode: c                     */
/* c-basic-offset: 8           */
/* indent-tabs-mode: nil       */
/* fill-column: 120            */
/* c-backslash-max-column: 120 */
/* End:                        */
