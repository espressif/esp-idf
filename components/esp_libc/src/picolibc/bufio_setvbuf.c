/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio-bufio.h>
#include <stdlib.h>
#include <stdbool.h>

int __real___bufio_setvbuf(FILE *f, char *buf, int mode, size_t size);
int __wrap___bufio_setvbuf(FILE *f, char *buf, int mode, size_t size)
{
    // File lock is already acquired by the caller
    int ret = -1;
    bool workaround = false;
    if (mode == _IONBF) {
        workaround = true;
        mode = _IOFBF;
        size = 1;
        buf = malloc(size);
        if (buf == NULL) {
            goto exit;
        }
    }
    ret = __real___bufio_setvbuf(f, buf, mode, size);
    if (workaround) {
        struct __file_bufio *bf = (struct __file_bufio *)f;

        // Free buf if not applied
        if (ret != 0 || bf->buf != buf) {
            free(buf);
            goto exit;
        }
        bf->bflags |= __BALL;
    }

exit:
    return ret;
}
