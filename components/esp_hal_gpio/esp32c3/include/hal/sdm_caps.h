/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define SDM_CAPS_GET(_attr) _SDM_ ## _attr

#define _SDM_INST_NUM                    1  /* Number of SDM instances */
#define _SDM_CHANS_PER_INST              4  /* Number of channels in each SDM instance */
#define _SDM_FUNC_CLOCK_SUPPORT_APB      1  /* SDM function clock can be sourced from APB */
