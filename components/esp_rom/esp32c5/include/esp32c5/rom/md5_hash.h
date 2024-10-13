/*
 * SPDX-FileCopyrightText: 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * MD5 internal definitions
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef _ROM_MD5_HASH_H_
#define _ROM_MD5_HASH_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MD5Context {
    uint32_t buf[4];
    uint32_t bits[2];
    uint8_t in[64];
};

void MD5Init(struct MD5Context *context);
void MD5Update(struct MD5Context *context, unsigned char const *buf, unsigned len);
void MD5Final(unsigned char digest[16], struct MD5Context *context);

#ifdef __cplusplus
}
#endif

#endif /* _ROM_MD5_HASH_H_ */
