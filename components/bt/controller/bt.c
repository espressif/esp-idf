// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "esp_heap_caps_init.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/portmacro.h"
#include "xtensa/core-macros.h"
#include "esp_types.h"
#include "esp_system.h"
#include "esp_task.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "esp_phy_init.h"
#include "esp_bt.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_ipc.h"
#include "driver/periph_ctrl.h"
#include "soc/rtc.h"
#include "soc/soc_memory_layout.h"
#include "esp32/clk.h"
#include "esp_coexist_internal.h"


#if CONFIG_BT_ENABLED

/* Macro definition
 ************************************************************************
 */

#define BTDM_LOG_TAG                        "BTDM_INIT"

#define BTDM_INIT_PERIOD                    (5000)    /* ms */

/* Bluetooth system and controller config */
#define BTDM_CFG_BT_DATA_RELEASE            (1<<0)
#define BTDM_CFG_HCI_UART                   (1<<1)
#define BTDM_CFG_CONTROLLER_RUN_APP_CPU     (1<<2)
#define BTDM_CFG_SCAN_DUPLICATE_OPTIONS     (1<<3)
#define BTDM_CFG_SEND_ADV_RESERVED_SIZE     (1<<4)
#define BTDM_CFG_BLE_FULL_SCAN_SUPPORTED    (1<<5)

/* Sleep mode */
#define BTDM_MODEM_SLEEP_MODE_NONE          (0)
#define BTDM_MODEM_SLEEP_MODE_ORIG          (1)
#define BTDM_MODEM_SLEEP_MODE_EVED          (2)  // sleep mode for BLE controller, used only for internal test.

/* Low Power Clock Selection */
#define BTDM_LPCLK_SEL_XTAL      (0)
#define BTDM_LPCLK_SEL_XTAL32K   (1)
#define BTDM_LPCLK_SEL_RTC_SLOW  (2)
#define BTDM_LPCLK_SEL_8M        (3)

/* Sleep and wakeup interval control */
#define BTDM_MIN_SLEEP_DURATION          (12) // threshold of interval in slots to allow to fall into modem sleep
#define BTDM_MODEM_WAKE_UP_DELAY         (4)  // delay in slots of modem wake up procedure, including re-enable PHY/RF

#ifdef CONFIG_PM_ENABLE
#ifndef CONFIG_BTDM_LPCLK_SEL_MAIN_XTAL
#define BTDM_ALLOW_LIGHT_SLEEP 1
#else
#define BTDM_ALLOW_LIGHT_SLEEP 0
#endif
#endif

#define BT_DEBUG(...)
#define BT_API_CALL_CHECK(info, api_call, ret) \
do{\
    esp_err_t __err = (api_call);\
    if ((ret) != __err) {\
        BT_DEBUG("%s %d %s ret=0x%X\n", __FUNCTION__, __LINE__, (info), __err);\
        return __err;\
    }\
} while(0)

#define OSI_FUNCS_TIME_BLOCKING  0xffffffff
#define OSI_VERSION              0x00010001
#define OSI_MAGIC_VALUE          0xFADEBEAD

/* SPIRAM Configuration */
#if CONFIG_SPIRAM_USE_MALLOC
#define BTDM_MAX_QUEUE_NUM       (5)
#endif

/* Types definition
 ************************************************************************
 */

/* VHCI function interface */
typedef struct vhci_host_callback {
    void (*notify_host_send_available)(void);               /*!< callback used to notify that the host can send packet to controller */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   /*!< callback used to notify that the controller has a packet to send to the host*/
} vhci_host_callback_t;

/* Dram region */
typedef struct {
    esp_bt_mode_t mode;
    intptr_t start;
    intptr_t end;
} btdm_dram_available_region_t;

/* PSRAM configuration */
#if CONFIG_SPIRAM_USE_MALLOC
typedef struct {
    QueueHandle_t handle;
    void *storage;
    void *buffer;
} btdm_queue_item_t;
#endif

/* OSI function */
struct osi_funcs_t {
    uint32_t _version;
    xt_handler (*_set_isr)(int n, xt_handler f, void *arg);
    void (*_ints_on)(unsigned int mask);
    void (*_interrupt_disable)(void);
    void (*_interrupt_restore)(void);
    void (*_task_yield)(void);
    void (*_task_yield_from_isr)(void);
    void *(*_semphr_create)(uint32_t max, uint32_t init);
    void (*_semphr_delete)(void *semphr);
    int32_t (*_semphr_take_from_isr)(void *semphr, void *hptw);
    int32_t (*_semphr_give_from_isr)(void *semphr, void *hptw);
    int32_t (*_semphr_take)(void *semphr, uint32_t block_time_ms);
    int32_t (*_semphr_give)(void *semphr);
    void *(*_mutex_create)(void);
    void (*_mutex_delete)(void *mutex);
    int32_t (*_mutex_lock)(void *mutex);
    int32_t (*_mutex_unlock)(void *mutex);
    void *(* _queue_create)(uint32_t queue_len, uint32_t item_size);
    void (* _queue_delete)(void *queue);
    int32_t (* _queue_send)(void *queue, void *item, uint32_t block_time_ms);
    int32_t (* _queue_send_from_isr)(void *queue, void *item, void *hptw);
    int32_t (* _queue_recv)(void *queue, void *item, uint32_t block_time_ms);
    int32_t (* _queue_recv_from_isr)(void *queue, void *item, void *hptw);
    int32_t (* _task_create)(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id);
    void (* _task_delete)(void *task_handle);
    bool (* _is_in_isr)(void);
    int (* _cause_sw_intr_to_core)(int core_id, int intr_no);
    void *(* _malloc)(uint32_t size);
    void *(* _malloc_internal)(uint32_t size);
    void (* _free)(void *p);
    int32_t (* _read_efuse_mac)(uint8_t mac[6]);
    void (* _srand)(unsigned int seed);
    int (* _rand)(void);
    uint32_t (* _btdm_lpcycles_2_us)(uint32_t cycles);
    uint32_t (* _btdm_us_2_lpcycles)(uint32_t us);
    bool (* _btdm_sleep_check_duration)(uint32_t *slot_cnt);
    void (* _btdm_sleep_enter_phase1)(uint32_t lpcycles);  /* called when interrupt is disabled */
    void (* _btdm_sleep_enter_phase2)(void);
    void (* _btdm_sleep_exit_phase1)(void);  /* called from ISR */
    void (* _btdm_sleep_exit_phase2)(void);  /* called from ISR */
    void (* _btdm_sleep_exit_phase3)(void);  /* called from task */
    int (* _coex_bt_request)(uint32_t event, uint32_t latency, uint32_t duration);
    int (* _coex_bt_release)(uint32_t event);
    int (* _coex_register_bt_cb)(coex_func_cb_t cb);
    uint32_t (* _coex_bb_reset_lock)(void);
    void (* _coex_bb_reset_unlock)(uint32_t restore);
    uint32_t _magic;
};


/* External functions or values
 ************************************************************************
 */

