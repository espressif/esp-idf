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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/portmacro.h"
#include "esp_types.h"
#include "esp_system.h"
#include "esp_task.h"
#include "esp_intr.h"
#include "esp_attr.h"
#include "esp_phy_init.h"
#include "bt.h"

#if CONFIG_BT_ENABLED

/* Bluetooth system and controller config */
#define BTDM_CFG_BT_EM_RELEASE              (1<<0)
#define BTDM_CFG_BT_DATA_RELEASE            (1<<1)
#define BTDM_CFG_HCI_UART                   (1<<2)
#define BTDM_CFG_CONTROLLER_RUN_APP_CPU     (1<<3)
/* Other reserved for future */

/* not for user call, so don't put to include file */
extern void btdm_osi_funcs_register(void *osi_funcs);
extern void btdm_controller_init(uint32_t config_mask, esp_bt_controller_config_t *config_opts);
extern void btdm_controller_schedule(void);
extern void btdm_controller_deinit(void);
extern int btdm_controller_enable(esp_bt_mode_t mode);
extern int btdm_controller_disable(esp_bt_mode_t mode);
extern void btdm_rf_bb_init(void);

/* VHCI function interface */
typedef struct vhci_host_callback {
    void (*notify_host_send_available)(void);               /*!< callback used to notify that the host can send packet to controller */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   /*!< callback used to notify that the controller has a packet to send to the host*/
} vhci_host_callback_t;

extern bool API_vhci_host_check_send_available(void);
extern void API_vhci_host_send_packet(uint8_t *data, uint16_t len);
extern void API_vhci_host_register_callback(const vhci_host_callback_t *callback);

#define BT_DEBUG(...)
#define BT_API_CALL_CHECK(info, api_call, ret) \
do{\
    esp_err_t __err = (api_call);\
    if ((ret) != __err) {\
        BT_DEBUG("%s %d %s ret=%d\n", __FUNCTION__, __LINE__, (info), __err);\
        return __err;\
    }\
} while(0)

struct osi_funcs_t {
    xt_handler (*_set_isr)(int n, xt_handler f, void *arg);
    void (*_ints_on)(unsigned int mask);
    void (*_interrupt_disable)(void);
    void (*_interrupt_restore)(void);
    void (*_task_yield)(void);
    void *(*_semphr_create)(uint32_t max, uint32_t init);
    int32_t (*_semphr_give_from_isr)(void *semphr, void *hptw);
    int32_t (*_semphr_take)(void *semphr, uint32_t block_time_ms);
    void *(*_mutex_create)(void);
    int32_t (*_mutex_lock)(void *mutex);
    int32_t (*_mutex_unlock)(void *mutex);
    int32_t (* _read_efuse_mac)(uint8_t mac[6]);
    void (* _srand)(unsigned int seed);
    int (* _rand)(void);
};

/* Static variable declare */
static bool btdm_bb_init_flag = false;
static esp_bt_controller_status_t btdm_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;
static esp_bt_controller_config_t btdm_cfg_opts;
static xTaskHandle btControllerTaskHandle;

static portMUX_TYPE global_int_mux = portMUX_INITIALIZER_UNLOCKED;

static void IRAM_ATTR interrupt_disable(void)
{
    portENTER_CRITICAL(&global_int_mux);
}

static void IRAM_ATTR interrupt_restore(void)
{
    portEXIT_CRITICAL(&global_int_mux);
}

static void *IRAM_ATTR semphr_create_wrapper(uint32_t max, uint32_t init)
{
    return (void *)xSemaphoreCreateCounting(max, init);
}

static int32_t IRAM_ATTR semphr_give_from_isr_wrapper(void *semphr, void *hptw)
{
    return (int32_t)xSemaphoreGiveFromISR(semphr, hptw);
}

static int32_t IRAM_ATTR semphr_take_wrapper(void *semphr, uint32_t block_time_ms)
{
    return (int32_t)xSemaphoreTake(semphr, block_time_ms / portTICK_PERIOD_MS);
}

static void *IRAM_ATTR mutex_create_wrapper(void)
{
    return (void *)xSemaphoreCreateMutex();
}

