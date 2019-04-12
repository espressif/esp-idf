/*
  ROM functions for hardware bigint support.

  It is not recommended to use these functions directly,
  use the wrapper functions in hwcrypto/mpi.h instead.

 */
// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
