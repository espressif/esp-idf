/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "esp_random.h"
#include "esp_heap_caps.h"
#include "esp_heap_caps_init.h"
#include <esp_mac.h>

#include "sdkconfig.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "esp_private/esp_modem_clock.h"

#ifdef ESP_PLATFORM
#include "esp_log.h"
#endif // ESP_PLATFORM

#if CONFIG_SW_COEXIST_ENABLE
#include "private/esp_coexist_internal.h"
#endif // CONFIG_SW_COEXIST_ENABLE

#include "nimble/nimble_npl_os.h"
#include "nimble/ble_hci_trans.h"
#include "os/endian.h"

#include "esp_bt.h"
#include "esp_intr_alloc.h"
#include "esp_sleep.h"
#include "esp_pm.h"
#include "esp_phy_init.h"
#include "esp_private/periph_ctrl.h"
#include "hci_uart.h"
#include "bt_osi_mem.h"

#if SOC_PM_RETENTION_HAS_CLOCK_BUG
#include "esp_private/sleep_retention.h"
#endif // SOC_PM_RETENTION_HAS_CLOCK_BUG

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
#include "esp_private/sleep_modem.h"
#endif // CONFIG_FREERTOS_USE_TICKLESS_IDLE

#ifdef CONFIG_BT_BLUEDROID_ENABLED
#include "hci/hci_hal.h"
#endif // CONFIG_BT_BLUEDROID_ENABLED

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_private/periph_ctrl.h"
#include "esp_sleep.h"

#include "hal/efuse_hal.h"
#include "soc/rtc.h"
/* Macro definition
 ************************************************************************
 */
#define NIMBLE_PORT_LOG_TAG          "BLE_INIT"
#define OSI_COEX_VERSION              0x00010006
#define OSI_COEX_MAGIC_VALUE          0xFADEBEAD

#define EXT_FUNC_VERSION             0x20221122
#define EXT_FUNC_MAGIC_VALUE         0xA5A5A5A5

#define BT_ASSERT_PRINT              ets_printf

#ifdef CONFIG_BT_BLUEDROID_ENABLED
/* ACL_DATA_MBUF_LEADINGSPCAE: The leadingspace in user info header for ACL data */
#define ACL_DATA_MBUF_LEADINGSPCAE    4
#endif // CONFIG_BT_BLUEDROID_ENABLED

/* Types definition
 ************************************************************************
 */
struct osi_coex_funcs_t {
    uint32_t _magic;
    uint32_t _version;
    void (* _coex_wifi_sleep_set)(bool sleep);
    int (* _coex_core_ble_conn_dyn_prio_get)(bool *low, bool *high);
    void (* _coex_schm_status_bit_set)(uint32_t type, uint32_t status);
    void (* _coex_schm_status_bit_clear)(uint32_t type, uint32_t status);
};

struct ext_funcs_t {
    uint32_t ext_version;
    int (*_esp_intr_alloc)(int source, int flags, intr_handler_t handler, void *arg, void **ret_handle);
    int (*_esp_intr_free)(void **ret_handle);
    void *(* _malloc)(size_t size);
    void (*_free)(void *p);
    void (*_hal_uart_start_tx)(int);
    int (*_hal_uart_init_cbs)(int, hci_uart_tx_char, hci_uart_tx_done, hci_uart_rx_char, void *);
    int (*_hal_uart_config)(int, int32_t, uint8_t, uint8_t, uart_parity_t, uart_hw_flowcontrol_t);
    int (*_hal_uart_close)(int);
    void (*_hal_uart_blocking_tx)(int, uint8_t);
    int (*_hal_uart_init)(int, void *);
    int (* _task_create)(void *task_func, const char *name, uint32_t stack_depth, void *param,
                         uint32_t prio, void *task_handle, uint32_t core_id);
    void (* _task_delete)(void *task_handle);
    void (*_osi_assert)(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2);
    uint32_t (* _os_random)(void);
    int (* _ecc_gen_key_pair)(uint8_t *public, uint8_t *priv);
    int (* _ecc_gen_dh_key)(const uint8_t *remote_pub_key_x, const uint8_t *remote_pub_key_y,
                            const uint8_t *local_priv_key, uint8_t *dhkey);
    void (* _esp_reset_rpa_moudle)(void);
    uint32_t magic;
};

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
typedef void (*interface_func_t) (uint32_t len, const uint8_t*addr, bool end);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

/* External functions or variables
 ************************************************************************
 */
extern int ble_osi_coex_funcs_register(struct osi_coex_funcs_t *coex_funcs);
extern int ble_controller_init(esp_bt_controller_config_t *cfg);
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
extern int r_ble_log_init_async(interface_func_t bt_controller_log_interface, bool task_create, uint8_t buffers, uint32_t *bufs_size);
extern int r_ble_log_deinit_async(void);
extern void r_ble_log_async_select_dump_buffers(uint8_t buffers);
extern void r_ble_log_async_output_dump_all(bool output);
extern void esp_panic_handler_reconfigure_wdts(uint32_t timeout_ms);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
extern int ble_controller_deinit(void);
extern int ble_controller_enable(uint8_t mode);
extern int ble_controller_disable(void);
extern int esp_register_ext_funcs (struct ext_funcs_t *);
extern void esp_unregister_ext_funcs (void);
extern int esp_ble_ll_set_public_addr(const uint8_t *addr);
extern int esp_register_npl_funcs (struct npl_funcs_t *p_npl_func);
extern void esp_unregister_npl_funcs (void);
extern void npl_freertos_mempool_deinit(void);
extern uint32_t r_os_cputime_get32(void);
extern uint32_t r_os_cputime_ticks_to_usecs(uint32_t ticks);
extern void r_ble_lll_rfmgmt_set_sleep_cb(void *s_cb, void *w_cb, void *s_arg,
                                          void *w_arg, uint32_t us_to_enabled);
extern void r_ble_rtc_wake_up_state_clr(void);
extern int os_msys_init(void);
extern void os_msys_deinit(void);
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
extern const sleep_retention_entries_config_t *esp_ble_mac_retention_link_get(uint8_t *size, uint8_t extra);
extern void esp_ble_set_wakeup_overhead(uint32_t overhead);
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */
extern void esp_ble_change_rtc_freq(uint32_t freq);
extern int ble_sm_alg_gen_dhkey(const uint8_t *peer_pub_key_x,
                                const uint8_t *peer_pub_key_y,
                                const uint8_t *our_priv_key, uint8_t *out_dhkey);
