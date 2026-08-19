/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>

#include "ble_user_cfg.h"
#include "btdm_user_cfg.h"
#include "esp_attr.h"
#include "esp_bit_defs.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_random.h"
#include "sdkconfig.h"

#if CONFIG_BT_NIMBLE_ENABLED
#include "nimble/nimble_port.h"
#endif // CONFIG_BT_NIMBLE_ENABLED

#include "esp_bt.h"
#include "modem/modem_syscon_struct.h"
#include "rom/ets_sys.h"

#include "ble_msys.h"
#include "btdm_endian.h"
#include "btdm_osal.h"

/*
 ***************************************************************************************************
 * Local Defined Macros
 ***************************************************************************************************
 */
#if CONFIG_BT_NIMBLE_ENABLED
#define BLE_HOST_CO_COUNT (8)
#define BLE_HOST_EV_COUNT (11 + BLE_HOST_CO_COUNT)
#define BLE_HOST_EVQ_COUNT (3)
#define BLE_HOST_SEM_COUNT (10)
#define BLE_HOST_MUTEX_COUNT (4)
#else
#define BLE_HOST_CO_COUNT (0)
#define BLE_HOST_EV_COUNT (0)
#define BLE_HOST_EVQ_COUNT (0)
#define BLE_HOST_SEM_COUNT (0)
#define BLE_HOST_MUTEX_COUNT (0)
#endif

#define TAG "BLE_CTRL_INIT"

/*
 ***************************************************************************************************
 * Local Type Definitions
 ***************************************************************************************************
 */
typedef enum {
    FILTER_DUPLICATE_PDUTYPE = BIT(0),
    FILTER_DUPLICATE_LENGTH = BIT(1),
    FILTER_DUPLICATE_ADDRESS = BIT(2),
    FILTER_DUPLICATE_ADVDATA = BIT(3),
    FILTER_DUPLICATE_DEFAULT = FILTER_DUPLICATE_PDUTYPE | FILTER_DUPLICATE_ADDRESS,
    FILTER_DUPLICATE_PDU_ALL = 0xF,
    FILTER_DUPLICATE_EXCEPTION_FOR_MESH = BIT(4),
    FILTER_DUPLICATE_AD_TYPE = BIT(5),
} disc_duplicate_mode_t;

/*
 ***************************************************************************************************
 * Local Function Declaration
 ***************************************************************************************************
 */

/*
 ***************************************************************************************************
 * External Functions
 ***************************************************************************************************
 */
const char *ble_controller_get_compile_version(void);

int r_ble_ll_get_npl_element_info(void *cfg, btdm_osal_elem_num_t *elem);

int r_ble_controller_init(esp_bt_controller_config_t *cfg);
int r_ble_controller_deinit(void);

extern int r_esp_ble_ll_set_public_addr(const uint8_t *addr);

extern void r_filter_duplicate_mode_enable(disc_duplicate_mode_t mode);
extern void r_filter_duplicate_mode_disable(disc_duplicate_mode_t mode);
extern void r_filter_duplicate_set_ring_list_max_num(uint32_t max_num);
extern void r_scan_duplicate_cache_refresh_set_time(uint32_t period_time);

int r_base_stack_initEnv(void *cfg);
void r_base_stack_deinitEnv(void);

#if DEFAULT_BT_LE_PAWR_SUPPORTED || CONFIG_BT_LE_ERROR_SIM_ENABLED ||                              \
    UC_BT_CTRL_LE_DECISION_ADV || UC_BT_CTRL_LE_DECISION_SCAN
int ble_single_env_init(void);
void ble_single_env_deinit(void);
int ble_single_init(void);
void ble_single_deinit(void);
#endif // DEFAULT_BT_LE_PAWR_SUPPORTED || CONFIG_BT_LE_ERROR_SIM_ENABLED ||
       // UC_BT_CTRL_LE_DECISION_ADV || UC_BT_CTRL_LE_DECISION_SCAN

#if CONFIG_BT_LE_RXBUF_OPT_ENABLED
extern void mmgmt_enableRxbufOptFeature(void);
#endif // CONFIG_BT_LE_RXBUF_OPT_ENABLED