static int32_t IRAM_ATTR mutex_lock_wrapper(void *mutex)
{
    return (int32_t)xSemaphoreTake(mutex, portMAX_DELAY);
}

static int32_t IRAM_ATTR mutex_unlock_wrapper(void *mutex)
{
    return (int32_t)xSemaphoreGive(mutex);
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

static struct osi_funcs_t osi_funcs = {
    ._set_isr = xt_set_interrupt_handler,
    ._ints_on = xt_ints_on,
    ._interrupt_disable = interrupt_disable,
    ._interrupt_restore = interrupt_restore,
    ._task_yield = vPortYield,
    ._semphr_create = semphr_create_wrapper,
    ._semphr_give_from_isr = semphr_give_from_isr_wrapper,
    ._semphr_take = semphr_take_wrapper,
    ._mutex_create = mutex_create_wrapper,
    ._mutex_lock = mutex_lock_wrapper,
    ._mutex_unlock = mutex_unlock_wrapper,
    ._read_efuse_mac = read_mac_wrapper,
    ._srand = srand_wrapper,
    ._rand = rand_wrapper,
};

bool esp_vhci_host_check_send_available(void)
{
    return API_vhci_host_check_send_available();
}

void esp_vhci_host_send_packet(uint8_t *data, uint16_t len)
{
    API_vhci_host_send_packet(data, len);
}

void esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback)
{
    API_vhci_host_register_callback((const vhci_host_callback_t *)callback);
}

static uint32_t btdm_config_mask_load(void)
{
    uint32_t mask = 0x0;

#ifdef CONFIG_BT_DRAM_RELEASE
    mask |= (BTDM_CFG_BT_EM_RELEASE | BTDM_CFG_BT_DATA_RELEASE);
#endif
#ifdef CONFIG_BT_HCI_UART
    mask |= BTDM_CFG_HCI_UART;
#endif
#ifdef CONFIG_BTDM_CONTROLLER_RUN_APP_CPU
    mask |= BTDM_CFG_CONTROLLER_RUN_APP_CPU;
#endif
    return mask;
}

static void bt_controller_task(void *pvParam)
{
    uint32_t btdm_cfg_mask = 0;

    btdm_cfg_mask = btdm_config_mask_load();

    btdm_osi_funcs_register(&osi_funcs);

    btdm_controller_init(btdm_cfg_mask, &btdm_cfg_opts);

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;
    /* Loop */
    btdm_controller_schedule();
}

esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg)
{
    BaseType_t ret;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(&btdm_cfg_opts, cfg, sizeof(esp_bt_controller_config_t));

    ret = xTaskCreatePinnedToCore(bt_controller_task, "btController",
                            ESP_TASK_BT_CONTROLLER_STACK, NULL,
                            ESP_TASK_BT_CONTROLLER_PRIO, &btControllerTaskHandle, CONFIG_BTDM_CONTROLLER_RUN_CPU);

    if (ret != pdPASS) {
        memset(&btdm_cfg_opts, 0x0, sizeof(esp_bt_controller_config_t));
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

void esp_bt_controller_deinit(void)
{
    memset(&btdm_cfg_opts, 0x0, sizeof(esp_bt_controller_config_t));
    vTaskDelete(btControllerTaskHandle);
    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;
}

esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode)
{
    int ret;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (mode != ESP_BT_MODE_BTDM) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_phy_load_cal_and_init();

    if (btdm_bb_init_flag == false) {
        btdm_bb_init_flag = true;
        btdm_rf_bb_init();  /* only initialise once */
    }

    ret = btdm_controller_enable(mode);
    if (ret) {
        return ESP_ERR_INVALID_STATE;
    }

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_ENABLED;

    return ESP_OK;
}

esp_err_t esp_bt_controller_disable(esp_bt_mode_t mode)
{
    int ret;

    if (btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (mode != ESP_BT_MODE_BTDM) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = btdm_controller_disable(mode);
    if (ret) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_phy_rf_deinit();

    btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

    return ESP_OK;
}

esp_bt_controller_status_t esp_bt_controller_get_status(void)
{
    return btdm_controller_status;
}

#endif
