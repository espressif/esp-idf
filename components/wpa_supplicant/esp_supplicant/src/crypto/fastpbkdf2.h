/*
 * SPDX-FileCopyrightText: 2015 Joseph Birr-Pixton <jpixton@gmail.com>
 *
 * SPDX-License-Identifier: CC0-1.0
 */

/*
 * fastpbkdf2 - Faster PBKDF2-HMAC calculation
 * Written in 2015 by Joseph Birr-Pixton <jpixton@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all
 * copyright and related and neighboring rights to this software to the
 * public domain worldwide. This software is distributed without any
 * warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication
 * along with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef FASTPBKDF2_H
#define FASTPBKDF2_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Calculates PBKDF2-HMAC-SHA1.
 *
 *  @p npw bytes at @p pw are the password input.
 *  @p nsalt bytes at @p salt are the salt input.
 *  @p iterations is the PBKDF2 iteration count and must be non-zero.
 *  @p nout bytes of output are written to @p out.  @p nout must be non-zero.
 *
 *  This function cannot fail; it does not report errors.
 */
void fastpbkdf2_hmac_sha1(const uint8_t *pw, size_t npw,
                          const uint8_t *salt, size_t nsalt,
                          uint32_t iterations,
                          uint8_t *out, size_t nout);
#ifdef __cplusplus
}
#endif

#endif
