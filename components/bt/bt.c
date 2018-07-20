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
#include "esp_intr.h"
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
#include "esp_clk.h"


#if CONFIG_BT_ENABLED

#define BTDM_LOG_TAG                        "BTDM_INIT"

#define BTDM_INIT_PERIOD                    (5000)    /* ms */

/* Bluetooth system and controller config */
#define BTDM_CFG_BT_DATA_RELEASE            (1<<0)
#define BTDM_CFG_HCI_UART                   (1<<1)
#define BTDM_CFG_CONTROLLER_RUN_APP_CPU     (1<<2)
#define BTDM_CFG_SCAN_DUPLICATE_OPTIONS     (1<<3)
#define BTDM_CFG_SEND_ADV_RESERVED_SIZE     (1<<4)
/* Other reserved for future */

/* not for user call, so don't put to include file */
extern void btdm_osi_funcs_register(void *osi_funcs);
extern int btdm_controller_init(uint32_t config_mask, esp_bt_controller_config_t *config_opts);
extern void btdm_controller_deinit(void);
extern int btdm_controller_enable(esp_bt_mode_t mode);
extern void btdm_controller_disable(void);
extern uint8_t btdm_controller_get_mode(void);
extern const char *btdm_controller_get_compile_version(void);
extern void btdm_rf_bb_init(void);
extern void btdm_controller_enable_sleep(bool enable);

#define BTDM_MODEM_SLEEP_MODE_NONE          (0)
#define BTDM_MODEM_SLEEP_MODE_ORIG          (1)
#define BTDM_MODEM_SLEEP_MODE_EVED          (2)
extern void btdm_controller_set_sleep_mode(uint8_t mode);
extern uint8_t btdm_controller_get_sleep_mode(void);
extern bool btdm_power_state_active(void);
extern void btdm_wakeup_request(void);

#define BTDM_LPCLK_SEL_XTAL      (0)
#define BTDM_LPCLK_SEL_XTAL32K   (1)
#define BTDM_LPCLK_SEL_RTC_SLOW  (2)
#define BTDM_LPCLK_SEL_8M        (3)
extern bool btdm_lpclk_select_src(uint32_t sel);
extern bool btdm_lpclk_set_div(uint32_t div);

#define BTDM_MIN_SLEEP_DURATION  (20)
/* VHCI function interface */
typedef struct vhci_host_callback {
    void (*notify_host_send_available)(void);               /*!< callback used to notify that the host can send packet to controller */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   /*!< callback used to notify that the controller has a packet to send to the host*/
} vhci_host_callback_t;

extern bool API_vhci_host_check_send_available(void);
extern void API_vhci_host_send_packet(uint8_t *data, uint16_t len);
extern void API_vhci_host_register_callback(const vhci_host_callback_t *callback);

extern int ble_txpwr_set(int power_type, int power_level);
extern int ble_txpwr_get(int power_type);
extern int bredr_txpwr_set(int min_power_level, int max_power_level);
extern int bredr_txpwr_get(int *min_power_level, int *max_power_level);
extern void bredr_sco_datapath_set(uint8_t data_path);

extern char _bss_start_btdm;
extern char _bss_end_btdm;
extern char _data_start_btdm;
extern char _data_end_btdm;
extern uint32_t _data_start_btdm_rom;
extern uint32_t _data_end_btdm_rom;

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

typedef struct {
    esp_bt_mode_t mode;
    intptr_t start;
    intptr_t end;
} btdm_dram_available_region_t;

/* the mode column will be modified by release function to indicate the available region */
static btdm_dram_available_region_t btdm_dram_available_region[] = {
    //following is .data
    {ESP_BT_MODE_BTDM,          0x3ffae6e0, 0x3ffaff10},
    //following is memory which HW will use
    {ESP_BT_MODE_BTDM,          0x3ffb0000, 0x3ffb09a8},
    {ESP_BT_MODE_BLE,           0x3ffb09a8, 0x3ffb1ddc},
    {ESP_BT_MODE_BTDM,          0x3ffb1ddc, 0x3ffb2730},
    {ESP_BT_MODE_CLASSIC_BT,    0x3ffb2730, 0x3ffb8000},
    //following is .bss
    {ESP_BT_MODE_BTDM,          0x3ffb8000, 0x3ffbbb28},
    {ESP_BT_MODE_CLASSIC_BT,    0x3ffbbb28, 0x3ffbdb28},
    {ESP_BT_MODE_BTDM,          0x3ffbdb28, 0x3ffc0000},
};

