/*
 * SPDX-FileCopyrightText: 2010-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>

extern int _bss_start_low, _bss_start_high;
extern int _bss_end_low, _bss_end_high;

char buf1[100 * 1024];
char buf2[100 * 1024];

static void test_mem_write(char* buf, size_t size_bytes, int seed)
{
    srand(seed);
    for (size_t i = 0; i < size_bytes; ++i) {
        buf[i] = (char) (rand() % 256);
    }
}

static void test_mem_read(char* buf, size_t size_bytes, int seed)
{
    size_t num_errors = 0;
    srand(seed);
    printf("Testing at %p ... ", buf);
    for (size_t i = 0; i < size_bytes; ++i) {
        if (buf[i] != (char) (rand() % 256)) {
            ++num_errors;
        }
    }
    printf("%s!\n", num_errors == 0 ? "OK" : "ERROR");
}

void app_main(void)
{
    if (! (((void *)&_bss_start_low <= (void *)buf2) && ((void *)buf2 < (void *)&_bss_end_low)))
        printf("buf2 (%p) is expected to be placed in low sram (%p .. %p)\n", buf2, &_bss_start_low, &_bss_end_low);
    else
        printf("buf2 placed in low sram\n");
    if (! ((void *)&_bss_start_high <= (void *)buf1 && (void *)buf1 < (void *)&_bss_end_high))
        printf("buf1 (%p) is expected to be placed in high sram (%p .. %p)\n", buf1, &_bss_start_high, &_bss_end_high);
    else
        printf("buf1 placed in high sram\n");

    test_mem_write(buf2, sizeof(buf1), 1);
    test_mem_write(buf1, sizeof(buf1), 0);
    test_mem_read(buf1, sizeof(buf1), 0);
    test_mem_read(buf2, sizeof(buf2), 1);
    test_mem_write(buf2, sizeof(buf1), 1);
    test_mem_read(buf1, sizeof(buf1), 0);
    test_mem_read(buf2, sizeof(buf2), 1);
}