#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
extern void coex_hw_timer_set(uint8_t idx, uint8_t src, uint8_t pti, uint32_t latency,
                              uint32_t perioidc);
extern void coex_hw_timer_enable(uint8_t idx);
extern void coex_hw_timer_disable(uint8_t idx);
#endif // CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE

extern int r_ble_txpwr_set(esp_ble_enhanced_power_type_t power_type, uint16_t handle,
                           int power_level);
extern int r_ble_txpwr_get(esp_ble_enhanced_power_type_t power_type, uint16_t handle);

#if UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED
void api_scan_hci_enableAdvDiscardVsEvent(bool en);
void api_adv_hci_enableScanReqRxdVsEvent(bool en);
void api_conn_hci_enableChanMapUpdCompVsEvent(bool en);
#endif // UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED

void api_sched_txn_bredrIsEnabled(bool en);

/*
 ***************************************************************************************************
 * Local Function Definitions
 ***************************************************************************************************
 */
int
ble_vhci_disc_duplicate_mode_enable(int mode)
{
    // TODO: use vendor hci to update
    r_filter_duplicate_mode_enable(mode);
    return true;
}

int
ble_vhci_disc_duplicate_mode_disable(int mode)
{
    // TODO: use vendor hci to update
    r_filter_duplicate_mode_disable(mode);
    return true;
}

int
ble_vhci_disc_duplicate_set_max_cache_size(int max_cache_size)
{
    // TODO: use vendor hci to update
    r_filter_duplicate_set_ring_list_max_num(max_cache_size);
    return true;
}

int
ble_vhci_disc_duplicate_set_period_refresh_time(int refresh_period_time)
{
    // TODO: use vendor hci to update
    r_scan_duplicate_cache_refresh_set_time(refresh_period_time);
    return true;
}

/**
 * @brief Config scan duplicate option mode from menuconfig (Adapt to the old
 * configuration method.)
 */
void
ble_controller_scan_duplicate_config(void)
{
    uint32_t duplicate_mode = FILTER_DUPLICATE_DEFAULT;
    uint32_t cache_size = 100;
#if CONFIG_BT_LE_SCAN_DUPL == true
    cache_size = CONFIG_BT_LE_LL_DUP_SCAN_LIST_COUNT;
    if (CONFIG_BT_LE_SCAN_DUPL_TYPE == 0) {
        duplicate_mode = FILTER_DUPLICATE_ADDRESS | FILTER_DUPLICATE_PDUTYPE;
    } else if (CONFIG_BT_LE_SCAN_DUPL_TYPE == 1) {
        duplicate_mode = FILTER_DUPLICATE_ADVDATA;
    } else if (CONFIG_BT_LE_SCAN_DUPL_TYPE == 2) {
        duplicate_mode = FILTER_DUPLICATE_ADDRESS | FILTER_DUPLICATE_ADVDATA;
    }
    duplicate_mode |= FILTER_DUPLICATE_EXCEPTION_FOR_MESH;

    ble_vhci_disc_duplicate_set_period_refresh_time(CONFIG_BT_LE_SCAN_DUPL_CACHE_REFRESH_PERIOD);
#endif

    ble_vhci_disc_duplicate_mode_disable(0xFFFFFFFF);
    ble_vhci_disc_duplicate_mode_enable(duplicate_mode);
    ble_vhci_disc_duplicate_set_max_cache_size(cache_size);
}

static esp_err_t
ble_controller_init(esp_bt_controller_config_t *cfg)
{
    uint8_t mac[6];
    esp_err_t ret = ESP_OK;

    ret = r_ble_controller_init(cfg);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "ble_controller_init failed %d", ret);
        return ret;
    }

#if CONFIG_BT_LE_LL_PEER_SCA_SET_ENABLE
    r_ble_ll_customize_peer_sca_set(CONFIG_BT_LE_LL_PEER_SCA);
#endif // CONFIG_BT_LE_LL_PEER_SCA_SET_ENABLE

    ble_controller_scan_duplicate_config();

    ret = ble_msys_init();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "msys_init failed %d", ret);
        return ret;
    }

    btdm_osal_read_efuse_mac((uint8_t *)mac);
    ESP_LOGI(TAG, "Bluetooth MAC: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3],
             mac[4], mac[5]);
    btdm_swap_in_place(mac, 6);
    r_esp_ble_ll_set_public_addr(mac);

    return ESP_OK;
}

