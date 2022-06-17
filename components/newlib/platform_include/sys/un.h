/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ESP_PLATFORM_SYS_UN_H_
#define _ESP_PLATFORM_SYS_UN_H_

#define	AF_UNIX		1		/* local to host (pipes) */

struct sockaddr_un {
            short                    sun_family;                /*AF_UNIX*/
            char                     sun_path[108];             /*path name */
};

#endif // _ESP_PLATFORM_SYS_UN_H_
