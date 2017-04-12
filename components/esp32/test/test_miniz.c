

#include <stdio.h>
#include "rom/miniz.h"
#include "unity.h"


#define DATASIZE (1024*64)

TEST_CASE("Test miniz compression/decompression", "[miniz][ignore]")
{
    int x;
    char b;
    char *inbuf, *outbuf;
    tdefl_compressor *comp;
    tinfl_decompressor *decomp;
    tdefl_status status;
    size_t inbytes = 0, outbytes = 0, inpos = 0, outpos = 0, compsz;
    printf("Allocating data buffer and filling it with semi-random data\n");
    inbuf = malloc(DATASIZE);
    TEST_ASSERT(inbuf != NULL);
    srand(0);
    for (x = 0; x < DATASIZE; x++) {
        inbuf[x] = (x & 1) ? rand() & 0xff : 0;
    }
    printf("Allocating compressor & outbuf (%d bytes)\n", sizeof(tdefl_compressor));
    comp = malloc(sizeof(tdefl_compressor));
    TEST_ASSERT(comp != NULL);
    outbuf = malloc(DATASIZE);
    TEST_ASSERT(outbuf != NULL);
    printf("Compressing...\n");
    status = tdefl_init(comp, NULL, NULL, TDEFL_WRITE_ZLIB_HEADER | 1500);
    TEST_ASSERT(status == TDEFL_STATUS_OKAY);
    while (inbytes != DATASIZE) {
        outbytes = DATASIZE - outpos;
        inbytes = DATASIZE - inpos;
        tdefl_compress(comp, &inbuf[inpos], &inbytes, &outbuf[outpos], &outbytes, TDEFL_FINISH);
        printf("...Compressed %d into %d bytes\n", inbytes, outbytes);
        inpos += inbytes; outpos += outbytes;
    }
    compsz = outpos;
    free(comp);
    //Kill inbuffer
    for (x = 0; x < DATASIZE; x++) {
        inbuf[x] = 0;
    }
    free(inbuf);

    inbuf = outbuf;
    outbuf = malloc(DATASIZE);
    TEST_ASSERT(outbuf != NULL);
    printf("Reinflating...\n");
    decomp = malloc(sizeof(tinfl_decompressor));
    TEST_ASSERT(decomp != NULL);
    tinfl_init(decomp);
    inpos = 0; outpos = 0;
    while (inbytes != compsz) {
        outbytes = DATASIZE - outpos;
        inbytes = compsz - inpos;
        tinfl_decompress(decomp, (const mz_uint8 *)&inbuf[inpos], &inbytes, (uint8_t *)outbuf, (mz_uint8 *)&outbuf[outpos], &outbytes, TINFL_FLAG_PARSE_ZLIB_HEADER);
        printf("...Decompressed %d into %d bytes\n", inbytes, outbytes);
        inpos += inbytes; outpos += outbytes;
    }
    printf("Checking if same...\n");
    srand(0);
    for (x = 0; x < DATASIZE; x++) {
        b = (x & 1) ? rand() & 0xff : 0;
        if (outbuf[x] != b) {
            printf("Pos %x: %hhx!=%hhx\n", x, outbuf[x], b);
            TEST_ASSERT(0);
        }
    }
    printf("Great Success!\n");
    free(inbuf);
    free(outbuf);
    free(decomp);
}
