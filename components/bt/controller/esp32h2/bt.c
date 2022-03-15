/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <esp_random.h>
#include <esp_mac.h>

#include "sdkconfig.h"

#include "os/os.h"
#include "sysinit/sysinit.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

#ifdef ESP_PLATFORM
#include "esp_log.h"
#endif

#if CONFIG_SW_COEXIST_ENABLE
#include "esp_coexist_internal.h"
#endif

#ifdef CONFIG_BT_NIMBLE_CONTROL_USE_UART_HCI
#include "transport/uart/ble_hci_uart.h"
#else
#include "transport/ram/ble_hci_ram.h"
#endif
#include "nimble/ble_hci_trans.h"

#include "nimble/nimble_npl_os.h"
#include "esp_bt.h"
#include "esp_intr_alloc.h"
#include "esp_sleep.h"
#include "esp_pm.h"
#include "esp_phy_init.h"
#include "soc/system_reg.h"
#include "hal/hal_uart.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "host/ble_hs.h"

#include "esp_private/periph_ctrl.h"

/* Macro definition
 ************************************************************************
 */

#define NIMBLE_PORT_LOG_TAG          "BLE_INIT"
#define OSI_COEX_VERSION              0x00010006
#define OSI_COEX_MAGIC_VALUE          0xFADEBEAD

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
    int (*_hal_uart_init_cbs)(int, hal_uart_tx_char, hal_uart_tx_done, hal_uart_rx_char, void *);
    int (*_hal_uart_config)(int, int32_t, uint8_t, uint8_t, enum hal_uart_parity, enum hal_uart_flow_ctl);
    int (*_hal_uart_close)(int);
    void (*_hal_uart_blocking_tx)(int, uint8_t);
    int (*_hal_uart_init)(int, void *);
    int (* _task_create)(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id);
    void (* _task_delete)(void *task_handle);
    void (*_osi_assert)(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2);
    uint32_t (* _os_random)(void);
    uint32_t magic;
};

struct ble_ll_trace_func_t {
    void (*_ble_ll_trace_u32_func)(uint32_t id, uint32_t p1);
    void (*_ble_ll_trace_u32x2_func)(uint32_t id, uint32_t p1, uint32_t p2);
    void (*_ble_ll_trace_u32x3_func)(uint32_t id, uint32_t p1, uint32_t p2, uint32_t p3);
};

/* External functions or variables
 ************************************************************************
 */

extern int ble_plf_set_log_level(int level);
extern int ble_osi_coex_funcs_register(struct osi_coex_funcs_t *coex_funcs);
extern int coex_core_ble_conn_dyn_prio_get(bool *low, bool *high);
extern int ble_controller_init(struct esp_bt_controller_config_t *cfg);
extern int ble_controller_deinit(void);
extern int ble_controller_enable(uint8_t mode);
extern int ble_controller_disable(void);
extern int esp_register_ext_funcs (struct ext_funcs_t *);
extern void esp_unregister_ext_funcs (void);
extern int esp_ble_ll_set_public_addr(const uint8_t *addr);
extern int esp_register_npl_funcs (struct npl_funcs_t *p_npl_func);
extern void esp_unregister_npl_funcs (void);
extern void npl_freertos_mempool_deinit(void);
extern void coex_pti_v2(void);
extern void bt_bb_v2_init_cmplx(uint8_t i);
extern int os_msys_buf_alloc(void);
extern uint32_t r_os_cputime_get32(void);
extern uint32_t r_os_cputime_ticks_to_usecs(uint32_t ticks);
extern void r_ble_ll_rfmgmt_set_sleep_cb(void *s_cb, void *w_cb, void *s_arg, void *w_arg, uint32_t us_to_enabled);
extern int os_msys_init(void);
extern void os_msys_buf_free(void);
extern void esp_ble_register_trace_funcs(struct ble_ll_trace_func_t *funcs);
extern void bt_bb_set_le_tx_on_delay(uint32_t delay_us);

/* Local Function Declaration
 *********************************************************************
 */
