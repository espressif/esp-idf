/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTIME_H_
#define _UTIME_H_

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct utimbuf {
    time_t actime;       // access time
    time_t modtime;      // modification time
};

int utime(const char *path, const struct utimbuf *times);

#ifdef __cplusplus
};
#endif

#endif /* _UTIME_H_ */
