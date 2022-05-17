// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
#include <string.h>
#include <stdio.h>
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "malloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "soc/dport_reg.h"
#include "esp_pm.h"
#include "soc/soc_memory_layout.h"
#include "hal/i2c_hal.h"
#include "soc/i2c_periph.h"
#include "driver/i2c.h"
#include "driver/periph_ctrl.h"

static const char *I2C_TAG = "i2c";
#define I2C_CHECK(a, str, ret)  if(!(a)) {                                             \
        ESP_LOGE(I2C_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);      \
        return (ret);                                                                   \
        }

/* DRAM_ATTR is required to avoid I2C array placed in flash, due to accessed from ISR */

#define I2C_ENTER_CRITICAL_ISR(mux)    portENTER_CRITICAL_ISR(mux)
#define I2C_EXIT_CRITICAL_ISR(mux)     portEXIT_CRITICAL_ISR(mux)
#define I2C_ENTER_CRITICAL(mux)        portENTER_CRITICAL(mux)
#define I2C_EXIT_CRITICAL(mux)         portEXIT_CRITICAL(mux)

#define I2C_DRIVER_ERR_STR             "i2c driver install error"
#define I2C_DRIVER_MALLOC_ERR_STR      "i2c driver malloc error"
#define I2C_NUM_ERROR_STR              "i2c number error"
#define I2C_TIMEING_VAL_ERR_STR        "i2c timing value error"
#define I2C_ADDR_ERROR_STR             "i2c null address error"
#define I2C_DRIVER_NOT_INSTALL_ERR_STR "i2c driver not installed"
#define I2C_SLAVE_BUFFER_LEN_ERR_STR   "i2c buffer size too small for slave mode"
#define I2C_EVT_QUEUE_ERR_STR          "i2c evt queue error"
#define I2C_SEM_ERR_STR                "i2c semaphore error"
#define I2C_BUF_ERR_STR                "i2c ringbuffer error"
#define I2C_MASTER_MODE_ERR_STR        "Only allowed in master mode"
#define I2C_MODE_SLAVE_ERR_STR         "Only allowed in slave mode"
#define I2C_CMD_MALLOC_ERR_STR         "i2c command link malloc error"
#define I2C_TRANS_MODE_ERR_STR         "i2c trans mode error"
#define I2C_MODE_ERR_STR               "i2c mode error"
#define I2C_SDA_IO_ERR_STR             "sda gpio number error"
#define I2C_SCL_IO_ERR_STR             "scl gpio number error"
#define I2C_SCL_SDA_EQUAL_ERR_STR      "scl and sda gpio numbers are the same"
#define I2C_CMD_LINK_INIT_ERR_STR      "i2c command link error"
#define I2C_GPIO_PULLUP_ERR_STR        "this i2c pin does not support internal pull-up"
#define I2C_ACK_TYPE_ERR_STR           "i2c ack type error"
#define I2C_DATA_LEN_ERR_STR           "i2c data read length error"
#define I2C_PSRAM_BUFFER_WARN_STR      "Using buffer allocated from psram"
#define I2C_LOCK_ERR_STR               "Power lock creation error"
#define I2C_FIFO_FULL_THRESH_VAL       (28)
#define I2C_FIFO_EMPTY_THRESH_VAL      (5)
#define I2C_IO_INIT_LEVEL              (1)
#define I2C_CMD_ALIVE_INTERVAL_TICK    (1000 / portTICK_PERIOD_MS)
#define I2C_CMD_EVT_ALIVE              (0)
#define I2C_CMD_EVT_DONE               (1)
#define I2C_EVT_QUEUE_LEN              (1)
#define I2C_SLAVE_TIMEOUT_DEFAULT      (32000)     /* I2C slave timeout value, APB clock cycle number */
#define I2C_SLAVE_SDA_SAMPLE_DEFAULT   (10)        /* I2C slave sample time after scl positive edge default value */
#define I2C_SLAVE_SDA_HOLD_DEFAULT     (10)        /* I2C slave hold time after scl negative edge default value */
#define I2C_MASTER_TOUT_CNUM_DEFAULT   (8)         /* I2C master timeout cycle number of I2C clock, after which the timeout interrupt will be triggered */
#define I2C_ACKERR_CNT_MAX             (10)
#define I2C_FILTER_CYC_NUM_DEF         (7)         /* The number of apb cycles filtered by default*/
#define I2C_CLR_BUS_SCL_NUM            (9)
#define I2C_CLR_BUS_HALF_PERIOD_US     (5)

#define I2C_CONTEX_INIT_DEF(uart_num) {\
    .hal.dev = I2C_LL_GET_HW(uart_num),\
    .spinlock = portMUX_INITIALIZER_UNLOCKED,\
    .hw_enabled = false,\
}

typedef struct {
    i2c_hw_cmd_t hw_cmd;
    uint8_t *data;     /*!< data address */
    uint8_t byte_cmd;  /*!< to save cmd for one byte command mode */
} i2c_cmd_t;

typedef struct i2c_cmd_link {
    i2c_cmd_t cmd;              /*!< command in current cmd link */
    struct i2c_cmd_link *next;  /*!< next cmd link */
} i2c_cmd_link_t;

typedef struct {
    i2c_cmd_link_t *head;     /*!< head of the command link */
    i2c_cmd_link_t *cur;      /*!< last node of the command link */
    i2c_cmd_link_t *free;     /*!< the first node to free of the command link */
} i2c_cmd_desc_t;

typedef enum {
    I2C_STATUS_READ,      /*!< read status for current master command */
    I2C_STATUS_WRITE,     /*!< write status for current master command */
    I2C_STATUS_IDLE,      /*!< idle status for current master command */
    I2C_STATUS_ACK_ERROR, /*!< ack error status for current master command */
    I2C_STATUS_DONE,      /*!< I2C command done */
    I2C_STATUS_TIMEOUT,   /*!< I2C bus status error, and operation timeout */
} i2c_status_t;

typedef struct {
    int type;
} i2c_cmd_evt_t;

typedef struct {
    int i2c_num;                     /*!< I2C port number */
    int mode;                        /*!< I2C mode, master or slave */
    intr_handle_t intr_handle;       /*!< I2C interrupt handle*/
    int cmd_idx;                     /*!< record current command index, for master mode */
    int status;                      /*!< record current command status, for master mode */
    int rx_cnt;                      /*!< record current read index, for master mode */
    uint8_t data_buf[SOC_I2C_FIFO_LEN ];  /*!< a buffer to store i2c fifo data */

    i2c_cmd_desc_t cmd_link;         /*!< I2C command link */
    QueueHandle_t cmd_evt_queue;     /*!< I2C command event queue */
#if CONFIG_SPIRAM_USE_MALLOC
    uint8_t *evt_queue_storage;      /*!< The buffer that will hold the items in the queue */
    int intr_alloc_flags;            /*!< Used to allocate the interrupt */
    StaticQueue_t evt_queue_buffer;  /*!< The buffer that will hold the queue structure*/
#endif
    xSemaphoreHandle cmd_mux;        /*!< semaphore to lock command process */
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif

    xSemaphoreHandle slv_rx_mux;     /*!< slave rx buffer mux */
    xSemaphoreHandle slv_tx_mux;     /*!< slave tx buffer mux */
    size_t rx_buf_length;            /*!< rx buffer length */
    RingbufHandle_t rx_ring_buf;     /*!< rx ringbuffer handler of slave mode */
    size_t tx_buf_length;            /*!< tx buffer length */
    RingbufHandle_t tx_ring_buf;     /*!< tx ringbuffer handler of slave mode */
} i2c_obj_t;

typedef struct {
    i2c_hal_context_t hal;      /*!< I2C hal context */
    portMUX_TYPE spinlock;
    bool hw_enabled;
#if !I2C_SUPPORT_HW_CLR_BUS
    int scl_io_num;
    int sda_io_num;
#endif
} i2c_context_t;

static i2c_context_t i2c_context[I2C_NUM_MAX] = {
    I2C_CONTEX_INIT_DEF(I2C_NUM_0),
    I2C_CONTEX_INIT_DEF(I2C_NUM_1),
};

