/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
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

#ifdef ESP_PLATFORM
#include "esp_log.h"
#endif

#if CONFIG_SW_COEXIST_ENABLE
#include "esp_coexist_internal.h"
#endif

#include "nimble/nimble_npl_os.h"
#include "nimble/ble_hci_trans.h"
#include "os/endian.h"

#include "esp_bt.h"
#include "esp_intr_alloc.h"
#include "esp_sleep.h"
#include "esp_pm.h"
#include "esp_phy_init.h"
#include "soc/syscon_reg.h"
#include "soc/modem_clkrst_reg.h"
#include "esp_private/periph_ctrl.h"
#include "hci_uart.h"
#include "bt_osi_mem.h"

#ifdef CONFIG_BT_BLUEDROID_ENABLED
#include "hci/hci_hal.h"
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_private/periph_ctrl.h"
#include "esp_sleep.h"

#include "soc/syscon_reg.h"
#include "soc/dport_access.h"
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
#endif

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
    int (* _task_create)(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id);
    void (* _task_delete)(void *task_handle);
    void (*_osi_assert)(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2);
    uint32_t (* _os_random)(void);
    int (* _ecc_gen_key_pair)(uint8_t *public, uint8_t *priv);
    int (* _ecc_gen_dh_key)(const uint8_t *remote_pub_key_x, const uint8_t *remote_pub_key_y, const uint8_t *local_priv_key, uint8_t *dhkey);
    void (* _esp_reset_rpa_moudle)(void);
    void (* _esp_bt_track_pll_cap)(void);
    uint32_t magic;
};


/* External functions or variables
 ************************************************************************
 */

extern int ble_osi_coex_funcs_register(struct osi_coex_funcs_t *coex_funcs);
extern int coex_core_ble_conn_dyn_prio_get(bool *low, bool *high);
extern int ble_controller_init(esp_bt_controller_config_t *cfg);
extern int ble_controller_deinit(void);
extern int ble_controller_enable(uint8_t mode);
extern int ble_controller_disable(void);
extern int esp_register_ext_funcs (struct ext_funcs_t *);
extern void esp_unregister_ext_funcs (void);
extern int esp_ble_ll_set_public_addr(const uint8_t *addr);
extern int esp_register_npl_funcs (struct npl_funcs_t *p_npl_func);
extern void esp_unregister_npl_funcs (void);
extern void npl_freertos_mempool_deinit(void);
extern void bt_bb_v2_init_cmplx(uint8_t i);
extern int os_msys_buf_alloc(void);
extern uint32_t r_os_cputime_get32(void);
extern uint32_t r_os_cputime_ticks_to_usecs(uint32_t ticks);
extern void r_ble_lll_rfmgmt_set_sleep_cb(void *s_cb, void *w_cb, void *s_arg, void *w_arg, uint32_t us_to_enabled);
extern void r_ble_rtc_wake_up_state_clr(void);
extern int os_msys_init(void);
extern void os_msys_buf_free(void);
extern int ble_sm_alg_gen_dhkey(const uint8_t *peer_pub_key_x,
                                const uint8_t *peer_pub_key_y,
                                const uint8_t *our_priv_key, uint8_t *out_dhkey);
extern int ble_sm_alg_gen_key_pair(uint8_t *pub, uint8_t *priv);
extern int ble_txpwr_set(esp_ble_enhanced_power_type_t power_type, uint16_t handle, int power_level);
extern int ble_txpwr_get(esp_ble_enhanced_power_type_t power_type, uint16_t handle);
extern int ble_get_npl_element_info(esp_bt_controller_config_t *cfg, ble_npl_count_info_t * npl_info);
extern uint32_t _bt_bss_start;
extern uint32_t _bt_bss_end;
extern uint32_t _nimble_bss_start;
extern uint32_t _nimble_bss_end;
extern uint32_t _nimble_data_start;
extern uint32_t _nimble_data_end;
extern uint32_t _bt_data_start;
extern uint32_t _bt_data_end;

/* Local Function Declaration
 *********************************************************************
 */
