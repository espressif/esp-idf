/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#ifdef CONFIG_VFS_SUPPORT_TERMIOS

#include <sys/termios.h>
#include <sys/errno.h>

speed_t cfgetispeed(const struct termios *p)
{
    return p ? p->c_ispeed : B0;
}

speed_t cfgetospeed(const struct termios *p)
{
    return p ? p->c_ospeed : B0;
}

int cfsetispeed(struct termios *p, speed_t sp)
{
    if (p) {
        p->c_ispeed = sp;
        return 0;
    } else {
        errno = EINVAL;
        return -1;
    }
}

int cfsetospeed(struct termios *p, speed_t sp)
{
    if (p) {
        p->c_ospeed = sp;
        return 0;
    } else {
        errno = EINVAL;
        return -1;
    }
}

#endif // CONFIG_VFS_SUPPORT_TERMIOS
