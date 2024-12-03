/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include <sys/lock.h>
#include <sys/reent.h>

void flockfile(FILE *fp)
{
    if (fp && !(fp->_flags & __SSTR)) {
        __lock_acquire_recursive(fp->_lock);
    }
}

int ftrylockfile(FILE *fp)
{
    if (fp && !(fp->_flags & __SSTR)) {
        return __lock_try_acquire_recursive(fp->_lock);
    }
    return 0;
}

void funlockfile(FILE *fp)
{
    if (fp && !(fp->_flags & __SSTR)) {
        __lock_release_recursive(fp->_lock);
    }
}