extern int ble_sm_alg_gen_key_pair(uint8_t *pub, uint8_t *priv);
extern int ble_txpwr_set(esp_ble_enhanced_power_type_t power_type, uint16_t handle, int power_level);
extern int ble_txpwr_get(esp_ble_enhanced_power_type_t power_type, uint16_t handle);
extern int ble_get_npl_element_info(esp_bt_controller_config_t *cfg, ble_npl_count_info_t * npl_info);
extern char *ble_controller_get_compile_version(void);
extern uint32_t _bt_bss_start;
extern uint32_t _bt_bss_end;
extern uint32_t _bt_controller_bss_start;
extern uint32_t _bt_controller_bss_end;
extern uint32_t _bt_data_start;
extern uint32_t _bt_data_end;
extern uint32_t _bt_controller_data_start;
extern uint32_t _bt_controller_data_end;

/* Local Function Declaration
 *********************************************************************
 */
static void coex_schm_status_bit_set_wrapper(uint32_t type, uint32_t status);
static void coex_schm_status_bit_clear_wrapper(uint32_t type, uint32_t status);
static int task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth,
                               void *param, uint32_t prio, void *task_handle, uint32_t core_id);
static void task_delete_wrapper(void *task_handle);
#if CONFIG_BT_LE_HCI_INTERFACE_USE_UART
static void hci_uart_start_tx_wrapper(int uart_no);
static int hci_uart_init_cbs_wrapper(int uart_no, hci_uart_tx_char tx_func,
                                     hci_uart_tx_done tx_done, hci_uart_rx_char rx_func, void *arg);
static int hci_uart_config_wrapper(int uart_no, int32_t speed, uint8_t databits, uint8_t stopbits,
                                   uart_parity_t parity, uart_hw_flowcontrol_t flow_ctl);
static int hci_uart_close_wrapper(int uart_no);
static void hci_uart_blocking_tx_wrapper(int port, uint8_t data);
static int hci_uart_init_wrapper(int uart_no, void *cfg);
#endif // CONFIG_BT_LE_HCI_INTERFACE_USE_UART
static int esp_intr_alloc_wrapper(int source, int flags, intr_handler_t handler,
                                  void *arg, void **ret_handle_in);
static int esp_intr_free_wrapper(void **ret_handle);
static void osi_assert_wrapper(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2);
static uint32_t osi_random_wrapper(void);
static void esp_reset_rpa_moudle(void);
static int esp_ecc_gen_key_pair(uint8_t *pub, uint8_t *priv);
static int esp_ecc_gen_dh_key(const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y,
                              const uint8_t *our_priv_key, uint8_t *out_dhkey);
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
static void esp_bt_controller_log_interface(uint32_t len, const uint8_t *addr, bool end);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
/* Local variable definition
 ***************************************************************************
 */
/* Static variable declare */
static DRAM_ATTR esp_bt_controller_status_t ble_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
const static uint32_t log_bufs_size[] = {CONFIG_BT_LE_LOG_CTRL_BUF1_SIZE, CONFIG_BT_LE_LOG_HCI_BUF_SIZE, CONFIG_BT_LE_LOG_CTRL_BUF2_SIZE};
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

/* This variable tells if BLE is running */
static bool s_ble_active = false;
#ifdef CONFIG_PM_ENABLE
static DRAM_ATTR esp_pm_lock_handle_t s_pm_lock = NULL;
#define BTDM_MIN_TIMER_UNCERTAINTY_US      (200)
#endif // CONFIG_PM_ENABLE

#define BLE_RTC_DELAY_US_LIGHT_SLEEP        (2500)
#define BLE_RTC_DELAY_US_MODEM_SLEEP        (500)

static const struct osi_coex_funcs_t s_osi_coex_funcs_ro = {
    ._magic = OSI_COEX_MAGIC_VALUE,
    ._version = OSI_COEX_VERSION,
    ._coex_wifi_sleep_set = NULL,
    ._coex_core_ble_conn_dyn_prio_get = NULL,
    ._coex_schm_status_bit_set = coex_schm_status_bit_set_wrapper,
    ._coex_schm_status_bit_clear = coex_schm_status_bit_clear_wrapper,
};

struct ext_funcs_t ext_funcs_ro = {
    .ext_version = EXT_FUNC_VERSION,
    ._esp_intr_alloc = esp_intr_alloc_wrapper,
    ._esp_intr_free = esp_intr_free_wrapper,
    ._malloc = bt_osi_mem_malloc_internal,
    ._free = bt_osi_mem_free,
#if CONFIG_BT_LE_HCI_INTERFACE_USE_UART
    ._hal_uart_start_tx     =  hci_uart_start_tx_wrapper,
    ._hal_uart_init_cbs     =  hci_uart_init_cbs_wrapper,
    ._hal_uart_config       =  hci_uart_config_wrapper,
    ._hal_uart_close        =  hci_uart_close_wrapper,
    ._hal_uart_blocking_tx  =  hci_uart_blocking_tx_wrapper,
    ._hal_uart_init         =  hci_uart_init_wrapper,
#endif //CONFIG_BT_LE_HCI_INTERFACE_USE_UART
    ._task_create = task_create_wrapper,
    ._task_delete = task_delete_wrapper,
    ._osi_assert = osi_assert_wrapper,
    ._os_random = osi_random_wrapper,
    ._ecc_gen_key_pair = esp_ecc_gen_key_pair,
    ._ecc_gen_dh_key = esp_ecc_gen_dh_key,
    ._esp_reset_rpa_moudle = esp_reset_rpa_moudle,
    .magic = EXT_FUNC_MAGIC_VALUE,
};

static void IRAM_ATTR esp_reset_rpa_moudle(void)
{

}

static void IRAM_ATTR osi_assert_wrapper(const uint32_t ln, const char *fn,
                                         uint32_t param1, uint32_t param2)
{
    BT_ASSERT_PRINT("BLE assert: line %d in function %s, param: 0x%x, 0x%x", ln, fn, param1, param2);
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    esp_ble_controller_log_dump_all(true);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    assert(0);
}

static uint32_t IRAM_ATTR osi_random_wrapper(void)
{
    return esp_random();
}

static void coex_schm_status_bit_set_wrapper(uint32_t type, uint32_t status)
{
#if CONFIG_SW_COEXIST_ENABLE
    coex_schm_status_bit_set(type, status);
#endif // CONFIG_SW_COEXIST_ENABLE
}