static void coex_schm_status_bit_set_wrapper(uint32_t type, uint32_t status);
static void coex_schm_status_bit_clear_wrapper(uint32_t type, uint32_t status);
static int task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id);
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
#endif
static int esp_intr_alloc_wrapper(int source, int flags, intr_handler_t handler, void *arg, void **ret_handle_in);
static int esp_intr_free_wrapper(void **ret_handle);
static void osi_assert_wrapper(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2);
static uint32_t osi_random_wrapper(void);
static void esp_reset_rpa_moudle(void);

/* Local variable definition
 ***************************************************************************
 */

/* Static variable declare */
static DRAM_ATTR esp_bt_controller_status_t ble_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

/* This variable tells if BLE is running */
static bool s_ble_active = false;
#ifdef CONFIG_PM_ENABLE
static DRAM_ATTR esp_pm_lock_handle_t s_pm_lock = NULL;
#define BTDM_MIN_TIMER_UNCERTAINTY_US      (200)
#endif /* #ifdef CONFIG_PM_ENABLE */

#ifdef CONFIG_BT_LE_WAKEUP_SOURCE_BLE_RTC_TIMER
#define BLE_RTC_DELAY_US                    (1100)
#endif

#ifdef CONFIG_BT_LE_WAKEUP_SOURCE_CPU_RTC_TIMER
#define BLE_RTC_DELAY_US                    (0)
static void ble_sleep_timer_callback(void *arg);
static DRAM_ATTR esp_timer_handle_t s_ble_sleep_timer = NULL;
#endif


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
    ._ecc_gen_key_pair = ble_sm_alg_gen_key_pair,
    ._ecc_gen_dh_key = ble_sm_alg_gen_dhkey,
    ._esp_reset_rpa_moudle = esp_reset_rpa_moudle,
    .magic = EXT_FUNC_MAGIC_VALUE,
};

static void IRAM_ATTR esp_reset_rpa_moudle(void)
{
    DPORT_SET_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG, BLE_RPA_REST_BIT);
    DPORT_CLEAR_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG, BLE_RPA_REST_BIT);
}

static void IRAM_ATTR osi_assert_wrapper(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2)
{
    BT_ASSERT_PRINT("BLE assert: line %d in function %s, param: 0x%x, 0x%x", ln, fn, param1, param2);
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
#endif
}

static void coex_schm_status_bit_clear_wrapper(uint32_t type, uint32_t status)
{
#if CONFIG_SW_COEXIST_ENABLE
    coex_schm_status_bit_clear(type, status);
#endif
}
#ifdef CONFIG_BT_BLUEDROID_ENABLED

bool esp_vhci_host_check_send_available(void)
{
    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return false;
    }
    return true;
}

/**
 * Allocates an mbuf for use by the nimble host.
 */
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

/**
 * Allocates an mbuf suitable for an HCI ACL data packet.
 *
 * @return                  An empty mbuf on success; null on memory
 *                              exhaustion.
 */
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
        memcpy((uint8_t *)cmd, data + 1, len - 1);
        ble_hci_trans_hs_cmd_tx((uint8_t *)cmd);
    }

    if (*(data) == DATA_TYPE_ACL) {
        struct os_mbuf *om = os_msys_get_pkthdr(len, ACL_DATA_MBUF_LEADINGSPCAE);
        assert(om);
        os_mbuf_append(om, &data[1], len - 1);
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

#endif
static int task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id)
{
    return (uint32_t)xTaskCreatePinnedToCore(task_func, name, stack_depth, param, prio, task_handle, (core_id < portNUM_PROCESSORS ? core_id : tskNO_AFFINITY));
}

