/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define SDM_CAPS_GET(_attr) _SDM_ ## _attr

#define _SDM_INST_NUM                    1  /* Number of SDM instances */
#define _SDM_CHANS_PER_INST              8  /* Number of channels in each SDM instance */
