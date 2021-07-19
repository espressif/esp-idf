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

#ifndef __ESP_SYS_SELECT_H__
#define __ESP_SYS_SELECT_H__

/* Newlib 2.2.0 does not provide sys/select.h, and fd_set is defined in sys/types.h */
#include <sys/types.h>
#ifndef fd_set
#include_next <sys/select.h>
#else // fd_set
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // fd_set

#if defined(FD_ISSET) || defined(FD_SET) || defined(FD_CLR)
#undef FD_SET
#undef FD_CLR
#undef FD_ISSET

#define __FD_SAFE_SET(n, code) do { if ((unsigned)(n) < FD_SETSIZE) { code; } } while(0)
#define __FD_SAFE_GET(n, code) (((unsigned)(n) < FD_SETSIZE) ? (code) : 0)

#define FD_SET(n, p)	__FD_SAFE_SET(n, ((p)->fds_bits[(n) / NFDBITS] |=  (1L << ((n) % NFDBITS))))
#define FD_CLR(n, p)	__FD_SAFE_SET(n, ((p)->fds_bits[(n) / NFDBITS] &= ~(1L << ((n) % NFDBITS))))
#define FD_ISSET(n, p)	__FD_SAFE_GET(n, ((p)->fds_bits[(n) / NFDBITS] &   (1L << ((n) % NFDBITS))))
#endif // FD_ISSET || FD_SET || FD_CLR

#endif //__ESP_SYS_SELECT_H__
