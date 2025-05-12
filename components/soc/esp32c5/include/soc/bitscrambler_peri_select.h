/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/*
 * @brief BitScrambler attachment targets
 * Note that these are the values written to HP_SYSTEM_BITSCRAMBLER_PERI_SEL_REG.
 */

#define SOC_BITSCRAMBLER_ATTACH_NONE    -1
#define SOC_BITSCRAMBLER_ATTACH_GPSPI2  1
#define SOC_BITSCRAMBLER_ATTACH_UHCI    2
#define SOC_BITSCRAMBLER_ATTACH_I2S0    3
#define SOC_BITSCRAMBLER_ATTACH_AES     4
#define SOC_BITSCRAMBLER_ATTACH_SHA     5
#define SOC_BITSCRAMBLER_ATTACH_ADC     6
#define SOC_BITSCRAMBLER_ATTACH_PARL_IO 7
#define SOC_BITSCRAMBLER_ATTACH_MAX     7
