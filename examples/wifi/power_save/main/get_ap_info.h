/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#if CONFIG_EXAMPLE_GET_AP_INFO_FROM_STDIN
/**
 * Initialize vfs console and prompt user input the AP info
 */
void get_ap_info_from_stdin(void);
#endif

/**
 * Get the AP ssid from user input or configure
 * @return  The string of SSID
 */
char *get_ap_ssid(void);

/**
 * Get the AP password from user input or configure
 * @return  The string of password
 */
char *get_ap_password(void);

#ifdef __cplusplus
}
#endif