#if CONFIG_SPIRAM_USE_MALLOC
typedef struct {
    QueueHandle_t handle;
    void *storage;
    void *buffer;
} btdm_queue_item_t;
#define BTDM_MAX_QUEUE_NUM       (5)
static btdm_queue_item_t btdm_queue_table[BTDM_MAX_QUEUE_NUM];
SemaphoreHandle_t btdm_queue_table_mux = NULL;
#endif /* #if CONFIG_SPIRAM_USE_MALLOC */

struct osi_funcs_t {
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
    void (* _free)(void *p);
    int32_t (* _read_efuse_mac)(uint8_t mac[6]);
    void (* _srand)(unsigned int seed);
    int (* _rand)(void);
    uint32_t (* _btdm_lpcycles_2_us)(uint32_t cycles);
    uint32_t (* _btdm_us_2_lpcycles)(uint32_t us);
    bool (* _btdm_sleep_check_duration)(uint32_t *slot_cnt);
    void (* _btdm_sleep_enter)(void);
    void (* _btdm_sleep_exit)(void);  /* called from ISR */
};


/* Static variable declare */
static bool btdm_bb_init_flag = false;
static esp_bt_controller_status_t btdm_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

static portMUX_TYPE global_int_mux = portMUX_INITIALIZER_UNLOCKED;

// measured average low power clock period in micro seconds
static uint32_t btdm_lpcycle_us = 0;
static uint8_t btdm_lpcycle_us_frac = 0; // number of fractional bit for btdm_lpcycle_us

#ifdef CONFIG_PM_ENABLE
static esp_pm_lock_handle_t s_pm_lock;
#endif

#if CONFIG_SPIRAM_USE_MALLOC
bool IRAM_ATTR btdm_queue_generic_register(const btdm_queue_item_t *queue)
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

bool IRAM_ATTR btdm_queue_generic_deregister(btdm_queue_item_t *queue)
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

static void *IRAM_ATTR semphr_create_wrapper(uint32_t max, uint32_t init)
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

static void IRAM_ATTR semphr_delete_wrapper(void *semphr)
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

static int32_t IRAM_ATTR semphr_take_wrapper(void *semphr, uint32_t block_time_ms)
{
    if (block_time_ms == OSI_FUNCS_TIME_BLOCKING) {
        return (int32_t)xSemaphoreTake(semphr, portMAX_DELAY);
    } else {
        return (int32_t)xSemaphoreTake(semphr, block_time_ms / portTICK_PERIOD_MS);
    }
}

static int32_t IRAM_ATTR semphr_give_wrapper(void *semphr)
{
    return (int32_t)xSemaphoreGive(semphr);
}

static void *IRAM_ATTR mutex_create_wrapper(void)
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

static void IRAM_ATTR mutex_delete_wrapper(void *mutex)
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

static int32_t IRAM_ATTR mutex_lock_wrapper(void *mutex)
{
    return (int32_t)xSemaphoreTake(mutex, portMAX_DELAY);
}

static int32_t IRAM_ATTR mutex_unlock_wrapper(void *mutex)
{
    return (int32_t)xSemaphoreGive(mutex);
}

static void *IRAM_ATTR queue_create_wrapper(uint32_t queue_len, uint32_t item_size)
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

static void IRAM_ATTR queue_delete_wrapper(void *queue)
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

static int32_t IRAM_ATTR queue_send_wrapper(void *queue, void *item, uint32_t block_time_ms)
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

static int32_t IRAM_ATTR queue_recv_wrapper(void *queue, void *item, uint32_t block_time_ms)
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

static int32_t IRAM_ATTR task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id)
{
    return (uint32_t)xTaskCreatePinnedToCore(task_func, name, stack_depth, param, prio, task_handle, (core_id < portNUM_PROCESSORS ? core_id : tskNO_AFFINITY));
}

static void IRAM_ATTR task_delete_wrapper(void *task_handle)
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
    // The number of lp cycles should not lead to overflow. Thrs: 100s (for 32kHz freq)
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
    /* wake up 3 slots in advance */
    *slot_cnt = *slot_cnt -3;
    return true;
}

static void IRAM_ATTR btdm_sleep_enter_wrapper(void)
{
    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        esp_modem_sleep_enter(MODEM_BLE_MODULE);
        esp_modem_sleep_enter(MODEM_CLASSIC_BT_MODULE);
    } else if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_EVED) {
        esp_modem_sleep_enter(MODEM_BLE_MODULE);
        // pause bluetooth baseband
        periph_module_disable(PERIPH_BT_BASEBAND_MODULE);
    }
}