/* not for user call, so don't put to include file */
/* OSI */
extern int btdm_osi_funcs_register(void *osi_funcs);
/* Initialise and De-initialise */
extern int btdm_controller_init(uint32_t config_mask, esp_bt_controller_config_t *config_opts);
extern void btdm_controller_deinit(void);
extern int btdm_controller_enable(esp_bt_mode_t mode);
extern void btdm_controller_disable(void);
extern uint8_t btdm_controller_get_mode(void);
extern const char *btdm_controller_get_compile_version(void);
extern void btdm_rf_bb_init_phase2(void); // shall be called after PHY/RF is enabled
/* Sleep */
extern void btdm_controller_enable_sleep(bool enable);
extern void btdm_controller_set_sleep_mode(uint8_t mode);
extern uint8_t btdm_controller_get_sleep_mode(void);
extern bool btdm_power_state_active(void);
extern void btdm_wakeup_request(bool request_lock);
extern void btdm_wakeup_request_end(void);
/* Low Power Clock */
extern bool btdm_lpclk_select_src(uint32_t sel);
extern bool btdm_lpclk_set_div(uint32_t div);
/* VHCI */
extern bool API_vhci_host_check_send_available(void);
extern void API_vhci_host_send_packet(uint8_t *data, uint16_t len);
extern int API_vhci_host_register_callback(const vhci_host_callback_t *callback);
/* TX power */
extern int ble_txpwr_set(int power_type, int power_level);
extern int ble_txpwr_get(int power_type);
extern int bredr_txpwr_set(int min_power_level, int max_power_level);
extern int bredr_txpwr_get(int *min_power_level, int *max_power_level);
extern void bredr_sco_datapath_set(uint8_t data_path);
extern void btdm_controller_scan_duplicate_list_clear(void);
/* Coexistence */
extern int coex_bt_request_wrapper(uint32_t event, uint32_t latency, uint32_t duration);
extern int coex_bt_release_wrapper(uint32_t event);
extern int coex_register_bt_cb_wrapper(coex_func_cb_t cb);
extern uint32_t coex_bb_reset_lock_wrapper(void);
extern void coex_bb_reset_unlock_wrapper(uint32_t restore);
extern void coex_ble_adv_priority_high_set(bool high);

extern char _bss_start_btdm;
extern char _bss_end_btdm;
extern char _data_start_btdm;
extern char _data_end_btdm;
extern uint32_t _data_start_btdm_rom;
extern uint32_t _data_end_btdm_rom;

extern uint32_t _bt_bss_start;
extern uint32_t _bt_bss_end;
extern uint32_t _nimble_bss_start;
extern uint32_t _nimble_bss_end;
extern uint32_t _btdm_bss_start;
extern uint32_t _btdm_bss_end;
extern uint32_t _bt_data_start;
extern uint32_t _bt_data_end;
extern uint32_t _nimble_data_start;
extern uint32_t _nimble_data_end;
extern uint32_t _btdm_data_start;
extern uint32_t _btdm_data_end;

/* Local Function Declare
 *********************************************************************
 */
#if CONFIG_SPIRAM_USE_MALLOC
static bool btdm_queue_generic_register(const btdm_queue_item_t *queue);
static bool btdm_queue_generic_deregister(btdm_queue_item_t *queue);
#endif /* CONFIG_SPIRAM_USE_MALLOC */
static void IRAM_ATTR interrupt_disable(void);
static void IRAM_ATTR interrupt_restore(void);
static void IRAM_ATTR task_yield_from_isr(void);
static void *semphr_create_wrapper(uint32_t max, uint32_t init);
static void semphr_delete_wrapper(void *semphr);
static int32_t IRAM_ATTR semphr_take_from_isr_wrapper(void *semphr, void *hptw);
static int32_t IRAM_ATTR semphr_give_from_isr_wrapper(void *semphr, void *hptw);
static int32_t  semphr_take_wrapper(void *semphr, uint32_t block_time_ms);
static int32_t  semphr_give_wrapper(void *semphr);
static void *mutex_create_wrapper(void);
static void mutex_delete_wrapper(void *mutex);
static int32_t mutex_lock_wrapper(void *mutex);
static int32_t mutex_unlock_wrapper(void *mutex);
static void *queue_create_wrapper(uint32_t queue_len, uint32_t item_size);
static void queue_delete_wrapper(void *queue);
static int32_t queue_send_wrapper(void *queue, void *item, uint32_t block_time_ms);
static int32_t IRAM_ATTR queue_send_from_isr_wrapper(void *queue, void *item, void *hptw);
static int32_t queue_recv_wrapper(void *queue, void *item, uint32_t block_time_ms);
static int32_t IRAM_ATTR queue_recv_from_isr_wrapper(void *queue, void *item, void *hptw);
static int32_t task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id);
static void task_delete_wrapper(void *task_handle);
static bool IRAM_ATTR is_in_isr_wrapper(void);
static void IRAM_ATTR cause_sw_intr(void *arg);
static int IRAM_ATTR cause_sw_intr_to_core_wrapper(int core_id, int intr_no);
static void *malloc_internal_wrapper(size_t size);
static int32_t IRAM_ATTR read_mac_wrapper(uint8_t mac[6]);
static void IRAM_ATTR srand_wrapper(unsigned int seed);
static int IRAM_ATTR rand_wrapper(void);
static uint32_t IRAM_ATTR btdm_lpcycles_2_us(uint32_t cycles);
static uint32_t IRAM_ATTR btdm_us_2_lpcycles(uint32_t us);
static bool IRAM_ATTR btdm_sleep_check_duration(uint32_t *slot_cnt);
static void btdm_sleep_enter_phase1_wrapper(uint32_t lpcycles);
static void btdm_sleep_enter_phase2_wrapper(void);
static void IRAM_ATTR btdm_sleep_exit_phase1_wrapper(void);
static void btdm_sleep_exit_phase3_wrapper(void);

/* Local variable definition
 ***************************************************************************
 */
