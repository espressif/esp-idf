/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint64_t sleep_duration_us;  /* Configured sleep duration for periodic wakeup, if set the ulp will automatically schedule the next wakeup */
} ulp_lp_core_memory_shared_cfg_t;

/**
 * @brief Returns a pointer to a struct shared between the main cpu and lp core,
 *        intended for sharing variables between the ulp component and ulp binary
 *
 * @return ulp_lp_core_memory_shared_cfg_t* Pointer to the shared config struct
 */
ulp_lp_core_memory_shared_cfg_t* ulp_lp_core_memory_shared_cfg_get(void);

#ifdef __cplusplus
}
#endif
