/* Copyright (c) 2016, Matthew E. Cross <matt.cross@gmail.com>
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

/* crc.h - Generic CRC implementation. */

#ifndef _CRC_H
#define _CRC_H

#include <stdint.h>

/* Paramters describing the CRC calculation to perform.  As described in http://www.ross.net/crc/download/crc_v3.txt */

struct crc_config
{
        uint8_t width; /* Max: 64. */
        uint64_t poly;
        uint64_t init;
        uint8_t refin;
        uint8_t refout;
        uint64_t xorout;
};

/* Start a CRC calculation over some data at a specified address.  Returns an intermediate CRC value that can be passed
   into crc_cont or crc_finish.  Note that you must pass at least "width" bits into crc_init */
uint64_t crc_init(struct crc_config *cfg, uint8_t *data, uint64_t len);

/* Continue calculating a CRC over some additional data. */
uint64_t crc_cont(struct crc_config *cfg, uint64_t crc, uint8_t *data, uint64_t len);

/* Finalize a CRC calculation - returns the CRC value calculated. */
uint64_t crc_finalize(struct crc_config *cfg, uint64_t crc);

/* All-in-one CRC calculation. */
static inline uint64_t crc_calculate(struct crc_config *cfg, uint8_t *data, uint64_t len)
{
        uint64_t crc = crc_init(cfg, data, len);

        return crc_finalize(cfg, crc);
}

#endif /* _CRC_H */



/* Local Variables:            */
/* mode: c                     */
/* c-basic-offset: 8           */
/* indent-tabs-mode: nil       */
/* fill-column: 120            */
/* c-backslash-max-column: 120 */
/* End:                        */