/* OSI funcs */
static const struct osi_funcs_t osi_funcs_ro = {
    ._version = OSI_VERSION,
    ._set_isr = xt_set_interrupt_handler,
    ._ints_on = xt_ints_on,
    ._interrupt_disable = interrupt_disable,
    ._interrupt_restore = interrupt_restore,
    ._task_yield = vPortYield,
    ._task_yield_from_isr = task_yield_from_isr,
    ._semphr_create = semphr_create_wrapper,
    ._semphr_delete = semphr_delete_wrapper,
    ._semphr_take_from_isr = semphr_take_from_isr_wrapper,
    ._semphr_give_from_isr = semphr_give_from_isr_wrapper,
    ._semphr_take = semphr_take_wrapper,
    ._semphr_give = semphr_give_wrapper,
    ._mutex_create = mutex_create_wrapper,
    ._mutex_delete = mutex_delete_wrapper,
    ._mutex_lock = mutex_lock_wrapper,
    ._mutex_unlock = mutex_unlock_wrapper,
    ._queue_create = queue_create_wrapper,
    ._queue_delete = queue_delete_wrapper,
    ._queue_send = queue_send_wrapper,
    ._queue_send_from_isr = queue_send_from_isr_wrapper,
    ._queue_recv = queue_recv_wrapper,
    ._queue_recv_from_isr = queue_recv_from_isr_wrapper,
    ._task_create = task_create_wrapper,
    ._task_delete = task_delete_wrapper,
    ._is_in_isr = is_in_isr_wrapper,
    ._cause_sw_intr_to_core = cause_sw_intr_to_core_wrapper,
    ._malloc = malloc,
    ._malloc_internal = malloc_internal_wrapper,
    ._free = free,
    ._read_efuse_mac = read_mac_wrapper,
    ._srand = srand_wrapper,
    ._rand = rand_wrapper,
    ._btdm_lpcycles_2_us = btdm_lpcycles_2_us,
    ._btdm_us_2_lpcycles = btdm_us_2_lpcycles,
    ._btdm_sleep_check_duration = btdm_sleep_check_duration,
    ._btdm_sleep_enter_phase1 = btdm_sleep_enter_phase1_wrapper,
    ._btdm_sleep_enter_phase2 = btdm_sleep_enter_phase2_wrapper,
    ._btdm_sleep_exit_phase1 = btdm_sleep_exit_phase1_wrapper,
    ._btdm_sleep_exit_phase2 = NULL,
    ._btdm_sleep_exit_phase3 = btdm_sleep_exit_phase3_wrapper,
    ._coex_bt_request = coex_bt_request_wrapper,
    ._coex_bt_release = coex_bt_release_wrapper,
    ._coex_register_bt_cb = coex_register_bt_cb_wrapper,
    ._coex_bb_reset_lock = coex_bb_reset_lock_wrapper,
    ._coex_bb_reset_unlock = coex_bb_reset_unlock_wrapper,
    ._magic = OSI_MAGIC_VALUE,
};

/* the mode column will be modified by release function to indicate the available region */
static btdm_dram_available_region_t btdm_dram_available_region[] = {
    //following is .data
    {ESP_BT_MODE_BTDM,          SOC_MEM_BT_DATA_START,      SOC_MEM_BT_DATA_END         },
    //following is memory which HW will use
    {ESP_BT_MODE_BTDM,          SOC_MEM_BT_EM_BTDM0_START,  SOC_MEM_BT_EM_BTDM0_END     },
    {ESP_BT_MODE_BLE,           SOC_MEM_BT_EM_BLE_START,    SOC_MEM_BT_EM_BLE_END      },
    {ESP_BT_MODE_BTDM,          SOC_MEM_BT_EM_BTDM1_START,  SOC_MEM_BT_EM_BTDM1_END     },
    {ESP_BT_MODE_CLASSIC_BT,    SOC_MEM_BT_EM_BREDR_START,  SOC_MEM_BT_EM_BREDR_REAL_END},
    //following is .bss
    {ESP_BT_MODE_BTDM,          SOC_MEM_BT_BSS_START,       SOC_MEM_BT_BSS_END          },
    {ESP_BT_MODE_BTDM,          SOC_MEM_BT_MISC_START,      SOC_MEM_BT_MISC_END         },
};

/* Reserve the full memory region used by Bluetooth Controller,
 *    some may be released later at runtime. */
SOC_RESERVE_MEMORY_REGION(SOC_MEM_BT_EM_START,   SOC_MEM_BT_EM_BREDR_REAL_END,  rom_bt_em);
SOC_RESERVE_MEMORY_REGION(SOC_MEM_BT_BSS_START,  SOC_MEM_BT_BSS_END,            rom_bt_bss);
SOC_RESERVE_MEMORY_REGION(SOC_MEM_BT_MISC_START, SOC_MEM_BT_MISC_END,           rom_bt_misc);
SOC_RESERVE_MEMORY_REGION(SOC_MEM_BT_DATA_START, SOC_MEM_BT_DATA_END,           rom_bt_data);

static DRAM_ATTR struct osi_funcs_t *osi_funcs_p;

#if CONFIG_SPIRAM_USE_MALLOC
static DRAM_ATTR btdm_queue_item_t btdm_queue_table[BTDM_MAX_QUEUE_NUM];
static DRAM_ATTR SemaphoreHandle_t btdm_queue_table_mux = NULL;
#endif /* #if CONFIG_SPIRAM_USE_MALLOC */

/* Static variable declare */
// timestamp when PHY/RF was switched on
static DRAM_ATTR int64_t s_time_phy_rf_just_enabled = 0;
static DRAM_ATTR esp_bt_controller_status_t btdm_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

static DRAM_ATTR portMUX_TYPE global_int_mux = portMUX_INITIALIZER_UNLOCKED;

// measured average low power clock period in micro seconds
static DRAM_ATTR uint32_t btdm_lpcycle_us = 0;
static DRAM_ATTR uint8_t btdm_lpcycle_us_frac = 0; // number of fractional bit for btdm_lpcycle_us

#ifdef CONFIG_PM_ENABLE
static DRAM_ATTR esp_timer_handle_t s_btdm_slp_tmr;
static DRAM_ATTR esp_pm_lock_handle_t s_pm_lock;
static DRAM_ATTR QueueHandle_t s_pm_lock_sem = NULL;
#if !BTDM_ALLOW_LIGHT_SLEEP
// pm_lock to prevent light sleep when using main crystal as Bluetooth low power clock
static DRAM_ATTR esp_pm_lock_handle_t s_light_sleep_pm_lock;
#endif /* #if !BTDM_ALLOW_LIGHT_SLEEP */
static void btdm_slp_tmr_callback(void *arg);
#endif

static inline void btdm_check_and_init_bb(void)
{
    /* init BT-BB if PHY/RF has been switched off since last BT-BB init */
    int64_t latest_ts = esp_phy_rf_get_on_ts();
    if (latest_ts != s_time_phy_rf_just_enabled ||
        s_time_phy_rf_just_enabled == 0) {
        btdm_rf_bb_init_phase2();
        s_time_phy_rf_just_enabled = latest_ts;
    }
}

#if CONFIG_SPIRAM_USE_MALLOC
static bool btdm_queue_generic_register(const btdm_queue_item_t *queue)
{
    if (!btdm_queue_table_mux || !queue) {
        return NULL;
    }

    bool ret = false;
    btdm_queue_item_t *item;
    xSemaphoreTake(btdm_queue_table_mux, portMAX_DELAY);
    for (int i = 0; i < BTDM_MAX_QUEUE_NUM; ++i) {
        item = &btdm_queue_table[i];
        if (item->handle == NULL) {
            memcpy(item, queue, sizeof(btdm_queue_item_t));
            ret = true;
            break;
        }
    }
    xSemaphoreGive(btdm_queue_table_mux);
    return ret;
}