static void
ble_controller_deinit(void)
{
    ble_msys_deinit();

    r_ble_controller_deinit();
}

/*
 ***************************************************************************************************
 * Public Function Definitions
 ***************************************************************************************************
 */
void
ble_osal_elem_calc(esp_bt_controller_config_t *cfg, btdm_osal_elem_num_t *elem)
{
    btdm_osal_elem_num_t ble_osal_elem;

    r_ble_ll_get_npl_element_info(cfg, &ble_osal_elem);

    elem->evt_count += ble_osal_elem.evt_count + BLE_HOST_EV_COUNT;
    elem->evtq_count += ble_osal_elem.evtq_count + BLE_HOST_EVQ_COUNT;
    elem->co_count += ble_osal_elem.co_count + BLE_HOST_CO_COUNT;
    elem->sem_count += ble_osal_elem.sem_count + BLE_HOST_SEM_COUNT;
    elem->mutex_count += ble_osal_elem.mutex_count + BLE_HOST_MUTEX_COUNT;
}

int
ble_stack_init(esp_bt_controller_config_t *cfg)
{
    int rc;

    ESP_LOGI(TAG, "BLE controller compile version [%s]\n", ble_controller_get_compile_version());

    rc = ble_controller_init(cfg);
    if (rc != ESP_OK) {
        ESP_LOGW(TAG, "ble_controller_init failed %d", rc);
        return rc;
    }

    /* Base stack initialization - always required */
    rc = r_base_stack_initEnv(&cfg->ble);
    if (rc != ESP_OK) {
        ESP_LOGW(TAG, "r_base_stack_initEnv failed %d", rc);
        return rc;
    }

#if UC_BLE_CTRL_PAWR_SUPPORTED || CONFIG_BT_LE_ERROR_SIM_ENABLED || UC_BT_CTRL_LE_DECISION_ADV ||  \
    UC_BT_CTRL_LE_DECISION_SCAN
    /* Single environment initialization */
    rc = ble_single_env_init();
    if (rc != ESP_OK) {
        ESP_LOGW(TAG, "ble_single_env_init failed %d", rc);
        return rc;
    }

    rc = ble_single_init();
    if (rc != ESP_OK) {
        ESP_LOGW(TAG, "ble_single_init failed %d", rc);
        return rc;
    }
#endif // UC_BLE_CTRL_PAWR_SUPPORTED || CONFIG_BT_LE_ERROR_SIM_ENABLED ||
       // UC_BT_CTRL_LE_DECISION_ADV || UC_BT_CTRL_LE_DECISION_SCAN

    return 0;
}

void
ble_stack_deinit(void)
{
    /* Deinitialize single environment and related modules */
#if UC_BLE_CTRL_PAWR_SUPPORTED || CONFIG_BT_LE_ERROR_SIM_ENABLED || UC_BT_CTRL_LE_DECISION_ADV ||  \
    UC_BT_CTRL_LE_DECISION_SCAN
    ble_single_deinit();
    ble_single_env_deinit();
#endif // UC_BLE_CTRL_PAWR_SUPPORTED || CONFIG_BT_LE_ERROR_SIM_ENABLED ||
       // UC_BT_CTRL_LE_DECISION_ADV || UC_BT_CTRL_LE_DECISION_SCAN

    /* Deinitialize base stack */
    r_base_stack_deinitEnv();

    /* Deinitialize BLE controller */
    ble_controller_deinit();
}

int
ble_stack_enable(void)
{
#if UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED
    api_scan_hci_enableAdvDiscardVsEvent(true);
    api_adv_hci_enableScanReqRxdVsEvent(true);
    api_conn_hci_enableChanMapUpdCompVsEvent(true);
#endif // UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    api_sched_txn_bredrIsEnabled(true);
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE

    return 0;
}

void
ble_stack_disable(void)
{
#if UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED
    api_conn_hci_enableChanMapUpdCompVsEvent(false);
    api_adv_hci_enableScanReqRxdVsEvent(false);
    api_scan_hci_enableAdvDiscardVsEvent(false);
#endif // UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED
}