static void IRAM_ATTR btdm_sleep_exit_wrapper(void)
{
    if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_ORIG) {
        esp_modem_sleep_exit(MODEM_BLE_MODULE);
        esp_modem_sleep_exit(MODEM_CLASSIC_BT_MODULE);
    } else if (btdm_controller_get_sleep_mode() == BTDM_MODEM_SLEEP_MODE_EVED) {
        // resume bluetooth baseband
        periph_module_enable(PERIPH_BT_BASEBAND_MODULE);
        esp_modem_sleep_exit(MODEM_BLE_MODULE);
    }
}

static struct osi_funcs_t osi_funcs = {
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
    ._free = free,
    ._read_efuse_mac = read_mac_wrapper,
    ._srand = srand_wrapper,
    ._rand = rand_wrapper,
    ._btdm_lpcycles_2_us = btdm_lpcycles_2_us,
    ._btdm_us_2_lpcycles = btdm_us_2_lpcycles,
    ._btdm_sleep_check_duration = btdm_sleep_check_duration,
    ._btdm_sleep_enter = btdm_sleep_enter_wrapper,
    ._btdm_sleep_exit = btdm_sleep_exit_wrapper,
};

bool esp_vhci_host_check_send_available(void)
{
    return API_vhci_host_check_send_available();
}

void esp_vhci_host_send_packet(uint8_t *data, uint16_t len)
{
    if (!btdm_power_state_active()) {
        btdm_wakeup_request();
    }
    API_vhci_host_send_packet(data, len);
}

void esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback)
{
    API_vhci_host_register_callback((const vhci_host_callback_t *)callback);
}

static uint32_t btdm_config_mask_load(void)
{
    uint32_t mask = 0x0;

    if (btdm_dram_available_region[0].mode == ESP_BT_MODE_BLE) {
        mask |= BTDM_CFG_BT_DATA_RELEASE;
    }

#if CONFIG_BTDM_CONTROLLER_HCI_MODE_UART_H4
    mask |= BTDM_CFG_HCI_UART;
#endif
#if CONFIG_BTDM_CONTROLLER_PINNED_TO_CORE == 1
    mask |= BTDM_CFG_CONTROLLER_RUN_APP_CPU;
#endif
    mask |= BTDM_CFG_SCAN_DUPLICATE_OPTIONS;

    mask |= BTDM_CFG_SEND_ADV_RESERVED_SIZE;

    return mask;
}

static void btdm_controller_mem_init(void)
{
    /* initialise .bss, .data and .etc section */
    memcpy(&_data_start_btdm, (void *)_data_start_btdm_rom, &_data_end_btdm - &_data_start_btdm);
    ESP_LOGD(BTDM_LOG_TAG, ".data initialise [0x%08x] <== [0x%08x]\n", (uint32_t)&_data_start_btdm, _data_start_btdm_rom);

    for (int i = 1; i < sizeof(btdm_dram_available_region)/sizeof(btdm_dram_available_region_t); i++) {
        if (btdm_dram_available_region[i].mode != ESP_BT_MODE_IDLE) {
            memset((void *)btdm_dram_available_region[i].start, 0x0, btdm_dram_available_region[i].end - btdm_dram_available_region[i].start);
            ESP_LOGD(BTDM_LOG_TAG, ".bss initialise [0x%08x] - [0x%08x]\n", btdm_dram_available_region[i].start, btdm_dram_available_region[i].end);
        }
    }
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
                ESP_LOGD(BTDM_LOG_TAG, "Release DRAM [0x%08x] - [0x%08x]\n", mem_start, mem_end);
                ESP_ERROR_CHECK( heap_caps_add_region(mem_start, mem_end));
                update = true;
            }
        } else {
            mem_end = btdm_dram_available_region[i].end;
            ESP_LOGD(BTDM_LOG_TAG, "Release DRAM [0x%08x] - [0x%08x]\n", mem_start, mem_end);
            ESP_ERROR_CHECK( heap_caps_add_region(mem_start, mem_end));
            update = true;
        }
    }

    return ESP_OK;
}

esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg)
{
    BaseType_t ret;
    uint32_t btdm_cfg_mask = 0;

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

#ifdef CONFIG_PM_ENABLE
    esp_err_t err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "bt", &s_pm_lock);
    if (err != ESP_OK) {
        return err;
    }