static void task_delete_wrapper(void *task_handle)
{
    vTaskDelete(task_handle);
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


static int hci_uart_config_wrapper(int port_num, int32_t baud_rate, uint8_t data_bits, uint8_t stop_bits,
                                   uart_parity_t parity, uart_hw_flowcontrol_t flow_ctl)
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

static int esp_intr_alloc_wrapper(int source, int flags, intr_handler_t handler, void *arg, void **ret_handle_in)
{
    int rc = esp_intr_alloc(source, flags | ESP_INTR_FLAG_IRAM, handler, arg, (intr_handle_t *)ret_handle_in);
    return rc;
}

static int esp_intr_free_wrapper(void **ret_handle)
{
    int rc = 0;
    rc = esp_intr_free((intr_handle_t) * ret_handle);
    *ret_handle = NULL;
    return rc;
}

IRAM_ATTR void controller_sleep_cb(uint32_t enable_tick, void *arg)
{
    if (!s_ble_active) {
        return;
    }
#ifdef CONFIG_PM_ENABLE
#ifdef CONFIG_BT_LE_WAKEUP_SOURCE_CPU_RTC_TIMER
    uint32_t delta_tick;
    uint32_t us_to_sleep;
    uint32_t sleep_tick;
    uint32_t tick_invalid = *(uint32_t*)(arg);
    assert(arg != NULL);
    if (!tick_invalid) {
        sleep_tick = r_os_cputime_get32();
        // start a timer to wake up and acquire the pm_lock before modem_sleep awakes
        delta_tick = enable_tick - sleep_tick;
        if (delta_tick & 0x80000000) {
            return;
        }
        us_to_sleep = r_os_cputime_ticks_to_usecs(delta_tick);
        if (us_to_sleep <= BTDM_MIN_TIMER_UNCERTAINTY_US) {
            return;
        }
        esp_err_t err = esp_timer_start_once(s_ble_sleep_timer, us_to_sleep - BTDM_MIN_TIMER_UNCERTAINTY_US);
        if (err != ESP_OK) {
            ESP_LOGW(NIMBLE_PORT_LOG_TAG, "ESP timer start failed\n");
            return;
        }
    }
#endif // CONFIG_BT_LE_WAKEUP_SOURCE_CPU_RTC_TIMER

#ifdef CONFIG_BT_LE_WAKEUP_SOURCE_BLE_RTC_TIMER
    r_ble_rtc_wake_up_state_clr();
#endif
    esp_pm_lock_release(s_pm_lock);
#endif // CONFIG_PM_ENABLE
    esp_phy_disable();
    s_ble_active = false;
}

IRAM_ATTR void controller_wakeup_cb(void *arg)
{
    if (s_ble_active) {
        return;
    }
    esp_phy_enable();
    // need to check if need to call pm lock here
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(s_pm_lock);
#endif //CONFIG_PM_ENABLE
    s_ble_active = true;
}

#ifdef CONFIG_PM_ENABLE
#ifdef CONFIG_BT_LE_WAKEUP_SOURCE_CPU_RTC_TIMER
static void ble_sleep_timer_callback(void * arg)
{

}

#endif // CONFIG_BT_LE_WAKEUP_SOURCE_CPU_RTC_TIMER
#endif // CONFIG_PM_ENABLE

esp_err_t controller_sleep_init(void)
{
    esp_err_t rc = 0;
#ifdef CONFIG_BT_LE_SLEEP_ENABLE
    ESP_LOGW(NIMBLE_PORT_LOG_TAG, "BLE modem sleep is enabled\n");
    r_ble_lll_rfmgmt_set_sleep_cb(controller_sleep_cb, controller_wakeup_cb, 0, 0, 500 + BLE_RTC_DELAY_US);

#ifdef CONFIG_PM_ENABLE
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON);
#endif // CONFIG_PM_ENABLE

#endif // CONFIG_BT_LE_SLEEP_ENABLE

    // enable light sleep
#ifdef CONFIG_PM_ENABLE
    rc = esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "bt", &s_pm_lock);
    if (rc != ESP_OK) {
        goto error;
    }
    esp_pm_lock_acquire(s_pm_lock);
#ifdef CONFIG_BT_LE_WAKEUP_SOURCE_CPU_RTC_TIMER
    esp_timer_create_args_t create_args = {
        .callback = ble_sleep_timer_callback,
        .arg = NULL,
        .name = "btSlp"
    };
    rc = esp_timer_create(&create_args, &s_ble_sleep_timer);
    if (rc != ESP_OK) {
        goto error;
    }
    ESP_LOGW(NIMBLE_PORT_LOG_TAG, "Enable light sleep, the wake up source is ESP timer");
