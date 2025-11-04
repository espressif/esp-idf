/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/soc_caps.h"

#if SOC_PAU_SUPPORTED
#include "hal/pau_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the addresses of all REGDMA Links
 * @param link_entries all linked lists addresses
 */
void pau_regdma_set_entry_link_addr(pau_regdma_link_addr_t *link_entries);

#if SOC_PM_SUPPORT_REGDMA_TRIGGERED_PHY
#if SOC_PM_PAU_REGDMA_LINK_MODEM
/**
 * @brief Set the address of modem REGDMA Link in modem state
 * @param link_addr linked lists address
 */
void pau_regdma_set_modem_link_addr(void *link_addr);
#endif // SOC_PM_PAU_REGDMA_LINK_MODEM

/**
 * @brief Software trigger regdma to perform modem link backup
 *
 * @param blocking software waits for regdma to complete
 */
void pau_regdma_trigger_modem_link_backup(bool blocking);

/**
 * @brief Software trigger regdma to perform modem link restore
 *
 * @param blocking software waits for regdma to complete
 */
void pau_regdma_trigger_modem_link_restore(bool blocking);

/**
 * @brief Completion process of regdma for modem link retention
 *
 */
void pau_regdma_modem_link_complete(void);

#if SOC_PM_PAU_REGDMA_MODEM_WIFIMAC_WORKAROUND
/**
 * @brief Software trigger REGDMA backup on WiFi MAC SEL link
 */
void pau_regdma_trigger_wifimac_link_backup(bool blocking);

/**
 * @brief Software trigger REGDMA restore on WiFi MAC SEL link
 */
void pau_regdma_trigger_wifimac_link_restore(bool blocking);
#endif

/**
 * @brief Enable pau done interrupt
 *
 */
void pau_regdma_done_int_enable(void);

/**
 * @brief Disable pau done interrupt
 *
 */
void pau_regdma_done_int_disable(void);

/**
 * @brief Check pau done interrupt status
 *
 */
bool pau_get_regdma_done_status(void);

/**
 * @brief Clear pau done interrupt status
 *
 */
void pau_clear_regdma_done_status(void);

#endif /* SOC_PM_SUPPORT_REGDMA_TRIGGERED_PHY */

#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
/**
 * @brief Set the address of system REGDMA Link in active state
 * @param link_addr linked lists address
 */
void pau_regdma_set_system_link_addr(void *link_addr);

/**
 * @brief Software trigger regdma to perform system link backup
 */
void pau_regdma_trigger_system_link_backup(void);

/**
 * @brief Software trigger regdma to perform system link restore
 */
void pau_regdma_trigger_system_link_restore(void);
#endif

/**
 * @brief Set the address of extra REGDMA Link in active state
 * @param link_addr linked lists address
 */
void pau_regdma_set_extra_link_addr(void *link_addr);

/**
 * @brief Software trigger regdma to perform extra link backup
 */
void pau_regdma_trigger_extra_link_backup(void);

/**
 * @brief Software trigger regdma to perform extra link restore
 */
void pau_regdma_trigger_extra_link_restore(void);

#if SOC_PAU_IN_TOP_DOMAIN
/**
 * @brief Rentention link entry selection, enable or disable the retention
 *        link entry configuration in always-on domain
 *
 * @param enable Set true to use always-on domain link configuration instead
 *
 * @return The origin aon link bypass enable status
 */
bool pau_regdma_enable_aon_link_entry(bool enable);
#endif

#ifdef __cplusplus
}
#endif

#endif //SOC_PAU_SUPPORTED
