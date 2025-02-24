/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/interrupt_matrix_reg.h"
#include "soc/clic_reg.h"
#include "soc/soc_caps.h"

// TODO: [ESP32H4] IDF-12303 inherit from verify code, need check

#define INTERRUPT_CURRENT_CORE_INT_THRESH_REG   (CLIC_INT_THRESH_REG)
#define INTERRUPT_OTHER_CORE_INT_THRESH_REG     (CLIC_INT_THRESH_REG + DUALCORE_CLIC_CTRL_OFF)
