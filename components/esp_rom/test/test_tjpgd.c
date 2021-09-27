#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "unity.h"

#include "esp_rom_tjpgd.h"
#include "test_tjpgd_logo.h"

typedef struct {
    const unsigned char *inData;
    int inPos;
    unsigned char *outData;
    int outW;
    int outH;
} JpegDev;

static uint32_t infunc(esp_rom_tjpgd_dec_t *decoder, uint8_t *buf, uint32_t len)
{
    JpegDev *jd = (JpegDev *)decoder->device;
    printf("Reading %d bytes from pos %d\n", len, jd->inPos);
    if (buf != NULL) {
        memcpy(buf, jd->inData + jd->inPos, len);
    }
    jd->inPos += len;
    return len;
}

static uint32_t outfunc(esp_rom_tjpgd_dec_t *decoder, void *bitmap, esp_rom_tjpgd_rect_t *rect)
{
    unsigned char *in = (unsigned char *)bitmap;
    unsigned char *out;
    int y;
    printf("Rect %d,%d - %d,%d\n", rect->top, rect->left, rect->bottom, rect->right);
    JpegDev *jd = (JpegDev *)decoder->device;
    for (y = rect->top; y <= rect->bottom; y++) {
        out = jd->outData + ((jd->outW * y) + rect->left) * 3;
        memcpy(out, in, ((rect->right - rect->left) + 1) * 3);
        in += ((rect->right - rect->left) + 1) * 3;
    }
    return 1;
}

#define TESTW 48
#define TESTH 48
#define WORKSZ 3100

TEST_CASE("Test JPEG decompression library", "[rom][tjpgd]")
{
    char aapix[] = " .:;+=xX$$";
    unsigned char *decoded, *p;
    char *work;
    int r;
    int x, y, v;
    esp_rom_tjpgd_dec_t decoder;
    JpegDev jd;
    decoded = malloc(TESTW * TESTH * 3);
    for (x = 0; x < TESTW * TESTH * 3; x += 2) {
        decoded[x] = 0;
        decoded[x + 1] = 0xff;
    }
    work = malloc(WORKSZ);
    memset(work, 0, WORKSZ);

    jd.inData = logo_jpg;
    jd.inPos = 0;
    jd.outData = decoded;
    jd.outW = TESTW;
    jd.outH = TESTH;

    r = esp_rom_tjpgd_prepare(&decoder, infunc, work, WORKSZ, (void *)&jd);
    TEST_ASSERT_EQUAL(r, JDR_OK);
    r = esp_rom_tjpgd_decomp(&decoder, outfunc, 0);
    TEST_ASSERT_EQUAL(r, JDR_OK);

    p = decoded + 2;
    for (y = 0; y < TESTH; y++) {
        for (x = 0; x < TESTH; x++) {
            v = ((*p) * (sizeof(aapix) - 2) * 2) / 256;
            printf("%c%c", aapix[v / 2], aapix[(v + 1) / 2]);
            p += 3;
        }
        printf("%c%c", ' ', '\n');
    }

    free(work);
    free(decoded);
}