void
ble_stack_reset(void)
{
}

esp_err_t
esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level)
{
    esp_err_t stat = ESP_FAIL;

    switch (power_type) {
    case ESP_BLE_PWR_TYPE_DEFAULT:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    case ESP_BLE_PWR_TYPE_ADV:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_ADV, 0xFF, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    case ESP_BLE_PWR_TYPE_SCAN:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_SCAN, 0, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    case ESP_BLE_PWR_TYPE_CONN_HDL0:
    case ESP_BLE_PWR_TYPE_CONN_HDL1:
    case ESP_BLE_PWR_TYPE_CONN_HDL2:
    case ESP_BLE_PWR_TYPE_CONN_HDL3:
    case ESP_BLE_PWR_TYPE_CONN_HDL4:
    case ESP_BLE_PWR_TYPE_CONN_HDL5:
    case ESP_BLE_PWR_TYPE_CONN_HDL6:
    case ESP_BLE_PWR_TYPE_CONN_HDL7:
    case ESP_BLE_PWR_TYPE_CONN_HDL8:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_CONN, power_type, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    default:
        stat = ESP_ERR_NOT_SUPPORTED;
        break;
    }

    return stat;
}

esp_err_t
esp_ble_tx_power_set_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle,
                              esp_power_level_t power_level)
{
    esp_err_t stat = ESP_FAIL;
    switch (power_type) {
    case ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    case ESP_BLE_ENHANCED_PWR_TYPE_SCAN:
    case ESP_BLE_ENHANCED_PWR_TYPE_INIT:
        if (r_ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_SCAN, 0, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    case ESP_BLE_ENHANCED_PWR_TYPE_ADV:
    case ESP_BLE_ENHANCED_PWR_TYPE_CONN:
        if (r_ble_txpwr_set(power_type, handle, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    default:
        stat = ESP_ERR_NOT_SUPPORTED;
        break;
    }

    return stat;
}

esp_power_level_t
esp_ble_tx_power_get(esp_ble_power_type_t power_type)
{
    int tx_level = 0;

    switch (power_type) {
    case ESP_BLE_PWR_TYPE_DEFAULT:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0);
        break;
    case ESP_BLE_PWR_TYPE_ADV:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_ADV, 0);
        break;
    case ESP_BLE_PWR_TYPE_SCAN:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_SCAN, 0);
        break;
    case ESP_BLE_PWR_TYPE_CONN_HDL0:
    case ESP_BLE_PWR_TYPE_CONN_HDL1:
    case ESP_BLE_PWR_TYPE_CONN_HDL2:
    case ESP_BLE_PWR_TYPE_CONN_HDL3:
    case ESP_BLE_PWR_TYPE_CONN_HDL4:
    case ESP_BLE_PWR_TYPE_CONN_HDL5:
    case ESP_BLE_PWR_TYPE_CONN_HDL6:
    case ESP_BLE_PWR_TYPE_CONN_HDL7:
    case ESP_BLE_PWR_TYPE_CONN_HDL8:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_CONN, power_type);
        break;
    default:
        return ESP_PWR_LVL_INVALID;
    }

    if (tx_level < 0) {
        return ESP_PWR_LVL_INVALID;
    }

    return (esp_power_level_t)tx_level;
}

esp_power_level_t
esp_ble_tx_power_get_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle)
{
    int tx_level = 0;

    switch (power_type) {
    case ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0);
        break;
    case ESP_BLE_ENHANCED_PWR_TYPE_SCAN:
    case ESP_BLE_ENHANCED_PWR_TYPE_INIT:
        tx_level = r_ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_SCAN, 0);
        break;
    case ESP_BLE_ENHANCED_PWR_TYPE_ADV:
    case ESP_BLE_ENHANCED_PWR_TYPE_CONN:
        tx_level = r_ble_txpwr_get(power_type, handle);
        break;
    default:
        return ESP_PWR_LVL_INVALID;
    }

    if (tx_level < 0) {
        return ESP_PWR_LVL_INVALID;
    }

    return (esp_power_level_t)tx_level;
}

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
#include "esp_private/sleep_retention.h"