static void coex_schm_status_bit_clear_wrapper(uint32_t type, uint32_t status)
{
#if CONFIG_SW_COEXIST_ENABLE
    coex_schm_status_bit_clear(type, status);
#endif // CONFIG_SW_COEXIST_ENABLE
}

#ifdef CONFIG_BT_BLUEDROID_ENABLED
bool esp_vhci_host_check_send_available(void)
{
    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return false;
    }
    return true;
}

static struct os_mbuf *ble_hs_mbuf_gen_pkt(uint16_t leading_space)
{
    struct os_mbuf *om;
    int rc;

    om = os_msys_get_pkthdr(0, 0);
    if (om == NULL) {
        return NULL;
    }

    if (om->om_omp->omp_databuf_len < leading_space) {
        rc = os_mbuf_free_chain(om);
        assert(rc == 0);
        return NULL;
    }

    om->om_data += leading_space;

    return om;
}

struct os_mbuf *ble_hs_mbuf_acl_pkt(void)
{
    return ble_hs_mbuf_gen_pkt(4 + 1);
}

void esp_vhci_host_send_packet(uint8_t *data, uint16_t len)
{
    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return;
    }

    if (*(data) == DATA_TYPE_COMMAND) {
        struct ble_hci_cmd *cmd = NULL;
        cmd = (struct ble_hci_cmd *) ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_CMD);
        assert(cmd);
        memcpy((uint8_t *)cmd, data + 1, len - 1);
        ble_hci_trans_hs_cmd_tx((uint8_t *)cmd);
    }

    if (*(data) == DATA_TYPE_ACL) {
        struct os_mbuf *om = os_msys_get_pkthdr(len, ACL_DATA_MBUF_LEADINGSPCAE);
        assert(om);
        assert(os_mbuf_append(om, &data[1], len - 1) == 0);
        ble_hci_trans_hs_acl_tx(om);
    }
}

esp_err_t esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback)
{
    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_FAIL;
    }

    ble_hci_trans_cfg_hs(ble_hs_hci_rx_evt, NULL, ble_hs_rx_data, NULL);

    return ESP_OK;
}
#endif // CONFIG_BT_BLUEDROID_ENABLED

static int task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth,
                                void *param, uint32_t prio, void *task_handle, uint32_t core_id)
{
    return (uint32_t)xTaskCreatePinnedToCore(task_func, name, stack_depth, param, prio, task_handle,
                                             (core_id < CONFIG_FREERTOS_NUMBER_OF_CORES ? core_id : tskNO_AFFINITY));
}

static void task_delete_wrapper(void *task_handle)
{
    vTaskDelete(task_handle);
}

static int esp_ecc_gen_key_pair(uint8_t *pub, uint8_t *priv)
{
    int rc = -1;
#if CONFIG_BT_LE_SM_LEGACY || CONFIG_BT_LE_SM_SC
    rc = ble_sm_alg_gen_key_pair(pub, priv);
#endif // CONFIG_BT_LE_SM_LEGACY || CONFIG_BT_LE_SM_SC
    return rc;
}

static int esp_ecc_gen_dh_key(const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y,
                              const uint8_t *our_priv_key, uint8_t *out_dhkey)
{
    int rc = -1;
#if CONFIG_BT_LE_SM_LEGACY || CONFIG_BT_LE_SM_SC
    rc = ble_sm_alg_gen_dhkey(peer_pub_key_x, peer_pub_key_y, our_priv_key, out_dhkey);
#endif // CONFIG_BT_LE_SM_LEGACY || CONFIG_BT_LE_SM_SC
    return rc;
}

#ifdef CONFIG_BT_LE_HCI_INTERFACE_USE_UART
static void hci_uart_start_tx_wrapper(int uart_no)
{
    hci_uart_start_tx(uart_no);
}

static int hci_uart_init_cbs_wrapper(int uart_no, hci_uart_tx_char tx_func,
                                     hci_uart_tx_done tx_done, hci_uart_rx_char rx_func, void *arg)
{
    int rc = -1;
    rc = hci_uart_init_cbs(uart_no, tx_func, tx_done, rx_func, arg);
    return rc;
}


static int hci_uart_config_wrapper(int port_num, int32_t baud_rate, uint8_t data_bits,
                                   uint8_t stop_bits, uart_parity_t parity,
                                   uart_hw_flowcontrol_t flow_ctl)
{
    int rc = -1;
    rc = hci_uart_config(port_num, baud_rate, data_bits, stop_bits, parity, flow_ctl);
    return rc;
}

static int hci_uart_close_wrapper(int uart_no)
{
    int rc = -1;
    rc = hci_uart_close(uart_no);
    return rc;
}

static void hci_uart_blocking_tx_wrapper(int port, uint8_t data)
{
    //This function is nowhere to use.
}

static int hci_uart_init_wrapper(int uart_no, void *cfg)
{
    //This function is nowhere to use.
    return 0;
}

#endif //CONFIG_BT_LE_HCI_INTERFACE_USE_UART

static int ble_hci_unregistered_hook(void*, void*)
{
    ESP_LOGD(NIMBLE_PORT_LOG_TAG,"%s ble hci rx_evt is not registered.",__func__);
    return 0;
}

static int esp_intr_alloc_wrapper(int source, int flags, intr_handler_t handler,
                                  void *arg, void **ret_handle_in)
{
    int rc = esp_intr_alloc(source, flags | ESP_INTR_FLAG_IRAM, handler,
                            arg, (intr_handle_t *)ret_handle_in);
    return rc;
}

static int esp_intr_free_wrapper(void **ret_handle)
{
    int rc = 0;
    rc = esp_intr_free((intr_handle_t) * ret_handle);
    *ret_handle = NULL;
    return rc;
}