static void coex_schm_status_bit_set_wrapper(uint32_t type, uint32_t status);
static void coex_schm_status_bit_clear_wrapper(uint32_t type, uint32_t status);
static int task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id);
static void task_delete_wrapper(void *task_handle);
static void hal_uart_start_tx_wrapper(int uart_no);
static int hal_uart_init_cbs_wrapper(int uart_no, hal_uart_tx_char tx_func,
                                     hal_uart_tx_done tx_done, hal_uart_rx_char rx_func, void *arg);
static int hal_uart_config_wrapper(int uart_no, int32_t speed, uint8_t databits, uint8_t stopbits,
                                   enum hal_uart_parity parity, enum hal_uart_flow_ctl flow_ctl);
static int hal_uart_close_wrapper(int uart_no);
static void hal_uart_blocking_tx_wrapper(int port, uint8_t data);
static int hal_uart_init_wrapper(int uart_no, void *cfg);
static int esp_intr_alloc_wrapper(int source, int flags, intr_handler_t handler, void *arg, void **ret_handle_in);
static int esp_intr_free_wrapper(void **ret_handle);
static void osi_assert_wrapper(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2);
static uint32_t osi_random_wrapper(void);

/* Local variable definition
 ***************************************************************************
 */

/* Static variable declare */
static DRAM_ATTR esp_bt_controller_status_t ble_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;
static bool s_is_sleep_state = false;

#ifdef CONFIG_PM_ENABLE
#ifdef CONFIG_BT_NIMBLE_WAKEUP_SOURCE_CPU_RTC_TIMER
uint32_t s_sleep_tick;
#endif
#endif

#ifdef CONFIG_PM_ENABLE
static DRAM_ATTR esp_timer_handle_t s_btdm_slp_tmr = NULL;
static DRAM_ATTR esp_pm_lock_handle_t s_pm_lock = NULL;
static bool s_pm_lock_acquired = true;
static DRAM_ATTR bool s_btdm_allow_light_sleep;
// pm_lock to prevent light sleep when using main crystal as Bluetooth low power clock
static DRAM_ATTR esp_pm_lock_handle_t s_light_sleep_pm_lock;
static void btdm_slp_tmr_callback(void *arg);
#define BTDM_MIN_TIMER_UNCERTAINTY_US      (200)
#endif /* #ifdef CONFIG_PM_ENABLE */

#ifdef CONFIG_BT_NIMBLE_WAKEUP_SOURCE_BLE_RTC_TIMER
#define BLE_RTC_DELAY_US                    (1100)
#else
#define BLE_RTC_DELAY_US                    (0)
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
    .ext_version = 0xE0000001,
    ._esp_intr_alloc = esp_intr_alloc_wrapper,
    ._esp_intr_free = esp_intr_free_wrapper,
    ._malloc = malloc,
    ._free = free,
    ._hal_uart_start_tx     =  hal_uart_start_tx_wrapper,
    ._hal_uart_init_cbs     =  hal_uart_init_cbs_wrapper,
    ._hal_uart_config       =  hal_uart_config_wrapper,
    ._hal_uart_close        =  hal_uart_close_wrapper,
    ._hal_uart_blocking_tx  =  hal_uart_blocking_tx_wrapper,
    ._hal_uart_init         =  hal_uart_init_wrapper,
    ._task_create = task_create_wrapper,
    ._task_delete = task_delete_wrapper,
    ._osi_assert = osi_assert_wrapper,
    ._os_random = osi_random_wrapper,
    .magic = 0xA5A5A5A5,
};

static void IRAM_ATTR osi_assert_wrapper(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2)
{
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

static int task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id)
{
    return (uint32_t)xTaskCreatePinnedToCore(task_func, name, stack_depth, param, prio, task_handle, (core_id < portNUM_PROCESSORS ? core_id : tskNO_AFFINITY));
}

static void task_delete_wrapper(void *task_handle)
{
    vTaskDelete(task_handle);
}

static void hal_uart_start_tx_wrapper(int uart_no)
{
#ifdef CONFIG_BT_NIMBLE_CONTROL_USE_UART_HCI
    hal_uart_start_tx(uart_no);
#endif
}

