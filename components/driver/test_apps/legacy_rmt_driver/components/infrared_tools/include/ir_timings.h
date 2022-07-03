/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timings for NEC protocol
 *
 */
#define NEC_LEADING_CODE_HIGH_US (9000)
#define NEC_LEADING_CODE_LOW_US (4500)
#define NEC_PAYLOAD_ONE_HIGH_US (560)
#define NEC_PAYLOAD_ONE_LOW_US (1690)
#define NEC_PAYLOAD_ZERO_HIGH_US (560)
#define NEC_PAYLOAD_ZERO_LOW_US (560)
#define NEC_REPEAT_CODE_HIGH_US (9000)
#define NEC_REPEAT_CODE_LOW_US (2250)
#define NEC_ENDING_CODE_HIGH_US (560)

/**
 * @brief Timings for RC5 protocol
 *
 */
#define RC5_PULSE_DURATION_US (889)

#ifdef __cplusplus
}
#endif
