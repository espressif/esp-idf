// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sdkconfig.h"

#ifdef CONFIG_SUPPORT_TERMIOS

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

#endif // CONFIG_SUPPORT_TERMIOS
