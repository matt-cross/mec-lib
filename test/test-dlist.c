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
        struct thing *thingp, *next_thingp;
        struct dlist *dl, *next_dl;
        unsigned num_things = 1000;
        unsigned i;

        thing_array = malloc(sizeof(struct thing) * num_things);
        TEST(thing_array);

        dlist_init(&head);

        printf("Adding %u items to dlist with dlist_insert_front...\n", num_things);
        for (i = num_things; i>0; i--) {
                thingp = &thing_array[i-1];

                thingp->a = i;
                snprintf(thingp->name, sizeof(thingp->name), "thing %u", i);

                dlist_insert_front(&head, &thingp->dl);
                TEST(is_dlist_valid(&head));
        }

        printf("Walking dlist with dlist_for_each_item...\n");
        i = 0;
        dlist_for_each_item(&head, thingp, struct thing, dl) {
                TEST(thingp->a == i+1);
                i = thingp->a;
        }

        printf("Walking dlist with dlist_for_each_safe (and deleting every other item)...\n");
        i = 0;
        dlist_for_each_safe(&head, dl, next_dl) {
                thingp = DLIST_ITEM(dl, struct thing, dl);

                TEST(thingp->a == i+1);
                i = thingp->a;

                if ((thingp->a % 2) == 0) {
                        dlist_del(&thingp->dl);
                        TEST(is_dlist_valid(&head));
                }
        }

        printf("Removing remaining items with dlist_pop_back...\n");
        i = 0;
        while (!is_dlist_empty(&head)) {
                i++;

                /* TRICKY!  Note that this (quite unintentionally) tests that the first argument to DLIST_ITEM is
                   evaluated only once. */
                thingp = DLIST_ITEM(dlist_pop_back(&head), struct thing, dl);

                TEST(is_dlist_valid(&head));
        }
        printf("  (Popped %u items)\n", i);
        TEST(i == (num_things / 2));



        printf("Adding %u items to dlist with dlist_insert_back...\n", num_things);
        for (i = 0; i<num_things; i++) {
                thingp = &thing_array[i];

                thingp->a = i+1;
                snprintf(thingp->name, sizeof(thingp->name), "thing %u", i+1);

                dlist_insert_back(&head, &thingp->dl);
                TEST(is_dlist_valid(&head));
        }

        printf("Walking list with dlist_for_each...\n");
        i = 0;
        dlist_for_each(&head, dl) {
                thingp = DLIST_ITEM(dl, struct thing, dl);

                TEST(thingp->a == i+1);
                i = thingp->a;
        }

        printf("Walking dlist with dlist_for_each_item_safe (and deleting every other item)...\n");
        i = 0;
        dlist_for_each_item_safe(&head, thingp, next_thingp, struct thing, dl) {
                TEST(thingp->a == i+1);
                i = thingp->a;

                if ((thingp->a % 2) == 0)
                        dlist_del(&thingp->dl);
        }

        printf("Deleting remaining items from list with dlist_pop_front\n");
        i = 0;
        while (!is_dlist_empty(&head)) {
                i++;

                thingp = DLIST_ITEM(dlist_pop_front(&head), struct thing, dl);

                TEST(is_dlist_valid(&head));
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
