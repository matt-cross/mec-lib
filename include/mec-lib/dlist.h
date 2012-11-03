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

/* dlist.h - doubly-linked list implementation. */

#ifndef _DLIST_H
#define _DLIST_H

#include <stddef.h>



struct dlist {
        struct dlist *next;
        struct dlist *prev;
};

/* Extract pointer to an item that contains dlist linkage. */
#define DLIST_ITEM(d,type,field)                                                \
        ({                                                                      \
                typeof(d) _dl = (d);                                            \
                                                                                \
                (_dl) ?                                                         \
                        (type *) ((char *)_dl - offsetof(type, field))          \
                        :                                                       \
                        (type *)NULL;                                           \
        })

/* Initialize the head of a dlist. */
static inline void dlist_init(struct dlist *head)
{
        head->next = head->prev = head;
}

static inline int is_dlist_empty(struct dlist *head)
{
        return head->next == head;
}

static inline void dlist_clear(struct dlist *d)
{
        d->next = d->prev = NULL;
}

static inline void dlist_insert_after(struct dlist *place, struct dlist *new)
{
        new->next = place->next;
        new->prev = place;

        new->next->prev = new;
        new->prev->next = new;
}

static inline void dlist_del(struct dlist *d)
{
        d->next->prev = d->prev;
        d->prev->next = d->next;

        dlist_clear(d);
}

static inline void dlist_insert_front(struct dlist *head, struct dlist *new)
{
        dlist_insert_after(head, new);
}

static inline void dlist_insert_back(struct dlist *head, struct dlist *new)
{
        dlist_insert_after(head->prev, new);
}

static inline struct dlist *dlist_pop_front(struct dlist *head)
{
        if (is_dlist_empty(head)) {
                return NULL;
        } else {
                struct dlist *d = head->next;

                dlist_del(d);
                return d;
        }
}

static inline struct dlist *dlist_pop_back(struct dlist *head)
{
        if (is_dlist_empty(head)) {
                return NULL;
        } else {
                struct dlist *d = head->prev;

                dlist_del(d);
                return d;
        }
}

#define dlist_for_each(head, d)                                         \
        for ((d) = (head)->next; (d) != (head); (d) = (d)->next)

#define dlist_for_each_safe(head, d, dn)                                \
        for ((d) = (head)->next, (dn) = (d)->next;                      \
             (d) != (head);                                             \
             (d) = (dn), (dn) = (d)->next)

#define dlist_for_each_item(head, p, type, field)                       \
        for ((p) = DLIST_ITEM((head)->next, type, field);               \
             &(p)->field != (head);                                     \
             (p) = DLIST_ITEM((p)->field.next, type, field))

#define dlist_for_each_item_safe(head, p, pn, type, field)              \
        for ((p) = DLIST_ITEM((head)->next, type, field),               \
                     (pn) = DLIST_ITEM((p)->field.next, type, field);   \
             &(p)->field != (head);                                     \
             (p) = (pn),                                                \
                     (pn) = DLIST_ITEM((p)->field.next, type, field))


#endif /* _DLIST_H */



/* Local Variables:            */
/* mode: c                     */
/* c-basic-offset: 8           */
/* indent-tabs-mode: nil       */
/* fill-column: 120            */
/* c-backslash-max-column: 120 */
/* End:                        */
