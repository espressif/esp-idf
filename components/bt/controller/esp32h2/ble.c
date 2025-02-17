/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>

#include "sdkconfig.h"
#include "esp_bt_cfg.h"

/* External functions or variables
 ************************************************************************
 */
int base_stack_initEnv(void);
void base_stack_deinitEnv(void);
int base_stack_enable(void);
void base_stack_disable(void);

int conn_stack_initEnv(void);
void conn_stack_deinitEnv(void);
int conn_stack_enable(void);
void conn_stack_disable(void);

#if CONFIG_BT_LE_ERROR_SIM_ENABLED
int conn_errorSim_initEnv(void);
void conn_errorSim_deinitEnv(void);
int conn_errorSim_enable(void);
void conn_errorSim_disable(void);
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED

/* Local functions definition
 ***************************************************************************
 */
int ble_stack_initEnv(void)
{
    int rc;

    rc = base_stack_initEnv();
    if (rc) {
        return rc;
    }

#if DEFAULT_BT_LE_MAX_CONNECTIONS
    rc = conn_stack_initEnv();
    if (rc) {
        return rc;
    }
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    rc = conn_errorSim_initEnv();
    if (rc) {
        return rc;
    }
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS

    return 0;
}

void ble_stack_deinitEnv(void)
{
#if DEFAULT_BT_LE_MAX_CONNECTIONS
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_errorSim_deinitEnv();
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_stack_deinitEnv();
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS

    base_stack_deinitEnv();
}

int ble_stack_enable(void)
{
    int rc;

    rc = base_stack_enable();
    if (rc) {
        return rc;
    }

#if DEFAULT_BT_LE_MAX_CONNECTIONS
    rc = conn_stack_enable();
    if (rc) {
        return rc;
    }
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    rc = conn_errorSim_enable();
    if (rc) {
        return rc;
    }
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS

    return 0;
}

void ble_stack_disable(void)
{
#if DEFAULT_BT_LE_MAX_CONNECTIONS
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_errorSim_disable();
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_stack_disable();
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS

    base_stack_disable();
}
