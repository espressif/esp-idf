/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/reent.h>
#include "esp_attr.h"

/**
 * This is the replacement for newlib's _REENT_INIT_PTR and __sinit.
 * The problem with __sinit is that it allocates three FILE structures
 * (stdin, stdout, stderr). Having individual standard streams for each task
 * is a bit too much on a small embedded system. So we point streams
 * to the streams of the global struct _reent, which are initialized in
 * startup code.
 */
void IRAM_ATTR esp_reent_init(struct _reent* r)
{
    memset(r, 0, sizeof(*r));
    _REENT_STDIN(r) = _REENT_STDIN(_GLOBAL_REENT);
    _REENT_STDOUT(r) = _REENT_STDOUT(_GLOBAL_REENT);
    _REENT_STDERR(r) = _REENT_STDERR(_GLOBAL_REENT);
    _REENT_CLEANUP(r) = _REENT_CLEANUP(_GLOBAL_REENT);
    _REENT_SDIDINIT(r) = _REENT_SDIDINIT(_GLOBAL_REENT);
}

/* only declared in private stdio header file, local.h */
extern void __sfp_lock_acquire(void);
extern void __sfp_lock_release(void);

void esp_reent_cleanup(void)
{
    struct _reent* r = __getreent();
    _reclaim_reent(r);

    r->_emergency = NULL;
    r->_mp = NULL;
    r->_r48 = NULL;
    r->_localtime_buf = NULL;
    r->_asctime_buf = NULL;
    r->_signal_buf = NULL;
    r->_misc = NULL;
    r->_cvtbuf = NULL;

    /* Clean up "glue" (lazily-allocated FILE objects) */
    struct _glue* prev = &_REENT_SGLUE(_GLOBAL_REENT);
    for (struct _glue* cur = _REENT_SGLUE(_GLOBAL_REENT)._next; cur != NULL;) {
        if (cur->_niobs == 0) {
            cur = cur->_next;
            continue;
        }
        bool has_open_files = false;
        for (int i = 0; i < cur->_niobs; ++i) {
            FILE* fp = &cur->_iobs[i];
            if (fp->_flags != 0) {
                has_open_files = true;
                break;
            }
        }
        if (has_open_files) {
            prev = cur;
            cur = cur->_next;
            continue;
        }
        struct _glue* next = cur->_next;
        prev->_next = next;
        free(cur);
        cur = next;
    }
}