static i2c_obj_t *p_i2c_obj[I2C_NUM_MAX] = {0};
static void i2c_isr_handler_default(void *arg);
static void IRAM_ATTR i2c_master_cmd_begin_static(i2c_port_t i2c_num, portBASE_TYPE* HPTaskAwoken);
static esp_err_t IRAM_ATTR i2c_hw_fsm_reset(i2c_port_t i2c_num);

static void i2c_hw_disable(i2c_port_t i2c_num)
{
    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    if (i2c_context[i2c_num].hw_enabled != false) {
        periph_module_disable(i2c_periph_signal[i2c_num].module);
        i2c_context[i2c_num].hw_enabled = false;
    }
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
}

static void i2c_hw_enable(i2c_port_t i2c_num)
{
    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    if (i2c_context[i2c_num].hw_enabled != true) {
        periph_module_enable(i2c_periph_signal[i2c_num].module);
        i2c_context[i2c_num].hw_enabled = true;
    }
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
}

/*
    For i2c master mode, we don't need to use a buffer for the data, the APIs will execute the master commands
and return after all of the commands have been sent out or when error occurs. So when we send master commands,
we should free or modify the source data only after the i2c_master_cmd_begin function returns.
    For i2c slave mode, we need a data buffer to stash the sending and receiving data, because the hardware fifo
has only 32 bytes.
*/
esp_err_t i2c_driver_install(i2c_port_t i2c_num, i2c_mode_t mode, size_t slv_rx_buf_len, size_t slv_tx_buf_len,
                             int intr_alloc_flags)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(mode == I2C_MODE_MASTER || ( slv_rx_buf_len > 100 || slv_tx_buf_len > 100 ), I2C_SLAVE_BUFFER_LEN_ERR_STR,
              ESP_ERR_INVALID_ARG);
    if (p_i2c_obj[i2c_num] == NULL) {

#if !CONFIG_SPIRAM_USE_MALLOC
        p_i2c_obj[i2c_num] = (i2c_obj_t *) calloc(1, sizeof(i2c_obj_t));
#else
        if ( !(intr_alloc_flags & ESP_INTR_FLAG_IRAM) ) {
            p_i2c_obj[i2c_num] = (i2c_obj_t *) calloc(1, sizeof(i2c_obj_t));
        } else {
            p_i2c_obj[i2c_num] = (i2c_obj_t *) heap_caps_calloc(1, sizeof(i2c_obj_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        }
#endif
        if (p_i2c_obj[i2c_num] == NULL) {
            ESP_LOGE(I2C_TAG, I2C_DRIVER_MALLOC_ERR_STR);
            return ESP_FAIL;
        }
        i2c_obj_t *p_i2c = p_i2c_obj[i2c_num];
        p_i2c->i2c_num = i2c_num;
        p_i2c->mode = mode;
        p_i2c->cmd_idx = 0;
        p_i2c->rx_cnt = 0;
        p_i2c->status = I2C_STATUS_IDLE;

#if CONFIG_SPIRAM_USE_MALLOC
        p_i2c->intr_alloc_flags = intr_alloc_flags;
#endif

        if (mode == I2C_MODE_SLAVE) {
            //we only use ringbuffer for slave mode.
            if (slv_rx_buf_len > 0) {
                p_i2c->rx_ring_buf = xRingbufferCreate(slv_rx_buf_len, RINGBUF_TYPE_BYTEBUF);
                if (p_i2c->rx_ring_buf == NULL) {
                    ESP_LOGE(I2C_TAG, I2C_BUF_ERR_STR);
                    goto err;
                }
                p_i2c->rx_buf_length = slv_rx_buf_len;
            } else {
                p_i2c->rx_ring_buf = NULL;
                p_i2c->rx_buf_length = 0;
            }
            if (slv_tx_buf_len > 0) {
                p_i2c->tx_ring_buf = xRingbufferCreate(slv_tx_buf_len, RINGBUF_TYPE_BYTEBUF);
                if (p_i2c->tx_ring_buf == NULL) {
                    ESP_LOGE(I2C_TAG, I2C_BUF_ERR_STR);
                    goto err;
                }
                p_i2c->tx_buf_length = slv_tx_buf_len;
            } else {
                p_i2c->tx_ring_buf = NULL;
                p_i2c->tx_buf_length = 0;
            }
            p_i2c->slv_rx_mux = xSemaphoreCreateMutex();
            p_i2c->slv_tx_mux = xSemaphoreCreateMutex();
            if (p_i2c->slv_rx_mux == NULL || p_i2c->slv_tx_mux == NULL) {
                ESP_LOGE(I2C_TAG, I2C_SEM_ERR_STR);
                goto err;
            }
        } else {
            //semaphore to sync sending process, because we only have 32 bytes for hardware fifo.
            p_i2c->cmd_mux = xSemaphoreCreateMutex();
#ifdef CONFIG_PM_ENABLE
            if (esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "i2c_driver", &p_i2c->pm_lock) != ESP_OK) {
                ESP_LOGE(I2C_TAG, I2C_LOCK_ERR_STR);
                goto err;
            }
#endif
#if !CONFIG_SPIRAM_USE_MALLOC
            p_i2c->cmd_evt_queue = xQueueCreate(I2C_EVT_QUEUE_LEN, sizeof(i2c_cmd_evt_t));
#else
            if ( !(intr_alloc_flags & ESP_INTR_FLAG_IRAM) ) {
                p_i2c->cmd_evt_queue = xQueueCreate(I2C_EVT_QUEUE_LEN, sizeof(i2c_cmd_evt_t));
            } else {
                p_i2c->evt_queue_storage = (uint8_t *)heap_caps_calloc(I2C_EVT_QUEUE_LEN, sizeof(i2c_cmd_evt_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
                if ( p_i2c->evt_queue_storage == NULL ) {
                    ESP_LOGE(I2C_TAG, I2C_DRIVER_MALLOC_ERR_STR);
                    goto err;
                }
                memset(&p_i2c->evt_queue_buffer, 0, sizeof(StaticQueue_t));
                p_i2c->cmd_evt_queue =  xQueueCreateStatic(I2C_EVT_QUEUE_LEN, sizeof(i2c_cmd_evt_t), p_i2c->evt_queue_storage, &p_i2c->evt_queue_buffer);
            }
#endif
            if (p_i2c->cmd_mux == NULL || p_i2c->cmd_evt_queue == NULL) {
                ESP_LOGE(I2C_TAG, I2C_SEM_ERR_STR);
                goto err;
            }
            //command link
            p_i2c->cmd_link.cur = NULL;
            p_i2c->cmd_link.head = NULL;
            p_i2c->cmd_link.free = NULL;

            p_i2c->tx_ring_buf = NULL;
            p_i2c->rx_buf_length = 0;
            p_i2c->tx_ring_buf = NULL;
            p_i2c->tx_buf_length = 0;
        }
    } else {
        ESP_LOGE(I2C_TAG, I2C_DRIVER_ERR_STR);
        return ESP_FAIL;
    }
    i2c_hw_enable(i2c_num);
    //Disable I2C interrupt.
    i2c_hal_disable_intr_mask(&(i2c_context[i2c_num].hal), I2C_INTR_MASK);
    i2c_hal_clr_intsts_mask(&(i2c_context[i2c_num].hal), I2C_INTR_MASK);
    //hook isr handler
    i2c_isr_register(i2c_num, i2c_isr_handler_default, p_i2c_obj[i2c_num], intr_alloc_flags, &p_i2c_obj[i2c_num]->intr_handle);
    //Enable I2C slave rx interrupt
    if (mode == I2C_MODE_SLAVE) {
        i2c_hal_enable_slave_rx_it(&(i2c_context[i2c_num].hal));
    }
    return ESP_OK;

err:
    //Some error has happened. Free/destroy all allocated things and return ESP_FAIL.
    if (p_i2c_obj[i2c_num]) {
        if (p_i2c_obj[i2c_num]->rx_ring_buf) {
            vRingbufferDelete(p_i2c_obj[i2c_num]->rx_ring_buf);
            p_i2c_obj[i2c_num]->rx_ring_buf = NULL;
            p_i2c_obj[i2c_num]->rx_buf_length = 0;
        }
        if (p_i2c_obj[i2c_num]->tx_ring_buf) {
            vRingbufferDelete(p_i2c_obj[i2c_num]->tx_ring_buf);
            p_i2c_obj[i2c_num]->tx_ring_buf = NULL;
            p_i2c_obj[i2c_num]->tx_buf_length = 0;
        }
        if (p_i2c_obj[i2c_num]->cmd_evt_queue) {
            vQueueDelete(p_i2c_obj[i2c_num]->cmd_evt_queue);
            p_i2c_obj[i2c_num]->cmd_evt_queue = NULL;
        }
        if (p_i2c_obj[i2c_num]->cmd_mux) {
            vSemaphoreDelete(p_i2c_obj[i2c_num]->cmd_mux);
        }
        if (p_i2c_obj[i2c_num]->slv_rx_mux) {
            vSemaphoreDelete(p_i2c_obj[i2c_num]->slv_rx_mux);
        }
        if (p_i2c_obj[i2c_num]->slv_tx_mux) {
            vSemaphoreDelete(p_i2c_obj[i2c_num]->slv_tx_mux);
        }
#ifdef CONFIG_PM_ENABLE
        if (p_i2c_obj[i2c_num]->pm_lock) {
            esp_pm_lock_delete(p_i2c_obj[i2c_num]->pm_lock);
            p_i2c_obj[i2c_num]->pm_lock = NULL;
        }
#endif
#if CONFIG_SPIRAM_USE_MALLOC
        if (p_i2c_obj[i2c_num]->evt_queue_storage) {
            free(p_i2c_obj[i2c_num]->evt_queue_storage);
            p_i2c_obj[i2c_num]->evt_queue_storage = NULL;
        }
#endif
    }
    free(p_i2c_obj[i2c_num]);
    p_i2c_obj[i2c_num] = NULL;
    return ESP_FAIL;
}

esp_err_t i2c_driver_delete(i2c_port_t i2c_num)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(p_i2c_obj[i2c_num] != NULL, I2C_DRIVER_ERR_STR, ESP_FAIL);

    i2c_obj_t *p_i2c = p_i2c_obj[i2c_num];
    i2c_hal_disable_intr_mask(&(i2c_context[i2c_num].hal), I2C_INTR_MASK);
    esp_intr_free(p_i2c->intr_handle);
    p_i2c->intr_handle = NULL;

    if (p_i2c->cmd_mux) {
        // Let any command in progress finish.
        xSemaphoreTake(p_i2c->cmd_mux, portMAX_DELAY);
        xSemaphoreGive(p_i2c->cmd_mux);
        vSemaphoreDelete(p_i2c->cmd_mux);
    }
    if (p_i2c_obj[i2c_num]->cmd_evt_queue) {
        vQueueDelete(p_i2c_obj[i2c_num]->cmd_evt_queue);
        p_i2c_obj[i2c_num]->cmd_evt_queue = NULL;
    }
    if (p_i2c->slv_rx_mux) {
        vSemaphoreDelete(p_i2c->slv_rx_mux);
    }
    if (p_i2c->slv_tx_mux) {
        vSemaphoreDelete(p_i2c->slv_tx_mux);
    }

    if (p_i2c->rx_ring_buf) {
        vRingbufferDelete(p_i2c->rx_ring_buf);
        p_i2c->rx_ring_buf = NULL;
        p_i2c->rx_buf_length = 0;
    }
    if (p_i2c->tx_ring_buf) {
        vRingbufferDelete(p_i2c->tx_ring_buf);
        p_i2c->tx_ring_buf = NULL;
        p_i2c->tx_buf_length = 0;
    }
#ifdef CONFIG_PM_ENABLE
    if (p_i2c->pm_lock) {
        esp_pm_lock_delete(p_i2c->pm_lock);
        p_i2c->pm_lock = NULL;
    }
#endif
#if CONFIG_SPIRAM_USE_MALLOC
    if (p_i2c_obj[i2c_num]->evt_queue_storage) {
        free(p_i2c_obj[i2c_num]->evt_queue_storage);
        p_i2c_obj[i2c_num]->evt_queue_storage = NULL;
    }
#endif

    free(p_i2c_obj[i2c_num]);
    p_i2c_obj[i2c_num] = NULL;

    i2c_hw_disable(i2c_num);
    return ESP_OK;
}

esp_err_t i2c_reset_tx_fifo(i2c_port_t i2c_num)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_txfifo_rst(&(i2c_context[i2c_num].hal));
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_reset_rx_fifo(i2c_port_t i2c_num)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_rxfifo_rst(&(i2c_context[i2c_num].hal));
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

static void IRAM_ATTR i2c_isr_handler_default(void *arg)
{
    i2c_obj_t *p_i2c = (i2c_obj_t *) arg;
    int i2c_num = p_i2c->i2c_num;
    // Interrupt protection.
    // On S2 target, the I2C may trigger a spurious interrupt,
    // in order to detect these false positive, check the I2C's hardware interrupt mask
    uint32_t int_mask;
    i2c_hal_get_intsts_mask(&(i2c_context[i2c_num].hal), &int_mask);
    if (int_mask == 0) {
        return;
    }
    i2c_intr_event_t evt_type = I2C_INTR_EVENT_ERR;
    portBASE_TYPE HPTaskAwoken = pdFALSE;
    portBASE_TYPE HPTaskAwokenCallee = pdFALSE;
    if (p_i2c->mode == I2C_MODE_MASTER) {
        if (p_i2c->status == I2C_STATUS_WRITE) {
            i2c_hal_master_handle_tx_event(&(i2c_context[i2c_num].hal), &evt_type);
        } else if (p_i2c->status == I2C_STATUS_READ) {
            i2c_hal_master_handle_rx_event(&(i2c_context[i2c_num].hal), &evt_type);
        }
        if (evt_type == I2C_INTR_EVENT_NACK) {
            p_i2c_obj[i2c_num]->status = I2C_STATUS_ACK_ERROR;
            i2c_master_cmd_begin_static(i2c_num, &HPTaskAwokenCallee);
        } else if (evt_type == I2C_INTR_EVENT_TOUT) {
            p_i2c_obj[i2c_num]->status = I2C_STATUS_TIMEOUT;
            i2c_master_cmd_begin_static(i2c_num, &HPTaskAwokenCallee);
        } else if (evt_type == I2C_INTR_EVENT_ARBIT_LOST) {
            p_i2c_obj[i2c_num]->status = I2C_STATUS_TIMEOUT;
            i2c_master_cmd_begin_static(i2c_num, &HPTaskAwokenCallee);
        } else if (evt_type == I2C_INTR_EVENT_END_DET) {
            i2c_master_cmd_begin_static(i2c_num, &HPTaskAwokenCallee);
        } else if (evt_type == I2C_INTR_EVENT_TRANS_DONE) {
            if (p_i2c->status != I2C_STATUS_ACK_ERROR && p_i2c->status != I2C_STATUS_IDLE) {
                i2c_master_cmd_begin_static(i2c_num, &HPTaskAwokenCallee);
            }
        } else {
            // Do nothing if there is no proper event.
            return;
        }
        i2c_cmd_evt_t evt = {
            .type = I2C_CMD_EVT_ALIVE
        };
        xQueueSendFromISR(p_i2c->cmd_evt_queue, &evt, &HPTaskAwoken);
    } else {
        i2c_hal_slave_handle_event(&(i2c_context[i2c_num].hal), &evt_type);
        if (evt_type == I2C_INTR_EVENT_TRANS_DONE || evt_type == I2C_INTR_EVENT_RXFIFO_FULL) {
            uint32_t rx_fifo_cnt;
            i2c_hal_get_rxfifo_cnt(&(i2c_context[i2c_num].hal), &rx_fifo_cnt);
            i2c_hal_read_rxfifo(&(i2c_context[i2c_num].hal), p_i2c->data_buf, rx_fifo_cnt);
            xRingbufferSendFromISR(p_i2c->rx_ring_buf, p_i2c->data_buf, rx_fifo_cnt, &HPTaskAwoken);
            i2c_hal_slave_clr_rx_it(&(i2c_context[i2c_num].hal));
        } else if (evt_type == I2C_INTR_EVENT_TXFIFO_EMPTY) {
            uint32_t tx_fifo_rem;
            i2c_hal_get_txfifo_cnt(&(i2c_context[i2c_num].hal), &tx_fifo_rem);
            size_t size = 0;
            uint8_t *data = (uint8_t *) xRingbufferReceiveUpToFromISR(p_i2c->tx_ring_buf, &size, tx_fifo_rem);
            if (data) {
                i2c_hal_write_txfifo(&(i2c_context[i2c_num].hal), data, size);
                vRingbufferReturnItemFromISR(p_i2c->tx_ring_buf, data, &HPTaskAwoken);
            } else {
                i2c_hal_disable_slave_tx_it(&(i2c_context[i2c_num].hal));
            }
            i2c_hal_slave_clr_tx_it(&(i2c_context[i2c_num].hal));
        }
    }
    //We only need to check here if there is a high-priority task needs to be switched.
    if (HPTaskAwoken == pdTRUE || HPTaskAwokenCallee == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t i2c_set_data_mode(i2c_port_t i2c_num, i2c_trans_mode_t tx_trans_mode, i2c_trans_mode_t rx_trans_mode)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(tx_trans_mode < I2C_DATA_MODE_MAX, I2C_TRANS_MODE_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(rx_trans_mode < I2C_DATA_MODE_MAX, I2C_TRANS_MODE_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_set_data_mode(&(i2c_context[i2c_num].hal), tx_trans_mode, rx_trans_mode);
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_get_data_mode(i2c_port_t i2c_num, i2c_trans_mode_t *tx_trans_mode, i2c_trans_mode_t *rx_trans_mode)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    i2c_hal_get_data_mode(&(i2c_context[i2c_num].hal), tx_trans_mode, rx_trans_mode);
    return ESP_OK;
}

/* Some slave device will die by accident and keep the SDA in low level,
 * in this case, master should send several clock to make the slave release the bus.
 * Slave mode of ESP32 might also get in wrong state that held the SDA low,
 * in this case, master device could send a stop signal to make esp32 slave release the bus.
 **/
static esp_err_t i2c_master_clear_bus(i2c_port_t i2c_num)
{
#if !I2C_SUPPORT_HW_CLR_BUS
    const int scl_half_period = I2C_CLR_BUS_HALF_PERIOD_US; // use standard 100kHz data rate
    int i = 0;
    int scl_io = i2c_context[i2c_num].scl_io_num;
    int sda_io = i2c_context[i2c_num].sda_io_num;
    gpio_set_direction(scl_io, GPIO_MODE_OUTPUT_OD);
    gpio_set_direction(sda_io, GPIO_MODE_INPUT_OUTPUT_OD);
    // If a SLAVE device was in a read operation when the bus was interrupted, the SLAVE device is controlling SDA.
    // The only bit during the 9 clock cycles of a READ byte the MASTER(ESP32) is guaranteed control over is during the ACK bit
    // period. If the slave is sending a stream of ZERO bytes, it will only release SDA during the ACK bit period.
    // So, this reset code needs to synchronize the bit stream with, Either, the ACK bit, Or a 1 bit to correctly generate
    // a STOP condition.
    gpio_set_level(scl_io, 0);
    gpio_set_level(sda_io, 1);
    ets_delay_us(scl_half_period);
    while (!gpio_get_level(sda_io) && (i++ < I2C_CLR_BUS_SCL_NUM)) {
        gpio_set_level(scl_io, 1);
        ets_delay_us(scl_half_period);
        gpio_set_level(scl_io, 0);
        ets_delay_us(scl_half_period);
    }
    gpio_set_level(sda_io, 0); // setup for STOP
    gpio_set_level(scl_io, 1);
    ets_delay_us(scl_half_period);
    gpio_set_level(sda_io, 1); // STOP, SDA low -> high while SCL is HIGH
    i2c_set_pin(i2c_num, sda_io, scl_io, 1, 1, I2C_MODE_MASTER);
#else
    i2c_hal_master_clr_bus(&(i2c_context[i2c_num].hal));
#endif
    return ESP_OK;
}

/**if the power and SDA/SCL wires are in proper condition, everything works find with reading the slave.
 * If we remove the power supply for the slave during I2C is reading, or directly connect SDA or SCL to ground,
 * this would cause the I2C FSM get stuck in wrong state, all we can do is to reset the I2C hardware in this case.
 **/
static esp_err_t i2c_hw_fsm_reset(i2c_port_t i2c_num)
{
// A workaround for avoiding cause timeout issue when using
// hardware reset.
#if !I2C_SUPPORT_HW_FSM_RST
    int scl_low_period, scl_high_period;
    int scl_start_hold, scl_rstart_setup;
    int scl_stop_hold, scl_stop_setup;
    int sda_hold, sda_sample;
    int timeout;
    uint8_t filter_cfg;

    i2c_hal_get_scl_timing(&(i2c_context[i2c_num].hal), &scl_high_period, &scl_low_period);
    i2c_hal_get_start_timing(&(i2c_context[i2c_num].hal), &scl_rstart_setup, &scl_start_hold);
    i2c_hal_get_stop_timing(&(i2c_context[i2c_num].hal), &scl_stop_setup, &scl_stop_hold);
    i2c_hal_get_sda_timing(&(i2c_context[i2c_num].hal), &sda_sample, &sda_hold);
    i2c_hal_get_tout(&(i2c_context[i2c_num].hal), &timeout);
    i2c_hal_get_filter(&(i2c_context[i2c_num].hal), &filter_cfg);

    //to reset the I2C hw module, we need re-enable the hw
    i2c_hw_disable(i2c_num);
    i2c_master_clear_bus(i2c_num);
    i2c_hw_enable(i2c_num);

    i2c_hal_master_init(&(i2c_context[i2c_num].hal), i2c_num);
    i2c_hal_disable_intr_mask(&(i2c_context[i2c_num].hal), I2C_INTR_MASK);
    i2c_hal_clr_intsts_mask(&(i2c_context[i2c_num].hal), I2C_INTR_MASK);
    i2c_hal_set_scl_timing(&(i2c_context[i2c_num].hal), scl_high_period, scl_low_period);
    i2c_hal_set_start_timing(&(i2c_context[i2c_num].hal), scl_rstart_setup, scl_start_hold);
    i2c_hal_set_stop_timing(&(i2c_context[i2c_num].hal), scl_stop_setup, scl_stop_hold);
    i2c_hal_set_sda_timing(&(i2c_context[i2c_num].hal), sda_sample, sda_hold);
    i2c_hal_set_tout(&(i2c_context[i2c_num].hal), timeout);
    i2c_hal_set_filter(&(i2c_context[i2c_num].hal), filter_cfg);
#else
    i2c_hal_master_fsm_rst(&(i2c_context[i2c_num].hal));
    i2c_master_clear_bus(i2c_num);
#endif
    return ESP_OK;
}

esp_err_t i2c_param_config(i2c_port_t i2c_num, const i2c_config_t *i2c_conf)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(i2c_conf != NULL, I2C_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(i2c_conf->mode < I2C_MODE_MAX, I2C_MODE_ERR_STR, ESP_ERR_INVALID_ARG);

    esp_err_t ret = i2c_set_pin(i2c_num, i2c_conf->sda_io_num, i2c_conf->scl_io_num,
                                i2c_conf->sda_pullup_en, i2c_conf->scl_pullup_en, i2c_conf->mode);
    if (ret != ESP_OK) {
        return ret;
    }
    i2c_hw_enable(i2c_num);
    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_disable_intr_mask(&(i2c_context[i2c_num].hal), I2C_INTR_MASK);
    i2c_hal_clr_intsts_mask(&(i2c_context[i2c_num].hal), I2C_INTR_MASK);
    if (i2c_conf->mode == I2C_MODE_SLAVE) {  //slave mode
        i2c_hal_slave_init(&(i2c_context[i2c_num].hal), i2c_num);
        i2c_hal_set_slave_addr(&(i2c_context[i2c_num].hal), i2c_conf->slave.slave_addr, i2c_conf->slave.addr_10bit_en);
        i2c_hal_set_rxfifo_full_thr(&(i2c_context[i2c_num].hal), I2C_FIFO_FULL_THRESH_VAL);
        i2c_hal_set_txfifo_empty_thr(&(i2c_context[i2c_num].hal), I2C_FIFO_EMPTY_THRESH_VAL);
        //set timing for data
        i2c_hal_set_sda_timing(&(i2c_context[i2c_num].hal), I2C_SLAVE_SDA_SAMPLE_DEFAULT, I2C_SLAVE_SDA_HOLD_DEFAULT);
        i2c_hal_set_tout(&(i2c_context[i2c_num].hal), I2C_SLAVE_TIMEOUT_DEFAULT);
        i2c_hal_enable_slave_rx_it(&(i2c_context[i2c_num].hal));
    } else {
        i2c_hal_master_init(&(i2c_context[i2c_num].hal), i2c_num);
        //Default, we enable hardware filter
        i2c_hal_set_filter(&(i2c_context[i2c_num].hal), I2C_FILTER_CYC_NUM_DEF);
        i2c_hal_set_bus_timing(&(i2c_context[i2c_num].hal), i2c_conf->master.clk_speed, I2C_SCLK_APB);
    }
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_set_period(i2c_port_t i2c_num, int high_period, int low_period)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((high_period <= I2C_SCL_HIGH_PERIOD_V) && (high_period > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((low_period <= I2C_SCL_LOW_PERIOD_V) && (low_period > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);

    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_set_scl_timing(&(i2c_context[i2c_num].hal), high_period, low_period);
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_get_period(i2c_port_t i2c_num, int *high_period, int *low_period)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX && high_period != NULL && low_period != NULL, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_get_scl_timing(&(i2c_context[i2c_num].hal), high_period, low_period);
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_filter_enable(i2c_port_t i2c_num, uint8_t cyc_num)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(p_i2c_obj[i2c_num] != NULL, I2C_DRIVER_ERR_STR, ESP_FAIL);
    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_set_filter(&(i2c_context[i2c_num].hal), cyc_num);
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_filter_disable(i2c_port_t i2c_num)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_set_filter(&(i2c_context[i2c_num].hal), 0);
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_set_start_timing(i2c_port_t i2c_num, int setup_time, int hold_time)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((hold_time <= I2C_SCL_START_HOLD_TIME_V) && (hold_time > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((setup_time <= I2C_SCL_RSTART_SETUP_TIME_V) && (setup_time > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);

    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_set_start_timing(&(i2c_context[i2c_num].hal), setup_time, hold_time);
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_get_start_timing(i2c_port_t i2c_num, int *setup_time, int *hold_time)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX && setup_time != NULL && hold_time != NULL, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_get_start_timing(&(i2c_context[i2c_num].hal), setup_time, hold_time);
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_set_stop_timing(i2c_port_t i2c_num, int setup_time, int hold_time)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((setup_time <= I2C_SCL_STOP_SETUP_TIME_V) && (setup_time > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((hold_time <= I2C_SCL_STOP_HOLD_TIME_V) && (hold_time > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);

    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_set_stop_timing(&(i2c_context[i2c_num].hal), setup_time, hold_time);
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_get_stop_timing(i2c_port_t i2c_num, int *setup_time, int *hold_time)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX && setup_time != NULL && hold_time != NULL, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_get_stop_timing(&(i2c_context[i2c_num].hal), setup_time, hold_time);
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_set_data_timing(i2c_port_t i2c_num, int sample_time, int hold_time)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((sample_time <= I2C_SDA_SAMPLE_TIME_V) && (sample_time > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((hold_time <= I2C_SDA_HOLD_TIME_V) && (hold_time > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);

    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_set_sda_timing(&(i2c_context[i2c_num].hal), sample_time, hold_time);
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_get_data_timing(i2c_port_t i2c_num, int *sample_time, int *hold_time)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX && sample_time != NULL && hold_time != NULL, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_get_sda_timing(&(i2c_context[i2c_num].hal), sample_time, hold_time);
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_set_timeout(i2c_port_t i2c_num, int timeout)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((timeout <= I2C_TIME_OUT_REG_V) && (timeout > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);

    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_set_tout(&(i2c_context[i2c_num].hal), timeout);
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    return ESP_OK;
}

esp_err_t i2c_get_timeout(i2c_port_t i2c_num, int *timeout)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX && timeout != NULL, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    i2c_hal_get_tout(&(i2c_context[i2c_num].hal), timeout);
    return ESP_OK;
}

esp_err_t i2c_isr_register(i2c_port_t i2c_num, void (*fn)(void *), void *arg, int intr_alloc_flags, intr_handle_t *handle)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(fn != NULL, I2C_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    esp_err_t ret = esp_intr_alloc(i2c_periph_signal[i2c_num].irq, intr_alloc_flags, fn, arg, handle);
    return ret;
}

esp_err_t i2c_isr_free(intr_handle_t handle)
{
    return esp_intr_free(handle);
}

esp_err_t i2c_set_pin(i2c_port_t i2c_num, int sda_io_num, int scl_io_num, bool sda_pullup_en, bool scl_pullup_en, i2c_mode_t mode)
{
    I2C_CHECK(( i2c_num < I2C_NUM_MAX ), I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(((sda_io_num < 0) || ((GPIO_IS_VALID_OUTPUT_GPIO(sda_io_num)))), I2C_SDA_IO_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(scl_io_num < 0 ||
              (GPIO_IS_VALID_OUTPUT_GPIO(scl_io_num)) ||
              (GPIO_IS_VALID_GPIO(scl_io_num) && mode == I2C_MODE_SLAVE),
              I2C_SCL_IO_ERR_STR,
              ESP_ERR_INVALID_ARG);
    I2C_CHECK(sda_io_num < 0 ||
              (sda_pullup_en == GPIO_PULLUP_ENABLE && GPIO_IS_VALID_OUTPUT_GPIO(sda_io_num)) ||
              sda_pullup_en == GPIO_PULLUP_DISABLE, I2C_GPIO_PULLUP_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(scl_io_num < 0 ||
              (scl_pullup_en == GPIO_PULLUP_ENABLE && GPIO_IS_VALID_OUTPUT_GPIO(scl_io_num)) ||
              scl_pullup_en == GPIO_PULLUP_DISABLE, I2C_GPIO_PULLUP_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((sda_io_num != scl_io_num), I2C_SCL_SDA_EQUAL_ERR_STR, ESP_ERR_INVALID_ARG);

    int sda_in_sig, sda_out_sig, scl_in_sig, scl_out_sig;
    sda_out_sig = i2c_periph_signal[i2c_num].sda_out_sig;
    sda_in_sig = i2c_periph_signal[i2c_num].sda_in_sig;
    scl_out_sig = i2c_periph_signal[i2c_num].scl_out_sig;
    scl_in_sig = i2c_periph_signal[i2c_num].scl_in_sig;
    if (sda_io_num >= 0) {
        gpio_set_level(sda_io_num, I2C_IO_INIT_LEVEL);
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[sda_io_num], PIN_FUNC_GPIO);
        gpio_set_direction(sda_io_num, GPIO_MODE_INPUT_OUTPUT_OD);

        if (sda_pullup_en == GPIO_PULLUP_ENABLE) {
            gpio_set_pull_mode(sda_io_num, GPIO_PULLUP_ONLY);
        } else {
            gpio_set_pull_mode(sda_io_num, GPIO_FLOATING);
        }
        gpio_matrix_out(sda_io_num, sda_out_sig, 0, 0);
        gpio_matrix_in(sda_io_num, sda_in_sig, 0);
    }
    if (scl_io_num >= 0) {
        gpio_set_level(scl_io_num, I2C_IO_INIT_LEVEL);
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[scl_io_num], PIN_FUNC_GPIO);
        gpio_set_direction(scl_io_num, GPIO_MODE_INPUT_OUTPUT_OD);
        gpio_matrix_out(scl_io_num, scl_out_sig, 0, 0);
        gpio_matrix_in(scl_io_num, scl_in_sig, 0);
        if (scl_pullup_en == GPIO_PULLUP_ENABLE) {
            gpio_set_pull_mode(scl_io_num, GPIO_PULLUP_ONLY);
        } else {
            gpio_set_pull_mode(scl_io_num, GPIO_FLOATING);
        }
    }
#if !I2C_SUPPORT_HW_CLR_BUS
    i2c_context[i2c_num].scl_io_num = scl_io_num;
    i2c_context[i2c_num].sda_io_num = sda_io_num;
#endif
    return ESP_OK;
}

i2c_cmd_handle_t i2c_cmd_link_create(void)
{
#if !CONFIG_SPIRAM_USE_MALLOC
    i2c_cmd_desc_t *cmd_desc = (i2c_cmd_desc_t *) calloc(1, sizeof(i2c_cmd_desc_t));
#else
    i2c_cmd_desc_t *cmd_desc = (i2c_cmd_desc_t *) heap_caps_calloc(1, sizeof(i2c_cmd_desc_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
#endif
    return (i2c_cmd_handle_t) cmd_desc;
}

void i2c_cmd_link_delete(i2c_cmd_handle_t cmd_handle)
{
    if (cmd_handle == NULL) {
        return;
    }
    i2c_cmd_desc_t *cmd = (i2c_cmd_desc_t *) cmd_handle;
    while (cmd->free) {
        i2c_cmd_link_t *ptmp = cmd->free;
        cmd->free = cmd->free->next;
        free(ptmp);
    }
    cmd->cur = NULL;
    cmd->free = NULL;
    cmd->head = NULL;
    free(cmd_handle);
    return;
}

static esp_err_t i2c_cmd_link_append(i2c_cmd_handle_t cmd_handle, i2c_cmd_t *cmd)
{
    i2c_cmd_desc_t *cmd_desc = (i2c_cmd_desc_t *) cmd_handle;
    if (cmd_desc->head == NULL) {
#if !CONFIG_SPIRAM_USE_MALLOC
        cmd_desc->head = (i2c_cmd_link_t *) calloc(1, sizeof(i2c_cmd_link_t));
#else
        cmd_desc->head = (i2c_cmd_link_t *) heap_caps_calloc(1, sizeof(i2c_cmd_link_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
#endif
        if (cmd_desc->head == NULL) {
            ESP_LOGE(I2C_TAG, I2C_CMD_MALLOC_ERR_STR);
            goto err;
        }
        cmd_desc->cur = cmd_desc->head;
        cmd_desc->free = cmd_desc->head;
    } else {
#if !CONFIG_SPIRAM_USE_MALLOC
        cmd_desc->cur->next = (i2c_cmd_link_t *) calloc(1, sizeof(i2c_cmd_link_t));
#else
        cmd_desc->cur->next = (i2c_cmd_link_t *) heap_caps_calloc(1, sizeof(i2c_cmd_link_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
#endif
        if (cmd_desc->cur->next == NULL) {
            ESP_LOGE(I2C_TAG, I2C_CMD_MALLOC_ERR_STR);
            goto err;
        }
        cmd_desc->cur = cmd_desc->cur->next;
    }
    memcpy((uint8_t *) &cmd_desc->cur->cmd, (uint8_t *) cmd, sizeof(i2c_cmd_t));
    cmd_desc->cur->next = NULL;
    return ESP_OK;

err:
    return ESP_FAIL;
}

esp_err_t i2c_master_start(i2c_cmd_handle_t cmd_handle)
{
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);
    i2c_cmd_t cmd;
    cmd.hw_cmd.ack_en = 0;
    cmd.hw_cmd.ack_exp = 0;
    cmd.hw_cmd.ack_val = 0;
    cmd.hw_cmd.op_code = I2C_CMD_RESTART;
    cmd.hw_cmd.byte_num = 0;
    cmd.data = NULL;
    return i2c_cmd_link_append(cmd_handle, &cmd);
}

esp_err_t i2c_master_stop(i2c_cmd_handle_t cmd_handle)
{
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);
    i2c_cmd_t cmd;
    cmd.hw_cmd.ack_en = 0;
    cmd.hw_cmd.ack_exp = 0;
    cmd.hw_cmd.ack_val = 0;
    cmd.hw_cmd.op_code = I2C_CMD_STOP;
    cmd.hw_cmd.byte_num = 0;
    cmd.data = NULL;
    return i2c_cmd_link_append(cmd_handle, &cmd);
}

esp_err_t i2c_master_write(i2c_cmd_handle_t cmd_handle, uint8_t *data, size_t data_len, bool ack_en)
{
    I2C_CHECK((data != NULL), I2C_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);

    uint8_t len_tmp;
    int data_offset = 0;
    esp_err_t ret;
    while (data_len > 0) {
        len_tmp = data_len > 0xff ? 0xff : data_len;
        data_len -= len_tmp;
        i2c_cmd_t cmd;
        cmd.hw_cmd.ack_en = ack_en;
        cmd.hw_cmd.ack_exp = 0;
        cmd.hw_cmd.ack_val = 0;
        cmd.hw_cmd.op_code = I2C_CMD_WRITE;
        cmd.hw_cmd.byte_num = len_tmp;
        cmd.data = data + data_offset;
        ret = i2c_cmd_link_append(cmd_handle, &cmd);
        data_offset += len_tmp;
        if (ret != ESP_OK) {
            return ret;
        }
    }
    return ESP_OK;
}

esp_err_t i2c_master_write_byte(i2c_cmd_handle_t cmd_handle, uint8_t data, bool ack_en)
{
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);
    i2c_cmd_t cmd;
    cmd.hw_cmd.ack_en = ack_en;
    cmd.hw_cmd.ack_exp = 0;
    cmd.hw_cmd.ack_val = 0;
    cmd.hw_cmd.op_code = I2C_CMD_WRITE;
    cmd.hw_cmd.byte_num = 1;
    cmd.data = NULL;
    cmd.byte_cmd = data;
    return i2c_cmd_link_append(cmd_handle, &cmd);
}

static esp_err_t i2c_master_read_static(i2c_cmd_handle_t cmd_handle, uint8_t *data, size_t data_len, i2c_ack_type_t ack)
{
    int len_tmp;
    int data_offset = 0;
    esp_err_t ret;
    while (data_len > 0) {
        len_tmp = data_len > 0xff ? 0xff : data_len;
        data_len -= len_tmp;
        i2c_cmd_t cmd;
        cmd.hw_cmd.ack_en = 0;
        cmd.hw_cmd.ack_exp = 0;
        cmd.hw_cmd.ack_val = ack & 0x1;
        cmd.hw_cmd.byte_num = len_tmp;
        cmd.hw_cmd.op_code = I2C_CMD_READ;
        cmd.data = data + data_offset;
        ret = i2c_cmd_link_append(cmd_handle, &cmd);
        data_offset += len_tmp;
        if (ret != ESP_OK) {
            return ret;
        }
    }
    return ESP_OK;
}

esp_err_t i2c_master_read_byte(i2c_cmd_handle_t cmd_handle, uint8_t *data, i2c_ack_type_t ack)
{
    I2C_CHECK((data != NULL), I2C_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(ack < I2C_MASTER_ACK_MAX, I2C_ACK_TYPE_ERR_STR, ESP_ERR_INVALID_ARG);

    i2c_cmd_t cmd;
    cmd.hw_cmd.ack_en = 0;
    cmd.hw_cmd.ack_exp = 0;
    cmd.hw_cmd.ack_val = ((ack == I2C_MASTER_LAST_NACK) ? I2C_MASTER_NACK : (ack & 0x1));
    cmd.hw_cmd.byte_num = 1;
    cmd.hw_cmd.op_code = I2C_CMD_READ;
    cmd.data = data;
    return i2c_cmd_link_append(cmd_handle, &cmd);
}

esp_err_t i2c_master_read(i2c_cmd_handle_t cmd_handle, uint8_t *data, size_t data_len, i2c_ack_type_t ack)
{
    I2C_CHECK((data != NULL), I2C_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(ack < I2C_MASTER_ACK_MAX, I2C_ACK_TYPE_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(data_len > 0, I2C_DATA_LEN_ERR_STR, ESP_ERR_INVALID_ARG);

    if (ack != I2C_MASTER_LAST_NACK) {
        return i2c_master_read_static(cmd_handle, data, data_len, ack);
    } else {
        if (data_len == 1) {
            return i2c_master_read_byte(cmd_handle, data, I2C_MASTER_NACK);
        } else {
            esp_err_t ret;
            if ((ret =  i2c_master_read_static(cmd_handle, data, data_len - 1, I2C_MASTER_ACK)) != ESP_OK) {
                return ret;
            }
            return i2c_master_read_byte(cmd_handle, data + data_len - 1, I2C_MASTER_NACK);
        }
    }
}

static void IRAM_ATTR i2c_master_cmd_begin_static(i2c_port_t i2c_num, portBASE_TYPE* HPTaskAwoken)
{
    i2c_obj_t *p_i2c = p_i2c_obj[i2c_num];
    i2c_cmd_evt_t evt = { 0 };
    if (p_i2c->cmd_link.head != NULL && p_i2c->status == I2C_STATUS_READ) {
        i2c_cmd_t *cmd = &p_i2c->cmd_link.head->cmd;
        i2c_hal_read_rxfifo(&(i2c_context[i2c_num].hal), cmd->data, p_i2c->rx_cnt);
        cmd->data += p_i2c->rx_cnt;
        if (cmd->hw_cmd.byte_num > 0) {
            p_i2c->cmd_idx = 0;
        } else {
            p_i2c->cmd_link.head = p_i2c->cmd_link.head->next;
        }
    } else if ((p_i2c->status == I2C_STATUS_ACK_ERROR)
               || (p_i2c->status == I2C_STATUS_TIMEOUT)) {
        assert(HPTaskAwoken != NULL);
        evt.type = I2C_CMD_EVT_DONE;
        xQueueOverwriteFromISR(p_i2c->cmd_evt_queue, &evt, HPTaskAwoken);
        return;
    } else if (p_i2c->status == I2C_STATUS_DONE) {
        return;
    }

    if (p_i2c->cmd_link.head == NULL) {
        assert(HPTaskAwoken != NULL);
        p_i2c->cmd_link.cur = NULL;
        evt.type = I2C_CMD_EVT_DONE;
        xQueueOverwriteFromISR(p_i2c->cmd_evt_queue, &evt, HPTaskAwoken);
        // Return to the IDLE status after cmd_eve_done signal were send out.
        p_i2c->status = I2C_STATUS_IDLE;
        return;
    }
    const i2c_hw_cmd_t hw_end_cmd = {
        .op_code = I2C_CMD_END
    };
    while (p_i2c->cmd_link.head) {
        i2c_cmd_t *cmd = &p_i2c->cmd_link.head->cmd;
        i2c_hw_cmd_t hw_cmd = cmd->hw_cmd;
        if (cmd->hw_cmd.op_code == I2C_CMD_WRITE) {
            uint8_t wr_filled = 0;
            uint8_t *write_pr = NULL;
            //TODO: to reduce interrupt number
            if (cmd->data) {
                wr_filled = (cmd->hw_cmd.byte_num > SOC_I2C_FIFO_LEN) ? SOC_I2C_FIFO_LEN : cmd->hw_cmd.byte_num;
                write_pr = cmd->data;
                cmd->data += wr_filled;
                cmd->hw_cmd.byte_num -= wr_filled;
            } else {
                wr_filled = 1;
                write_pr = &(cmd->byte_cmd);
                cmd->hw_cmd.byte_num--;
            }
            hw_cmd.byte_num = wr_filled;
            i2c_hal_write_txfifo(&(i2c_context[i2c_num].hal), write_pr, wr_filled);
            i2c_hal_write_cmd_reg(&(i2c_context[i2c_num].hal), hw_cmd, p_i2c->cmd_idx);
            i2c_hal_write_cmd_reg(&(i2c_context[i2c_num].hal), hw_end_cmd, p_i2c->cmd_idx + 1);
            i2c_hal_enable_master_tx_it(&(i2c_context[i2c_num].hal));
            p_i2c->cmd_idx = 0;
            if (cmd->hw_cmd.byte_num == 0) {
                p_i2c->cmd_link.head = p_i2c->cmd_link.head->next;
            }
            p_i2c->status = I2C_STATUS_WRITE;
            break;
        } else if (cmd->hw_cmd.op_code == I2C_CMD_READ) {
            //TODO: to reduce interrupt number
            p_i2c->rx_cnt = cmd->hw_cmd.byte_num > SOC_I2C_FIFO_LEN ? SOC_I2C_FIFO_LEN : cmd->hw_cmd.byte_num;
            cmd->hw_cmd.byte_num -= p_i2c->rx_cnt;
            hw_cmd.byte_num = p_i2c->rx_cnt;
            i2c_hal_write_cmd_reg(&(i2c_context[i2c_num].hal), hw_cmd, p_i2c->cmd_idx);
            i2c_hal_write_cmd_reg(&(i2c_context[i2c_num].hal), hw_end_cmd, p_i2c->cmd_idx + 1);
            i2c_hal_enable_master_rx_it(&(i2c_context[i2c_num].hal));
            p_i2c->status = I2C_STATUS_READ;
            break;
        } else {
            i2c_hal_write_cmd_reg(&(i2c_context[i2c_num].hal), hw_cmd, p_i2c->cmd_idx);
        }
        p_i2c->cmd_idx++;
        p_i2c->cmd_link.head = p_i2c->cmd_link.head->next;
        if (p_i2c->cmd_link.head == NULL || p_i2c->cmd_idx >= 15) {
            p_i2c->cmd_idx = 0;
            break;
        }
    }
    i2c_hal_trans_start(&(i2c_context[i2c_num].hal));
    return;
}

#if CONFIG_SPIRAM_USE_MALLOC
//Check whether read or write buffer in cmd_link is internal.
static bool is_cmd_link_buffer_internal(i2c_cmd_link_t *link)
{
    i2c_cmd_link_t *cmd_link = link;
    while (cmd_link != NULL)  {
        if (cmd_link->cmd.hw_cmd.op_code == I2C_CMD_WRITE || cmd_link->cmd.hw_cmd.op_code == I2C_CMD_READ) {
            if (cmd_link->cmd.data != NULL && !esp_ptr_internal(cmd_link->cmd.data)) {
                return false;
            }
        }
        cmd_link = cmd_link->next;
    }
    return true;
}
#endif

esp_err_t i2c_master_cmd_begin(i2c_port_t i2c_num, i2c_cmd_handle_t cmd_handle, TickType_t ticks_to_wait)
{
    I2C_CHECK(( i2c_num < I2C_NUM_MAX ), I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(p_i2c_obj[i2c_num] != NULL, I2C_DRIVER_NOT_INSTALL_ERR_STR, ESP_ERR_INVALID_STATE);
    I2C_CHECK(p_i2c_obj[i2c_num]->mode == I2C_MODE_MASTER, I2C_MASTER_MODE_ERR_STR, ESP_ERR_INVALID_STATE);
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);

#if CONFIG_SPIRAM_USE_MALLOC
    //If the i2c read or write buffer is not in internal RAM, we will return ESP_FAIL
    //to avoid the ISR handler function crashing when the cache is disabled.
    if ((p_i2c_obj[i2c_num]->intr_alloc_flags & ESP_INTR_FLAG_IRAM)) {
        if (!is_cmd_link_buffer_internal(((i2c_cmd_desc_t *)cmd_handle)->head) ) {
            ESP_LOGE(I2C_TAG, I2C_PSRAM_BUFFER_WARN_STR);
            return ESP_ERR_INVALID_ARG;
        }
    }
#endif
    // Sometimes when the FSM get stuck, the ACK_ERR interrupt will occur endlessly until we reset the FSM and clear bus.
    static uint8_t clear_bus_cnt = 0;
    esp_err_t ret = ESP_FAIL;
    i2c_obj_t *p_i2c = p_i2c_obj[i2c_num];
    portTickType ticks_start = xTaskGetTickCount();
    portBASE_TYPE res = xSemaphoreTake(p_i2c->cmd_mux, ticks_to_wait);
    if (res == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(p_i2c->pm_lock);
#endif
    xQueueReset(p_i2c->cmd_evt_queue);
    if (p_i2c->status == I2C_STATUS_TIMEOUT
            || i2c_hal_is_bus_busy(&(i2c_context[i2c_num].hal))) {
        i2c_hw_fsm_reset(i2c_num);
        clear_bus_cnt = 0;
    }
    i2c_reset_tx_fifo(i2c_num);
    i2c_reset_rx_fifo(i2c_num);
    i2c_cmd_desc_t *cmd = (i2c_cmd_desc_t *) cmd_handle;
    p_i2c->cmd_link.free = cmd->free;
    p_i2c->cmd_link.cur = cmd->cur;
    p_i2c->cmd_link.head = cmd->head;
    p_i2c->status = I2C_STATUS_IDLE;
    p_i2c->cmd_idx = 0;
    p_i2c->rx_cnt = 0;
    i2c_reset_tx_fifo(i2c_num);
    i2c_reset_rx_fifo(i2c_num);
    // These two interrupts some times can not be cleared when the FSM gets stuck.
    // so we disable them when these two interrupt occurs and re-enable them here.
    i2c_hal_disable_intr_mask(&(i2c_context[i2c_num].hal), I2C_INTR_MASK);
    i2c_hal_clr_intsts_mask(&(i2c_context[i2c_num].hal), I2C_INTR_MASK);
    //start send commands, at most 32 bytes one time, isr handler will process the remaining commands.
    i2c_master_cmd_begin_static(i2c_num, NULL);

    // Wait event bits
    i2c_cmd_evt_t evt;
    while (1) {
        TickType_t wait_time = xTaskGetTickCount();
        if (wait_time - ticks_start > ticks_to_wait) { // out of time
            wait_time = I2C_CMD_ALIVE_INTERVAL_TICK;
        } else {
            wait_time = ticks_to_wait - (wait_time - ticks_start);
            if (wait_time < I2C_CMD_ALIVE_INTERVAL_TICK) {
                wait_time = I2C_CMD_ALIVE_INTERVAL_TICK;
            }
        }
        // In master mode, since we don't have an interrupt to detective bus error or FSM state, what we do here is to make
        // sure the interrupt mechanism for master mode is still working.
        // If the command sending is not finished and there is no interrupt any more, the bus is probably dead caused by external noise.
        portBASE_TYPE evt_res = xQueueReceive(p_i2c->cmd_evt_queue, &evt, wait_time);
        if (evt_res == pdTRUE) {
            if (evt.type == I2C_CMD_EVT_DONE) {
                if (p_i2c->status == I2C_STATUS_TIMEOUT) {
                    // If the I2C slave are powered off or the SDA/SCL are connected to ground, for example,
                    // I2C hw FSM would get stuck in wrong state, we have to reset the I2C module in this case.
                    i2c_hw_fsm_reset(i2c_num);
                    clear_bus_cnt = 0;
                    ret = ESP_ERR_TIMEOUT;
                } else if (p_i2c->status == I2C_STATUS_ACK_ERROR) {
                    clear_bus_cnt++;
                    if (clear_bus_cnt >= I2C_ACKERR_CNT_MAX) {
                        clear_bus_cnt = 0;
                    }
                    ret = ESP_FAIL;
                } else {
                    ret = ESP_OK;
                }
                break;
            }
            if (evt.type == I2C_CMD_EVT_ALIVE) {
            }
        } else {
            ret = ESP_ERR_TIMEOUT;
            // If the I2C slave are powered off or the SDA/SCL are connected to ground, for example,
            // I2C hw FSM would get stuck in wrong state, we have to reset the I2C module in this case.
            i2c_hw_fsm_reset(i2c_num);
            clear_bus_cnt = 0;
            break;
        }
    }
    p_i2c->status = I2C_STATUS_DONE;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(p_i2c->pm_lock);
#endif
    xSemaphoreGive(p_i2c->cmd_mux);
    return ret;
}

int i2c_slave_write_buffer(i2c_port_t i2c_num, const uint8_t *data, int size, TickType_t ticks_to_wait)
{
    I2C_CHECK(( i2c_num < I2C_NUM_MAX ), I2C_NUM_ERROR_STR, ESP_FAIL);
    I2C_CHECK(p_i2c_obj[i2c_num] != NULL, I2C_DRIVER_ERR_STR, ESP_FAIL);
    I2C_CHECK((data != NULL), I2C_ADDR_ERROR_STR, ESP_FAIL);
    I2C_CHECK(p_i2c_obj[i2c_num]->mode == I2C_MODE_SLAVE, I2C_MODE_SLAVE_ERR_STR, ESP_FAIL);
    i2c_obj_t *p_i2c = p_i2c_obj[i2c_num];

    portBASE_TYPE res;
    int cnt = 0;
    portTickType ticks_end = xTaskGetTickCount() + ticks_to_wait;

    res = xSemaphoreTake(p_i2c->slv_tx_mux, ticks_to_wait);
    if (res == pdFALSE) {
        return 0;
    }
    ticks_to_wait = ticks_end - xTaskGetTickCount();
    res = xRingbufferSend(p_i2c->tx_ring_buf, data, size, ticks_to_wait);
    if (res == pdFALSE) {
        cnt = 0;
    } else {
        I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
        i2c_hal_enable_slave_tx_it(&(i2c_context[i2c_num].hal));
        I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
        cnt = size;
    }
    xSemaphoreGive(p_i2c->slv_tx_mux);
    return cnt;
}

int i2c_slave_read_buffer(i2c_port_t i2c_num, uint8_t *data, size_t max_size, TickType_t ticks_to_wait)
{
    I2C_CHECK(( i2c_num < I2C_NUM_MAX ), I2C_NUM_ERROR_STR, ESP_FAIL);
    I2C_CHECK(p_i2c_obj[i2c_num] != NULL, I2C_DRIVER_ERR_STR, ESP_FAIL);
    I2C_CHECK((data != NULL), I2C_ADDR_ERROR_STR, ESP_FAIL);
    I2C_CHECK(p_i2c_obj[i2c_num]->mode == I2C_MODE_SLAVE, I2C_MODE_SLAVE_ERR_STR, ESP_FAIL);

    size_t size = 0;
    size_t size_rem = max_size;
    i2c_obj_t *p_i2c = p_i2c_obj[i2c_num];
    if (xSemaphoreTake(p_i2c->slv_rx_mux, ticks_to_wait) == pdFALSE) {
        return 0;
    }
    portTickType ticks_rem = ticks_to_wait;
    portTickType ticks_end = xTaskGetTickCount() + ticks_to_wait;
    I2C_ENTER_CRITICAL(&(i2c_context[i2c_num].spinlock));
    i2c_hal_enable_slave_rx_it(&(i2c_context[i2c_num].hal));
    I2C_EXIT_CRITICAL(&(i2c_context[i2c_num].spinlock));
    while (size_rem && ticks_rem <= ticks_to_wait) {
        uint8_t *pdata = (uint8_t *) xRingbufferReceiveUpTo(p_i2c->rx_ring_buf, &size, ticks_to_wait, size_rem);
        if (pdata && size > 0) {
            memcpy(data, pdata, size);
            vRingbufferReturnItem(p_i2c->rx_ring_buf, pdata);
            data += size;
            size_rem -= size;
        }
        if (ticks_to_wait != portMAX_DELAY) {
            ticks_rem = ticks_end - xTaskGetTickCount();
        }
    }
    xSemaphoreGive(p_i2c->slv_rx_mux);
    return max_size - size_rem;
}
