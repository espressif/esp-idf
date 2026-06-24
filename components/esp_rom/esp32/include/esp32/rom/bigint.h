/*
  ROM functions for hardware bigint support.

  It is not recommended to use these functions directly,
  use the wrapper functions in hwcrypto/mpi.h instead.

 */
/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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

//TODO: add comment here
void ets_bigint_enable(void);

void ets_bigint_disable(void);

void ets_bigint_wait_finish(void);

bool ets_bigint_mod_power_prepare(uint32_t *x, uint32_t *y, uint32_t *m,
                                  uint32_t m_dash, uint32_t *rb, uint32_t len, bool again);

bool ets_bigint_mod_power_getz(uint32_t *z, uint32_t len);

bool ets_bigint_mult_prepare(uint32_t *x, uint32_t *y, uint32_t len);

bool ets_bigint_mult_getz(uint32_t *z, uint32_t len);

bool ets_bigint_montgomery_mult_prepare(uint32_t *x, uint32_t *y, uint32_t *m,
                                        uint32_t m_dash, uint32_t len, bool again);

bool ets_bigint_montgomery_mult_getz(uint32_t *z, uint32_t len);

bool ets_bigint_mod_mult_prepare(uint32_t *x, uint32_t *y, uint32_t *m,
                                 uint32_t m_dash, uint32_t *rb, uint32_t len, bool again);

bool ets_bigint_mod_mult_getz(uint32_t *m, uint32_t *z, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* _ROM_BIGINT_H_ */
