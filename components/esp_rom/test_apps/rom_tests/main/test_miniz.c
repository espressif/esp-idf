/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "unity.h"
#include "miniz.h"

#define DATASIZE (1024 * 32)

TEST_CASE("Test miniz compression/decompression", "[rom][miniz]")
{
    size_t inbytes = 0, outbytes = 0, inpos = 0, outpos = 0, compsz;
    printf("Allocating data buffer and filling it with semi-random data\r\n");
    uint8_t *inbuf = calloc(1, DATASIZE);
    TEST_ASSERT_NOT_NULL_MESSAGE(inbuf, "allocate input buffer failed");

    srand(0); // semi random data
    for (int i = 0; i < DATASIZE; i++) {
        inbuf[i] = (i & 0x01) ? rand() & 0xff : 0;
    }

    printf("Allocating compressor\r\n");
    tdefl_compressor *comp = calloc(1, sizeof(tdefl_compressor));
    TEST_ASSERT_NOT_NULL_MESSAGE(comp, "allocate tdefl_compressor failed");

    uint8_t *outbuf = calloc(1, DATASIZE);
    TEST_ASSERT_NOT_NULL_MESSAGE(outbuf, "allocate output buffer failed");

    printf("Compressing...\r\n");
    tdefl_status comp_status = tdefl_init(comp, NULL, NULL, TDEFL_WRITE_ZLIB_HEADER | 1500);
    TEST_ASSERT_EQUAL_MESSAGE(TDEFL_STATUS_OKAY, comp_status, "tdefl_init failed");

    while (1) {
        outbytes = DATASIZE - outpos;
        inbytes = DATASIZE - inpos;
        comp_status = tdefl_compress(comp, &inbuf[inpos], &inbytes, &outbuf[outpos], &outbytes, TDEFL_FINISH);
        inpos += inbytes;
        outpos += outbytes;
        printf("...Compressed %d into %d bytes\r\n", inpos, outpos);
        if (comp_status == TDEFL_STATUS_DONE) {
            break;
        } else if (comp_status != TDEFL_STATUS_OKAY) {
            TEST_ASSERT_MESSAGE(0, "tdefl_compress failed");
        }
    }

    compsz = outpos;
    free(comp);
    free(inbuf);

    inbuf = outbuf;
    outbuf = calloc(1, DATASIZE);
    TEST_ASSERT_NOT_NULL_MESSAGE(outbuf, "allocate output buffer failed");

    printf("Decompressing...\r\n");
    tinfl_decompressor *decomp = calloc(1, sizeof(tinfl_decompressor));
    TEST_ASSERT_NOT_NULL_MESSAGE(decomp, "allocate tinfl_decompressor failed");
    tinfl_init(decomp);

    inpos = 0;
    outpos = 0;
    while (1) {
        outbytes = DATASIZE - outpos;
        inbytes = compsz - inpos;
        tinfl_status decomp_status = tinfl_decompress(decomp, &inbuf[inpos], &inbytes, outbuf, &outbuf[outpos], &outbytes, TINFL_FLAG_PARSE_ZLIB_HEADER);
        inpos += inbytes;
        outpos += outbytes;
        printf("...Decompressed %d into %d bytes\r\n", inpos, outpos);
        if (decomp_status == TINFL_STATUS_DONE) {
            break;
        } else if (decomp_status < TINFL_STATUS_DONE) {
            printf("decomp status=%d\r\n", decomp_status);
            TEST_ASSERT_MESSAGE(0, "tinfl_decompress failed");
        }
    }

    printf("Verifying data between compression and decompression...\r\n");
    srand(0); // semi random data
    for (int i = 0; i < DATASIZE; i++) {
        uint8_t original = (i & 1) ? rand() & 0xff : 0;
        TEST_ASSERT_EQUAL_MESSAGE(original, outbuf[i], "data after decompression doesn't match the original one");
    }
    printf("Great Success!\n");
    free(inbuf);
    free(outbuf);
    free(decomp);
}