static bool btdm_queue_generic_deregister(btdm_queue_item_t *queue)
{
    if (!btdm_queue_table_mux || !queue) {
        return false;
    }

    bool ret = false;
    btdm_queue_item_t *item;
    xSemaphoreTake(btdm_queue_table_mux, portMAX_DELAY);
    for (int i = 0; i < BTDM_MAX_QUEUE_NUM; ++i) {
        item = &btdm_queue_table[i];
        if (item->handle == queue->handle) {
            memcpy(queue, item, sizeof(btdm_queue_item_t));
            memset(item, 0, sizeof(btdm_queue_item_t));
            ret = true;
            break;
        }
    }
    xSemaphoreGive(btdm_queue_table_mux);
    return ret;
}

#endif /* CONFIG_SPIRAM_USE_MALLOC */

static void IRAM_ATTR interrupt_disable(void)
{
    if (xPortInIsrContext()) {
        portENTER_CRITICAL_ISR(&global_int_mux);
    } else {
        portENTER_CRITICAL(&global_int_mux);
    }
}

static void IRAM_ATTR interrupt_restore(void)
{
    if (xPortInIsrContext()) {
        portEXIT_CRITICAL_ISR(&global_int_mux);
    } else {
        portEXIT_CRITICAL(&global_int_mux);
    }
}

static void IRAM_ATTR task_yield_from_isr(void)
{
    portYIELD_FROM_ISR();
}