static int hal_uart_init_cbs_wrapper(int uart_no, hal_uart_tx_char tx_func,
                                     hal_uart_tx_done tx_done, hal_uart_rx_char rx_func, void *arg)
{
    int rc = -1;
#ifdef CONFIG_BT_NIMBLE_CONTROL_USE_UART_HCI
    rc = hal_uart_init_cbs(uart_no, tx_func, tx_done, rx_func, arg);
#endif
    return rc;
}

static int hal_uart_config_wrapper(int uart_no, int32_t speed, uint8_t databits, uint8_t stopbits,
                                   enum hal_uart_parity parity, enum hal_uart_flow_ctl flow_ctl)
{
    int rc = -1;
#ifdef CONFIG_BT_NIMBLE_CONTROL_USE_UART_HCI
    rc = hal_uart_config(uart_no, speed, databits, stopbits, parity, flow_ctl);
#endif
    return rc;
}

static int hal_uart_close_wrapper(int uart_no)
{
    int rc = -1;
#ifdef CONFIG_BT_NIMBLE_CONTROL_USE_UART_HCI
    rc = hal_uart_close(uart_no);
#endif
    return rc;
}

static void hal_uart_blocking_tx_wrapper(int port, uint8_t data)
{
#ifdef CONFIG_BT_NIMBLE_CONTROL_USE_UART_HCI
    hal_uart_blocking_tx(port, data);
#endif
}

static int hal_uart_init_wrapper(int uart_no, void *cfg)
{
    int rc = -1;
#ifdef CONFIG_BT_NIMBLE_CONTROL_USE_UART_HCI
    rc = hal_uart_init(uart_no, cfg);
#endif
    return rc;
}

static int esp_intr_alloc_wrapper(int source, int flags, intr_handler_t handler, void *arg, void **ret_handle_in)
{
    int rc = esp_intr_alloc(source, flags, handler, arg, (intr_handle_t *)ret_handle_in);
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
    if (s_is_sleep_state) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "sleep state error");
        assert(0);
    }
    s_is_sleep_state = true;
#ifdef CONFIG_PM_ENABLE
#ifdef CONFIG_BT_NIMBLE_WAKEUP_SOURCE_CPU_RTC_TIMER
    uint32_t tick_invalid = *(uint32_t *)(arg);
    if (!tick_invalid) {
        s_sleep_tick = r_os_cputime_get32();
        assert(enable_tick >= s_sleep_tick);
        // start a timer to wake up and acquire the pm_lock before modem_sleep awakes
        uint32_t us_to_sleep = os_cputime_ticks_to_usecs(enable_tick - s_sleep_tick);
        assert(us_to_sleep > BTDM_MIN_TIMER_UNCERTAINTY_US);
        if (esp_timer_start_once(s_btdm_slp_tmr, us_to_sleep - BTDM_MIN_TIMER_UNCERTAINTY_US) != ESP_OK) {
            ESP_LOGW(NIMBLE_PORT_LOG_TAG, "timer start failed");
        }
    }
#endif // CONFIG_NIMBLE_WAKEUP_SOURCE_CPU_RTC_TIMER
    if (s_pm_lock_acquired) {
        esp_pm_lock_release(s_pm_lock);
        s_pm_lock_acquired = false;
    }

#endif // CONFIG_PM_ENABLE
}

IRAM_ATTR void controller_wakeup_cb(void *arg)
{
    if (!s_is_sleep_state) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "wake up state error");
        assert(0);
    }
    s_is_sleep_state = false;

// need to check if need to call pm lock here
#ifdef CONFIG_PM_ENABLE
    if (!s_pm_lock_acquired) {
        s_pm_lock_acquired = true;
        esp_pm_lock_acquire(s_pm_lock);
    }
#ifdef CONFIG_BT_NIMBLE_WAKEUP_SOURCE_CPU_RTC_TIMER
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TIMER) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "wake up source %d", esp_sleep_get_wakeup_cause());
    }
#endif
#ifdef CONFIG_BT_NIMBLE_WAKEUP_SOURCE_BLE_RTC_TIMER
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_BT) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "wake up source %d", esp_sleep_get_wakeup_cause());
    }
#endif
#endif
}

#ifdef CONFIG_PM_ENABLE
#ifdef CONFIG_BT_NIMBLE_WAKEUP_SOURCE_CPU_RTC_TIMER
static void btdm_slp_tmr_callback(void *arg)
{
    (void)(arg);
    if (!s_pm_lock_acquired) {
        s_pm_lock_acquired = true;
        esp_pm_lock_acquire(s_pm_lock);
    }
}

