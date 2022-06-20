/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef _ESP_SYS_UNISTD_H
#define _ESP_SYS_UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif

#include_next <sys/unistd.h>

int truncate(const char *, off_t __length);
int gethostname(char *__name, size_t __len);

#ifdef __cplusplus
}
#endif
#endif /* _SYS_UNISTD_H */
