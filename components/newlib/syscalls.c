// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <reent.h>
#include <sys/fcntl.h>


int system(const char* str)
{
    errno = ENOSYS;
    return -1;
}

int _system_r(struct _reent *r, const char *str)
{
    __errno_r(r) = ENOSYS;
    return -1;
}

int raise(int sig)
{
    abort();
}

int _raise_r(struct _reent *r, int sig)
{
    abort();
}

void* _sbrk_r(struct _reent *r, ptrdiff_t sz)
{
    abort();
}

int _getpid_r(struct _reent *r)
{
    __errno_r(r) = ENOSYS;
    return -1;
}

int _kill_r(struct _reent *r, int pid, int sig)
{
    __errno_r(r) = ENOSYS;
    return -1;
}

void _exit(int __status)
{
    abort();
}

/* Replaces newlib fcntl, which has been compiled without HAVE_FCNTL */
int fcntl(int fd, int cmd, ...)
{
    va_list args;
    va_start(args, cmd);
    int arg = va_arg(args, int);
    va_end(args);
    struct _reent* r = __getreent();
    return _fcntl_r(r, fd, cmd, arg);
}

/* No-op function, used to force linking this file,
   instead of the syscalls implementation from libgloss.
 */
void newlib_include_syscalls_impl()
{
}
