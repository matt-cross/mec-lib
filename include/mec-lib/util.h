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

/* util.h - Miscellaneous utility macros. */

#ifndef _UTIL_H
#define _UTIL_H



#define MEC_MIN(a,b)                            \
        ({                                      \
                typeof(a) _a = (a);             \
                typeof(b) _b = (b);             \
                                                \
                (_a < _b) ? _a : _b;            \
        })



#endif /* _UTIL_H */



/* Local Variables:            */
/* mode: c                     */
/* c-basic-offset: 8           */
/* indent-tabs-mode: nil       */
/* fill-column: 120            */
/* c-backslash-max-column: 120 */
/* End:                        */