#endif
#endif // CONFIG_PM_ENABLE

void controller_sleep_init(void)
{

#ifdef CONFIG_NIMBLE_SLEEP_ENABLE
    s_is_sleep_state = false;
#ifdef CONFIG_PM_ENABLE
    s_btdm_allow_light_sleep = true;
#endif // CONFIG_PM_ENABLE
    ESP_LOGW(NIMBLE_PORT_LOG_TAG, "BLE modem sleep is enabled");
    // register sleep callbacks
    r_ble_ll_rfmgmt_set_sleep_cb(controller_sleep_cb, controller_wakeup_cb, 0, 0, 500 + BLE_RTC_DELAY_US);
#else
#ifdef CONFIG_PM_ENABLE
    s_btdm_allow_light_sleep = false;
#endif // CONFIG_PM_ENABLE
#endif // CONFIG_NIMBLE_SLEEP_ENABLE

    // enable light sleep
#ifdef CONFIG_PM_ENABLE
    if (!s_btdm_allow_light_sleep) {
        if (esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "btnosleep", &s_light_sleep_pm_lock) != ESP_OK) {
            goto error;
        }
    }
    if (esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "bt", &s_pm_lock) != ESP_OK) {
        goto error;
    }
#ifdef CONFIG_BT_NIMBLE_WAKEUP_SOURCE_CPU_RTC_TIMER
    esp_timer_create_args_t create_args = {
        .callback = btdm_slp_tmr_callback,
        .arg = NULL,
        .name = "btSlp"
    };
    if ( esp_timer_create(&create_args, &s_btdm_slp_tmr) != ESP_OK) {
        goto error;
    }
    ESP_LOGW(NIMBLE_PORT_LOG_TAG, "light sleep enable success, CPU RTC timer wake up");
#endif //CONFIG_NIMBLE_WAKEUP_SOURCE_CPU_RTC_TIMER

#ifdef CONFIG_BT_NIMBLE_WAKEUP_SOURCE_BLE_RTC_TIMER
    esp_sleep_enable_bt_wakeup();
    ESP_LOGW(NIMBLE_PORT_LOG_TAG, "light sleep enable success, BLE RTC timer wake up");
#endif // CONFIG_NIMBLE_WAKEUP_SOURCE_BLE_RTC_TIMER

    s_pm_lock_acquired = true;

    if (!s_btdm_allow_light_sleep) {
        esp_pm_lock_acquire(s_light_sleep_pm_lock);
    }
    if (s_pm_lock) {
        esp_pm_lock_acquire(s_pm_lock);
    }

    return;

error:
    if (!s_btdm_allow_light_sleep) {
        if (s_light_sleep_pm_lock != NULL) {
            esp_pm_lock_delete(s_light_sleep_pm_lock);
            s_light_sleep_pm_lock = NULL;
        }
    }
    if (s_pm_lock != NULL) {
        esp_pm_lock_delete(s_pm_lock);
        s_pm_lock = NULL;
    }
#ifdef CONFIG_BT_NIMBLE_WAKEUP_SOURCE_CPU_RTC_TIMER
    if (s_btdm_slp_tmr != NULL) {
        esp_timer_delete(s_btdm_slp_tmr);
        s_btdm_slp_tmr = NULL;
    }
#endif // CONFIG_NIMBLE_WAKEUP_SOURCE_CPU_RTC_TIMER
#ifdef CONFIG_BT_NIMBLE_WAKEUP_SOURCE_BLE_RTC_TIMER
    esp_sleep_disable_bt_wakeup();
#endif // CONFIG_NIMBLE_WAKEUP_SOURCE_BLE_RTC_TIMER
#endif

}

