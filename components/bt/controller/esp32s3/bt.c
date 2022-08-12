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
#include "esp_attr.h"
#include "esp_phy_init.h"
#include "esp_bt.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_ipc.h"
#include "driver/periph_ctrl.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc_memory_layout.h"
#include "esp_clk.h"
#include "esp_coexist_internal.h"

#if CONFIG_BT_ENABLED

/* Macro definition
 ************************************************************************
 */

#define BTDM_LOG_TAG                        "BTDM_INIT"

#define BTDM_INIT_PERIOD                    (5000)    /* ms */

/* Low Power Clock Selection */
#define BTDM_LPCLK_SEL_XTAL      (0)
#define BTDM_LPCLK_SEL_XTAL32K   (1)
#define BTDM_LPCLK_SEL_RTC_SLOW  (2)
#define BTDM_LPCLK_SEL_8M        (3)

/* Sleep and wakeup interval control */
#define BTDM_MIN_SLEEP_DURATION          (24) // threshold of interval in half slots to allow to fall into modem sleep
#define BTDM_MODEM_WAKE_UP_DELAY         (8)  // delay in half slots of modem wake up procedure, including re-enable PHY/RF

#define BTDM_MODEM_SLEEP_IN_EFFECT       (1)

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
#define OSI_VERSION              0x00010005
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
    uint32_t _magic;
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
    uint32_t (* _btdm_lpcycles_2_hus)(uint32_t cycles, uint32_t *error_corr);
    uint32_t (* _btdm_hus_2_lpcycles)(uint32_t hus);
    bool (* _btdm_sleep_check_duration)(int32_t *slot_cnt);
    void (* _btdm_sleep_enter_phase1)(uint32_t lpcycles);  /* called when interrupt is disabled */
    void (* _btdm_sleep_enter_phase2)(void);
    void (* _btdm_sleep_exit_phase1)(void);  /* called from ISR */
    void (* _btdm_sleep_exit_phase2)(void);  /* called from ISR */
    void (* _btdm_sleep_exit_phase3)(void);  /* called from task */
    void (* _coex_wifi_sleep_set)(bool sleep);
    int (* _coex_core_ble_conn_dyn_prio_get)(bool *low, bool *high);
    void (* _coex_schm_status_bit_set)(uint32_t type, uint32_t status);
    void (* _coex_schm_status_bit_clear)(uint32_t type, uint32_t status);
};


/* External functions or values
 ************************************************************************
 */

/* not for user call, so don't put to include file */
/* OSI */
extern int btdm_osi_funcs_register(void *osi_funcs);
/* Initialise and De-initialise */
extern int btdm_controller_init(esp_bt_controller_config_t *config_opts);
extern void btdm_controller_deinit(void);
extern int btdm_controller_enable(esp_bt_mode_t mode);
extern void btdm_controller_disable(void);
extern uint8_t btdm_controller_get_mode(void);
extern const char *btdm_controller_get_compile_version(void);
extern void btdm_rf_bb_init_phase2(void); // shall be called after PHY/RF is enabled
/* Sleep */
extern void btdm_controller_enable_sleep(bool enable);
extern uint8_t btdm_controller_get_sleep_mode(void);
extern bool btdm_power_state_active(void);
extern void btdm_wakeup_request(bool request_lock);
extern void btdm_wakeup_request_end(void);
/* Low Power Clock */
extern bool btdm_lpclk_select_src(uint32_t sel);
extern bool btdm_lpclk_set_div(uint32_t div);
extern int btdm_hci_tl_io_event_post(int event);

/* VHCI */
extern bool API_vhci_host_check_send_available(void);
extern void API_vhci_host_send_packet(uint8_t *data, uint16_t len);
extern int API_vhci_host_register_callback(const vhci_host_callback_t *callback);
/* TX power */
extern int ble_txpwr_set(int power_type, int power_level);
extern int ble_txpwr_get(int power_type);

extern void coex_pti_v2(void);
extern uint16_t l2c_ble_link_get_tx_buf_num(void);

