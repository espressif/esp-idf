/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief MCPWM peripheral contains many submodules, whose drivers are scattered in different header files.
 *        This header file serves as a prelude, contains every thing that is needed to work with the MCPWM peripheral.
 */

#pragma once

#include "driver/mcpwm_timer.h"
#include "driver/mcpwm_oper.h"
#include "driver/mcpwm_cmpr.h"
#include "driver/mcpwm_gen.h"
#include "driver/mcpwm_fault.h"
#include "driver/mcpwm_sync.h"
#include "driver/mcpwm_cap.h"
#include "driver/mcpwm_etm.h"
