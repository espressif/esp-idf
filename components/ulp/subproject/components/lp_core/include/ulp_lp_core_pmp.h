/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#if CONFIG_ULP_LP_CORE_MEMPROT
void lp_core_configure_pmp(void);
#endif