static void *semphr_create_wrapper(uint32_t max, uint32_t init)
{
#if !CONFIG_SPIRAM_USE_MALLOC
    return (void *)xSemaphoreCreateCounting(max, init);
#else
    StaticQueue_t *queue_buffer = NULL;
    QueueHandle_t handle = NULL;

    queue_buffer = heap_caps_malloc(sizeof(StaticQueue_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (!queue_buffer) {
        goto error;
    }

    handle = xSemaphoreCreateCountingStatic(max, init, queue_buffer);
    if (!handle) {
        goto error;
    }

    btdm_queue_item_t item = {
        .handle = handle,
        .storage = NULL,
        .buffer = queue_buffer,
    };

    if (!btdm_queue_generic_register(&item)) {
        goto error;
    }
    return handle;

 error:
    if (handle) {
        vSemaphoreDelete(handle);
    }
    if (queue_buffer) {
        free(queue_buffer);
    }

    return NULL;
#endif
}

static void semphr_delete_wrapper(void *semphr)
{
#if !CONFIG_SPIRAM_USE_MALLOC
    vSemaphoreDelete(semphr);
#else
    btdm_queue_item_t item = {
        .handle = semphr,
        .storage = NULL,
        .buffer = NULL,
    };

    if (btdm_queue_generic_deregister(&item)) {
        vSemaphoreDelete(item.handle);
        free(item.buffer);
    }

    return;
#endif
}

static int32_t IRAM_ATTR semphr_take_from_isr_wrapper(void *semphr, void *hptw)
{
    return (int32_t)xSemaphoreTakeFromISR(semphr, hptw);
}

static int32_t IRAM_ATTR semphr_give_from_isr_wrapper(void *semphr, void *hptw)
{
    return (int32_t)xSemaphoreGiveFromISR(semphr, hptw);
}

static int32_t semphr_take_wrapper(void *semphr, uint32_t block_time_ms)
{
    if (block_time_ms == OSI_FUNCS_TIME_BLOCKING) {
        return (int32_t)xSemaphoreTake(semphr, portMAX_DELAY);
    } else {
        return (int32_t)xSemaphoreTake(semphr, block_time_ms / portTICK_PERIOD_MS);
    }
}

static int32_t semphr_give_wrapper(void *semphr)
{
    return (int32_t)xSemaphoreGive(semphr);
}

static void *mutex_create_wrapper(void)
{
#if CONFIG_SPIRAM_USE_MALLOC
    StaticQueue_t *queue_buffer = NULL;
    QueueHandle_t handle = NULL;

    queue_buffer = heap_caps_malloc(sizeof(StaticQueue_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (!queue_buffer) {
        goto error;
    }

    handle = xSemaphoreCreateMutexStatic(queue_buffer);
    if (!handle) {
        goto error;
    }

    btdm_queue_item_t item = {
        .handle = handle,
        .storage = NULL,
        .buffer = queue_buffer,
    };

    if (!btdm_queue_generic_register(&item)) {
        goto error;
    }
    return handle;

 error:
    if (handle) {
        vSemaphoreDelete(handle);
    }
    if (queue_buffer) {
        free(queue_buffer);
    }

    return NULL;
#else
    return (void *)xSemaphoreCreateMutex();
#endif
}

static void mutex_delete_wrapper(void *mutex)
{
#if !CONFIG_SPIRAM_USE_MALLOC
    vSemaphoreDelete(mutex);
#else
    btdm_queue_item_t item = {
        .handle = mutex,
        .storage = NULL,
        .buffer = NULL,
    };

    if (btdm_queue_generic_deregister(&item)) {
        vSemaphoreDelete(item.handle);
        free(item.buffer);
    }

    return;
#endif
}

static int32_t mutex_lock_wrapper(void *mutex)
{
    return (int32_t)xSemaphoreTake(mutex, portMAX_DELAY);
}

static int32_t mutex_unlock_wrapper(void *mutex)
{
    return (int32_t)xSemaphoreGive(mutex);
}

static void *queue_create_wrapper(uint32_t queue_len, uint32_t item_size)
{
#if CONFIG_SPIRAM_USE_MALLOC
    StaticQueue_t *queue_buffer = NULL;
    uint8_t *queue_storage = NULL;
    QueueHandle_t handle = NULL;

    queue_buffer = heap_caps_malloc(sizeof(StaticQueue_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (!queue_buffer) {
        goto error;
    }

    queue_storage = heap_caps_malloc((queue_len*item_size), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (!queue_storage ) {
        goto error;
    }

    handle = xQueueCreateStatic(queue_len, item_size, queue_storage, queue_buffer);
    if (!handle) {
        goto error;
    }

    btdm_queue_item_t item = {
        .handle = handle,
        .storage = queue_storage,
        .buffer = queue_buffer,
    };

    if (!btdm_queue_generic_register(&item)) {
        goto error;
    }

    return handle;

 error:
    if (handle) {
        vQueueDelete(handle);
    }
    if (queue_storage) {
        free(queue_storage);
    }
    if (queue_buffer) {
        free(queue_buffer);
    }

    return NULL;
#else
    return (void *)xQueueCreate(queue_len, item_size);
#endif
}

static void queue_delete_wrapper(void *queue)
{
#if !CONFIG_SPIRAM_USE_MALLOC
    vQueueDelete(queue);
#else
    btdm_queue_item_t item = {
        .handle = queue,
        .storage = NULL,
        .buffer = NULL,
    };

    if (btdm_queue_generic_deregister(&item)) {
        vQueueDelete(item.handle);
        free(item.storage);
        free(item.buffer);
    }

    return;
#endif
}

static int32_t queue_send_wrapper(void *queue, void *item, uint32_t block_time_ms)
{
    if (block_time_ms == OSI_FUNCS_TIME_BLOCKING) {
        return (int32_t)xQueueSend(queue, item, portMAX_DELAY);
    } else {
        return (int32_t)xQueueSend(queue, item, block_time_ms / portTICK_PERIOD_MS);
    }
}

static int32_t IRAM_ATTR queue_send_from_isr_wrapper(void *queue, void *item, void *hptw)
{
    return (int32_t)xQueueSendFromISR(queue, item, hptw);
}

static int32_t queue_recv_wrapper(void *queue, void *item, uint32_t block_time_ms)
{
    if (block_time_ms == OSI_FUNCS_TIME_BLOCKING) {
        return (int32_t)xQueueReceive(queue, item, portMAX_DELAY);
    } else {
        return (int32_t)xQueueReceive(queue, item, block_time_ms / portTICK_PERIOD_MS);
    }
}

static int32_t IRAM_ATTR queue_recv_from_isr_wrapper(void *queue, void *item, void *hptw)
{
    return (int32_t)xQueueReceiveFromISR(queue, item, hptw);
}

static int32_t task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id)
{
    return (uint32_t)xTaskCreatePinnedToCore(task_func, name, stack_depth, param, prio, task_handle, (core_id < portNUM_PROCESSORS ? core_id : tskNO_AFFINITY));
}

static void task_delete_wrapper(void *task_handle)
{
    vTaskDelete(task_handle);
}

static bool IRAM_ATTR is_in_isr_wrapper(void)
{
    return !xPortCanYield();
}

static void IRAM_ATTR cause_sw_intr(void *arg)
{
    /* just convert void * to int, because the width is the same */
    uint32_t intr_no = (uint32_t)arg;
    XTHAL_SET_INTSET((1<<intr_no));
}

static int IRAM_ATTR cause_sw_intr_to_core_wrapper(int core_id, int intr_no)
{
    esp_err_t err = ESP_OK;

    if (xPortGetCoreID() == core_id) {
        cause_sw_intr((void *)intr_no);
    } else {
        err = esp_ipc_call(core_id, cause_sw_intr, (void *)intr_no);
    }

    return err;
}

static void *malloc_internal_wrapper(size_t size)
{
    return heap_caps_malloc(size, MALLOC_CAP_8BIT|MALLOC_CAP_DMA|MALLOC_CAP_INTERNAL);
}

static int32_t IRAM_ATTR read_mac_wrapper(uint8_t mac[6])
{
    return esp_read_mac(mac, ESP_MAC_BT);
}

static void IRAM_ATTR srand_wrapper(unsigned int seed)
{
    /* empty function */
}

static int IRAM_ATTR rand_wrapper(void)
{
    return (int)esp_random();
}

static uint32_t IRAM_ATTR btdm_lpcycles_2_us(uint32_t cycles)
{
    // The number of lp cycles should not lead to overflow. Thrs: 100s
    // clock measurement is conducted
    uint64_t us = (uint64_t)btdm_lpcycle_us * cycles;
    us = (us + (1 << (btdm_lpcycle_us_frac - 1))) >> btdm_lpcycle_us_frac;
    return (uint32_t)us;
}

/*
 * @brief Converts a duration in slots into a number of low power clock cycles.
 */
static uint32_t IRAM_ATTR btdm_us_2_lpcycles(uint32_t us)
{
    // The number of sleep duration(us) should not lead to overflow. Thrs: 100s
    // Compute the sleep duration in us to low power clock cycles, with calibration result applied
    // clock measurement is conducted
    uint64_t cycles = ((uint64_t)(us) << btdm_lpcycle_us_frac) / btdm_lpcycle_us;

    return (uint32_t)cycles;
}

static bool IRAM_ATTR btdm_sleep_check_duration(uint32_t *slot_cnt)
{
    if (*slot_cnt < BTDM_MIN_SLEEP_DURATION) {
        return false;
    }
    /* wake up in advance considering the delay in enabling PHY/RF */
    *slot_cnt -= BTDM_MODEM_WAKE_UP_DELAY;
    return true;
}

static void btdm_sleep_enter_phase1_wrapper(uint32_t lpcycles)
{
#ifdef CONFIG_PM_ENABLE
    // start a timer to wake up and acquire the pm_lock before modem_sleep awakes
    uint32_t us_to_sleep = btdm_lpcycles_2_us(lpcycles);

#define BTDM_MIN_TIMER_UNCERTAINTY_US      (500)
    assert(us_to_sleep > BTDM_MIN_TIMER_UNCERTAINTY_US);
    // allow a maximum time uncertainty to be about 488ppm(1/2048) at least as clock drift
    // and set the timer in advance
    uint32_t uncertainty = (us_to_sleep >> 11);
    if (uncertainty < BTDM_MIN_TIMER_UNCERTAINTY_US) {
        uncertainty = BTDM_MIN_TIMER_UNCERTAINTY_US;
    }

    if (esp_timer_start_once(s_btdm_slp_tmr, us_to_sleep - uncertainty) != ESP_OK) {
        ESP_LOGW(BTDM_LOG_TAG, "timer start failed");
    }
#endif
}

static void btdm_sleep_enter_phase2_wrapper(void)
{
    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        esp_modem_sleep_enter(MODEM_BLE_MODULE);
        esp_modem_sleep_enter(MODEM_CLASSIC_BT_MODULE);
#ifdef CONFIG_PM_ENABLE
        esp_pm_lock_release(s_pm_lock);
        semphr_give_wrapper(s_pm_lock_sem);
#endif
    } else if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_EVED) {
        esp_modem_sleep_enter(MODEM_BLE_MODULE);
        // pause bluetooth baseband
        periph_module_disable(PERIPH_BT_BASEBAND_MODULE);
    }
}

static void IRAM_ATTR btdm_sleep_exit_phase1_wrapper(void)
{
#ifdef CONFIG_PM_ENABLE
    if (semphr_take_from_isr_wrapper(s_pm_lock_sem, NULL) == pdTRUE) {
        esp_pm_lock_acquire(s_pm_lock);
    }
#endif
}

static void btdm_sleep_exit_phase3_wrapper(void)
{
    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        esp_modem_sleep_exit(MODEM_BLE_MODULE);
        esp_modem_sleep_exit(MODEM_CLASSIC_BT_MODULE);
        btdm_check_and_init_bb();
#ifdef CONFIG_PM_ENABLE
        esp_timer_stop(s_btdm_slp_tmr);
#endif
    } else if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_EVED) {
        // resume bluetooth baseband
        periph_module_enable(PERIPH_BT_BASEBAND_MODULE);
        esp_modem_sleep_exit(MODEM_BLE_MODULE);
    }
}

#ifdef CONFIG_PM_ENABLE
static void IRAM_ATTR btdm_slp_tmr_callback(void *arg)
{
    if (semphr_take_wrapper(s_pm_lock_sem, 0) == pdTRUE) {
        esp_pm_lock_acquire(s_pm_lock);
    }
}
#endif

bool esp_vhci_host_check_send_available(void)
{
    return API_vhci_host_check_send_available();
}

void esp_vhci_host_send_packet(uint8_t *data, uint16_t len)
{
    bool do_wakeup_request = false;

    if (!btdm_power_state_active()) {
#if CONFIG_PM_ENABLE
        if (semphr_take_wrapper(s_pm_lock_sem, 0)) {
            esp_pm_lock_acquire(s_pm_lock);
        }
        esp_timer_stop(s_btdm_slp_tmr);
#endif
        do_wakeup_request = true;
        btdm_wakeup_request(true);
    }

    API_vhci_host_send_packet(data, len);

    if (do_wakeup_request) {
        btdm_wakeup_request_end();
    }
}

esp_err_t esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback)
{
    return API_vhci_host_register_callback((const vhci_host_callback_t *)callback) == 0 ? ESP_OK : ESP_FAIL;
}

static uint32_t btdm_config_mask_load(void)
{
    uint32_t mask = 0x0;

#if CONFIG_BTDM_CTRL_HCI_MODE_UART_H4
    mask |= BTDM_CFG_HCI_UART;
#endif
#if CONFIG_BTDM_CTRL_PINNED_TO_CORE == 1
    mask |= BTDM_CFG_CONTROLLER_RUN_APP_CPU;
#endif
#if CONFIG_BTDM_CTRL_FULL_SCAN_SUPPORTED
    mask |= BTDM_CFG_BLE_FULL_SCAN_SUPPORTED;
#endif /* CONFIG_BTDM_CTRL_FULL_SCAN_SUPPORTED */
    mask |= BTDM_CFG_SCAN_DUPLICATE_OPTIONS;

    mask |= BTDM_CFG_SEND_ADV_RESERVED_SIZE;

    return mask;
}

static void btdm_controller_mem_init(void)
{
    /* initialise .data section */
    memcpy(&_data_start_btdm, (void *)_data_start_btdm_rom, &_data_end_btdm - &_data_start_btdm);
    ESP_LOGD(BTDM_LOG_TAG, ".data initialise [0x%08x] <== [0x%08x]", (uint32_t)&_data_start_btdm, _data_start_btdm_rom);

    //initial em, .bss section
    for (int i = 1; i < sizeof(btdm_dram_available_region)/sizeof(btdm_dram_available_region_t); i++) {
        if (btdm_dram_available_region[i].mode != ESP_BT_MODE_IDLE) {
            memset((void *)btdm_dram_available_region[i].start, 0x0, btdm_dram_available_region[i].end - btdm_dram_available_region[i].start);
            ESP_LOGD(BTDM_LOG_TAG, ".bss initialise [0x%08x] - [0x%08x]", btdm_dram_available_region[i].start, btdm_dram_available_region[i].end);
        }
    }
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

esp_err_t esp_bt_controller_mem_release(esp_bt_mode_t mode)
{
    bool update = true;
    intptr_t mem_start, mem_end;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

    //already released
    if (!(mode & btdm_dram_available_region[0].mode)) {
        return ESP_ERR_INVALID_STATE;
    }

    for (int i = 0; i < sizeof(btdm_dram_available_region)/sizeof(btdm_dram_available_region_t); i++) {
        //skip the share mode, idle mode and other mode
        if (btdm_dram_available_region[i].mode == ESP_BT_MODE_IDLE
                || (mode & btdm_dram_available_region[i].mode) != btdm_dram_available_region[i].mode) {
            //clear the bit of the mode which will be released
            btdm_dram_available_region[i].mode &= ~mode;
            continue;
        } else {
            //clear the bit of the mode which will be released
            btdm_dram_available_region[i].mode &= ~mode;
        }

        if (update) {
            mem_start = btdm_dram_available_region[i].start;
            mem_end = btdm_dram_available_region[i].end;
            update = false;
        }

        if (i < sizeof(btdm_dram_available_region)/sizeof(btdm_dram_available_region_t) - 1) {
            mem_end = btdm_dram_available_region[i].end;
            if (btdm_dram_available_region[i+1].mode != ESP_BT_MODE_IDLE
                    && (mode & btdm_dram_available_region[i+1].mode) == btdm_dram_available_region[i+1].mode
                    && mem_end == btdm_dram_available_region[i+1].start) {
                continue;
            } else {
                ESP_LOGD(BTDM_LOG_TAG, "Release DRAM [0x%08x] - [0x%08x]", mem_start, mem_end);
                ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
                update = true;
            }
        } else {
            mem_end = btdm_dram_available_region[i].end;
            ESP_LOGD(BTDM_LOG_TAG, "Release DRAM [0x%08x] - [0x%08x]", mem_start, mem_end);
            ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
            update = true;
        }
    }

    if (mode == ESP_BT_MODE_BTDM) {
        mem_start = (intptr_t)&_btdm_bss_start;
        mem_end = (intptr_t)&_btdm_bss_end;
        if (mem_start != mem_end) {
            ESP_LOGD(BTDM_LOG_TAG, "Release BTDM BSS [0x%08x] - [0x%08x]", mem_start, mem_end);
            ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
        }
        mem_start = (intptr_t)&_btdm_data_start;
        mem_end = (intptr_t)&_btdm_data_end;
        if (mem_start != mem_end) {
            ESP_LOGD(BTDM_LOG_TAG, "Release BTDM Data [0x%08x] - [0x%08x]", mem_start, mem_end);
            ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
        }
    }
    return ESP_OK;
}

esp_err_t esp_bt_mem_release(esp_bt_mode_t mode)
{
    int ret;
    intptr_t mem_start, mem_end;

    ret = esp_bt_controller_mem_release(mode);
    if (ret != ESP_OK) {
        return ret;
    }

    if (mode == ESP_BT_MODE_BTDM) {
        mem_start = (intptr_t)&_bt_bss_start;
        mem_end = (intptr_t)&_bt_bss_end;
        if (mem_start != mem_end) {
            ESP_LOGD(BTDM_LOG_TAG, "Release BT BSS [0x%08x] - [0x%08x]", mem_start, mem_end);
            ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
        }
        mem_start = (intptr_t)&_bt_data_start;
        mem_end = (intptr_t)&_bt_data_end;
        if (mem_start != mem_end) {
            ESP_LOGD(BTDM_LOG_TAG, "Release BT Data [0x%08x] - [0x%08x]", mem_start, mem_end);
            ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
        }

        mem_start = (intptr_t)&_nimble_bss_start;
        mem_end = (intptr_t)&_nimble_bss_end;
        if (mem_start != mem_end) {
            ESP_LOGD(BTDM_LOG_TAG, "Release NimBLE BSS [0x%08x] - [0x%08x]", mem_start, mem_end);
            ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
        }
        mem_start = (intptr_t)&_nimble_data_start;
        mem_end = (intptr_t)&_nimble_data_end;
        if (mem_start != mem_end) {
            ESP_LOGD(BTDM_LOG_TAG, "Release NimBLE Data [0x%08x] - [0x%08x]", mem_start, mem_end);
            ESP_ERROR_CHECK(try_heap_caps_add_region(mem_start, mem_end));
        }
    }
    return ESP_OK;
}

esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg)
{
    esp_err_t err;
    uint32_t btdm_cfg_mask = 0;

    osi_funcs_p = (struct osi_funcs_t *)malloc_internal_wrapper(sizeof(struct osi_funcs_t));
    if (osi_funcs_p == NULL) {
        return ESP_ERR_NO_MEM;
    }

    memcpy(osi_funcs_p, &osi_funcs_ro, sizeof(struct osi_funcs_t));
    if (btdm_osi_funcs_register(osi_funcs_p) != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

    //if all the bt available memory was already released, cannot initialize bluetooth controller
    if (btdm_dram_available_region[0].mode == ESP_BT_MODE_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (cfg->controller_task_prio != ESP_TASK_BT_CONTROLLER_PRIO
            || cfg->controller_task_stack_size < ESP_TASK_BT_CONTROLLER_STACK) {
        return ESP_ERR_INVALID_ARG;
    }

    //overwrite some parameters
    cfg->bt_max_sync_conn = CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN_EFF;
    cfg->magic  = ESP_BT_CONTROLLER_CONFIG_MAGIC_VAL;

    if (((cfg->mode & ESP_BT_MODE_BLE) && (cfg->ble_max_conn <= 0 || cfg->ble_max_conn > BTDM_CONTROLLER_BLE_MAX_CONN_LIMIT))
            || ((cfg->mode & ESP_BT_MODE_CLASSIC_BT) && (cfg->bt_max_acl_conn <= 0 || cfg->bt_max_acl_conn > BTDM_CONTROLLER_BR_EDR_MAX_ACL_CONN_LIMIT))
            || ((cfg->mode & ESP_BT_MODE_CLASSIC_BT) && (cfg->bt_max_sync_conn > BTDM_CONTROLLER_BR_EDR_MAX_SYNC_CONN_LIMIT))) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(BTDM_LOG_TAG, "BT controller compile version [%s]", btdm_controller_get_compile_version());

#if CONFIG_SPIRAM_USE_MALLOC
    btdm_queue_table_mux = xSemaphoreCreateMutex();
    if (btdm_queue_table_mux == NULL) {
        return ESP_ERR_NO_MEM;
    }
    memset(btdm_queue_table, 0, sizeof(btdm_queue_item_t) * BTDM_MAX_QUEUE_NUM);
#endif

#ifdef CONFIG_PM_ENABLE
#if !BTDM_ALLOW_LIGHT_SLEEP
    if ((err = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "btLS", &s_light_sleep_pm_lock)) != ESP_OK) {
        goto error;
    }
#endif /* #if !BTDM_ALLOW_LIGHT_SLEEP */
    if ((err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "bt", &s_pm_lock)) != ESP_OK) {
        goto error;
    }
    esp_timer_create_args_t create_args = {
        .callback = btdm_slp_tmr_callback,
        .arg = NULL,
        .name = "btSlp"
    };
    if ((err = esp_timer_create(&create_args, &s_btdm_slp_tmr)) != ESP_OK) {
        goto error;
    }

    s_pm_lock_sem = semphr_create_wrapper(1, 0);
    if (s_pm_lock_sem == NULL) {
        err = ESP_ERR_NO_MEM;
        goto error;
    }
#endif

    btdm_controller_mem_init();

    periph_module_enable(PERIPH_BT_MODULE);

    btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
    btdm_lpcycle_us = 32 << btdm_lpcycle_us_frac;
#if CONFIG_BTDM_MODEM_SLEEP_MODE_ORIG
    bool select_src_ret = false;
    bool set_div_ret = false;
#if CONFIG_BTDM_LPCLK_SEL_MAIN_XTAL
    select_src_ret = btdm_lpclk_select_src(BTDM_LPCLK_SEL_XTAL);
    set_div_ret = btdm_lpclk_set_div(rtc_clk_xtal_freq_get() * 2 - 1);
    assert(select_src_ret && set_div_ret);
    btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
    btdm_lpcycle_us = 2 << (btdm_lpcycle_us_frac);
#elif CONFIG_BTDM_LPCLK_SEL_EXT_32K_XTAL
    select_src_ret = btdm_lpclk_select_src(BTDM_LPCLK_SEL_XTAL32K);
    set_div_ret = btdm_lpclk_set_div(0);
    assert(select_src_ret && set_div_ret);
    btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
    btdm_lpcycle_us = (RTC_CLK_CAL_FRACT > 15) ? (1000000 << (RTC_CLK_CAL_FRACT - 15)) :
        (1000000 >> (15 - RTC_CLK_CAL_FRACT));
    assert(btdm_lpcycle_us != 0);
#endif // CONFIG_BTDM_LPCLK_SEL_XX
    btdm_controller_set_sleep_mode(BTDM_MODEM_SLEEP_MODE_ORIG);
#elif CONFIG_BTDM_MODEM_SLEEP_MODE_EVED
    btdm_controller_set_sleep_mode(BTDM_MODEM_SLEEP_MODE_EVED);
#else
    btdm_controller_set_sleep_mode(BTDM_MODEM_SLEEP_MODE_NONE);
#endif

    btdm_cfg_mask = btdm_config_mask_load();

    if (btdm_controller_init(btdm_cfg_mask, cfg) != 0) {
        err = ESP_ERR_NO_MEM;
        goto error;
    }

    #ifdef CONFIG_BTDM_COEX_BLE_ADV_HIGH_PRIORITY
        coex_ble_adv_priority_high_set(true);
    #else
        coex_ble_adv_priority_high_set(false);
    #endif

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

    return ESP_OK;

error:
#ifdef CONFIG_PM_ENABLE
#if !BTDM_ALLOW_LIGHT_SLEEP
    if (s_light_sleep_pm_lock != NULL) {
        esp_pm_lock_delete(s_light_sleep_pm_lock);
        s_light_sleep_pm_lock = NULL;
    }
#endif /* #if !BTDM_ALLOW_LIGHT_SLEEP */
    if (s_pm_lock != NULL) {
        esp_pm_lock_delete(s_pm_lock);
        s_pm_lock = NULL;
    }
    if (s_btdm_slp_tmr != NULL) {
        esp_timer_delete(s_btdm_slp_tmr);
        s_btdm_slp_tmr = NULL;
    }
    if (s_pm_lock_sem) {
        semphr_delete_wrapper(s_pm_lock_sem);
        s_pm_lock_sem = NULL;
    }
#endif
    return err;
}

esp_err_t esp_bt_controller_deinit(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        return ESP_ERR_INVALID_STATE;
    }

    btdm_controller_deinit();

    periph_module_disable(PERIPH_BT_MODULE);

#ifdef CONFIG_PM_ENABLE
#if !BTDM_ALLOW_LIGHT_SLEEP
    esp_pm_lock_delete(s_light_sleep_pm_lock);
    s_light_sleep_pm_lock = NULL;
#endif /* #if !BTDM_ALLOW_LIGHT_SLEEP */
    esp_pm_lock_delete(s_pm_lock);
    s_pm_lock = NULL;
    esp_timer_stop(s_btdm_slp_tmr);
    esp_timer_delete(s_btdm_slp_tmr);
    s_btdm_slp_tmr = NULL;
    semphr_delete_wrapper(s_pm_lock_sem);
    s_pm_lock_sem = NULL;
#endif

#if CONFIG_SPIRAM_USE_MALLOC
    vSemaphoreDelete(btdm_queue_table_mux);
    btdm_queue_table_mux = NULL;
    memset(btdm_queue_table, 0, sizeof(btdm_queue_item_t) * BTDM_MAX_QUEUE_NUM);
#endif

    free(osi_funcs_p);
    osi_funcs_p = NULL;

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

    btdm_lpcycle_us = 0;
    btdm_controller_set_sleep_mode(BTDM_MODEM_SLEEP_MODE_NONE);

    return ESP_OK;
}

esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode)
{
    int ret;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        return ESP_ERR_INVALID_STATE;
    }

    //As the history reason, mode should be equal to the mode which set in esp_bt_controller_init()
    if (mode != btdm_controller_get_mode()) {
        return ESP_ERR_INVALID_ARG;
    }

#ifdef CONFIG_PM_ENABLE
#if !BTDM_ALLOW_LIGHT_SLEEP
    esp_pm_lock_acquire(s_light_sleep_pm_lock);
#endif /* #if !BTDM_ALLOW_LIGHT_SLEEP */
    esp_pm_lock_acquire(s_pm_lock);
#endif

    esp_phy_load_cal_and_init(PHY_BT_MODULE);

    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_NONE) {
        //Just register to sleep module, make the modem sleep modules check BT sleep status when sleep enter.
        //Thus, it will prevent WIFI from disabling RF when BT is not in sleep but is using RF.
        esp_modem_sleep_register(MODEM_BLE_MODULE);
        esp_modem_sleep_register(MODEM_CLASSIC_BT_MODULE);
        esp_modem_sleep_exit(MODEM_BLE_MODULE);
        esp_modem_sleep_exit(MODEM_CLASSIC_BT_MODULE);
    } else if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        esp_modem_sleep_register(MODEM_BLE_MODULE);
        esp_modem_sleep_register(MODEM_CLASSIC_BT_MODULE);
    } else if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_EVED) {
        esp_modem_sleep_register(MODEM_BLE_MODULE);
    }

    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        btdm_controller_enable_sleep(true);
    }

    // inititalize bluetooth baseband
    btdm_check_and_init_bb();

    ret = btdm_controller_enable(mode);
    if (ret) {
        if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_NONE
                || btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
            esp_modem_sleep_deregister(MODEM_BLE_MODULE);
            esp_modem_sleep_deregister(MODEM_CLASSIC_BT_MODULE);
        } else if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_EVED) {
            esp_modem_sleep_deregister(MODEM_BLE_MODULE);
        }
        esp_phy_rf_deinit(PHY_BT_MODULE);