void esp_bt_rtc_slow_clk_select(uint8_t slow_clk_src)
{
    /* Select slow clock source for BT momdule */
    // switch (slow_clk_src) {
    //     case MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL:
    //         ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Using main XTAL as clock source");
    //         uint32_t chip_version = efuse_hal_chip_revision();
    //         if (chip_version == 0) {
    //             modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (400 - 1));
    //         } else{
    //             modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (5 - 1));
    //         }
    //         break;
    //     case MODEM_CLOCK_LPCLK_SRC_RC_SLOW:
    //         ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Using 136 kHz RC as clock source, can only run legacy ADV or SCAN due to low clock accuracy!");
    //         modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (5 - 1));
    //         break;
    //     case MODEM_CLOCK_LPCLK_SRC_XTAL32K:
    //         ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Using external 32.768 kHz XTAL as clock source");
    //         modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (1 - 1));
    //         break;
    //     case MODEM_CLOCK_LPCLK_SRC_RC32K:
    //         ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Using 32 kHz RC as clock source, can only run legacy ADV or SCAN due to low clock accuracy!");
    //         modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (1 - 1));
    //         break;
    //     case MODEM_CLOCK_LPCLK_SRC_EXT32K:
    //         ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Using 32 kHz oscillator as clock source, can only run legacy ADV or SCAN due to low clock accuracy!");
    //         modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (1 - 1));
    //         break;
    //     default:
    // }
}

IRAM_ATTR void controller_sleep_cb(uint32_t enable_tick, void *arg)
{
    if (!s_ble_active) {
        return;
    }
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
    r_ble_rtc_wake_up_state_clr();
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */
    esp_phy_disable(PHY_MODEM_BT);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(s_pm_lock);
#endif // CONFIG_PM_ENABLE
    s_ble_active = false;
}

IRAM_ATTR void controller_wakeup_cb(void *arg)
{
    if (s_ble_active) {
        return;
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(s_pm_lock);
    r_ble_rtc_wake_up_state_clr();
#endif //CONFIG_PM_ENABLE
    esp_phy_enable(PHY_MODEM_BT);
    s_ble_active = true;
}

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
static esp_err_t sleep_modem_ble_mac_modem_state_init(uint8_t extra)
{
    uint8_t size;
    const sleep_retention_entries_config_t *ble_mac_modem_config = esp_ble_mac_retention_link_get(&size, extra);
    esp_err_t err = sleep_retention_entries_create(ble_mac_modem_config, size, REGDMA_LINK_PRI_BT_MAC_BB, SLEEP_RETENTION_MODULE_BLE_MAC);
    if (err == ESP_OK) {
        ESP_LOGI(NIMBLE_PORT_LOG_TAG, "Modem BLE MAC retention initialization");
    }
    return err;
}

static void sleep_modem_ble_mac_modem_state_deinit(void)
{
    sleep_retention_entries_destroy(SLEEP_RETENTION_MODULE_BLE_MAC);
}

void sleep_modem_light_sleep_overhead_set(uint32_t overhead)
{
    esp_ble_set_wakeup_overhead(overhead);
}
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */


esp_err_t controller_sleep_init(void)
{
    esp_err_t rc = 0;

#ifdef CONFIG_BT_LE_SLEEP_ENABLE
    ESP_LOGW(NIMBLE_PORT_LOG_TAG, "BLE modem sleep is enabled");
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
    r_ble_lll_rfmgmt_set_sleep_cb(controller_sleep_cb, controller_wakeup_cb, 0, 0,
                                BLE_RTC_DELAY_US_LIGHT_SLEEP);
#else
    r_ble_lll_rfmgmt_set_sleep_cb(controller_sleep_cb, controller_wakeup_cb, 0, 0,
                                BLE_RTC_DELAY_US_MODEM_SLEEP);
#endif /* FREERTOS_USE_TICKLESS_IDLE */
#endif // CONFIG_BT_LE_SLEEP_ENABLE

#ifdef CONFIG_PM_ENABLE
    rc = esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "bt", &s_pm_lock);
    if (rc != ESP_OK) {
        goto error;
    }
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
    /* Create a new regdma link for BLE related register restoration */
    rc = sleep_modem_ble_mac_modem_state_init(1);
    assert(rc == 0);
    esp_sleep_enable_bt_wakeup();
    ESP_LOGW(NIMBLE_PORT_LOG_TAG, "Enable light sleep, the wake up source is BLE timer");

    rc = esp_pm_register_inform_out_light_sleep_overhead_callback(sleep_modem_light_sleep_overhead_set);
    if (rc != ESP_OK) {
        goto error;
    }

#if SOC_PM_RETENTION_HAS_CLOCK_BUG && CONFIG_MAC_BB_PD
    sleep_modem_register_mac_bb_module_prepare_callback(sleep_modem_mac_bb_power_down_prepare,
                                                   sleep_modem_mac_bb_power_up_prepare);
#endif // SOC_PM_RETENTION_HAS_CLOCK_BUG && CONFIG_MAC_BB_PD
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */
    return rc;

error:

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
#if SOC_PM_RETENTION_HAS_CLOCK_BUG && CONFIG_MAC_BB_PD
    sleep_modem_unregister_mac_bb_module_prepare_callback(sleep_modem_mac_bb_power_down_prepare,
                                                     sleep_modem_mac_bb_power_up_prepare);
#endif // SOC_PM_RETENTION_HAS_CLOCK_BUG && CONFIG_MAC_BB_PD
    esp_sleep_disable_bt_wakeup();
    esp_pm_unregister_inform_out_light_sleep_overhead_callback(sleep_modem_light_sleep_overhead_set);
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */
    /*lock should release first and then delete*/
    if (s_pm_lock != NULL) {
        esp_pm_lock_delete(s_pm_lock);
        s_pm_lock = NULL;
    }
#endif // CONFIG_PM_ENABLE

    return rc;
}

void controller_sleep_deinit(void)
{
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
#if SOC_PM_RETENTION_HAS_CLOCK_BUG && CONFIG_MAC_BB_PD
    sleep_modem_unregister_mac_bb_module_prepare_callback(sleep_modem_mac_bb_power_down_prepare,
                                                     sleep_modem_mac_bb_power_up_prepare);
#endif // SOC_PM_RETENTION_HAS_CLOCK_BUG && CONFIG_MAC_BB_PD
    r_ble_rtc_wake_up_state_clr();
    esp_sleep_disable_bt_wakeup();
    sleep_modem_ble_mac_modem_state_deinit();
    esp_pm_unregister_inform_out_light_sleep_overhead_callback(sleep_modem_light_sleep_overhead_set);
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */
#ifdef CONFIG_PM_ENABLE
    /* lock should be released first */
    esp_pm_lock_delete(s_pm_lock);
    s_pm_lock = NULL;
#endif //CONFIG_PM_ENABLE
}

