/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ROM_BIGINT_H_
#define _ROM_BIGINT_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void ets_bigint_enable(void);

void ets_bigint_disable(void);

int ets_bigint_multiply(const uint32_t *x, const uint32_t *y, uint32_t len_words);

int ets_bigint_modmult(const uint32_t *x, const uint32_t *y, const uint32_t *m, uint32_t m_dash, const uint32_t *rb, uint32_t len_words);

int ets_bigint_modexp(const uint32_t *x, const uint32_t *y, const uint32_t *m, uint32_t m_dash, const uint32_t *rb, bool constant_time, uint32_t len_words);

void ets_bigint_wait_finish(void);

int ets_bigint_getz(uint32_t *z, uint32_t len_words);

#ifdef __cplusplus
}
#endif

#endif /* _ROM_BIGINT_H_ */
