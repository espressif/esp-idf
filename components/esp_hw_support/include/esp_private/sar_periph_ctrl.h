/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * SAR related peripherals are interdependent. This file
 * provides a united control to these registers, as multiple
 * components require these controls.
 *
 * See target/sar_periph_ctrl.c to know involved peripherals
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise SAR related peripheral register settings
 * Should only be used when running into app stage
 */
void sar_periph_ctrl_init(void);

#ifdef __cplusplus
}
#endif
