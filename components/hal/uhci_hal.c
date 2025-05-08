/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for UHCI (common part)
#include "hal/uhci_hal.h"
#include "hal/uhci_ll.h"

void uhci_hal_init(uhci_hal_context_t *hal, int uhci_num)
{
    hal->dev = UHCI_LL_GET_HW(uhci_num);
    uhci_ll_init(hal->dev);
}

void uhci_hal_deinit(uhci_hal_context_t *hal)
{
    hal->dev = NULL;
}
