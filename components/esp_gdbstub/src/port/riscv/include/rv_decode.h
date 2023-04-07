/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_gdbstub_common.h"

uintptr_t rv_compute_next_pc(esp_gdbstub_frame_t *frame, uintptr_t inst_addr);