extern const sleep_retention_entries_config_t *r_esp_ble_mac_retention_link_get(uint8_t *size,
                                                                                uint8_t extra);

esp_err_t
sleep_modem_ble_mac_modem_state_init(void)
{
    uint8_t size;
    const sleep_retention_entries_config_t *ble_mac_modem_config =
        r_esp_ble_mac_retention_link_get(&size, 0);
    esp_err_t err = sleep_retention_entries_create(ble_mac_modem_config, size, REGDMA_LINK_PRI_5,
                                                   SLEEP_RETENTION_MODULE_BLE_MAC);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Modem BLE MAC retention initialization");
    }
    return err;
}
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */

/*
 ***************************************************************************************************
 * Function Registry
 ***************************************************************************************************
 */
int
wr_base_registry_genEccKeyPair(uint8_t *pub, uint8_t *priv)
{
    return -1;
}

int
wr_base_registry_genEcdhKey(const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y,
                         const uint8_t *our_priv_key, uint8_t *out_dhkey)
{
    return -1;
}

#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2
void IRAM_ATTR
wr_base_registry_resetModem(uint8_t modem, uint8_t start)
{
    if (modem == 0x05) {
        if (start) {
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
            coex_hw_timer_set(0x04, 0x02, 15, 0, 5000);
            coex_hw_timer_enable(0x04);
#endif // CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
            MODEM_SYSCON.modem_rst_conf.val |= (BIT(16) | BIT(18));
            MODEM_SYSCON.modem_rst_conf.val &= ~(BIT(16) | BIT(18));
        } else {
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
            coex_hw_timer_disable(0x04);
#endif // CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
        }
    }
}
#endif // CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2

#if UC_BT_CTRL_LE_BIS_BCAST_ENABLED || UC_BT_CTRL_LE_BIS_SYNC_ENABLED
#if CONFIG_BT_NIMBLE_CRYPTO_STACK_MBEDTLS
#include "mbedtls/aes.h"
#include "mbedtls/cipher.h"
#include "mbedtls/cmac.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#else
#include "tinycrypt/aes.h"
#include "tinycrypt/cmac_mode.h"
#include "tinycrypt/constants.h"
#include "tinycrypt/ecc_dh.h"
#include "tinycrypt/utils.h"
#endif // CONFIG_BT_NIMBLE_CRYPTO_STACK_MBEDTLS
int
ext_ble_ll_iso_aes_cmac(const uint8_t *key, const uint8_t *in, size_t len, uint8_t *out)
{
#if CONFIG_BT_NIMBLE_CRYPTO_STACK_MBEDTLS
    int ret = 0;
    mbedtls_cipher_context_t ctx;
    const mbedtls_cipher_info_t *cipher_info;

    mbedtls_cipher_init(&ctx);

    // Choose AES-128 cipher info (or AES-256 if using 32-byte key)
    cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
    if (cipher_info == NULL) {
        ret = -1;
        goto exit;
    }

    if ((ret = mbedtls_cipher_setup(&ctx, cipher_info)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_cipher_cmac_starts(&ctx, key, 128)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_cipher_cmac_update(&ctx, in, len)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_cipher_cmac_finish(&ctx, out)) != 0) {
        goto exit;
    }

    ret = 0;

exit:
    mbedtls_cipher_free(&ctx);
    return ret;
#else
    // TinyCrypt implementation
    struct tc_aes_key_sched_struct sched;
    struct tc_cmac_struct state;
    // Initialize CMAC context, set key and scheduling information
    // If initialization fails, return -1
    if (tc_cmac_setup(&state, key, &sched) == TC_CRYPTO_FAIL) {
        return -1;
    }

    if (tc_cmac_update(&state, in, len) == TC_CRYPTO_FAIL) {
        return -1;
    }

    if (tc_cmac_final(out, &state) == TC_CRYPTO_FAIL) {
        return -1;
    }

    return 0;
#endif // CONFIG_BT_NIMBLE_CRYPTO_STACK_MBEDTLS
}
#endif // UC_BT_CTRL_LE_BIS_BCAST_ENABLED || UC_BT_CTRL_LE_BIS_SYNC_ENABLED