#ifdef CONFIG_PM_ENABLE
#if !BTDM_ALLOW_LIGHT_SLEEP
        esp_pm_lock_release(s_light_sleep_pm_lock);
#endif /* #if !BTDM_ALLOW_LIGHT_SLEEP */
        esp_pm_lock_release(s_pm_lock);
#endif
        return ESP_ERR_INVALID_STATE;
    }

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_ENABLED;

    return ESP_OK;
}

esp_err_t esp_bt_controller_disable(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    // disable modem sleep and wake up from sleep mode
    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        btdm_controller_enable_sleep(false);
        if (!btdm_power_state_active()) {
            btdm_wakeup_request(false);
        }
        while (!btdm_power_state_active()) {
            ets_delay_us(1000);
        }
    }

    btdm_controller_disable();

    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_NONE
            || btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        esp_modem_sleep_deregister(MODEM_BLE_MODULE);
        esp_modem_sleep_deregister(MODEM_CLASSIC_BT_MODULE);
    } else if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_EVED) {
        esp_modem_sleep_deregister(MODEM_BLE_MODULE);
    }
    esp_phy_rf_deinit(PHY_BT_MODULE);
    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

#ifdef CONFIG_PM_ENABLE
#if !BTDM_ALLOW_LIGHT_SLEEP
    esp_pm_lock_release(s_light_sleep_pm_lock);
