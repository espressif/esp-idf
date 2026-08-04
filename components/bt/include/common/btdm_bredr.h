/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "btdm_user_cfg.h"
#include "bredr_user_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ***************************************************************************************************
 * Local Defined Macros
 ***************************************************************************************************
 */
#define ESP_BREDR_CTRL_CONFIG_MAGIC_VAL    (0x5A5AA5A5)
#define ESP_BREDR_CTRL_CONFIG_VERSION      (0x20260722)

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
#define _BT_CTRL_BREDR_INIT_CONFIG_DEFAULT()                                                       \
    {                                                                                              \
        .bredr_version = ESP_BREDR_CTRL_CONFIG_VERSION,                                            \
        .sleep_mode = 0,                                                                           \
        .bt_test_mode_en = UC_BR_EDR_TEST_MODE_EN,                                                 \
        .acl_cca_en = UC_BR_EDR_TX_CCA_EN,                                                         \
        .cca_rssi_thr = -UC_BR_EDR_CCA_RSSI_THRESH,                                                \
        .max_acl_conn = UC_BR_EDR_MAX_ACL_CONN,                                                    \
        .max_sync_conn = UC_BR_EDR_CTRL_MAX_SYNC_CONN,                                             \
        .cpb_tx_link_num = UC_BR_EDR_CPB_TX_LINK_NB,                                               \
        .cpb_rx_link_num = UC_BR_EDR_CPB_RX_LINK_NB,                                               \
        .bt_sco_datapath = UC_BR_EDR_SCO_DATA_PATH,                                                \
        .hci_tl_type = 0,                                                                          \
        .hci_tl_funcs = NULL,                                                                      \
        .cfg_mask = 0,                                                                             \
        .hw_target_code = 0,                                                                       \
        .acl_min_tx_pwr = CONFIG_BT_CTRL_BR_EDR_TX_PWR_ACL_MIN,                                    \
        .acl_max_tx_pwr = CONFIG_BT_CTRL_BR_EDR_TX_PWR_ACL_MAX,                                    \
        .apb_tx_pwr = UC_BR_EDR_TX_PWR_APB,                                                        \
        .page_tx_pwr = CONFIG_BT_CTRL_BR_EDR_TX_PWR_PAGE,                                          \
        .pscan_tx_pwr = CONFIG_BT_CTRL_BR_EDR_TX_PWR_PSCAN,                                        \
        .iscan_tx_pwr = CONFIG_BT_CTRL_BR_EDR_TX_PWR_ISCAN,                                        \
        .cpb_tx_pwr = UC_BR_EDR_TX_PWR_CPB,                                                        \
        .strain_tx_pwr = UC_BR_EDR_TX_PWR_STRAIN,                                                  \
        .rf_hw_type = 0,                                                                           \
        .static_aclu_tx_buf_nb = UC_BR_EDR_ACLU_TX_BUF_NB - UC_BR_EDR_DYNAMIC_ACLU_TX_BUF_NB,      \
        .dynamic_aclu_tx_buf_nb = UC_BR_EDR_DYNAMIC_ACLU_TX_BUF_NB,                                \
        .aclu_rx_buf_nb = UC_BR_EDR_ACLU_RX_BUF_NB,                                                \
        .sync_tx_buf_nb = 0,                                                                       \
        .sync_rx_buf_nb = 0,                                                                       \
        .esco_ev4_supp = UC_BR_EDR_ESCO_EV4_SUPP,                                                  \
        .esco_ev5_supp = UC_BR_EDR_ESCO_EV5_SUPP,                                                  \
        .esco_ev3_2_supp = UC_BR_EDR_ESCO_EV3_2_SUPP,                                              \
        .esco_ev3_3_supp = UC_BR_EDR_ESCO_EV3_3_SUPP,                                              \
        .esco_3_slots_supp = UC_BR_EDR_ESCO_3_SLOTS_SUPP,                                          \
        .bt_legacy_auth_vs_evt = UC_BR_EDR_LEGACY_AUTH_VENDOR_EVT,                                 \
        .inq_filter_en = 0,                                                                        \
        .dtm_en = 0,                                                                               \
        .static_sync_tx_buf_nb = UC_BR_EDR_STATIC_SYNC_TX_BUF_NB,                                  \
        .dynamic_sync_tx_buf_nb = UC_BR_EDR_DYNAMIC_SYNC_TX_BUF_NB,                                \
        .static_sync_rx_buf_nb = UC_BR_EDR_STATIC_SYNC_RX_BUF_NB,                                  \
        .sync_rx_buf_nb_per_link = UC_BR_EDR_SYNC_RX_BUF_NB_PER_LINK,                              \
        .bredr_magic = ESP_BREDR_CTRL_CONFIG_MAGIC_VAL,                                            \
    }
#else
#define _BT_CTRL_BREDR_INIT_CONFIG_DEFAULT() {0}
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE

/*
 ***************************************************************************************************
 * Type Definitions
 ***************************************************************************************************
 */
/**
 * @brief BR/EDR audio data transport path
 */
typedef enum {
    ESP_SCO_DATA_PATH_HCI = 0, /*!< data over HCI transport */
    ESP_SCO_DATA_PATH_PCM = 1, /*!< data over PCM interface */
} esp_sco_data_path_t;

/**
 * @brief mempool handle type used in BR/EDR controller
 */