#endif

    ESP_LOGI(BTDM_LOG_TAG, "BT controller compile version [%s]\n", btdm_controller_get_compile_version());

#if CONFIG_SPIRAM_USE_MALLOC
    btdm_queue_table_mux = xSemaphoreCreateMutex();
    if (btdm_queue_table == NULL) {
#ifdef CONFIG_PM_ENABLE
        esp_pm_lock_delete(s_pm_lock);
        s_pm_lock = NULL;
#endif
        return ESP_ERR_NO_MEM;
    }
    memset(btdm_queue_table, 0, sizeof(btdm_queue_item_t) * BTDM_MAX_QUEUE_NUM);
#endif

    btdm_osi_funcs_register(&osi_funcs);

    btdm_controller_mem_init();

    periph_module_enable(PERIPH_BT_MODULE);

    btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
    btdm_lpcycle_us = 32 << btdm_lpcycle_us_frac;
#if CONFIG_BTDM_MODEM_SLEEP_MODE_ORIG
    bool select_src_ret = false;
    bool set_div_ret = false;
#if CONFIG_BTDM_LPCLK_SEL_MAIN_XTAL
    select_src_ret = btdm_lpclk_select_src(BTDM_LPCLK_SEL_XTAL);
    set_div_ret = btdm_lpclk_set_div(rtc_clk_xtal_freq_get() * 32 - 1);
    assert(select_src_ret && set_div_ret);
    btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
    btdm_lpcycle_us = 32 << btdm_lpcycle_us_frac;
#elif CONFIG_BTDM_LPCLK_SEL_EXT_32K_XTAL
    select_src_ret = btdm_lpclk_select_src(BTDM_LPCLK_SEL_XTAL32K);
    set_div_ret = btdm_lpclk_set_div(0);
    assert(select_src_ret && set_div_ret);
    btdm_lpcycle_us_frac = RTC_CLK_CAL_FRACT;
    btdm_lpcycle_us = esp_clk_slowclk_cal_get();
    assert(btdm_lpcycle_us != 0);
#endif // CONFIG_BTDM_LPCLK_SEL_XX
    btdm_controller_set_sleep_mode(BTDM_MODEM_SLEEP_MODE_ORIG);
#elif CONFIG_BTDM_MODEM_SLEEP_MODE_EVED
    btdm_controller_set_sleep_mode(BTDM_MODEM_SLEEP_MODE_EVED);
#else
    btdm_controller_set_sleep_mode(BTDM_MODEM_SLEEP_MODE_NONE);
#endif

    btdm_cfg_mask = btdm_config_mask_load();

    ret = btdm_controller_init(btdm_cfg_mask, cfg);
    if (ret) {
#ifdef CONFIG_PM_ENABLE
        esp_pm_lock_delete(s_pm_lock);
        s_pm_lock = NULL;
#endif
        return ESP_ERR_NO_MEM;
    }

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;
    return ESP_OK;
}

esp_err_t esp_bt_controller_deinit(void)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        return ESP_ERR_INVALID_STATE;
    }

    btdm_controller_deinit();

    periph_module_disable(PERIPH_BT_MODULE);

#if CONFIG_SPIRAM_USE_MALLOC
    vSemaphoreDelete(btdm_queue_table_mux);
    btdm_queue_table_mux = NULL;
    memset(btdm_queue_table, 0, sizeof(btdm_queue_item_t) * BTDM_MAX_QUEUE_NUM);
#endif

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

    btdm_lpcycle_us = 0;
    btdm_controller_set_sleep_mode(BTDM_MODEM_SLEEP_MODE_NONE);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_delete(s_pm_lock);
    s_pm_lock = NULL;
#endif

    return ESP_OK;
}

esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode)
{
    int ret;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        return ESP_ERR_INVALID_STATE;
    }

    //check the mode is available mode
    if (mode & ~btdm_dram_available_region[0].mode) {
        return ESP_ERR_INVALID_ARG;
    }

#ifdef CONFIG_PM_ENABLE
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

    if (btdm_bb_init_flag == false) {
        btdm_bb_init_flag = true;
        btdm_rf_bb_init();  /* only initialise once */
    }

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
            btdm_wakeup_request();
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

    btdm_wakeup_request();
}

esp_err_t esp_bredr_sco_datapath_set(esp_sco_data_path_t data_path)
{
    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    bredr_sco_datapath_set(data_path);
    return ESP_OK;
}

#endif /*  CONFIG_BT_ENABLED */