#endif /* #if !BTDM_ALLOW_LIGHT_SLEEP */
    esp_pm_lock_release(s_pm_lock);
#endif

    return ESP_OK;
}

esp_bt_controller_status_t esp_bt_controller_get_status(void)
{
    return btdm_controller_status;
}


/* extra functions */
esp_err_t esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level)
{
    if (ble_txpwr_set(power_type, power_level) != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type)
{
    return (esp_power_level_t)ble_txpwr_get(power_type);
}

esp_err_t esp_bredr_tx_power_set(esp_power_level_t min_power_level, esp_power_level_t max_power_level)
{
    esp_err_t err;
    int ret;

    ret = bredr_txpwr_set(min_power_level, max_power_level);

    if (ret == 0) {
        err = ESP_OK;
    } else if (ret == -1) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        err = ESP_ERR_INVALID_STATE;
    }

    return err;
}

esp_err_t esp_bredr_tx_power_get(esp_power_level_t *min_power_level, esp_power_level_t *max_power_level)
{
    if (bredr_txpwr_get((int *)min_power_level, (int *)max_power_level) != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t esp_bt_sleep_enable (void)
{
    esp_err_t status;
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        esp_modem_sleep_register(MODEM_BLE_MODULE);
        esp_modem_sleep_register(MODEM_CLASSIC_BT_MODULE);
        btdm_controller_enable_sleep (true);
        status = ESP_OK;
    } else if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_EVED) {
        esp_modem_sleep_register(MODEM_BLE_MODULE);
        btdm_controller_enable_sleep (true);
        status = ESP_OK;
    } else {
        status = ESP_ERR_NOT_SUPPORTED;
    }

    return status;
}

esp_err_t esp_bt_sleep_disable (void)
{
    esp_err_t status;
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        esp_modem_sleep_deregister(MODEM_BLE_MODULE);
        esp_modem_sleep_deregister(MODEM_CLASSIC_BT_MODULE);
        btdm_controller_enable_sleep (false);
        status = ESP_OK;
    } else if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_EVED) {
        esp_modem_sleep_deregister(MODEM_BLE_MODULE);
        btdm_controller_enable_sleep (false);
        status = ESP_OK;
    } else {
        status = ESP_ERR_NOT_SUPPORTED;
    }

    return status;
}

bool esp_bt_controller_is_sleeping(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED ||
        btdm_controller_get_sleep_mode() != BTDM_MODEM_SLEEP_MODE_ORIG) {
        return false;
    }

    return !btdm_power_state_active();
}

void esp_bt_controller_wakeup_request(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED ||
        btdm_controller_get_sleep_mode() != BTDM_MODEM_SLEEP_MODE_ORIG) {
        return;
    }

    btdm_wakeup_request(false);
}

esp_err_t esp_bredr_sco_datapath_set(esp_sco_data_path_t data_path)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    bredr_sco_datapath_set(data_path);
    return ESP_OK;
}

esp_err_t esp_ble_scan_dupilcate_list_flush(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btdm_controller_scan_duplicate_list_clear();
    return ESP_OK;
}

#endif /*  CONFIG_BT_ENABLED */
