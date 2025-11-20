/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_openthread.h>

#ifdef __cplusplus
extern "C" {
#endif

void otAppNcpInit(otInstance *aInstance);

#if CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE
esp_err_t esp_console_redirect_to_otlog(void);
#endif // CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE

#ifdef __cplusplus
}
#endif
