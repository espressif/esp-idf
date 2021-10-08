/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SOC_TOUCH_SHIELD_CHANNEL            (14) /*!< The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) */
#define SOC_TOUCH_DENOISE_CHANNEL           (0)  /*!< T0 is an internal channel that does not have a corresponding external GPIO.
                                                    T0 will work simultaneously with the measured channel Tn. Finally, the actual
                                                    measured value of Tn is the value after subtracting lower bits of T0. */

#ifdef __cplusplus
}
#endif
