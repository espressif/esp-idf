/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio-bufio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define GET_BUF(mf) (*(mf->pbuf))
#define GET_SIZE(mf) (*(mf->psize))

struct __file_mem {
    struct __file_ext xfile;
    char    **pbuf;
    size_t  *psize; // == strlen(*pbuf);
    size_t  len;    // Total allocated size
    size_t  pos;
};

static int __fmem_put(char c, FILE *f)
{
    struct __file_mem *mf = (struct __file_mem *) f;
    if (mf->len == mf->pos) {
        size_t newsize = mf->len * 3 / 2;
        char *tmp = realloc(GET_BUF(mf), newsize);
        if (!tmp) {
            return _FDEV_ERR;
        }
        GET_BUF(mf) = tmp;
        mf->len = newsize;
    }
    GET_BUF(mf)[mf->pos++] = c;
    if (mf->pos > GET_SIZE(mf)) {
        GET_SIZE(mf) = mf->pos;
    }
    return (unsigned char) c;
}

static int __fmem_get(FILE *f)
{
    struct __file_mem *mf = (struct __file_mem *) f;
    int c;
    if (mf->pos < GET_SIZE(mf)) {
        c = (unsigned char) GET_BUF(mf)[mf->pos++];
        if (c == '\0') {
            c = _FDEV_EOF;
        }
    } else {
        c = _FDEV_ERR;
    }
    return c;
}

static int __fmem_flush(FILE *f)
{
    struct __file_mem *mf = (struct __file_mem *) f;
    GET_BUF(mf)[mf->pos] = '\0';
    GET_SIZE(mf) = mf->pos;
    return 0;
}

static off_t __fmem_seek(FILE *f, off_t pos, int whence)
{
    struct __file_mem *mf = (struct __file_mem *) f;

    switch (whence) {
    case SEEK_SET:
        break;
    case SEEK_CUR:
        pos += mf->pos;
        break;
    case SEEK_END:
        pos += GET_SIZE(mf);
        break;
    }
    if (pos < 0 || GET_SIZE(mf) < (size_t) pos) {
        return EOF;
    }
    mf->pos = pos;
    return pos;
}

static int __fmem_close(FILE *f)
{
    __fmem_flush(f);
    free(f);
    return 0;
}

FILE *
open_memstream(char **buf, size_t *size)
{
    struct __file_mem *mf;

    if (!buf || !size) {
        errno = EINVAL;
        return NULL;
    }
    /* Allocate file structure and necessary buffers */
    mf = calloc(1, sizeof(struct __file_mem));

    if (mf == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    *buf = malloc(BUFSIZ);
    if (*buf == NULL) {
        errno = ENOMEM;
        free(mf);
        return NULL;
    }

    *mf = (struct __file_mem) {
        .xfile = FDEV_SETUP_EXT(__fmem_put, __fmem_get, __fmem_flush, __fmem_close,
                                __fmem_seek, NULL, __SWR),
                 .pbuf = buf,
                 .psize = size,
                 .len = BUFSIZ,
                 .pos = 0,
    };
    *size = 0;
    *buf[0] = '\0';
    return (FILE *) mf;
}