#endif //CONFIG_BT_LE_WAKEUP_SOURCE_CPU_RTC_TIMER

#ifdef CONFIG_BT_LE_WAKEUP_SOURCE_BLE_RTC_TIMER
    esp_sleep_enable_bt_wakeup();
    ESP_LOGW(NIMBLE_PORT_LOG_TAG, "Enable light sleep, the wake up source is BLE timer");
#endif // CONFIG_BT_LE_WAKEUP_SOURCE_BLE_RTC_TIMER

    return rc;

error:
    /*lock should release first and then delete*/
    if (s_pm_lock != NULL) {
        esp_pm_lock_release(s_pm_lock);
        esp_pm_lock_delete(s_pm_lock);
        s_pm_lock = NULL;
    }
#ifdef CONFIG_BT_LE_WAKEUP_SOURCE_CPU_RTC_TIMER
    if (s_ble_sleep_timer != NULL) {
        esp_timer_stop(s_ble_sleep_timer);
        esp_timer_delete(s_ble_sleep_timer);
        s_ble_sleep_timer = NULL;
    }
#endif // CONFIG_BT_LE_WAKEUP_SOURCE_CPU_RTC_TIMER

#ifdef CONFIG_BT_LE_WAKEUP_SOURCE_BLE_RTC_TIMER
    esp_sleep_disable_bt_wakeup();
#endif // CONFIG_BT_LE_WAKEUP_SOURCE_BLE_RTC_TIMER

#endif //CONFIG_PM_ENABLE
    return rc;
}

void controller_sleep_deinit(void)
{
#ifdef CONFIG_PM_ENABLE
#ifdef CONFIG_BT_LE_WAKEUP_SOURCE_BLE_RTC_TIMER
    r_ble_rtc_wake_up_state_clr();
    esp_sleep_disable_bt_wakeup();
#endif //CONFIG_BT_LE_WAKEUP_SOURCE_BLE_RTC_TIMER
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_AUTO);

    /*lock should release first and then delete*/
    if (s_ble_active) {
        esp_pm_lock_release(s_pm_lock);
    }

    esp_pm_lock_delete(s_pm_lock);
    s_pm_lock = NULL;
#ifdef CONFIG_BT_LE_WAKEUP_SOURCE_CPU_RTC_TIMER
    if (s_ble_sleep_timer != NULL) {
        esp_timer_stop(s_ble_sleep_timer);
        esp_timer_delete(s_ble_sleep_timer);
        s_ble_sleep_timer = NULL;
    }
#endif //CONFIG_BT_LE_WAKEUP_SOURCE_CPU_RTC_TIMER
#endif //CONFIG_PM_ENABLE
}

void ble_rtc_clk_init(void)
{
    // modem_clkrst_reg
    // LP_TIMER_SEL_XTAL32K -> 0
    // LP_TIMER_SEL_XTAL -> 1
    // LP_TIMER_SEL_8M -> 0
    // LP_TIMER_SEL_RTC_SLOW -> 0
    SET_PERI_REG_BITS(MODEM_CLKRST_MODEM_LP_TIMER_CONF_REG, 1, 0, MODEM_CLKRST_LP_TIMER_SEL_XTAL32K_S);
    SET_PERI_REG_BITS(MODEM_CLKRST_MODEM_LP_TIMER_CONF_REG, 1, 1, MODEM_CLKRST_LP_TIMER_SEL_XTAL_S);
    SET_PERI_REG_BITS(MODEM_CLKRST_MODEM_LP_TIMER_CONF_REG, 1, 0, MODEM_CLKRST_LP_TIMER_SEL_8M_S);
    SET_PERI_REG_BITS(MODEM_CLKRST_MODEM_LP_TIMER_CONF_REG, 1, 0, MODEM_CLKRST_LP_TIMER_SEL_RTC_SLOW_S);

#ifdef CONFIG_XTAL_FREQ_26
    // LP_TIMER_CLK_DIV_NUM -> 130
    SET_PERI_REG_BITS(MODEM_CLKRST_MODEM_LP_TIMER_CONF_REG, MODEM_CLKRST_LP_TIMER_CLK_DIV_NUM, 129, MODEM_CLKRST_LP_TIMER_CLK_DIV_NUM_S);
#else
    // LP_TIMER_CLK_DIV_NUM -> 250
    SET_PERI_REG_BITS(MODEM_CLKRST_MODEM_LP_TIMER_CONF_REG, MODEM_CLKRST_LP_TIMER_CLK_DIV_NUM, 249, MODEM_CLKRST_LP_TIMER_CLK_DIV_NUM_S);
#endif // CONFIG_XTAL_FREQ_26

    // MODEM_CLKRST_ETM_CLK_ACTIVE -> 1
    // MODEM_CLKRST_ETM_CLK_SEL -> 0
    SET_PERI_REG_BITS(MODEM_CLKRST_ETM_CLK_CONF_REG, 1, 1, MODEM_CLKRST_ETM_CLK_ACTIVE_S);
    SET_PERI_REG_BITS(MODEM_CLKRST_ETM_CLK_CONF_REG, 1, 0, MODEM_CLKRST_ETM_CLK_SEL_S);

}

esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg)
{
    esp_err_t ret = ESP_OK;
    ble_npl_count_info_t npl_info;
    memset(&npl_info, 0, sizeof(ble_npl_count_info_t));
    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_ERR_INVALID_STATE;
    }

    if (!cfg) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "cfg is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    ble_rtc_clk_init();

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

    /* Initialize the global memory pool */
    ret = os_msys_buf_alloc();
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "os msys alloc failed");
        goto free_mem;
    }

    os_msys_init();
#if CONFIG_BT_NIMBLE_ENABLED
    // ble_npl_eventq_init() need to use npl function in rom and must be called after esp_bt_controller_init()
    /* Initialize default event queue */
    ble_npl_eventq_init(nimble_port_get_dflt_eventq());
#endif
    esp_phy_modem_init();
    periph_module_enable(PERIPH_BT_MODULE);

    // init phy
    esp_phy_enable();
    s_ble_active = true;

    // init bb
    bt_bb_v2_init_cmplx(1);

    if (ble_osi_coex_funcs_register((struct osi_coex_funcs_t *)&s_osi_coex_funcs_ro) != 0) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "osi coex funcs reg failed");
        ret = ESP_ERR_INVALID_ARG;
        goto free_controller;
    }

#if CONFIG_SW_COEXIST_ENABLE
    coex_init();
#endif
    ret = ble_controller_init(cfg);
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "ble_controller_init failed %d", ret);
        goto free_controller;
    }

    ret = controller_sleep_init();
    if (ret != ESP_OK) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "controller_sleep_init failed %d", ret);
        goto free_controller;
    }

    uint8_t mac[6];
    ESP_ERROR_CHECK(esp_read_mac((uint8_t *)mac, ESP_MAC_BT));

    swap_in_place(mac, 6);

    esp_ble_ll_set_public_addr(mac);

    ble_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

    ble_hci_trans_cfg_hs((ble_hci_trans_rx_cmd_fn *)ble_hci_unregistered_hook,NULL,
                         (ble_hci_trans_rx_acl_fn *)ble_hci_unregistered_hook,NULL);
    return ESP_OK;
free_controller:
    controller_sleep_deinit();
    ble_controller_deinit();
    esp_phy_disable();
    esp_phy_modem_deinit();
#if CONFIG_BT_NIMBLE_ENABLED
    ble_npl_eventq_deinit(nimble_port_get_dflt_eventq());
#endif // CONFIG_BT_NIMBLE_ENABLED
free_mem:
    os_msys_buf_free();
    npl_freertos_mempool_deinit();
    esp_unregister_npl_funcs();
    npl_freertos_funcs_deinit();
    esp_unregister_ext_funcs();
    return ret;
}

esp_err_t esp_bt_controller_deinit(void)
{
    if ((ble_controller_status < ESP_BT_CONTROLLER_STATUS_INITED) || (ble_controller_status >= ESP_BT_CONTROLLER_STATUS_ENABLED)) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_FAIL;
    }

    controller_sleep_deinit();

    if (s_ble_active) {
        esp_phy_disable();
        s_ble_active = false;
    }

    ble_controller_deinit();

