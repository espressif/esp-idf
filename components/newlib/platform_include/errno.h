
/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ESP_PLATFORM_ERRNO_H_
#define _ESP_PLATFORM_ERRNO_H_

#include_next "errno.h"

//
// Possibly define some missing errors
//
#ifndef ESHUTDOWN
#define ESHUTDOWN      110  /* Cannot send after transport endpoint shutdown */
#endif

#ifndef EAI_SOCKTYPE
#define	EAI_SOCKTYPE	10	/* ai_socktype not supported */
#endif

#ifndef EAI_AGAIN
#define	EAI_AGAIN	     2   /* temporary failure in name resolution */
#endif

#ifndef EAI_BADFLAGS
#define	EAI_BADFLAGS	 3	 /* invalid value for ai_flags */
#endif

#endif // _ESP_PLATFORM_ERRNO_H_
