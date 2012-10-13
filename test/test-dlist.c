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

/* test-dlist.c - Unit tests for dlist's. */

#include <stdio.h>
#include <stdlib.h>
#include "mec-lib/dlist.h"



#define TEST(_expr)                             \
        do {                                    \
                if (!(_expr)) {                 \
                        fprintf(stderr, "TEST FAILED @ %s:%d '%s' not true\n",  \
                                __FILE__, __LINE__, #_expr );                   \
                        abort();                                                \
                }                                                               \
        } while (0)

int is_dlist_valid(struct dlist *head)
{
        struct dlist *d;

        if ((head->next->prev != head) ||
            (head->prev->next != head) ) {
                return 0;
        }

        for (d = head->next; d != head; d = d->next) {
                if ((d->next->prev != d) ||
                    (d->prev->next != d) ) {
                        return 0;
                }
        }

        return 1;
}

struct thing {
        int a;
        char name[40];
        struct dlist dl;
};

int main(void)
{
        struct dlist head;
        struct thing *thing_array;
        struct thing *thingp;
        unsigned num_things = 1000;
        unsigned i;

        thing_array = malloc(sizeof(struct thing) * num_things);
        TEST(thing_array);

        dlist_init(&head);

        for (i = num_things; i>0; i--) {
                thingp = &thing_array[i-1];

                thingp->a = i;
                snprintf(thingp->name, sizeof(thingp->name), "thing %u", i);

                dlist_insert_front(&head, &thingp->dl);
                TEST(is_dlist_valid(&head));
        }

        i = 0;
        dlist_for_each_item(&head, thingp, struct thing, dl) {
                TEST(thingp->a == i+1);
                i = thingp->a;
        }

        while (!is_dlist_empty(&head)) {
                thingp = DLIST_ITEM(dlist_pop_back(&head), struct thing, dl);

                TEST(is_dlist_valid(&head));
        }

        return 0;
}



/* Local Variables:            */
/* mode: c                     */
/* c-basic-offset: 8           */
/* indent-tabs-mode: nil       */
/* fill-column: 120            */
/* c-backslash-max-column: 120 */
/* End:                        */