#if CONFIG_BT_NIMBLE_ENABLED
    /* De-initialize default event queue */
    ble_npl_eventq_deinit(nimble_port_get_dflt_eventq());
#endif
    os_msys_buf_free();

    esp_unregister_npl_funcs();

    esp_unregister_ext_funcs();

    /* De-initialize npl functions */
    npl_freertos_funcs_deinit();

    npl_freertos_mempool_deinit();

    esp_phy_modem_deinit();

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
#if CONFIG_SW_COEXIST_ENABLE
    coex_enable();
#endif
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

esp_err_t esp_bt_mem_release(esp_bt_mode_t mode)
{
    intptr_t mem_start, mem_end;

    if (mode == ESP_BT_MODE_BLE) {
	mem_start = (intptr_t)&_bt_bss_start;
        mem_end = (intptr_t)&_bt_bss_end;
        if (mem_start != mem_end) {
            ESP_LOGD(NIMBLE_PORT_LOG_TAG, "Release BT BSS [0x%08x] - [0x%08x]", mem_start, mem_end);
            ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
        }

        mem_start = (intptr_t)&_bt_data_start;
        mem_end = (intptr_t)&_bt_data_end;
        if (mem_start != mem_end) {
            ESP_LOGD(NIMBLE_PORT_LOG_TAG, "Release BT Data [0x%08x] - [0x%08x]", mem_start, mem_end);
            ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
        }

        mem_start = (intptr_t)&_nimble_bss_start;
	mem_end = (intptr_t)&_nimble_bss_end;
	if (mem_start != mem_end) {
	    ESP_LOGD(NIMBLE_PORT_LOG_TAG, "Release NimBLE BSS [0x%08x] - [0x%08x]", mem_start, mem_end);
	    ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
	}

	mem_start = (intptr_t)&_nimble_data_start;
	mem_end = (intptr_t)&_nimble_data_end;
	if (mem_start != mem_end) {
	    ESP_LOGD(NIMBLE_PORT_LOG_TAG, "Release NimBLE Data [0x%08x] - [0x%08x]", mem_start, mem_end);
	    ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
	}
    }

    return ESP_OK;
}


esp_bt_controller_status_t esp_bt_controller_get_status(void)
{
    return ble_controller_status;
}

/* extra functions */
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

esp_err_t esp_ble_tx_power_set_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle, esp_power_level_t power_level)
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

esp_power_level_t esp_ble_tx_power_get_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle)
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


#if (!CONFIG_BT_NIMBLE_ENABLED) && (CONFIG_BT_CONTROLLER_ENABLED == true)

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
#endif

#else
#include "tinycrypt/aes.h"
#include "tinycrypt/constants.h"
#include "tinycrypt/utils.h"

#if CONFIG_BT_LE_SM_SC
#include "tinycrypt/cmac_mode.h"
#include "tinycrypt/ecc_dh.h"
#endif

#endif

#if CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS
#if CONFIG_BT_LE_SM_SC
static mbedtls_ecp_keypair keypair;
#endif
#endif

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
    if ((rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    NULL, 0)) != 0) {
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
#endif

    swap_buf(out_dhkey, dh, 32);
    return 0;
}

/* based on Core Specification 4.2 Vol 3. Part H 2.3.5.6.1 */
static const uint8_t ble_sm_alg_dbg_priv_key[32] = {
    0x3f, 0x49, 0xf6, 0xd4, 0xa3, 0xc5, 0x5f, 0x38, 0x74, 0xc9, 0xb3, 0xe3,
    0xd2, 0x10, 0x3f, 0x50, 0x4a, 0xff, 0x60, 0x7b, 0xeb, 0x40, 0xb7, 0x99,
    0x58, 0x99, 0xb8, 0xa6, 0xcd, 0x3c, 0x1a, 0xbd
};

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
#endif

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
#endif

        /* Make sure generated key isn't debug key. */
    } while (memcmp(priv, ble_sm_alg_dbg_priv_key, 32) == 0);

    swap_buf(pub, pk, 32);
    swap_buf(&pub[32], &pk[32], 32);
    swap_in_place(priv, 32);
#endif

    return 0;
}

#endif