typedef enum {
    FILTER_DUPLICATE_PDUTYPE = BIT(0),
    FILTER_DUPLICATE_LENGTH  = BIT(1),
    FILTER_DUPLICATE_ADDRESS = BIT(2),
    FILTER_DUPLICATE_ADVDATA = BIT(3),
    FILTER_DUPLICATE_DEFAULT = FILTER_DUPLICATE_PDUTYPE | FILTER_DUPLICATE_ADDRESS,
    FILTER_DUPLICATE_PDU_ALL = 0xF,
    FILTER_DUPLICATE_EXCEPTION_FOR_MESH = BIT(4),
    FILTER_DUPLICATE_AD_TYPE = BIT(5),
}disc_duplicate_mode_t;


extern void r_filter_duplicate_mode_enable(disc_duplicate_mode_t mode);
extern void r_filter_duplicate_mode_disable(disc_duplicate_mode_t mode);
extern void r_filter_duplicate_set_ring_list_max_num(uint32_t max_num);
extern void r_scan_duplicate_cache_refresh_set_time(uint32_t period_time);

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

int ble_vhci_disc_duplicate_set_max_cache_size(int max_cache_size){
    // TODO: use vendor hci to update
    r_filter_duplicate_set_ring_list_max_num(max_cache_size);
    return true;
}

int ble_vhci_disc_duplicate_set_period_refresh_time(int refresh_period_time){
    // TODO: use vendor hci to update
    r_scan_duplicate_cache_refresh_set_time(refresh_period_time);
    return true;
}

/**
 * @brief Config scan duplicate option mode from menuconfig (Adapt to the old configuration method.)
 */
void ble_controller_scan_duplicate_config(void)
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

esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg)
{
    uint8_t mac[6];
    esp_err_t ret = ESP_OK;
    ble_npl_count_info_t npl_info;
    uint32_t slow_clk_freq = 0;

    memset(&npl_info, 0, sizeof(ble_npl_count_info_t));

    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_ERR_INVALID_STATE;
    }

    if (!cfg) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "cfg is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    ret = esp_register_ext_funcs(&ext_funcs_ro);
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "register extend functions failed");
        return ret;
    }

    /* Initialize the function pointers for OS porting */
    npl_freertos_funcs_init();
    struct npl_funcs_t *p_npl_funcs = npl_freertos_funcs_get();
    if (!p_npl_funcs) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "npl functions get failed");
        return ESP_ERR_INVALID_ARG;
    }

    ret = esp_register_npl_funcs(p_npl_funcs);
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "npl functions register failed");
        goto free_mem;
    }

    ble_get_npl_element_info(cfg, &npl_info);
    npl_freertos_set_controller_npl_info(&npl_info);
    if (npl_freertos_mempool_init() != 0) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "npl mempool init failed");
        ret = ESP_ERR_INVALID_ARG;
        goto free_mem;
    }

#if CONFIG_BT_NIMBLE_ENABLED
    /* ble_npl_eventq_init() needs to use npl functions in rom and
     * must be called after esp_bt_controller_init().
     */
    ble_npl_eventq_init(nimble_port_get_dflt_eventq());
#endif // CONFIG_BT_NIMBLE_ENABLED
    /* Enable BT-related clocks */
    modem_clock_module_enable(PERIPH_BT_MODULE);
    modem_clock_module_mac_reset(PERIPH_BT_MODULE);
    /* Select slow clock source for BT momdule */
#if CONFIG_BT_LE_LP_CLK_SRC_MAIN_XTAL
   esp_bt_rtc_slow_clk_select(MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL);
   slow_clk_freq = 100000;
#else
#if CONFIG_RTC_CLK_SRC_INT_RC
    esp_bt_rtc_slow_clk_select(MODEM_CLOCK_LPCLK_SRC_RC_SLOW);
    slow_clk_freq = 30000;
#elif CONFIG_RTC_CLK_SRC_EXT_CRYS
    if (rtc_clk_slow_src_get() == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
        esp_bt_rtc_slow_clk_select(MODEM_CLOCK_LPCLK_SRC_XTAL32K);
        slow_clk_freq = 32768;
    } else {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "32.768kHz XTAL not detected, fall back to main XTAL as Bluetooth sleep clock");
        esp_bt_rtc_slow_clk_select(MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL);
        slow_clk_freq = 100000;
    }
#elif CONFIG_RTC_CLK_SRC_INT_RC32K
    esp_bt_rtc_slow_clk_select(MODEM_CLOCK_LPCLK_SRC_RC32K);
    slow_clk_freq = 32000;
#elif CONFIG_RTC_CLK_SRC_EXT_OSC
    esp_bt_rtc_slow_clk_select(MODEM_CLOCK_LPCLK_SRC_EXT32K);
    slow_clk_freq = 32000;
#else
    ESP_LOGE(NIMBLE_PORT_LOG_TAG, "Unsupported clock source");
    assert(0);
#endif
#endif /* CONFIG_BT_LE_LP_CLK_SRC_MAIN_XTAL */
    esp_phy_modem_init();

    if (ble_osi_coex_funcs_register((struct osi_coex_funcs_t *)&s_osi_coex_funcs_ro) != 0) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "osi coex funcs reg failed");
        ret = ESP_ERR_INVALID_ARG;
        goto modem_deint;
    }

#if CONFIG_SW_COEXIST_ENABLE
    coex_init();
#endif // CONFIG_SW_COEXIST_ENABLE

    ret = ble_controller_init(cfg);
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "ble_controller_init failed %d", ret);
        goto modem_deint;
    }

    ESP_LOGI(NIMBLE_PORT_LOG_TAG, "ble controller commit:[%s]", ble_controller_get_compile_version());
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    interface_func_t bt_controller_log_interface;
    bt_controller_log_interface = esp_bt_controller_log_interface;
    uint8_t buffers = 0;
#if CONFIG_BT_LE_CONTROLLER_LOG_CTRL_ENABLED
    buffers |= ESP_BLE_LOG_BUF_CONTROLLER;
#endif // CONFIG_BT_LE_CONTROLLER_LOG_CTRL_ENABLED
#if CONFIG_BT_LE_CONTROLLER_LOG_HCI_ENABLED
    buffers |= ESP_BLE_LOG_BUF_HCI;
#endif // CONFIG_BT_LE_CONTROLLER_LOG_HCI_ENABLED
#if CONFIG_BT_LE_CONTROLLER_LOG_DUMP_ONLY
    ret = r_ble_log_init_async(bt_controller_log_interface, false, buffers, (uint32_t *)log_bufs_size);
#else
    ret = r_ble_log_init_async(bt_controller_log_interface, true, buffers, (uint32_t *)log_bufs_size);