extern char _bss_start_btdm;
extern char _bss_end_btdm;
extern char _data_start_btdm;
extern char _data_end_btdm;
extern uint32_t _data_start_btdm_rom;
extern uint32_t _data_end_btdm_rom;

extern uint32_t _bt_bss_start;
extern uint32_t _bt_bss_end;
extern uint32_t _btdm_bss_start;
extern uint32_t _btdm_bss_end;
extern uint32_t _bt_data_start;
extern uint32_t _bt_data_end;
extern uint32_t _btdm_data_start;
extern uint32_t _btdm_data_end;

extern char _bt_tmp_bss_start;
extern char _bt_tmp_bss_end;

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
static uint32_t IRAM_ATTR btdm_lpcycles_2_hus(uint32_t cycles, uint32_t *error_corr);
static uint32_t IRAM_ATTR btdm_hus_2_lpcycles(uint32_t hus);
static bool IRAM_ATTR btdm_sleep_check_duration(int32_t *slot_cnt);
static void btdm_sleep_enter_phase1_wrapper(uint32_t lpcycles);
static void btdm_sleep_enter_phase2_wrapper(void);
static void IRAM_ATTR btdm_sleep_exit_phase1_wrapper(void);
static void btdm_sleep_exit_phase3_wrapper(void);
static void coex_wifi_sleep_set_hook(bool sleep);
static void coex_schm_status_bit_set_wrapper(uint32_t type, uint32_t status);
static void coex_schm_status_bit_clear_wrapper(uint32_t type, uint32_t status);

/* Local variable definition
 ***************************************************************************
 */
/* OSI funcs */
static const struct osi_funcs_t osi_funcs_ro = {
    ._magic = OSI_MAGIC_VALUE,
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
    ._btdm_lpcycles_2_hus = btdm_lpcycles_2_hus,
    ._btdm_hus_2_lpcycles = btdm_hus_2_lpcycles,
    ._btdm_sleep_check_duration = btdm_sleep_check_duration,
    ._btdm_sleep_enter_phase1 = btdm_sleep_enter_phase1_wrapper,
    ._btdm_sleep_enter_phase2 = btdm_sleep_enter_phase2_wrapper,
    ._btdm_sleep_exit_phase1 = btdm_sleep_exit_phase1_wrapper,
    ._btdm_sleep_exit_phase2 = NULL,
    ._btdm_sleep_exit_phase3 = btdm_sleep_exit_phase3_wrapper,
    ._coex_wifi_sleep_set = coex_wifi_sleep_set_hook,
    ._coex_core_ble_conn_dyn_prio_get = NULL,
    ._coex_schm_status_bit_set = coex_schm_status_bit_set_wrapper,
    ._coex_schm_status_bit_clear = coex_schm_status_bit_clear_wrapper,
};

static DRAM_ATTR struct osi_funcs_t *osi_funcs_p;

#if CONFIG_SPIRAM_USE_MALLOC
static DRAM_ATTR btdm_queue_item_t btdm_queue_table[BTDM_MAX_QUEUE_NUM];
static DRAM_ATTR SemaphoreHandle_t btdm_queue_table_mux = NULL;
#endif /* #if CONFIG_SPIRAM_USE_MALLOC */

/* Static variable declare */
// timestamp when PHY/RF was switched on
// static DRAM_ATTR int64_t s_time_phy_rf_just_enabled = 0;
static DRAM_ATTR esp_bt_controller_status_t btdm_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

static DRAM_ATTR portMUX_TYPE global_int_mux = portMUX_INITIALIZER_UNLOCKED;

// measured average low power clock period in micro seconds
static DRAM_ATTR uint32_t btdm_lpcycle_us = 0;
static DRAM_ATTR uint8_t btdm_lpcycle_us_frac = 0; // number of fractional bit for btdm_lpcycle_us

