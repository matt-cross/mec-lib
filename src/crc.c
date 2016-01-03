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

#include <assert.h>
#include <mec-lib/crc.h>

/* crc.c - Generic CRC implementation. */

static uint64_t reflect(uint64_t value, uint8_t bits)
{
        uint64_t out = 0;
        
        for (unsigned i=0; i<bits; i++) {
                if (value & (1ULL << i)) {
                        out |= 1ULL << (bits - 1 - i);
                }
        }

        return out;
}

uint64_t crc_init(struct crc_config *cfg, uint8_t *data, uint64_t len)
{
        return crc_cont(cfg, cfg->init, data, len);
}

uint64_t crc_cont(struct crc_config *cfg, uint64_t crc, uint8_t *data, uint64_t len)
{
        /* Note that this is kind of like the "SIMPLE" algorithm defined in the "Painless Guide to CRC Error Detection
           Algorithms" combined with the "DIRECT TABLE" algorithm.  It uses the insights for the "direct table"
           algorithm, but without the table.  We use this algorithm so that we can use the "init" value defined in the
           catalogs as used in the "DIRECT TABLE" algorithm, but without having to create tables. */
        
        while (len) {
                uint8_t in = cfg->refin ? reflect(*data, 8) : *data;

                for (unsigned i=0; i<8; i++) {
                        unsigned crc_hi = (crc >> (cfg->width - 1)) & 0x1;
                        unsigned in_hi = (in >> 7) & 0x1;
                        unsigned do_xor = crc_hi ^ in_hi;
                        
                        crc <<= 1;
                        if (cfg->width < 64) {
                                crc &= (1ULL << cfg->width) - 1;
                        }
                        
                        if (do_xor) {
                                crc ^= cfg->poly;
                        }

                        in <<= 1;
                }

                len--;
                data++;
        }

        /* Return the internal register.  Note that it is not reflected or XOR'd on output, that is taken care of in
           crc_finalize (if necessary). */
        return crc;
}

uint64_t crc_finalize(struct crc_config *cfg, uint64_t crc)
{
        /* Handle outbound reflection and XOR: */
        if (cfg->refout) {
                crc = reflect(crc, cfg->width);
        }

        return crc ^ cfg->xorout;
}



/* Local Variables:            */
/* mode: c                     */
/* c-basic-offset: 8           */
/* indent-tabs-mode: nil       */
/* fill-column: 120            */
/* c-backslash-max-column: 120 */
/* End:                        */
