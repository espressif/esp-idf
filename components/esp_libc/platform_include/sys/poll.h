/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_PLATFORM_SYS_POLL_H_
#define _ESP_PLATFORM_SYS_POLL_H_

#define POLLIN      (1u << 0)      /* data other than high-priority may be read without blocking */
#define POLLRDNORM  (1u << 1)      /* normal data may be read without blocking */
#define POLLRDBAND  (1u << 2)      /* priority data may be read without blocking */
#define POLLPRI     (POLLRDBAND)   /* high-priority data may be read without blocking */
// Note: POLLPRI is made equivalent to POLLRDBAND in order to fit all these events into one byte
#define POLLOUT     (1u << 3)      /* normal data may be written without blocking */
#define POLLWRNORM  (POLLOUT)      /* equivalent to POLLOUT */
#define POLLWRBAND  (1u << 4)      /* priority data my be written */
#define POLLERR     (1u << 5)      /* some poll error occurred */
#define POLLHUP     (1u << 6)      /* file descriptor was "hung up" */
#define POLLNVAL    (1u << 7)      /* the specified file descriptor is invalid */

#ifdef __cplusplus
extern "C" {
#endif

struct pollfd {
    int fd;        /* The descriptor. */
    short events;  /* The event(s) is/are specified here. */
    short revents; /* Events found are returned here. */
};

typedef unsigned int nfds_t;

int poll(struct pollfd *fds, nfds_t nfds, int timeout);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _ESP_PLATFORM_SYS_POLL_H_
