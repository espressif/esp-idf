/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/clic_reg.h"
#include "soc/soc_caps.h"

#define INTERRUPT_CURRENT_CORE_INT_THRESH_REG   (CLIC_INT_THRESH_REG)
#define INTERRUPT_OTHER_CORE_INT_THRESH_REG     (CLIC_INT_THRESH_REG + DUALCORE_CLIC_CTRL_OFF)
