/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_openthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function initializes the NCP for a single instance
 *
 * @param[in]    aInstance   The OpenThread instance.
 */
void otAppNcpInit(otInstance *aInstance);

/**
 * @brief This function initializes the NCP for multiple instances
 *
 * @param[in]    aInstances   The array of OpenThread instances.
 * @param[in]    aCount       The number of instances.
 */
void otAppNcpInitMulti(otInstance **aInstances, uint8_t aCount);

#if CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE
esp_err_t esp_console_redirect_to_otlog(void);
#endif // CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE

#ifdef __cplusplus
}
#endif
