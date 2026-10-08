/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#ifndef __HCI_COMMON_H__
#define __HCI_COMMON_H__

#include "sdkconfig.h"
#include "esp_err.h"

#if CONFIG_ENABLE_HCI_CONSOLE
/**
 * @brief Start the console REPL and register the enabled command groups of this example.
 */
esp_err_t hci_console_init(void);
#endif

#endif /* __HCI_COMMON_H__ */