void controller_sleep_deinit(void)
{
#ifdef CONFIG_PM_ENABLE
    if (!s_btdm_allow_light_sleep) {
        if (s_light_sleep_pm_lock != NULL) {
            esp_pm_lock_delete(s_light_sleep_pm_lock);
            s_light_sleep_pm_lock = NULL;
        }
    }
    if (s_pm_lock != NULL) {
        esp_pm_lock_delete(s_pm_lock);
        s_pm_lock = NULL;
    }
#ifdef CONFIG_BT_NIMBLE_WAKEUP_SOURCE_CPU_RTC_TIMER
    if (s_btdm_slp_tmr != NULL) {
        esp_timer_stop(s_btdm_slp_tmr);
        esp_timer_delete(s_btdm_slp_tmr);
        s_btdm_slp_tmr = NULL;
    }
#endif
    s_pm_lock_acquired = false;
#endif

}

esp_err_t esp_bt_controller_init(struct esp_bt_controller_config_t *cfg)
{
    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_FAIL;
    }

    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (esp_register_ext_funcs(&ext_funcs_ro) != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Initialize the function pointers for OS porting */
    npl_freertos_funcs_init();

    struct npl_funcs_t *p_npl_funcs = npl_freertos_funcs_get();
    if (!p_npl_funcs) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "npl functions get failed");
        return ESP_ERR_INVALID_ARG;
    }

    if (esp_register_npl_funcs(p_npl_funcs) != 0) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "npl functions register failed");
        return ESP_ERR_INVALID_ARG;
    }

    if (npl_freertos_mempool_init() != 0) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "npl mempool init failed");
        return ESP_ERR_INVALID_ARG;
    }

    /* Initialize the global memory pool */
    if (os_msys_buf_alloc() != 0) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "os msys alloc failed");
        return ESP_ERR_INVALID_ARG;
    }

    os_msys_init();

    // ble_npl_eventq_init() need to use npl function in rom and must be called after esp_bt_controller_init()
    /* Initialize default event queue */
    ble_npl_eventq_init(nimble_port_get_dflt_eventq());

    periph_module_enable(PERIPH_BT_MODULE);

    // init phy
    esp_phy_enable();

    // set bb delay
    bt_bb_set_le_tx_on_delay(50);

    if (ble_osi_coex_funcs_register((struct osi_coex_funcs_t *)&s_osi_coex_funcs_ro) != 0) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "osi coex funcs reg failed");
        return ESP_ERR_INVALID_ARG;
    }

#if CONFIG_SW_COEXIST_ENABLE
    coex_enable();
#endif

    if (ble_controller_init(cfg) != 0) {
        return ESP_ERR_NO_MEM;
    }

    controller_sleep_init();

    uint8_t mac[6];
    ESP_ERROR_CHECK(esp_read_mac((uint8_t *)mac, ESP_MAC_BT));

    swap_in_place(mac, 6);

    esp_ble_ll_set_public_addr(mac);

    ble_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

    return ESP_OK;
}

esp_err_t esp_bt_controller_deinit(void)
{
    if ((ble_controller_status < ESP_BT_CONTROLLER_STATUS_INITED) || (ble_controller_status >= ESP_BT_CONTROLLER_STATUS_ENABLED)) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_FAIL;
    }

    controller_sleep_deinit();

    if (ble_controller_deinit() != 0) {
        return ESP_FAIL;
    }

    /* De-initialize default event queue */
    ble_npl_eventq_deinit(nimble_port_get_dflt_eventq());

    os_msys_buf_free();

    esp_unregister_npl_funcs();

    esp_unregister_ext_funcs();

    /* De-initialize npl functions */
    npl_freertos_funcs_deinit();

    npl_freertos_mempool_deinit();

    esp_phy_disable();

    ble_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

    return ESP_OK;
}

esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode)
{
    if (mode != ESP_BT_MODE_BLE) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller mode");
        return ESP_FAIL;
    }
    if (ble_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        ESP_LOGW(NIMBLE_PORT_LOG_TAG, "invalid controller state");
        return ESP_FAIL;
    }
    if (ble_controller_enable(mode) != 0) {
        return ESP_FAIL;
    }

    ble_controller_status = ESP_BT_CONTROLLER_STATUS_ENABLED;
    return ESP_OK;
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
    return ESP_OK;
}

esp_err_t esp_bt_mem_release(esp_bt_mode_t mode)
{
    ESP_LOGW(NIMBLE_PORT_LOG_TAG, "%s not implemented, return OK", __func__);
    return ESP_OK;
}