#endif // CONFIG_BT_CONTROLLER_LOG_DUMP
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "ble_controller_log_init failed %d", ret);
        goto controller_init_err;
    }
#endif // CONFIG_BT_CONTROLLER_LOG_ENABLED

    esp_ble_change_rtc_freq(slow_clk_freq);

    ble_controller_scan_duplicate_config();

    ret = os_msys_init();
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "msys_init failed %d", ret);
        goto free_controller;
    }

    ret = controller_sleep_init();
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "controller_sleep_init failed %d", ret);
        goto free_controller;
    }
    ESP_ERROR_CHECK(esp_read_mac((uint8_t *)mac, ESP_MAC_BT));
    swap_in_place(mac, 6);
    esp_ble_ll_set_public_addr(mac);

    ble_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

    ble_hci_trans_cfg_hs((ble_hci_trans_rx_cmd_fn *)ble_hci_unregistered_hook,NULL,
                         (ble_hci_trans_rx_acl_fn *)ble_hci_unregistered_hook,NULL);
    return ESP_OK;

free_controller:
    controller_sleep_deinit();
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
controller_init_err:
    r_ble_log_deinit_async();
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    os_msys_deinit();
    ble_controller_deinit();
modem_deint:
    esp_phy_modem_deinit();
    // modem_clock_deselect_lp_clock_source(PERIPH_BT_MODULE);
    modem_clock_module_disable(PERIPH_BT_MODULE);
#if CONFIG_BT_NIMBLE_ENABLED
    ble_npl_eventq_deinit(nimble_port_get_dflt_eventq());
#endif // CONFIG_BT_NIMBLE_ENABLED
free_mem:
    npl_freertos_mempool_deinit();
    esp_unregister_npl_funcs();
    npl_freertos_funcs_deinit();
    esp_unregister_ext_funcs();
    return ret;
}

esp_err_t esp_bt_controller_deinit(void)
{
    if ((ble_controller_status < ESP_BT_CONTROLLER_STATUS_INITED) ||
        (ble_controller_status >= ESP_BT_CONTROLLER_STATUS_ENABLED)) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_FAIL;
    }

    controller_sleep_deinit();

    os_msys_deinit();

    esp_phy_modem_deinit();
    // modem_clock_deselect_lp_clock_source(PERIPH_BT_MODULE);
    modem_clock_module_disable(PERIPH_BT_MODULE);

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    r_ble_log_deinit_async();
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    ble_controller_deinit();

#if CONFIG_BT_NIMBLE_ENABLED
    /* De-initialize default event queue */
    ble_npl_eventq_deinit(nimble_port_get_dflt_eventq());
#endif // CONFIG_BT_NIMBLE_ENABLED

    esp_unregister_npl_funcs();

    esp_unregister_ext_funcs();

    /* De-initialize npl functions */
    npl_freertos_funcs_deinit();

    npl_freertos_mempool_deinit();

    ble_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

    return ESP_OK;
}

esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode)
{
    esp_err_t ret = ESP_OK;

    if (mode != ESP_BT_MODE_BLE) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller mode");
        return ESP_FAIL;
    }
    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_FAIL;
    }
    if (!s_ble_active) {
#if CONFIG_PM_ENABLE
        esp_pm_lock_acquire(s_pm_lock);
#endif  // CONFIG_PM_ENABLE
        esp_phy_enable(PHY_MODEM_BT);
        s_ble_active = true;
    }
    esp_btbb_enable();
#if CONFIG_SW_COEXIST_ENABLE
    coex_enable();
#endif // CONFIG_SW_COEXIST_ENABLE

    if (ble_controller_enable(mode) != 0) {
        ret = ESP_FAIL;
        goto error;
    }
    ble_controller_status = ESP_BT_CONTROLLER_STATUS_ENABLED;
    return ESP_OK;

error:
#if CONFIG_SW_COEXIST_ENABLE
    coex_disable();
#endif
    esp_btbb_disable();
    if (s_ble_active) {
        esp_phy_disable(PHY_MODEM_BT);
#if CONFIG_PM_ENABLE
        esp_pm_lock_release(s_pm_lock);
#endif  // CONFIG_PM_ENABLE
        s_ble_active = false;
    }
    return ret;
}

esp_err_t esp_bt_controller_disable(void)
{
    if (ble_controller_status < ESP_BT_CONTROLLER_STATUS_ENABLED) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_FAIL;
    }
    if (ble_controller_disable() != 0) {
        return ESP_FAIL;
    }
#if CONFIG_SW_COEXIST_ENABLE
    coex_disable();
#endif
    esp_btbb_disable();
    if (s_ble_active) {
        esp_phy_disable(PHY_MODEM_BT);
#if CONFIG_PM_ENABLE
        esp_pm_lock_release(s_pm_lock);
#endif  // CONFIG_PM_ENABLE
        s_ble_active = false;
    }
    ble_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;
    return ESP_OK;
}

esp_err_t esp_bt_controller_mem_release(esp_bt_mode_t mode)
{
    ESP_LOGD(NIMBLE_PORT_LOG_TAG, "%s not implemented, return OK", __func__);
    return ESP_OK;
}

static esp_err_t try_heap_caps_add_region(intptr_t start, intptr_t end)
{
    int ret = heap_caps_add_region(start, end);

    /* heap_caps_add_region() returns ESP_ERR_INVALID_SIZE if the memory region is
     * is too small to fit a heap. This cannot be termed as a fatal error and hence
     * we replace it by ESP_OK
     */
    if (ret == ESP_ERR_INVALID_SIZE) {
        return ESP_OK;
    }
    return ret;
}


typedef struct {
    intptr_t start;
    intptr_t end;
    const char* name;
} bt_area_t;

static esp_err_t esp_bt_mem_release_area(const bt_area_t *area)
{
    esp_err_t ret = ESP_OK;
    intptr_t mem_start = area->start;
    intptr_t mem_end = area->end;
    if (mem_start != mem_end) {
        ESP_LOGD(NIMBLE_PORT_LOG_TAG, "Release %s [0x%08x] - [0x%08x], len %d", area->name, mem_start, mem_end, mem_end - mem_start);
        ret = try_heap_caps_add_region(mem_start, mem_end);
    }
    return ret;
}