#ifdef CONFIG_PM_ENABLE
static DRAM_ATTR esp_timer_handle_t s_btdm_slp_tmr;
static DRAM_ATTR esp_pm_lock_handle_t s_pm_lock;
static DRAM_ATTR esp_pm_lock_handle_t s_light_sleep_pm_lock; // pm_lock to prevent light sleep due to incompatibility currently
static DRAM_ATTR QueueHandle_t s_pm_lock_sem = NULL;
static void btdm_slp_tmr_callback(void *arg);
#endif
static inline void btdm_check_and_init_bb(void)
{
    // todo:
    // btdm_rf_bb_init_phase2();
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
    return (bool)xPortInIsrContext();
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
    return heap_caps_malloc(size, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL|MALLOC_CAP_DMA);
}

static int32_t IRAM_ATTR read_mac_wrapper(uint8_t mac[6])
{
    esp_read_mac(mac, ESP_MAC_BT);
    ESP_LOGI(BTDM_LOG_TAG, "Bluetooth MAC: 0x%02x:%02x:%02x:%02x:%02x:%02x\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return ESP_OK;
}

static void IRAM_ATTR srand_wrapper(unsigned int seed)
{
    /* empty function */
}

static int IRAM_ATTR rand_wrapper(void)
{
    return (int)esp_random();
}

static uint32_t IRAM_ATTR btdm_lpcycles_2_hus(uint32_t cycles, uint32_t *error_corr)
{
    uint64_t local_error_corr = (error_corr == NULL) ? 0 : (uint64_t)(*error_corr);
    uint64_t res = (uint64_t)btdm_lpcycle_us * cycles * 2;
    local_error_corr += res;
    res = (local_error_corr >> btdm_lpcycle_us_frac);
    local_error_corr -= (res << btdm_lpcycle_us_frac);
    if (error_corr) {
        *error_corr = (uint32_t) local_error_corr;
    }
    return (uint32_t)res;
}

/*
 * @brief Converts a duration in half us into a number of low power clock cycles.
 */
static uint32_t IRAM_ATTR btdm_hus_2_lpcycles(uint32_t hus)
{
    // The number of sleep duration(us) should not lead to overflow. Thrs: 100s
    // Compute the sleep duration in us to low power clock cycles, with calibration result applied
    // clock measurement is conducted
    uint64_t cycles = ((uint64_t)(hus) << btdm_lpcycle_us_frac) / btdm_lpcycle_us;
    cycles >>= 1;

    return (uint32_t)cycles;
}

static bool IRAM_ATTR btdm_sleep_check_duration(int32_t *half_slot_cnt)
{
    if (*half_slot_cnt < BTDM_MIN_SLEEP_DURATION) {
        return false;
    }
    /* wake up in advance considering the delay in enabling PHY/RF */
    *half_slot_cnt -= BTDM_MODEM_WAKE_UP_DELAY;
    return true;
}

static void btdm_sleep_enter_phase1_wrapper(uint32_t lpcycles)
{
#ifdef CONFIG_PM_ENABLE
    // start a timer to wake up and acquire the pm_lock before modem_sleep awakes
    uint32_t us_to_sleep = btdm_lpcycles_2_us(lpcycles);

#define BTDM_MIN_TIMER_UNCERTAINTY_US      (1800)
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
    if (btdm_controller_get_sleep_mode() == ESP_BT_SLEEP_MODE_1) {
#if BTDM_MODEM_SLEEP_IN_EFFECT
        //esp_phy_disable();
#endif /* BTDM_MODEM_SLEEP_IN_EFFECT */

#ifdef CONFIG_PM_ENABLE
        esp_pm_lock_release(s_pm_lock);
        semphr_give_wrapper(s_pm_lock_sem);
#endif
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
    if (btdm_controller_get_sleep_mode() == ESP_BT_SLEEP_MODE_1) {
#if BTDM_MODEM_SLEEP_IN_EFFECT
         //esp_phy_enable();
#endif /* BTDM_MODEM_SLEEP_IN_EFFECT */
        btdm_check_and_init_bb();
#ifdef CONFIG_PM_ENABLE
        esp_timer_stop(s_btdm_slp_tmr);
#endif
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

static void btdm_controller_mem_init(void)
{
    memset(&_bss_start_btdm, 0, &_bss_end_btdm - &_bss_start_btdm);
    memcpy(&_data_start_btdm, (void *)_data_start_btdm_rom, &_data_end_btdm - &_data_start_btdm);
    // memset(&_bt_tmp_bss_start, 0, &_bt_tmp_bss_end - &_bt_tmp_bss_start);
}

esp_err_t esp_bt_controller_mem_release(esp_bt_mode_t mode)
{
    ESP_LOGW(BTDM_LOG_TAG, "%s not implemented, return OK", __func__);
    return ESP_OK;
}

esp_err_t esp_bt_mem_release(esp_bt_mode_t mode)
{
    ESP_LOGW(BTDM_LOG_TAG, "%s not implemented, return OK", __func__);
    return ESP_OK;
}

esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg)
{
    esp_err_t err;

    btdm_controller_mem_init();

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

    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (cfg->controller_task_prio != ESP_TASK_BT_CONTROLLER_PRIO
        || cfg->controller_task_stack_size < ESP_TASK_BT_CONTROLLER_STACK) {
        ESP_LOGE(BTDM_LOG_TAG, "Invalid controller task prioriy or stack size");
        return ESP_ERR_INVALID_ARG;
    }

    //overwrite some parameters
    cfg->magic = ESP_BT_CTRL_CONFIG_MAGIC_VAL;

    if (cfg->bluetooth_mode != ESP_BT_MODE_BLE) {
        ESP_LOGE(BTDM_LOG_TAG, "%s controller only support BLE only mode", __func__);
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (cfg->bluetooth_mode & ESP_BT_MODE_BLE) {
        if ((cfg->ble_max_act <= 0) || (cfg->ble_max_act > BT_CTRL_BLE_MAX_ACT_LIMIT)) {
            ESP_LOGE(BTDM_LOG_TAG, "Invalid value of ble_max_act");
            return ESP_ERR_INVALID_ARG;
        }
    }

    if (cfg->sleep_mode == ESP_BT_SLEEP_MODE_1) {
        if (cfg->sleep_clock == ESP_BT_SLEEP_CLOCK_NONE) {
            ESP_LOGE(BTDM_LOG_TAG, "SLEEP_MODE_1 enabled but sleep clock not configured");
            return ESP_ERR_INVALID_ARG;
        }
    }

    ESP_LOGI(BTDM_LOG_TAG, "BT controller compile version [%s]", btdm_controller_get_compile_version());

#if CONFIG_SPIRAM_USE_MALLOC
    btdm_queue_table_mux = xSemaphoreCreateMutex();
    if (btdm_queue_table_mux == NULL) {
        return ESP_ERR_NO_MEM;
    }
    memset(btdm_queue_table, 0, sizeof(btdm_queue_item_t) * BTDM_MAX_QUEUE_NUM);
#endif

    if (cfg->sleep_mode == ESP_BT_SLEEP_MODE_1) {
#ifdef CONFIG_PM_ENABLE
        if ((err = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "btLS", &s_light_sleep_pm_lock)) != ESP_OK) {
            goto error;
        }
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

        do {// todo: rewrite this block of code for chip
        #if CONFIG_IDF_ENV_FPGA
            // overwrite the sleep clock for FPGA
            cfg->sleep_clock = ESP_BT_SLEEP_CLOCK_FPGA_32K;
            ESP_LOGW(BTDM_LOG_TAG, "%s sleep clock overwrite on FPGA", __func__);
        #endif
            bool select_src_ret = false;
            bool set_div_ret = false;
            if (cfg->sleep_clock == ESP_BT_SLEEP_CLOCK_MAIN_XTAL) {
                select_src_ret = btdm_lpclk_select_src(BTDM_LPCLK_SEL_XTAL);
                set_div_ret = btdm_lpclk_set_div(rtc_clk_xtal_freq_get() * 2);
                assert(select_src_ret && set_div_ret);
                btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
                btdm_lpcycle_us = 2 << (btdm_lpcycle_us_frac);
            } else if (cfg->sleep_clock == ESP_BT_SLEEP_CLOCK_EXT_32K_XTAL) {
                select_src_ret = btdm_lpclk_select_src(BTDM_LPCLK_SEL_XTAL32K);
                set_div_ret = btdm_lpclk_set_div(0);
                assert(select_src_ret && set_div_ret);
                btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
                btdm_lpcycle_us = esp_clk_slowclk_cal_get();
                assert(btdm_lpcycle_us != 0);
            } else if (cfg->sleep_clock == ESP_BT_SLEEP_CLOCK_FPGA_32K) {
                // on FPGA, the low power clock is hard-wired to a 32kHz(clock cycle 31.25us) oscillator
                btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
                btdm_lpcycle_us = 125 << (btdm_lpcycle_us_frac - 2);
            }
        } while (0);
    }

    periph_module_enable(PERIPH_BT_MODULE);
    // must do fpga_init and phy init before controller init
    esp_phy_enable();

    if (btdm_controller_init(cfg) != 0) {
        err = ESP_ERR_NO_MEM;
        goto error;
    }
    coex_pti_v2();

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

    return ESP_OK;

error:
#ifdef CONFIG_PM_ENABLE
    if (s_light_sleep_pm_lock != NULL) {
        esp_pm_lock_delete(s_light_sleep_pm_lock);
        s_light_sleep_pm_lock = NULL;
    }
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

    esp_phy_disable();
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_delete(s_light_sleep_pm_lock);
    s_light_sleep_pm_lock = NULL;
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
        ESP_LOGE(BTDM_LOG_TAG, "invalid mode %d, controller support mode is %d", mode, btdm_controller_get_mode());
        return ESP_ERR_INVALID_ARG;
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(s_light_sleep_pm_lock);
    esp_pm_lock_acquire(s_pm_lock);
#endif

    // esp_phy_enable();

#if CONFIG_SW_COEXIST_ENABLE
    coex_enable();
#endif

    if (btdm_controller_get_sleep_mode() == ESP_BT_SLEEP_MODE_1) {
        btdm_controller_enable_sleep(true);
    }
    // inititalize bluetooth baseband
    btdm_check_and_init_bb();

    ret = btdm_controller_enable(mode);
    if (ret) {
        // esp_phy_disable();
#ifdef CONFIG_PM_ENABLE
        esp_pm_lock_release(s_light_sleep_pm_lock);
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
    if (btdm_controller_get_sleep_mode() == ESP_BT_SLEEP_MODE_1) {
        btdm_controller_enable_sleep(false);
        if (!btdm_power_state_active()) {
            btdm_wakeup_request(true);
        }
        while (!btdm_power_state_active()) {
            esp_rom_delay_us(1000);
        }
    }
    btdm_controller_disable();

#if CONFIG_SW_COEXIST_ENABLE
    coex_disable();
#endif

    // esp_phy_disable();
    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(s_light_sleep_pm_lock);
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
    ESP_LOGW(BTDM_LOG_TAG, "%s not implemented, return OK", __func__);
    return ESP_OK;
}

esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type)
{
    ESP_LOGW(BTDM_LOG_TAG, "%s not implemented, return 0", __func__);
    return 0;
}

esp_err_t esp_bt_sleep_enable (void)
{
    esp_err_t status;
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (btdm_controller_get_sleep_mode() == ESP_BT_SLEEP_MODE_1) {
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
    if (btdm_controller_get_sleep_mode() == ESP_BT_SLEEP_MODE_1) {
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
        btdm_controller_get_sleep_mode() != ESP_BT_SLEEP_MODE_1) {
        return false;
    }

    return !btdm_power_state_active();
}

void esp_bt_controller_wakeup_request(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED ||
        btdm_controller_get_sleep_mode() != ESP_BT_SLEEP_MODE_1) {
        return;
    }

    btdm_wakeup_request(false);
}

int IRAM_ATTR esp_bt_h4tl_eif_io_event_notify(int event)
{
    return btdm_hci_tl_io_event_post(event);
}

uint16_t esp_bt_get_tx_buf_num(void)
{
    return l2c_ble_link_get_tx_buf_num();
}

static void coex_wifi_sleep_set_hook(bool sleep)
{

}
#endif /*  CONFIG_BT_ENABLED */
