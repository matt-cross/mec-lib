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

/* test-crc.c - Unit tests for generic CRC. */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mec-lib/crc.h"



#define TEST(_expr)                                                             \
        do {                                                                    \
                if (!(_expr)) {                                                 \
                        fprintf(stderr, "TEST FAILED @ %s:%d '%s' not true\n",  \
                                __FILE__, __LINE__, #_expr );                   \
                        abort();                                                \
                }                                                               \
        } while (0)


struct crc_test_cfg {
        char *name;
        uint64_t check;
        struct crc_config cfg;
};

/* These test vectors are based on the CRC catalogue at http://reveng.sourceforge.net/crc-catalogue/ */

struct crc_test_cfg test_cfgs[] = {
        { .name = "CRC-3/ROHC",       .check = 0x6, { .width = 3, .poly = 0x3, .init = 0x7, .refin = 1, .refout = 1, .xorout = 0x0, } },

        { .name = "CRC-4/ITU",        .check = 0x7, { .width = 4, .poly = 0x3, .init = 0x0, .refin = 1, .refout = 1, .xorout = 0x0, } },

        { .name = "CRC-5/EPC",        .check = 0x00, { .width = 5, .poly = 0x09, .init = 0x09, .refin = 0, .refout = 0, .xorout = 0x00, } },
        { .name = "CRC-5/ITU",        .check = 0x07, { .width = 5, .poly = 0x15, .init = 0x00, .refin = 1, .refout = 1, .xorout = 0x00, } },
        { .name = "CRC-5/USB",        .check = 0x19, { .width = 5, .poly = 0x05, .init = 0x1f, .refin = 1, .refout = 1, .xorout = 0x1f, } },

        { .name = "CRC-6/CDMA2000-A", .check = 0x0d, { .width = 6, .poly = 0x27, .init = 0x3f, .refin = 0, .refout = 0, .xorout = 0x00, } },
        { .name = "CRC-6/CDMA2000-B", .check = 0x3b, { .width = 6, .poly = 0x07, .init = 0x3f, .refin = 0, .refout = 0, .xorout = 0x00, } },
        { .name = "CRC-6/DARC",       .check = 0x26, { .width = 6, .poly = 0x19, .init = 0x00, .refin = 1, .refout = 1, .xorout = 0x00, } },
        { .name = "CRC-6/ITU",        .check = 0x06, { .width = 6, .poly = 0x03, .init = 0x00, .refin = 1, .refout = 1, .xorout = 0x00, } },

        { .name = "CRC-7",            .check = 0x75, { .width = 7, .poly = 0x09, .init = 0x00, .refin = 0, .refout = 0, .xorout = 0x00, } },
        { .name = "CRC-7/ROHC",       .check = 0x53, { .width = 7, .poly = 0x4f, .init = 0x7f, .refin = 1, .refout = 1, .xorout = 0x00, } },

        { .name = "CRC-8",            .check = 0xf4, { .width = 8, .poly = 0x07, .init = 0x00, .refin = 0, .refout = 0, .xorout = 0x00, } },
        { .name = "CRC-8/CDMA2000",   .check = 0xda, { .width = 8, .poly = 0x9b, .init = 0xff, .refin = 0, .refout = 0, .xorout = 0x00, } },
        { .name = "CRC-8/DARC",       .check = 0x15, { .width = 8, .poly = 0x39, .init = 0x00, .refin = 1, .refout = 1, .xorout = 0x00, } },
        { .name = "CRC-8/DVB-S2",     .check = 0xbc, { .width = 8, .poly = 0xd5, .init = 0x00, .refin = 0, .refout = 0, .xorout = 0x00, } },
        { .name = "CRC-8/EBU",        .check = 0x97, { .width = 8, .poly = 0x1d, .init = 0xff, .refin = 1, .refout = 1, .xorout = 0x00, } },
        { .name = "CRC-8/I-CODE",     .check = 0x7e, { .width = 8, .poly = 0x1d, .init = 0xfd, .refin = 0, .refout = 0, .xorout = 0x00, } },
        { .name = "CRC-8/ITU",        .check = 0xa1, { .width = 8, .poly = 0x07, .init = 0x00, .refin = 0, .refout = 0, .xorout = 0x55, } },
        { .name = "CRC-8/MAXIM",      .check = 0xa1, { .width = 8, .poly = 0x31, .init = 0x00, .refin = 1, .refout = 1, .xorout = 0x00, } },
        { .name = "CRC-8/ROHC",       .check = 0xd0, { .width = 8, .poly = 0x07, .init = 0xff, .refin = 1, .refout = 1, .xorout = 0x00, } },
        { .name = "CRC-8/WCDMA",      .check = 0x25, { .width = 8, .poly = 0x9b, .init = 0x00, .refin = 1, .refout = 1, .xorout = 0x00, } },

        { .name = "CRC-10",           .check = 0x199, { .width = 10, .poly = 0x233, .init = 0x000, .refin = 0, .refout = 0, .xorout = 0x000, } },
        { .name = "CRC-10/CDMA2000",  .check = 0x233, { .width = 10, .poly = 0x3d9, .init = 0x3ff, .refin = 0, .refout = 0, .xorout = 0x000, } },

        { .name = "CRC-11",           .check = 0x5a3, { .width = 11, .poly = 0x385, .init = 0x01a, .refin = 0, .refout = 0, .xorout = 0x000, } },

        { .name = "CRC-12/3GPP",      .check = 0xdaf, { .width = 12, .poly = 0x80f, .init = 0x000, .refin = 0, .refout = 1, .xorout = 0x000, } },
        { .name = "CRC-12/CDMA2000",  .check = 0xd4d, { .width = 12, .poly = 0xf13, .init = 0xfff, .refin = 0, .refout = 0, .xorout = 0x000, } },
        { .name = "CRC-12/DECT",      .check = 0xf5b, { .width = 12, .poly = 0x80f, .init = 0x000, .refin = 0, .refout = 0, .xorout = 0x000, } },

        { .name = "CRC-13/BBC",         .check = 0x04fa, { .width = 13, .poly = 0x1cf5, .init = 0x0000, .refin = 0, .refout = 0, .xorout = 0x0000, } },

        { .name = "CRC-14/DARC",        .check = 0x082d, { .width = 14, .poly = 0x0805, .init = 0x0000, .refin = 1, .refout = 1, .xorout = 0x0000, } },

        { .name = "CRC-15",             .check = 0x059e, { .width = 15, .poly = 0x4599, .init = 0x0000, .refin = 0, .refout = 0, .xorout = 0x0000, } },
        { .name = "CRC-15/MPT1327",     .check = 0x2566, { .width = 15, .poly = 0x6815, .init = 0x0000, .refin = 0, .refout = 0, .xorout = 0x0001, } },

        { .name = "CRC-16",             .check = 0xBB3D, { .width = 16, .poly = 0x8005, .init = 0x0000, .refin = 1, .refout = 1, .xorout = 0x0000, } },
        { .name = "CRC-16/BUYPASS",     .check = 0xFEE8, { .width = 16, .poly = 0x8005, .init = 0x0000, .refin = 0, .refout = 0, .xorout = 0x0000, } },
        { .name = "CRC-16/AUG-CCITT",   .check = 0xE5CC, { .width = 16, .poly = 0x1021, .init = 0x1d0f, .refin = 0, .refout = 0, .xorout = 0x0000, } },
        { .name = "CRC-16/CCITT-FALSE", .check = 0x29b1, { .width = 16, .poly = 0x1021, .init = 0xffff, .refin = 0, .refout = 0, .xorout = 0x0000, } },
        { .name = "CRC-16/CDMA2000",    .check = 0x4c06, { .width = 16, .poly = 0xc867, .init = 0xffff, .refin = 0, .refout = 0, .xorout = 0x0000, } },
        { .name = "CRC-16/DDS-110",     .check = 0x9ecf, { .width = 16, .poly = 0x8005, .init = 0x800d, .refin = 0, .refout = 0, .xorout = 0x0000, } },
        { .name = "CRC-16/DECT-R",      .check = 0x007e, { .width = 16, .poly = 0x0589, .init = 0x0000, .refin = 0, .refout = 0, .xorout = 0x0001, } },
        { .name = "CRC-16/DECT-X",      .check = 0x007f, { .width = 16, .poly = 0x0589, .init = 0x0000, .refin = 0, .refout = 0, .xorout = 0x0000, } },
        { .name = "CRC-16/DNP",         .check = 0xea82, { .width = 16, .poly = 0x3d65, .init = 0x0000, .refin = 1, .refout = 1, .xorout = 0xffff, } },
        { .name = "CRC-16/EN-13757",    .check = 0xc2b7, { .width = 16, .poly = 0x3d65, .init = 0x0000, .refin = 0, .refout = 0, .xorout = 0xffff, } },
        { .name = "CRC-16/GENIBUS",     .check = 0xd64e, { .width = 16, .poly = 0x1021, .init = 0xffff, .refin = 0, .refout = 0, .xorout = 0xffff, } },
        { .name = "CRC-16/MAXIM",       .check = 0x44c2, { .width = 16, .poly = 0x8005, .init = 0x0000, .refin = 1, .refout = 1, .xorout = 0xffff, } },
        { .name = "CRC-16/MCRF4XX",     .check = 0x6f91, { .width = 16, .poly = 0x1021, .init = 0xffff, .refin = 1, .refout = 1, .xorout = 0x0000, } },
        { .name = "CRC-16/RIELLO",      .check = 0x63d0, { .width = 16, .poly = 0x1021, .init = 0xb2aa, .refin = 1, .refout = 1, .xorout = 0x0000, } },
        { .name = "CRC-16/T10-DIF",     .check = 0xd0db, { .width = 16, .poly = 0x8bb7, .init = 0x0000, .refin = 0, .refout = 0, .xorout = 0x0000, } },
        { .name = "CRC-16/TELEDISK",    .check = 0x0fb3, { .width = 16, .poly = 0xa097, .init = 0x0000, .refin = 0, .refout = 0, .xorout = 0x0000, } },
        { .name = "CRC-16/TMS37157",    .check = 0x26b1, { .width = 16, .poly = 0x1021, .init = 0x89ec, .refin = 1, .refout = 1, .xorout = 0x0000, } },
        { .name = "CRC-16/USB",         .check = 0xb4c8, { .width = 16, .poly = 0x8005, .init = 0xffff, .refin = 1, .refout = 1, .xorout = 0xffff, } },
        { .name = "CRC-A",              .check = 0xbf05, { .width = 16, .poly = 0x1021, .init = 0xc6c6, .refin = 1, .refout = 1, .xorout = 0x0000, } },
        { .name = "CRC-16/CCITT",       .check = 0x2189, { .width = 16, .poly = 0x1021, .init = 0x0000, .refin = 1, .refout = 1, .xorout = 0x0000, } },
        { .name = "MODBUS",             .check = 0x4b37, { .width = 16, .poly = 0x8005, .init = 0xffff, .refin = 1, .refout = 1, .xorout = 0x0000, } },
        { .name = "X-25",               .check = 0x906e, { .width = 16, .poly = 0x1021, .init = 0xffff, .refin = 1, .refout = 1, .xorout = 0xffff, } },
        { .name = "XMODEM",             .check = 0x31c3, { .width = 16, .poly = 0x1021, .init = 0x0000, .refin = 0, .refout = 0, .xorout = 0x0000, } },

        { .name = "CRC-24",           .check = 0x21cf02, { .width = 24, .poly = 0x864cfb, .init = 0xb704ce, .refin = 0, .refout = 0, .xorout = 0x000000, } },
        { .name = "CRC-24/FLEXRAY-A", .check = 0x7979bd, { .width = 24, .poly = 0x5d6dcb, .init = 0xfedcba, .refin = 0, .refout = 0, .xorout = 0x000000, } },
        { .name = "CRC-24/FLEXRAY-B", .check = 0x1f23b8, { .width = 24, .poly = 0x5d6dcb, .init = 0xabcdef, .refin = 0, .refout = 0, .xorout = 0x000000, } },

        { .name = "CRC-31/PHILIPS", .check = 0x0ce9e46c, { .width = 31, .poly = 0x04c11db7, .init = 0x7fffffff, .refin = 0, .refout = 0, .xorout = 0x7fffffff, } },

        { .name = "CRC-32",         .check = 0xcbf43926, { .width = 32, .poly = 0x04c11db7, .init = 0xffffffff, .refin = 1, .refout = 1, .xorout = 0xffffffff, } },
        { .name = "CRC-32/BZIP2",   .check = 0xfc891918, { .width = 32, .poly = 0x04c11db7, .init = 0xffffffff, .refin = 0, .refout = 0, .xorout = 0xffffffff, } },
        { .name = "CRC-32C",        .check = 0xe3069283, { .width = 32, .poly = 0x1edc6f41, .init = 0xffffffff, .refin = 1, .refout = 1, .xorout = 0xffffffff, } },
        { .name = "CRC-32D",        .check = 0x87315576, { .width = 32, .poly = 0xa833982b, .init = 0xffffffff, .refin = 1, .refout = 1, .xorout = 0xffffffff, } },
        { .name = "CRC-32/MPEG-2",  .check = 0x0376e6e7, { .width = 32, .poly = 0x04c11db7, .init = 0xffffffff, .refin = 0, .refout = 0, .xorout = 0x00000000, } },
        { .name = "CRC-32/POSIX",   .check = 0x765e7680, { .width = 32, .poly = 0x04c11db7, .init = 0x00000000, .refin = 0, .refout = 0, .xorout = 0xffffffff, } },
        { .name = "CRC-32Q",        .check = 0x3010bf7f, { .width = 32, .poly = 0x814141ab, .init = 0x00000000, .refin = 0, .refout = 0, .xorout = 0x00000000, } },
        { .name = "JAMCRC",         .check = 0x340bc6d9, { .width = 32, .poly = 0x04c11db7, .init = 0xffffffff, .refin = 1, .refout = 1, .xorout = 0x00000000, } },
        { .name = "XFER",           .check = 0xbd0be338, { .width = 32, .poly = 0x000000af, .init = 0x00000000, .refin = 0, .refout = 0, .xorout = 0x00000000, } },

        { .name = "CRC-40/GSM", .check = 0xd4164fc646, { .width = 40, .poly = 0x0004820009, .init = 0x0000000000, .refin = 0, .refout = 0, .xorout = 0xffffffffff, } },

        { .name="CRC-64",    .check=0x6c40df5f0b497347, { .width=64, .poly=0x42f0e1eba9ea3693, .init=0x0000000000000000, .refin=0, .refout=0, .xorout=0x0000000000000000 } },
        { .name="CRC-64/WE", .check=0x62ec59e3f1a4f00a, { .width=64, .poly=0x42f0e1eba9ea3693, .init=0xffffffffffffffff, .refin=0, .refout=0, .xorout=0xffffffffffffffff } },
        { .name="CRC-64/XZ", .check=0x995dc9bbdf1939fa, { .width=64, .poly=0x42f0e1eba9ea3693, .init=0xffffffffffffffff, .refin=1, .refout=1, .xorout=0xffffffffffffffff } },

};
unsigned num_test_cfgs = sizeof(test_cfgs) / sizeof(test_cfgs[0]);

static int hex_digits(unsigned width)
{
        return (width + 3) / 4;
}

int main(void)
{
        char *test = "123456789";

        for (unsigned i=0; i<num_test_cfgs; i++) {
                printf("Checking %-20s: expected check value 0x%.*"PRIx64"...", test_cfgs[i].name, hex_digits(test_cfgs[i].cfg.width), test_cfgs[i].check);

                uint64_t crc = crc_calculate(&test_cfgs[i].cfg, (uint8_t *) test, strlen(test));

                printf(" got 0x%.*"PRIx64"\n", hex_digits(test_cfgs[i].cfg.width), crc);

                TEST(crc == test_cfgs[i].check);
        }
}


/* Local Variables:            */
/* mode: c                     */
/* c-basic-offset: 8           */
/* indent-tabs-mode: nil       */
/* fill-column: 120            */
/* c-backslash-max-column: 120 */
/* End:                        */