static esp_err_t esp_bt_mem_release_areas(const bt_area_t *area1, const bt_area_t *area2)
{
    esp_err_t ret = ESP_OK;

    if (area1->end == area2->start) {
        bt_area_t merged_area = {
            .start = area1->start,
            .end = area2->end,
            .name = area1->name
        };
        ret = esp_bt_mem_release_area(&merged_area);
    } else {
        esp_bt_mem_release_area(area1);
        ret = esp_bt_mem_release_area(area2);
    }

    return ret;
}

esp_err_t esp_bt_mem_release(esp_bt_mode_t mode)
{
    esp_err_t ret = ESP_OK;

    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_area_t bss = {
        .start = (intptr_t)&_bt_bss_start,
        .end   = (intptr_t)&_bt_bss_end,
        .name  = "BT BSS",
    };
    bt_area_t cont_bss = {
        .start = (intptr_t)&_bt_controller_bss_start,
        .end   = (intptr_t)&_bt_controller_bss_end,
        .name  = "BT Controller BSS",
    };
    bt_area_t data = {
        .start = (intptr_t)&_bt_data_start,
        .end   = (intptr_t)&_bt_data_end,
        .name  = "BT Data",
    };
    bt_area_t cont_data = {
        .start = (intptr_t)&_bt_controller_data_start,
        .end   = (intptr_t)&_bt_controller_data_end,
        .name  = "BT Controller Data"
    };

    if (mode & ESP_BT_MODE_BLE) {
        /* Start by freeing Bluetooth BSS section */
        if (ret == ESP_OK) {
            ret = esp_bt_mem_release_areas(&bss, &cont_bss);
        }

        /* Do the same thing with the Bluetooth data section */
        if (ret == ESP_OK) {
            ret = esp_bt_mem_release_areas(&data, &cont_data);
        }
    }

    return ret;
}


esp_bt_controller_status_t esp_bt_controller_get_status(void)
{
    return ble_controller_status;
}

esp_err_t esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level)
{
    esp_err_t stat = ESP_FAIL;

    switch (power_type) {
    case ESP_BLE_PWR_TYPE_DEFAULT:
    case ESP_BLE_PWR_TYPE_ADV:
    case ESP_BLE_PWR_TYPE_SCAN:
        if (ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0, power_level) == 0) {
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
        if (ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_CONN, power_type, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    default:
        stat = ESP_ERR_NOT_SUPPORTED;
        break;
    }

    return stat;
}

esp_err_t esp_ble_tx_power_set_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle,
                                        esp_power_level_t power_level)
{
    esp_err_t stat = ESP_FAIL;
    switch (power_type) {
    case ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT:
    case ESP_BLE_ENHANCED_PWR_TYPE_SCAN:
    case ESP_BLE_ENHANCED_PWR_TYPE_INIT:
        if (ble_txpwr_set(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    case ESP_BLE_ENHANCED_PWR_TYPE_ADV:
    case ESP_BLE_ENHANCED_PWR_TYPE_CONN:
        if (ble_txpwr_set(power_type, handle, power_level) == 0) {
            stat = ESP_OK;
        }
        break;
    default:
        stat = ESP_ERR_NOT_SUPPORTED;
        break;
    }

    return stat;
}

esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type)
{
    int tx_level = 0;

    switch (power_type) {
    case ESP_BLE_PWR_TYPE_ADV:
    case ESP_BLE_PWR_TYPE_SCAN:
    case ESP_BLE_PWR_TYPE_DEFAULT:
        tx_level = ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0);
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
        tx_level = ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_CONN, power_type);
        break;
    default:
        return ESP_PWR_LVL_INVALID;
    }

    if (tx_level < 0) {
        return ESP_PWR_LVL_INVALID;
    }

    return (esp_power_level_t)tx_level;
}

esp_power_level_t esp_ble_tx_power_get_enhanced(esp_ble_enhanced_power_type_t power_type,
                                                uint16_t handle)
{
    int tx_level = 0;

    switch (power_type) {
    case ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT:
    case ESP_BLE_ENHANCED_PWR_TYPE_SCAN:
    case ESP_BLE_ENHANCED_PWR_TYPE_INIT:
        tx_level = ble_txpwr_get(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0);
        break;
    case ESP_BLE_ENHANCED_PWR_TYPE_ADV:
    case ESP_BLE_ENHANCED_PWR_TYPE_CONN:
        tx_level = ble_txpwr_get(power_type, handle);
        break;
    default:
        return ESP_PWR_LVL_INVALID;
    }

    if (tx_level < 0) {
       return ESP_PWR_LVL_INVALID;
    }

    return (esp_power_level_t)tx_level;
}

#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
static void esp_bt_controller_log_interface(uint32_t len, const uint8_t *addr, bool end)
{
    for (int i = 0; i < len; i++) {
        esp_rom_printf("%02x ", addr[i]);
    }
    if (end) {
        esp_rom_printf("\n");
    }
}

void esp_ble_controller_log_dump_all(bool output)
{
    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

    portENTER_CRITICAL_SAFE(&spinlock);
    esp_panic_handler_reconfigure_wdts(5000);
    BT_ASSERT_PRINT("\r\n[DUMP_START:");
    r_ble_log_async_output_dump_all(output);
    BT_ASSERT_PRINT(":DUMP_END]\r\n");
    portEXIT_CRITICAL_SAFE(&spinlock);
}
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

#if (!CONFIG_BT_NIMBLE_ENABLED) && (CONFIG_BT_CONTROLLER_ENABLED)
#if CONFIG_BT_LE_SM_LEGACY || CONFIG_BT_LE_SM_SC
#define BLE_SM_KEY_ERR 0x17
#if CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
#include "mbedtls/aes.h"
#if CONFIG_BT_LE_SM_SC
#include "mbedtls/cipher.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/cmac.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecp.h"

static mbedtls_ecp_keypair keypair;
#endif // CONFIG_BT_LE_SM_SC

#else
#include "tinycrypt/aes.h"
#include "tinycrypt/constants.h"
#include "tinycrypt/utils.h"

#if CONFIG_BT_LE_SM_SC
#include "tinycrypt/cmac_mode.h"
#include "tinycrypt/ecc_dh.h"
#endif // CONFIG_BT_LE_SM_SC
#endif // CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS

/* Based on Core Specification 4.2 Vol 3. Part H 2.3.5.6.1 */
static const uint8_t ble_sm_alg_dbg_priv_key[32] = {
    0x3f, 0x49, 0xf6, 0xd4, 0xa3, 0xc5, 0x5f, 0x38, 0x74, 0xc9, 0xb3, 0xe3,
    0xd2, 0x10, 0x3f, 0x50, 0x4a, 0xff, 0x60, 0x7b, 0xeb, 0x40, 0xb7, 0x99,
    0x58, 0x99, 0xb8, 0xa6, 0xcd, 0x3c, 0x1a, 0xbd
};

int ble_sm_alg_gen_dhkey(const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y,
                         const uint8_t *our_priv_key, uint8_t *out_dhkey)
{
    uint8_t dh[32];
    uint8_t pk[64];
    uint8_t priv[32];
    int rc = BLE_SM_KEY_ERR;

    swap_buf(pk, peer_pub_key_x, 32);
    swap_buf(&pk[32], peer_pub_key_y, 32);
    swap_buf(priv, our_priv_key, 32);

#if CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
    struct mbedtls_ecp_point pt = {0}, Q = {0};
    mbedtls_mpi z = {0}, d = {0};
    mbedtls_ctr_drbg_context ctr_drbg = {0};
    mbedtls_entropy_context entropy = {0};

    uint8_t pub[65] = {0};
    /* Hardcoded first byte of pub key for MBEDTLS_ECP_PF_UNCOMPRESSED */
    pub[0] = 0x04;
    memcpy(&pub[1], pk, 64);

    /* Initialize the required structures here */
    mbedtls_ecp_point_init(&pt);
    mbedtls_ecp_point_init(&Q);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
    mbedtls_mpi_init(&d);
    mbedtls_mpi_init(&z);

    /* Below 3 steps are to validate public key on curve secp256r1 */
    if (mbedtls_ecp_group_load(&keypair.MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_SECP256R1) != 0) {
        goto exit;
    }

    if (mbedtls_ecp_point_read_binary(&keypair.MBEDTLS_PRIVATE(grp), &pt, pub, 65) != 0) {
        goto exit;
    }

    if (mbedtls_ecp_check_pubkey(&keypair.MBEDTLS_PRIVATE(grp), &pt) != 0) {
        goto exit;
    }

    /* Set PRNG */
    if ((rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0)) != 0) {
        goto exit;
    }

    /* Prepare point Q from pub key */
    if (mbedtls_ecp_point_read_binary(&keypair.MBEDTLS_PRIVATE(grp), &Q, pub, 65) != 0) {
        goto exit;
    }

    if (mbedtls_mpi_read_binary(&d, priv, 32) != 0) {
        goto exit;
    }

    rc = mbedtls_ecdh_compute_shared(&keypair.MBEDTLS_PRIVATE(grp), &z, &Q, &d,
                                     mbedtls_ctr_drbg_random, &ctr_drbg);
    if (rc != 0) {
        goto exit;
    }

    rc = mbedtls_mpi_write_binary(&z, dh, 32);
    if (rc != 0) {
        goto exit;
    }

exit:
    mbedtls_ecp_point_free(&pt);
    mbedtls_mpi_free(&z);
    mbedtls_mpi_free(&d);
    mbedtls_ecp_point_free(&Q);
    mbedtls_entropy_free(&entropy);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    if (rc != 0) {
        return BLE_SM_KEY_ERR;
    }

#else
    if (uECC_valid_public_key(pk, &curve_secp256r1) < 0) {
        return BLE_SM_KEY_ERR;
    }

    rc = uECC_shared_secret(pk, priv, dh, &curve_secp256r1);
    if (rc == TC_CRYPTO_FAIL) {
        return BLE_SM_KEY_ERR;
    }
#endif // CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS

    swap_buf(out_dhkey, dh, 32);
    return 0;
}

#if CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
static int mbedtls_gen_keypair(uint8_t *public_key, uint8_t *private_key)
{
    int rc = BLE_SM_KEY_ERR;
    mbedtls_entropy_context entropy = {0};
    mbedtls_ctr_drbg_context ctr_drbg = {0};

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_ecp_keypair_init(&keypair);

    if ((rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    NULL, 0)) != 0) {
        goto exit;
    }

    if ((rc = mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, &keypair,
                                  mbedtls_ctr_drbg_random, &ctr_drbg)) != 0) {
        goto exit;
    }

    if ((rc = mbedtls_mpi_write_binary(&keypair.MBEDTLS_PRIVATE(d), private_key, 32)) != 0) {
        goto exit;
    }

    size_t olen = 0;
    uint8_t pub[65] = {0};

    if ((rc = mbedtls_ecp_point_write_binary(&keypair.MBEDTLS_PRIVATE(grp), &keypair.MBEDTLS_PRIVATE(Q), MBEDTLS_ECP_PF_UNCOMPRESSED,
              &olen, pub, 65)) != 0) {
        goto exit;
    }

    memcpy(public_key, &pub[1], 64);

exit:
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    if (rc != 0) {
        mbedtls_ecp_keypair_free(&keypair);
        return BLE_SM_KEY_ERR;
    }

    return 0;
}
#endif  // CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS

/**
 * pub: 64 bytes
 * priv: 32 bytes
 */
int ble_sm_alg_gen_key_pair(uint8_t *pub, uint8_t *priv)
{
#if CONFIG_BT_LE_SM_SC_DEBUG_KEYS
    swap_buf(pub, ble_sm_alg_dbg_pub_key, 32);
    swap_buf(&pub[32], &ble_sm_alg_dbg_pub_key[32], 32);
    swap_buf(priv, ble_sm_alg_dbg_priv_key, 32);
#else
    uint8_t pk[64];

    do {
#if CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
        if (mbedtls_gen_keypair(pk, priv) != 0) {
            return BLE_SM_KEY_ERR;
        }
#else
        if (uECC_make_key(pk, priv, &curve_secp256r1) != TC_CRYPTO_SUCCESS) {
            return BLE_SM_KEY_ERR;
        }
#endif  // CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
        /* Make sure generated key isn't debug key. */
    } while (memcmp(priv, ble_sm_alg_dbg_priv_key, 32) == 0);

    swap_buf(pub, pk, 32);
    swap_buf(&pub[32], &pk[32], 32);
    swap_in_place(priv, 32);
#endif // CONFIG_BT_LE_SM_SC_DEBUG_KEYS
    return 0;
}

#endif // CONFIG_BT_LE_SM_LEGACY || CONFIG_BT_LE_SM_SC
#endif // (!CONFIG_BT_NIMBLE_ENABLED) && (CONFIG_BT_CONTROLLER_ENABLED)