typedef void *orca_mempool_t;

/**
 * @brief mempool operations used in BR/EDR controller
 */
struct orca_mempool_ops {
    orca_mempool_t (*create_with_pool)(void *mem, size_t pool_bytes,
                                       size_t max_bytes); /*!< mempool construction */
    void (*destroy)(orca_mempool_t mempool);              /*!< mempool destruction */
    void *(*malloc)(orca_mempool_t mempool, size_t size); /*!< allocate memory from mempool */
    void (*free)(orca_mempool_t mempool, void *ptr);      /*!< release memory */
};

/**
 * @brief BR/EDR controller configuration options
 */
typedef struct {
    /*
     * Following parameters can not be configured runtime when call esp_bt_controller_init()
     * They will be overwritten by constant values from menuconfig options or from macros.
     * So, do not modify the value when esp_bt_controller_init()
     */
    uint32_t bredr_version; /*!< version number of the defined structure */
    /*
     * Following parameters can be configured runtime, when call esp_bt_controller_init()
     */
    uint8_t sleep_mode;      /*!< controller sleep mode */
    uint8_t bt_test_mode_en; /*!< enable br/edr test mode */
    uint8_t acl_cca_en;      /*!< Enable BR/EDR ACL Transmit Clear Channel Assessment (TX CCA) */
    int8_t cca_rssi_thr; /*!< BR/EDR RSSI threshold for Transmit Clear Channel Assessment (CCA) */
    uint8_t
        max_acl_conn; /*!< Maximum number of BR/EDR ACL connections. Configurable in menuconfig */
    uint8_t max_sync_conn;   /*!< Maximum number of BR/EDR synchronous connections. Configurable in
                                menuconfig */
    uint8_t cpb_tx_link_num; /*!< BR/EDR CPB TX link number */
    uint8_t cpb_rx_link_num; /*!< BR/EDR CPB RX link number */
    uint8_t bt_sco_datapath; /*!< SCO data path, i.e. HCI(0) or PCM(1) */
    uint8_t hci_tl_type;     /*!< HCI transport layer, UART, VHCI, etc */
    void *hci_tl_funcs; /*!< hci transport functions used, must be set when hci_tl_type is UART */
    uint32_t cfg_mask;  /*!< reserved */
    uint32_t hw_target_code;        /*!< hardware target */
    int8_t acl_min_tx_pwr;          /*!< Default transmit power for ACL minimum */
    int8_t acl_max_tx_pwr;          /*!< Default transmit power for ACL maximum */
    int8_t apb_tx_pwr;              /*!< Default transmit power for APB */
    int8_t page_tx_pwr;             /*!< Default transmit power for Page */
    int8_t pscan_tx_pwr;            /*!< Default transmit power for Page Scan */
    int8_t iscan_tx_pwr;            /*!< Default transmit power for Inquiry Scan */
    int8_t cpb_tx_pwr;              /*!< Default transmit power for CPB TX */
    int8_t strain_tx_pwr;           /*!< Default transmit power for Synchronization Train */
    uint8_t rf_hw_type;             /*!< RF hardware type */
    uint8_t static_aclu_tx_buf_nb;  /*!< Static ACL-U TX buffer number */
    uint8_t dynamic_aclu_tx_buf_nb; /*!< Dynamic ACL-U TX buffer number */
    uint8_t aclu_rx_buf_nb;         /*!< ACL-U RX buffer number */
    uint8_t sync_tx_buf_nb;         /*!< SCO TX buffer number */
    uint8_t sync_rx_buf_nb;         /*!< SCO RX buffer number */
    uint8_t esco_ev4_supp : 1;      /*!< eSCO support EV4 packet type */
    uint8_t esco_ev5_supp : 1;      /*!< eSCO support EV5 packet type */
    uint8_t esco_ev3_2_supp : 1;    /*!< eSCO support 2-EV3 packet type */
    uint8_t esco_ev3_3_supp : 1;    /*!< eSCO support 3-EV3 packet type */
    uint8_t esco_3_slots_supp : 1;  /*!< eSCO support 2-EV5 and/or 3-EV5 packet type */
    uint8_t bt_legacy_auth_vs_evt
        : 1; /*!< 1 if BR/EDR Legacy Authentication Vendor Specific Event is enabled, which is
                required to protect from BIAS attack; 0 otherwise. Configurable in menuconfig */
    uint8_t inq_filter_en : 1;            /*!< Enable inquiry result filter */
    uint8_t dtm_en : 1;                   /*!< BR/EDR direct test mode */
    uint32_t mempool_size;                /*!< Required mempool size used in BR/EDR controller */
    struct orca_mempool_ops *mempool_ops; /*!< mempool operations used in BR/EDR controller */
    uint8_t static_sync_tx_buf_nb;        /*!< static sync tx buffers(allocated at init) */
    uint8_t dynamic_sync_tx_buf_nb;       /*!< dynamic sync tx buffers(allocated during sync connection establishment) */
    uint8_t static_sync_rx_buf_nb;        /*!< total number of static sync rx buffers */
    uint8_t sync_rx_buf_nb_per_link;      /*!< sync rx buffers per link (static + dynamic combined) */
    uint32_t bredr_magic;                 /*!< Magic number */
} esp_bt_ctrl_bredr_config_t;

#ifdef __cplusplus
}
#endif
