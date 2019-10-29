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
#include "soc/i2c_periph.h"
#include "soc/soc_memory_layout.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_pm.h"
#include "sdkconfig.h"

static const char* I2C_TAG = "i2c";
#define I2C_CHECK(a, str, ret)  if(!(a)) {                                             \
        ESP_LOGE(I2C_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);      \
        return (ret);                                                                   \
        }

static portMUX_TYPE i2c_spinlock[I2C_NUM_MAX] = {portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED};
/* DRAM_ATTR is required to avoid I2C array placed in flash, due to accessed from ISR */
static DRAM_ATTR i2c_dev_t* const I2C[I2C_NUM_MAX] = { &I2C0, &I2C1 };

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

typedef struct {
    uint8_t byte_num;  /*!< cmd byte number */
    uint8_t ack_en;    /*!< ack check enable */
    uint8_t ack_exp;   /*!< expected ack level to get */
    uint8_t ack_val;   /*!< ack value to send */
    uint8_t* data;     /*!< data address */
    uint8_t byte_cmd;  /*!< to save cmd for one byte command mode */
    i2c_opmode_t op_code; /*!< hardware cmd type */
} i2c_cmd_t;

typedef typeof(I2C[0]->command[0]) i2c_hw_cmd_t;

typedef struct i2c_cmd_link{
    i2c_cmd_t cmd;              /*!< command in current cmd link */
    struct i2c_cmd_link *next;  /*!< next cmd link */
} i2c_cmd_link_t;

typedef struct {
    i2c_cmd_link_t* head;     /*!< head of the command link */
    i2c_cmd_link_t* cur;      /*!< last node of the command link */
    i2c_cmd_link_t* free;     /*!< the first node to free of the command link */
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
    uint8_t data_buf[I2C_FIFO_LEN];  /*!< a buffer to store i2c fifo data */

    i2c_cmd_desc_t cmd_link;         /*!< I2C command link */
    QueueHandle_t cmd_evt_queue;     /*!< I2C command event queue */
#if CONFIG_SPIRAM_USE_MALLOC
    uint8_t* evt_queue_storage;      /*!< The buffer that will hold the items in the queue */
    int intr_alloc_flags;            /*!< Used to allocate the interrupt */
    StaticQueue_t evt_queue_buffer;  /*!< The buffer that will hold the queue structure*/
#endif
    xSemaphoreHandle cmd_mux;        /*!< semaphore to lock command process */
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
    size_t tx_fifo_remain;           /*!< tx fifo remain length, for master mode */
    size_t rx_fifo_remain;           /*!< rx fifo remain length, for master mode */

    xSemaphoreHandle slv_rx_mux;     /*!< slave rx buffer mux */
    xSemaphoreHandle slv_tx_mux;     /*!< slave tx buffer mux */
    size_t rx_buf_length;            /*!< rx buffer length */
    RingbufHandle_t rx_ring_buf;     /*!< rx ringbuffer handler of slave mode */
    size_t tx_buf_length;            /*!< tx buffer length */
    RingbufHandle_t tx_ring_buf;     /*!< tx ringbuffer handler of slave mode */
} i2c_obj_t;

static i2c_obj_t *p_i2c_obj[I2C_NUM_MAX] = {0};
static void i2c_isr_handler_default(void* arg);
static void IRAM_ATTR i2c_master_cmd_begin_static(i2c_port_t i2c_num);
static esp_err_t IRAM_ATTR i2c_hw_fsm_reset(i2c_port_t i2c_num);

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
    uint32_t intr_mask = 0;
    if (p_i2c_obj[i2c_num] == NULL) {

#if !CONFIG_SPIRAM_USE_MALLOC
        p_i2c_obj[i2c_num] = (i2c_obj_t*) calloc(1, sizeof(i2c_obj_t));
#else
        if( !(intr_alloc_flags & ESP_INTR_FLAG_IRAM) ) {
            p_i2c_obj[i2c_num] = (i2c_obj_t*) calloc(1, sizeof(i2c_obj_t));
        } else {
            p_i2c_obj[i2c_num] = (i2c_obj_t*) heap_caps_calloc(1, sizeof(i2c_obj_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
        }
#endif
        if (p_i2c_obj[i2c_num] == NULL) {
            ESP_LOGE(I2C_TAG, I2C_DRIVER_MALLOC_ERR_STR);
            return ESP_FAIL;
        }
        i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];
        p_i2c->i2c_num = i2c_num;
        p_i2c->mode = mode;
        p_i2c->cmd_idx = 0;
        p_i2c->rx_cnt = 0;
        p_i2c->status = I2C_STATUS_IDLE;

#if CONFIG_SPIRAM_USE_MALLOC
        p_i2c->intr_alloc_flags = intr_alloc_flags;
#endif
        p_i2c->rx_fifo_remain = I2C_FIFO_LEN;
        p_i2c->tx_fifo_remain = I2C_FIFO_LEN;

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
            intr_mask |= ( I2C_RXFIFO_FULL_INT_ENA_M | I2C_TRANS_COMPLETE_INT_ENA_M);
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
            if( !(intr_alloc_flags & ESP_INTR_FLAG_IRAM) ) {
                p_i2c->cmd_evt_queue = xQueueCreate(I2C_EVT_QUEUE_LEN, sizeof(i2c_cmd_evt_t));
            } else {
                p_i2c->evt_queue_storage = (uint8_t *)heap_caps_calloc(I2C_EVT_QUEUE_LEN, sizeof(i2c_cmd_evt_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
                if( p_i2c->evt_queue_storage == NULL ) {
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
            intr_mask |= I2C_ARBITRATION_LOST_INT_ENA_M | I2C_TIME_OUT_INT_ST_M;
        }
    } else {
        ESP_LOGE(I2C_TAG, I2C_DRIVER_ERR_STR);
        return ESP_FAIL;
    }
    //hook isr handler
    i2c_isr_register(i2c_num, i2c_isr_handler_default, p_i2c_obj[i2c_num], intr_alloc_flags, &p_i2c_obj[i2c_num]->intr_handle);
    intr_mask |= ( I2C_TRANS_COMPLETE_INT_ENA_M |
                   I2C_TRANS_START_INT_ENA_M |
                   I2C_ACK_ERR_INT_ENA_M |
                   I2C_RXFIFO_OVF_INT_ENA_M |
                   I2C_SLAVE_TRAN_COMP_INT_ENA_M);
    I2C[i2c_num]->int_clr.val = intr_mask;
    I2C[i2c_num]->int_ena.val = intr_mask;
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

static esp_err_t i2c_hw_enable(i2c_port_t i2c_num)
{
    if (i2c_num == I2C_NUM_0) {
        periph_module_enable(PERIPH_I2C0_MODULE);
    } else if (i2c_num == I2C_NUM_1) {
        periph_module_enable(PERIPH_I2C1_MODULE);
    }
    return ESP_OK;
}

static esp_err_t i2c_hw_disable(i2c_port_t i2c_num)
{
    if (i2c_num == I2C_NUM_0) {
        periph_module_disable(PERIPH_I2C0_MODULE);
    } else if (i2c_num == I2C_NUM_1) {
        periph_module_disable(PERIPH_I2C1_MODULE);
    }
    return ESP_OK;
}

esp_err_t i2c_driver_delete(i2c_port_t i2c_num)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(p_i2c_obj[i2c_num] != NULL, I2C_DRIVER_ERR_STR, ESP_FAIL);

    i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];

    I2C[i2c_num]->int_ena.val = 0;
    esp_intr_free(p_i2c->intr_handle);
    p_i2c->intr_handle = NULL;

    if (p_i2c->cmd_mux) {
        xSemaphoreTake(p_i2c->cmd_mux, portMAX_DELAY);
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
    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    I2C[i2c_num]->fifo_conf.tx_fifo_rst = 1;
    I2C[i2c_num]->fifo_conf.tx_fifo_rst = 0;
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_reset_rx_fifo(i2c_port_t i2c_num)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    I2C[i2c_num]->fifo_conf.rx_fifo_rst = 1;
    I2C[i2c_num]->fifo_conf.rx_fifo_rst = 0;
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

static void IRAM_ATTR i2c_isr_handler_default(void* arg)
{
    i2c_obj_t* p_i2c = (i2c_obj_t*) arg;
    int i2c_num = p_i2c->i2c_num;
    uint32_t status = I2C[i2c_num]->int_status.val;
    int idx = 0;

    portBASE_TYPE HPTaskAwoken = pdFALSE;
    while (status != 0) {
        status = I2C[i2c_num]->int_status.val;
        if (status & I2C_TX_SEND_EMPTY_INT_ST_M) {
            I2C[i2c_num]->int_clr.tx_send_empty = 1;
        } else if (status & I2C_RX_REC_FULL_INT_ST_M) {
            I2C[i2c_num]->int_clr.rx_rec_full = 1;
        } else if (status & I2C_ACK_ERR_INT_ST_M) {
            I2C[i2c_num]->int_ena.ack_err = 0;
            I2C[i2c_num]->int_clr.ack_err = 1;
            if (p_i2c->mode == I2C_MODE_MASTER) {
                p_i2c_obj[i2c_num]->status = I2C_STATUS_ACK_ERROR;
                I2C[i2c_num]->int_clr.ack_err = 1;
                //get error ack value from slave device, stop the commands
                i2c_master_cmd_begin_static(i2c_num);
            }
        } else if (status & I2C_TRANS_START_INT_ST_M) {
            I2C[i2c_num]->int_clr.trans_start = 1;
        } else if (status & I2C_TIME_OUT_INT_ST_M) {
            I2C[i2c_num]->int_ena.time_out = 0;
            I2C[i2c_num]->int_clr.time_out = 1;
            p_i2c_obj[i2c_num]->status = I2C_STATUS_TIMEOUT;
            i2c_master_cmd_begin_static(i2c_num);
        } else if (status & I2C_TRANS_COMPLETE_INT_ST_M) {
            I2C[i2c_num]->int_clr.trans_complete = 1;
            if (p_i2c->mode == I2C_MODE_SLAVE) {
                int rx_fifo_cnt = I2C[i2c_num]->status_reg.rx_fifo_cnt;
                for (idx = 0; idx < rx_fifo_cnt; idx++) {
                    #if CONFIG_IDF_TARGET_ESP32
                    p_i2c->data_buf[idx] = I2C[i2c_num]->fifo_data.data;
                    #elif CONFIG_IDF_TARGET_ESP32S2BETA
                    p_i2c->data_buf[idx] = READ_PERI_REG(I2C_DATA_APB_REG(i2c_num));
                    #endif
                }
                xRingbufferSendFromISR(p_i2c->rx_ring_buf, p_i2c->data_buf, rx_fifo_cnt, &HPTaskAwoken);
                I2C[i2c_num]->int_clr.rx_fifo_full = 1;
            } else {
                // add check for unexcepted situations caused by noise.
                if (p_i2c->status != I2C_STATUS_ACK_ERROR && p_i2c->status != I2C_STATUS_IDLE) {
                    i2c_master_cmd_begin_static(i2c_num);
                }
            }
        } else if (status & I2C_MASTER_TRAN_COMP_INT_ST_M) {
            I2C[i2c_num]->int_clr.master_tran_comp = 1;
        } else if (status & I2C_ARBITRATION_LOST_INT_ST_M) {
            I2C[i2c_num]->int_clr.arbitration_lost = 1;
            p_i2c_obj[i2c_num]->status = I2C_STATUS_TIMEOUT;
            i2c_master_cmd_begin_static(i2c_num);
        } else if (status & I2C_SLAVE_TRAN_COMP_INT_ST_M) {
            I2C[i2c_num]->int_clr.slave_tran_comp = 1;
        } else if (status & I2C_END_DETECT_INT_ST_M) {
            I2C[i2c_num]->int_ena.end_detect = 0;
            I2C[i2c_num]->int_clr.end_detect = 1;
            i2c_master_cmd_begin_static(i2c_num);
        } else if (status & I2C_RXFIFO_OVF_INT_ST_M) {
            I2C[i2c_num]->int_clr.rx_fifo_ovf = 1;
        } else if (status & I2C_TXFIFO_EMPTY_INT_ST_M) {
            int tx_fifo_rem = I2C_FIFO_LEN - I2C[i2c_num]->status_reg.tx_fifo_cnt;
            size_t size = 0;
            uint8_t *data = (uint8_t*) xRingbufferReceiveUpToFromISR(p_i2c->tx_ring_buf, &size, tx_fifo_rem);
            if (data) {
                for (idx = 0; idx < size; idx++) {
                    WRITE_PERI_REG(I2C_DATA_APB_REG(i2c_num), data[idx]);
                }
                vRingbufferReturnItemFromISR(p_i2c->tx_ring_buf, data, &HPTaskAwoken);
                I2C[i2c_num]->int_ena.tx_fifo_empty = 1;
                I2C[i2c_num]->int_clr.tx_fifo_empty = 1;
            } else {
                I2C[i2c_num]->int_ena.tx_fifo_empty = 0;
                I2C[i2c_num]->int_clr.tx_fifo_empty = 1;
            }
        } else if (status & I2C_RXFIFO_FULL_INT_ST_M) {
            int rx_fifo_cnt = I2C[i2c_num]->status_reg.rx_fifo_cnt;
            for (idx = 0; idx < rx_fifo_cnt; idx++) {
                #if CONFIG_IDF_TARGET_ESP32
                p_i2c->data_buf[idx] = I2C[i2c_num]->fifo_data.data;
                #elif CONFIG_IDF_TARGET_ESP32S2BETA
                p_i2c->data_buf[idx] = READ_PERI_REG(I2C_DATA_APB_REG(i2c_num));
                #endif
            }
            xRingbufferSendFromISR(p_i2c->rx_ring_buf, p_i2c->data_buf, rx_fifo_cnt, &HPTaskAwoken);
            I2C[i2c_num]->int_clr.rx_fifo_full = 1;
        } else {
            I2C[i2c_num]->int_clr.val = status;
        }
    }
    if (p_i2c->mode == I2C_MODE_MASTER) {
        i2c_cmd_evt_t evt;
        evt.type = I2C_CMD_EVT_ALIVE;
        xQueueSendFromISR(p_i2c->cmd_evt_queue, &evt, &HPTaskAwoken);
    }
    //We only need to check here if there is a high-priority task needs to be switched.
    if(HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t i2c_set_data_mode(i2c_port_t i2c_num, i2c_trans_mode_t tx_trans_mode, i2c_trans_mode_t rx_trans_mode)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(tx_trans_mode < I2C_DATA_MODE_MAX, I2C_TRANS_MODE_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(rx_trans_mode < I2C_DATA_MODE_MAX, I2C_TRANS_MODE_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    I2C[i2c_num]->ctr.rx_lsb_first = rx_trans_mode; //set rx data msb first
    I2C[i2c_num]->ctr.tx_lsb_first = tx_trans_mode; //set tx data msb first
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_get_data_mode(i2c_port_t i2c_num, i2c_trans_mode_t *tx_trans_mode, i2c_trans_mode_t *rx_trans_mode)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    if (tx_trans_mode) {
        *tx_trans_mode = I2C[i2c_num]->ctr.tx_lsb_first;
    }
    if (rx_trans_mode) {
        *rx_trans_mode = I2C[i2c_num]->ctr.rx_lsb_first;
    }
    return ESP_OK;
}

/* Some slave device will die by accident and keep the SDA in low level,
 * in this case, master should send several clock to make the slave release the bus.
 * Slave mode of ESP32 might also get in wrong state that held the SDA low,
 * in this case, master device could send a stop signal to make esp32 slave release the bus.
 **/
static esp_err_t i2c_master_clear_bus(i2c_port_t i2c_num)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    const int scl_half_period = I2C_CLR_BUS_HALF_PERIOD_US; // use standard 100kHz data rate
    int sda_in_sig = 0, scl_in_sig = 0;
    int i = 0;
    if (i2c_num == I2C_NUM_0) {
        sda_in_sig = I2CEXT0_SDA_IN_IDX;
        scl_in_sig = I2CEXT0_SCL_IN_IDX;
    } else if (i2c_num == I2C_NUM_1) {
        sda_in_sig = I2CEXT1_SDA_IN_IDX;
        scl_in_sig = I2CEXT1_SCL_IN_IDX;
    }
    int scl_io = GPIO.func_in_sel_cfg[scl_in_sig].func_sel;
    int sda_io = GPIO.func_in_sel_cfg[sda_in_sig].func_sel;
    I2C_CHECK((GPIO_IS_VALID_OUTPUT_GPIO(scl_io)), I2C_SCL_IO_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((GPIO_IS_VALID_OUTPUT_GPIO(sda_io)), I2C_SDA_IO_ERR_STR, ESP_ERR_INVALID_ARG);
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
    while(!gpio_get_level(sda_io) && (i++ < I2C_CLR_BUS_SCL_NUM)) {
        gpio_set_level(scl_io, 1);
        ets_delay_us(scl_half_period);
        gpio_set_level(scl_io, 0);
        ets_delay_us(scl_half_period);
    }
    gpio_set_level(sda_io,0); // setup for STOP
    gpio_set_level(scl_io,1);
    ets_delay_us(scl_half_period);
    gpio_set_level(sda_io, 1); // STOP, SDA low -> high while SCL is HIGH
    i2c_set_pin(i2c_num, sda_io, scl_io, 1, 1, I2C_MODE_MASTER);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    I2C[i2c_num]->scl_sp_conf.scl_rst_slv_num = 9;
    I2C[i2c_num]->scl_sp_conf.scl_rst_slv_en = 1;
#endif
    return ESP_OK;
}

/**if the power and SDA/SCL wires are in proper condition, everything works find with reading the slave.
 * If we remove the power supply for the slave during I2C is reading, or directly connect SDA or SCL to ground,
 * this would cause the I2C FSM get stuck in wrong state, all we can do is to reset the I2C hardware in this case.
 **/
static esp_err_t i2c_hw_fsm_reset(i2c_port_t i2c_num)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    uint32_t ctr              = I2C[i2c_num]->ctr.val;
    uint32_t fifo_conf        = I2C[i2c_num]->fifo_conf.val;
    uint32_t scl_low_period   = I2C[i2c_num]->scl_low_period.val;
    uint32_t scl_high_period  = I2C[i2c_num]->scl_high_period.val;
    uint32_t scl_start_hold   = I2C[i2c_num]->scl_start_hold.val;
    uint32_t scl_rstart_setup = I2C[i2c_num]->scl_rstart_setup.val;
    uint32_t scl_stop_hold    = I2C[i2c_num]->scl_stop_hold.val;
    uint32_t scl_stop_setup   = I2C[i2c_num]->scl_stop_setup.val;
    uint32_t sda_hold         = I2C[i2c_num]->sda_hold.val;
    uint32_t sda_sample       = I2C[i2c_num]->sda_sample.val;
    uint32_t timeout          = I2C[i2c_num]->timeout.val;
    uint32_t scl_filter_cfg   = I2C[i2c_num]->scl_filter_cfg.val;
    uint32_t sda_filter_cfg   = I2C[i2c_num]->sda_filter_cfg.val;
    uint32_t slave_addr       = I2C[i2c_num]->slave_addr.val;

    //to reset the I2C hw module, we need re-enable the hw
    i2c_hw_disable(i2c_num);
    i2c_master_clear_bus(i2c_num);
    i2c_hw_enable(i2c_num);

    I2C[i2c_num]->ctr.val              = ctr & (~I2C_TRANS_START_M);
    I2C[i2c_num]->fifo_conf.val        = fifo_conf;
    I2C[i2c_num]->scl_low_period.val   = scl_low_period;
    I2C[i2c_num]->scl_high_period.val  = scl_high_period;
    I2C[i2c_num]->scl_start_hold.val   = scl_start_hold;
    I2C[i2c_num]->scl_rstart_setup.val = scl_rstart_setup;
    I2C[i2c_num]->scl_stop_hold.val    = scl_stop_hold;
    I2C[i2c_num]->scl_stop_setup.val   = scl_stop_setup;
    I2C[i2c_num]->sda_hold.val         = sda_hold;
    I2C[i2c_num]->sda_sample.val       = sda_sample;
    I2C[i2c_num]->timeout.val          = timeout;
    I2C[i2c_num]->scl_filter_cfg.val   = scl_filter_cfg;
    I2C[i2c_num]->sda_filter_cfg.val   = sda_filter_cfg;
    I2C[i2c_num]->slave_addr.val       = slave_addr;
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    i2c_master_clear_bus(i2c_num);

    I2C[i2c_num]->ctr.fsm_rst = 1;
    I2C[i2c_num]->ctr.fsm_rst = 0;
    I2C[i2c_num]->fifo_conf.rx_fifo_rst = 1;
    I2C[i2c_num]->fifo_conf.rx_fifo_rst = 0;
    I2C[i2c_num]->fifo_conf.tx_fifo_rst = 1;
    I2C[i2c_num]->fifo_conf.tx_fifo_rst = 0;
#endif

    I2C[i2c_num]->int_ena.val = 0;
    uint32_t intr_mask = I2C_TRANS_COMPLETE_INT_ENA_M
                         | I2C_TRANS_START_INT_ENA_M
                         | I2C_ACK_ERR_INT_ENA_M
                         | I2C_RXFIFO_OVF_INT_ENA_M
                         | I2C_SLAVE_TRAN_COMP_INT_ENA_M
                         | I2C_TIME_OUT_INT_ENA_M
                         | I2C_RXFIFO_FULL_INT_ENA_M
                         | I2C_ARBITRATION_LOST_INT_ENA_M;

    I2C[i2c_num]->int_clr.val = intr_mask;
    I2C[i2c_num]->int_ena.val = intr_mask;
    return ESP_OK;
}

esp_err_t i2c_param_config(i2c_port_t i2c_num, const i2c_config_t* i2c_conf)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(i2c_conf != NULL, I2C_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(i2c_conf->mode < I2C_MODE_MAX, I2C_MODE_ERR_STR, ESP_ERR_INVALID_ARG);

    esp_err_t ret = i2c_set_pin(i2c_num, i2c_conf->sda_io_num, i2c_conf->scl_io_num,
                                i2c_conf->sda_pullup_en, i2c_conf->scl_pullup_en, i2c_conf->mode);
    if (ret != ESP_OK) {
        return ret;
    }

    // Reset the I2C hardware in case there is a soft reboot.
    i2c_hw_disable(i2c_num);
    i2c_hw_enable(i2c_num);
    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    I2C[i2c_num]->ctr.rx_lsb_first = I2C_DATA_MODE_MSB_FIRST; //set rx data msb first
    I2C[i2c_num]->ctr.tx_lsb_first = I2C_DATA_MODE_MSB_FIRST; //set tx data msb first
    I2C[i2c_num]->ctr.ms_mode = i2c_conf->mode; //mode for master or slave
    I2C[i2c_num]->ctr.sda_force_out = 1; // set open-drain output mode
    I2C[i2c_num]->ctr.scl_force_out = 1; // set open-drain output mode
    I2C[i2c_num]->ctr.sample_scl_level = 0; //sample at high level of clock

#if CONFIG_IDF_TARGET_ESP32S2BETA
    I2C[i2c_num]->ctr.ref_always_on = 1;
    I2C[i2c_num]->sda_filter_cfg.val = 0;
    I2C[i2c_num]->scl_filter_cfg.val = 0;
#endif

    if (i2c_conf->mode == I2C_MODE_SLAVE) {  //slave mode
        I2C[i2c_num]->slave_addr.addr = i2c_conf->slave.slave_addr;
        I2C[i2c_num]->slave_addr.en_10bit = i2c_conf->slave.addr_10bit_en;
        I2C[i2c_num]->fifo_conf.nonfifo_en = 0;
        I2C[i2c_num]->fifo_conf.fifo_addr_cfg_en = 0;
        I2C[i2c_num]->fifo_conf.rx_fifo_full_thrhd = I2C_FIFO_FULL_THRESH_VAL;
        I2C[i2c_num]->fifo_conf.tx_fifo_empty_thrhd = I2C_FIFO_EMPTY_THRESH_VAL;
        I2C[i2c_num]->ctr.trans_start = 0;
        I2C[i2c_num]->timeout.tout = I2C_SLAVE_TIMEOUT_DEFAULT;
        //set timing for data
        I2C[i2c_num]->sda_hold.time = I2C_SLAVE_SDA_HOLD_DEFAULT;
        I2C[i2c_num]->sda_sample.time = I2C_SLAVE_SDA_SAMPLE_DEFAULT;
    } else {
        I2C[i2c_num]->fifo_conf.nonfifo_en = 0;
        int cycle = (I2C_APB_CLK_FREQ / i2c_conf->master.clk_speed);
        int half_cycle = cycle / 2;
        I2C[i2c_num]->timeout.tout = cycle * I2C_MASTER_TOUT_CNUM_DEFAULT;
        //set timing for data
        I2C[i2c_num]->sda_hold.time = half_cycle / 2;

#if CONFIG_IDF_TARGET_ESP32
        I2C[i2c_num]->sda_sample.time = half_cycle / 2;
        I2C[i2c_num]->scl_low_period.period = half_cycle;
        I2C[i2c_num]->scl_high_period.period = half_cycle;
#elif CONFIG_IDF_TARGET_ESP32S2BETA
        int low_period = half_cycle;
        int wait_high = 0;
        int high_period = cycle - low_period - wait_high;
        I2C[i2c_num]->sda_sample.time = high_period / 2;
        I2C[i2c_num]->scl_low_period.period = low_period;
        I2C[i2c_num]->scl_high_period.period = high_period;
        I2C[i2c_num]->scl_high_period.scl_wait_high_period = wait_high;
#endif

        //set timing for start signal
        I2C[i2c_num]->scl_start_hold.time = half_cycle;
        I2C[i2c_num]->scl_rstart_setup.time = half_cycle;
        //set timing for stop signal
        I2C[i2c_num]->scl_stop_hold.time = half_cycle;
        I2C[i2c_num]->scl_stop_setup.time = half_cycle;
        //Default, we enable hardware filter
        i2c_filter_enable(i2c_num, I2C_FILTER_CYC_NUM_DEF);
    }

    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_set_period(i2c_port_t i2c_num, int high_period, int low_period)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((high_period <= I2C_SCL_HIGH_PERIOD_V) && (high_period > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((low_period <= I2C_SCL_LOW_PERIOD_V) && (low_period > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);

    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    I2C[i2c_num]->scl_high_period.period = high_period;
    I2C[i2c_num]->scl_low_period.period = low_period;
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_get_period(i2c_port_t i2c_num, int* high_period, int* low_period)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    if (high_period) {
        *high_period = I2C[i2c_num]->scl_high_period.period;
    }
    if (low_period) {
        *low_period = I2C[i2c_num]->scl_low_period.period;
    }
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_filter_enable(i2c_port_t i2c_num, uint8_t cyc_num)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    I2C[i2c_num]->scl_filter_cfg.thres = cyc_num;
    I2C[i2c_num]->sda_filter_cfg.thres = cyc_num;
    I2C[i2c_num]->scl_filter_cfg.en = 1;
    I2C[i2c_num]->sda_filter_cfg.en = 1;
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_filter_disable(i2c_port_t i2c_num)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    I2C[i2c_num]->scl_filter_cfg.en = 0;
    I2C[i2c_num]->sda_filter_cfg.en = 0;
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_set_start_timing(i2c_port_t i2c_num, int setup_time, int hold_time)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((hold_time <= I2C_SCL_START_HOLD_TIME_V) && (hold_time > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((setup_time <= I2C_SCL_RSTART_SETUP_TIME_V) && (setup_time > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);

    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    I2C[i2c_num]->scl_start_hold.time = hold_time;
    I2C[i2c_num]->scl_rstart_setup.time = setup_time;
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_get_start_timing(i2c_port_t i2c_num, int* setup_time, int* hold_time)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    if (hold_time) {
        *hold_time = I2C[i2c_num]->scl_start_hold.time;
    }
    if (setup_time) {
        *setup_time = I2C[i2c_num]->scl_rstart_setup.time;
    }
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_set_stop_timing(i2c_port_t i2c_num, int setup_time, int hold_time)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((setup_time <= I2C_SCL_STOP_SETUP_TIME_V) && (setup_time > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((hold_time <= I2C_SCL_STOP_HOLD_TIME_V) && (hold_time > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);

    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    I2C[i2c_num]->scl_stop_hold.time = hold_time;
    I2C[i2c_num]->scl_stop_setup.time = setup_time;
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_get_stop_timing(i2c_port_t i2c_num, int* setup_time, int* hold_time)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    if (setup_time) {
        *setup_time = I2C[i2c_num]->scl_stop_setup.time;
    }
    if (hold_time) {
        *hold_time = I2C[i2c_num]->scl_stop_hold.time;
    }
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_set_data_timing(i2c_port_t i2c_num, int sample_time, int hold_time)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((sample_time <= I2C_SDA_SAMPLE_TIME_V) && (sample_time > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((hold_time <= I2C_SDA_HOLD_TIME_V) && (hold_time > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);

    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    I2C[i2c_num]->sda_hold.time = hold_time;
    I2C[i2c_num]->sda_sample.time = sample_time;
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_get_data_timing(i2c_port_t i2c_num, int* sample_time, int* hold_time)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    if (sample_time) {
        *sample_time = I2C[i2c_num]->sda_sample.time;
    }
    if (hold_time) {
        *hold_time = I2C[i2c_num]->sda_hold.time;
    }
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_set_timeout(i2c_port_t i2c_num, int timeout)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((timeout <= I2C_TIME_OUT_REG_V) && (timeout > 0), I2C_TIMEING_VAL_ERR_STR, ESP_ERR_INVALID_ARG);

    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    I2C[i2c_num]->timeout.tout = timeout;
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_get_timeout(i2c_port_t i2c_num, int* timeout)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    if (timeout) {
        *timeout = I2C[i2c_num]->timeout.tout;
    }
    return ESP_OK;
}

esp_err_t i2c_isr_register(i2c_port_t i2c_num, void (*fn)(void*), void * arg, int intr_alloc_flags, intr_handle_t *handle)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(fn != NULL, I2C_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    esp_err_t ret;
    switch (i2c_num) {
        case I2C_NUM_1:
            ret = esp_intr_alloc(ETS_I2C_EXT1_INTR_SOURCE, intr_alloc_flags, fn, arg, handle);
            break;
        case I2C_NUM_0:
            default:
            ret = esp_intr_alloc(ETS_I2C_EXT0_INTR_SOURCE, intr_alloc_flags, fn, arg, handle);
            break;
    }
    return ret;
}

esp_err_t i2c_isr_free(intr_handle_t handle)
{
    return esp_intr_free(handle);
}

esp_err_t i2c_set_pin(i2c_port_t i2c_num, int sda_io_num, int scl_io_num, gpio_pullup_t sda_pullup_en, gpio_pullup_t scl_pullup_en, i2c_mode_t mode)
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

    int sda_in_sig, sda_out_sig, scl_in_sig, scl_out_sig;
    switch (i2c_num) {
        case I2C_NUM_1:
            sda_out_sig = I2CEXT1_SDA_OUT_IDX;
            sda_in_sig = I2CEXT1_SDA_IN_IDX;
            scl_out_sig = I2CEXT1_SCL_OUT_IDX;
            scl_in_sig = I2CEXT1_SCL_IN_IDX;
            break;
        case I2C_NUM_0:
            default:
            sda_out_sig = I2CEXT0_SDA_OUT_IDX;
            sda_in_sig = I2CEXT0_SDA_IN_IDX;
            scl_out_sig = I2CEXT0_SCL_OUT_IDX;
            scl_in_sig = I2CEXT0_SCL_IN_IDX;
            break;
    }
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
        if (mode == I2C_MODE_MASTER) {
            gpio_set_direction(scl_io_num, GPIO_MODE_INPUT_OUTPUT_OD);
            gpio_matrix_out(scl_io_num, scl_out_sig, 0, 0);
        } else {
            gpio_set_direction(scl_io_num, GPIO_MODE_INPUT);
        }
        if (scl_pullup_en == GPIO_PULLUP_ENABLE) {
            gpio_set_pull_mode(scl_io_num, GPIO_PULLUP_ONLY);
        } else {
            gpio_set_pull_mode(scl_io_num, GPIO_FLOATING);
        }
        gpio_matrix_in(scl_io_num, scl_in_sig, 0);
    }
    return ESP_OK;
}

i2c_cmd_handle_t i2c_cmd_link_create(void)
{
#if !CONFIG_SPIRAM_USE_MALLOC
    i2c_cmd_desc_t* cmd_desc = (i2c_cmd_desc_t*) calloc(1, sizeof(i2c_cmd_desc_t));
#else
    i2c_cmd_desc_t* cmd_desc = (i2c_cmd_desc_t*) heap_caps_calloc(1, sizeof(i2c_cmd_desc_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
    return (i2c_cmd_handle_t) cmd_desc;
}

void i2c_cmd_link_delete(i2c_cmd_handle_t cmd_handle)
{
    if (cmd_handle == NULL) {
        return;
    }
    i2c_cmd_desc_t* cmd = (i2c_cmd_desc_t*) cmd_handle;
    while (cmd->free) {
        i2c_cmd_link_t* ptmp = cmd->free;
        cmd->free = cmd->free->next;
        free(ptmp);
    }
    cmd->cur = NULL;
    cmd->free = NULL;
    cmd->head = NULL;
    free(cmd_handle);
    return;
}

static esp_err_t i2c_cmd_link_append(i2c_cmd_handle_t cmd_handle, i2c_cmd_t* cmd)
{
    i2c_cmd_desc_t* cmd_desc = (i2c_cmd_desc_t*) cmd_handle;
    if (cmd_desc->head == NULL) {
#if !CONFIG_SPIRAM_USE_MALLOC
        cmd_desc->head = (i2c_cmd_link_t*) calloc(1, sizeof(i2c_cmd_link_t));
#else
        cmd_desc->head = (i2c_cmd_link_t*) heap_caps_calloc(1, sizeof(i2c_cmd_link_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
        if (cmd_desc->head == NULL) {
            ESP_LOGE(I2C_TAG, I2C_CMD_MALLOC_ERR_STR);
            goto err;
        }
        cmd_desc->cur = cmd_desc->head;
        cmd_desc->free = cmd_desc->head;
    } else {
#if !CONFIG_SPIRAM_USE_MALLOC
        cmd_desc->cur->next = (i2c_cmd_link_t*) calloc(1, sizeof(i2c_cmd_link_t));
#else
        cmd_desc->cur->next = (i2c_cmd_link_t*) heap_caps_calloc(1, sizeof(i2c_cmd_link_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
        if (cmd_desc->cur->next == NULL) {
            ESP_LOGE(I2C_TAG, I2C_CMD_MALLOC_ERR_STR);
            goto err;
        }
        cmd_desc->cur = cmd_desc->cur->next;
    }
    memcpy((uint8_t*) &cmd_desc->cur->cmd, (uint8_t*) cmd, sizeof(i2c_cmd_t));
    cmd_desc->cur->next = NULL;
    return ESP_OK;

    err:
    return ESP_FAIL;
}

esp_err_t i2c_master_start(i2c_cmd_handle_t cmd_handle)
{
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);
    i2c_cmd_t cmd;
    cmd.ack_en = 0;
    cmd.ack_exp = 0;
    cmd.ack_val = 0;
    cmd.byte_num = 0;
    cmd.data = NULL;
    cmd.op_code = I2C_CMD_RESTART;
    return i2c_cmd_link_append(cmd_handle, &cmd);
}

esp_err_t i2c_master_stop(i2c_cmd_handle_t cmd_handle)
{
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);
    i2c_cmd_t cmd;
    cmd.ack_en = 0;
    cmd.ack_exp = 0;
    cmd.ack_val = 0;
    cmd.byte_num = 0;
    cmd.data = NULL;
    cmd.op_code = I2C_CMD_STOP;
    return i2c_cmd_link_append(cmd_handle, &cmd);
}

esp_err_t i2c_master_write(i2c_cmd_handle_t cmd_handle, uint8_t* data, size_t data_len, bool ack_en)
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
        cmd.ack_en = ack_en;
        cmd.ack_exp = 0;
        cmd.ack_val = 0;
        cmd.byte_num = len_tmp;
        cmd.op_code = I2C_CMD_WRITE;
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
    cmd.ack_en = ack_en;
    cmd.ack_exp = 0;
    cmd.ack_val = 0;
    cmd.byte_num = 1;
    cmd.op_code = I2C_CMD_WRITE;
    cmd.data = NULL;
    cmd.byte_cmd = data;
    return i2c_cmd_link_append(cmd_handle, &cmd);
}

static esp_err_t i2c_master_read_static(i2c_cmd_handle_t cmd_handle, uint8_t* data, size_t data_len, i2c_ack_type_t ack)
{
    int len_tmp;
    int data_offset = 0;
    esp_err_t ret;
    while (data_len > 0) {
        len_tmp = data_len > 0xff ? 0xff : data_len;
        data_len -= len_tmp;
        i2c_cmd_t cmd;
        cmd.ack_en = 0;
        cmd.ack_exp = 0;
        cmd.ack_val = ack & 0x1;
        cmd.byte_num = len_tmp;
        cmd.op_code = I2C_CMD_READ;
        cmd.data = data + data_offset;
        ret = i2c_cmd_link_append(cmd_handle, &cmd);
        data_offset += len_tmp;
        if (ret != ESP_OK) {
            return ret;
        }
    }
    return ESP_OK;
}

esp_err_t i2c_master_read_byte(i2c_cmd_handle_t cmd_handle, uint8_t* data, i2c_ack_type_t ack)
{
    I2C_CHECK((data != NULL), I2C_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(ack < I2C_MASTER_ACK_MAX, I2C_ACK_TYPE_ERR_STR, ESP_ERR_INVALID_ARG);

    i2c_cmd_t cmd;
    cmd.ack_en = 0;
    cmd.ack_exp = 0;
    cmd.ack_val = ((ack == I2C_MASTER_LAST_NACK) ? I2C_MASTER_NACK : (ack & 0x1));
    cmd.byte_num = 1;
    cmd.op_code = I2C_CMD_READ;
    cmd.data = data;
    return i2c_cmd_link_append(cmd_handle, &cmd);
}

esp_err_t i2c_master_read(i2c_cmd_handle_t cmd_handle, uint8_t* data, size_t data_len, i2c_ack_type_t ack)
{
    I2C_CHECK((data != NULL), I2C_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(ack < I2C_MASTER_ACK_MAX, I2C_ACK_TYPE_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(data_len > 0, I2C_DATA_LEN_ERR_STR, ESP_ERR_INVALID_ARG);

    if(ack != I2C_MASTER_LAST_NACK) {
        return i2c_master_read_static(cmd_handle, data, data_len, ack);
    } else {
        if(data_len == 1) {
            return i2c_master_read_byte(cmd_handle, data, I2C_MASTER_NACK);
        } else {
            esp_err_t ret;
            if((ret =  i2c_master_read_static(cmd_handle, data, data_len - 1, I2C_MASTER_ACK)) != ESP_OK) {
                return ret;
            }
            return i2c_master_read_byte(cmd_handle, data + data_len - 1, I2C_MASTER_NACK);
        }
    }
}

static void IRAM_ATTR i2c_master_cmd_begin_static(i2c_port_t i2c_num)
{
    i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];
    portBASE_TYPE HPTaskAwoken = pdFALSE;
    i2c_cmd_evt_t evt;
    //This should never happen
    if (p_i2c->mode == I2C_MODE_SLAVE) {
        return;
    }
    if (p_i2c->status == I2C_STATUS_DONE) {
        return;
    } else if ((p_i2c->status == I2C_STATUS_ACK_ERROR)
            || (p_i2c->status == I2C_STATUS_TIMEOUT)) {
        I2C[i2c_num]->int_ena.end_detect = 0;
        I2C[i2c_num]->int_clr.end_detect = 1;
        if(p_i2c->status == I2C_STATUS_TIMEOUT) {
            I2C[i2c_num]->int_clr.time_out = 1;
            I2C[i2c_num]->int_ena.val = 0;
        }
        evt.type = I2C_CMD_EVT_DONE;
        xQueueOverwriteFromISR(p_i2c->cmd_evt_queue, &evt, &HPTaskAwoken);
        if (HPTaskAwoken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
        return;
    } else if (p_i2c->cmd_link.head != NULL && p_i2c->status == I2C_STATUS_READ) {
        i2c_cmd_t *cmd = &p_i2c->cmd_link.head->cmd;
        while (p_i2c->rx_cnt-- > 0) {
            *cmd->data++ = READ_PERI_REG(I2C_DATA_APB_REG(i2c_num));
        }
        if (cmd->byte_num > 0) {
            p_i2c->rx_fifo_remain = I2C_FIFO_LEN;
            p_i2c->cmd_idx = 0;
        } else {
            p_i2c->cmd_link.head = p_i2c->cmd_link.head->next;
        }
    }
    if (p_i2c->cmd_link.head == NULL) {
        p_i2c->cmd_link.cur = NULL;
        evt.type = I2C_CMD_EVT_DONE;
        xQueueOverwriteFromISR(p_i2c->cmd_evt_queue, &evt, &HPTaskAwoken);
        if (HPTaskAwoken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
        // Return to the IDLE status after cmd_eve_done signal were send out.
        p_i2c->status = I2C_STATUS_IDLE;
        return;
    }
    while (p_i2c->cmd_link.head) {
        i2c_cmd_t *cmd = &p_i2c->cmd_link.head->cmd;
        i2c_hw_cmd_t * const p_cur_hw_cmd = &I2C[i2c_num]->command[p_i2c->cmd_idx];
        i2c_hw_cmd_t hw_cmd = {
            .ack_en = cmd->ack_en,
            .ack_exp = cmd->ack_exp,
            .ack_val = cmd->ack_val,
            .byte_num = cmd->byte_num,
            .op_code = cmd->op_code
        };
        const i2c_hw_cmd_t hw_end_cmd = {
            .op_code = I2C_CMD_END
        };
        if (cmd->op_code == I2C_CMD_WRITE) {
            uint32_t wr_filled = 0;
            //TODO: to reduce interrupt number
            if (cmd->data) {
                while (p_i2c->tx_fifo_remain > 0 && cmd->byte_num > 0) {
                    WRITE_PERI_REG(I2C_DATA_APB_REG(i2c_num), *cmd->data++);
                    p_i2c->tx_fifo_remain--;
                    cmd->byte_num--;
                    wr_filled++;
                }
            } else {
                WRITE_PERI_REG(I2C_DATA_APB_REG(i2c_num), cmd->byte_cmd);
                p_i2c->tx_fifo_remain--;
                cmd->byte_num--;
                wr_filled ++;
            }
            hw_cmd.byte_num = wr_filled;
            *p_cur_hw_cmd = hw_cmd;
            *(p_cur_hw_cmd + 1) = hw_end_cmd;
            p_i2c->tx_fifo_remain = I2C_FIFO_LEN;
            p_i2c->cmd_idx = 0;
            if (cmd->byte_num > 0) {
            } else {
                p_i2c->cmd_link.head = p_i2c->cmd_link.head->next;
            }
            p_i2c->status = I2C_STATUS_WRITE;
            break;
        } else if(cmd->op_code == I2C_CMD_READ) {
            //TODO: to reduce interrupt number
            p_i2c->rx_cnt = cmd->byte_num > p_i2c->rx_fifo_remain ? p_i2c->rx_fifo_remain : cmd->byte_num;
            cmd->byte_num -= p_i2c->rx_cnt;
            hw_cmd.byte_num = p_i2c->rx_cnt;
            hw_cmd.ack_val = cmd->ack_val;
            *p_cur_hw_cmd = hw_cmd;
            *(p_cur_hw_cmd + 1) = hw_end_cmd;
            p_i2c->status = I2C_STATUS_READ;
            break;
        } else {
            *p_cur_hw_cmd = hw_cmd;
        }
        p_i2c->cmd_idx++;
        p_i2c->cmd_link.head = p_i2c->cmd_link.head->next;
        if (p_i2c->cmd_link.head == NULL || p_i2c->cmd_idx >= 15) {
            p_i2c->tx_fifo_remain = I2C_FIFO_LEN;
            p_i2c->cmd_idx = 0;
            break;
        }
    }
    I2C[i2c_num]->int_clr.end_detect = 1;
    I2C[i2c_num]->int_ena.end_detect = 1;
    I2C[i2c_num]->ctr.trans_start = 0;
    I2C[i2c_num]->ctr.trans_start = 1;
    return;
}

#if CONFIG_SPIRAM_USE_MALLOC
//Check whether read or write buffer in cmd_link is internal.
static bool is_cmd_link_buffer_internal(i2c_cmd_link_t *link)
{
    i2c_cmd_link_t* cmd_link = link;
    while(cmd_link != NULL)  {
        if (cmd_link->cmd.op_code == I2C_CMD_WRITE || cmd_link->cmd.op_code == I2C_CMD_READ) {
            if( cmd_link->cmd.data != NULL && !esp_ptr_internal(cmd_link->cmd.data)) {
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
    if( (p_i2c_obj[i2c_num]->intr_alloc_flags & ESP_INTR_FLAG_IRAM) ) {
        if( !is_cmd_link_buffer_internal(((i2c_cmd_desc_t*)cmd_handle)->head) ) {
            ESP_LOGE(I2C_TAG, I2C_PSRAM_BUFFER_WARN_STR);
            return ESP_ERR_INVALID_ARG;
        }
    }
#endif
    // Sometimes when the FSM get stuck, the ACK_ERR interrupt will occur endlessly until we reset the FSM and clear bus.
    static uint8_t clear_bus_cnt = 0;
    esp_err_t ret = ESP_FAIL;
    i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];
    portTickType ticks_start = xTaskGetTickCount();
    portBASE_TYPE res = xSemaphoreTake(p_i2c->cmd_mux, ticks_to_wait);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(p_i2c->pm_lock);
#endif
    if (res == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }
    xQueueReset(p_i2c->cmd_evt_queue);
    if (p_i2c->status == I2C_STATUS_TIMEOUT
        || I2C[i2c_num]->status_reg.bus_busy == 1) {
        i2c_hw_fsm_reset(i2c_num);
        clear_bus_cnt = 0;
    }
    i2c_reset_tx_fifo(i2c_num);
    i2c_reset_rx_fifo(i2c_num);
    i2c_cmd_desc_t* cmd = (i2c_cmd_desc_t*) cmd_handle;
    p_i2c->cmd_link.free = cmd->free;
    p_i2c->cmd_link.cur = cmd->cur;
    p_i2c->cmd_link.head = cmd->head;
    p_i2c->status = I2C_STATUS_IDLE;
    p_i2c->cmd_idx = 0;
    p_i2c->rx_cnt = 0;
    p_i2c->tx_fifo_remain = I2C_FIFO_LEN;
    p_i2c->rx_fifo_remain = I2C_FIFO_LEN;
    i2c_reset_tx_fifo(i2c_num);
    i2c_reset_rx_fifo(i2c_num);
    // These two interrupts some times can not be cleared when the FSM gets stuck.
    // so we disable them when these two interrupt occurs and re-enable them here.
    I2C[i2c_num]->int_ena.ack_err = 1;
    I2C[i2c_num]->int_ena.time_out = 1;
    //start send commands, at most 32 bytes one time, isr handler will process the remaining commands.
    i2c_master_cmd_begin_static(i2c_num);

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
                    if(clear_bus_cnt >= I2C_ACKERR_CNT_MAX) {
                        i2c_master_clear_bus(i2c_num);
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

int i2c_slave_write_buffer(i2c_port_t i2c_num, uint8_t* data, int size, TickType_t ticks_to_wait)
{
    I2C_CHECK(( i2c_num < I2C_NUM_MAX ), I2C_NUM_ERROR_STR, ESP_FAIL);
    I2C_CHECK((data != NULL), I2C_ADDR_ERROR_STR, ESP_FAIL);
    I2C_CHECK(p_i2c_obj[i2c_num]->mode == I2C_MODE_SLAVE, I2C_MODE_SLAVE_ERR_STR, ESP_FAIL);
    i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];

    portBASE_TYPE res;
    int cnt = 0;
    portTickType ticks_start = xTaskGetTickCount();

    res = xSemaphoreTake(p_i2c->slv_tx_mux, ticks_to_wait);
    if (res == pdFALSE) {
        return 0;
    }
    TickType_t ticks_end = xTaskGetTickCount();
    if (ticks_end - ticks_start > ticks_to_wait) {
        ticks_to_wait = 0;
    } else {
        ticks_to_wait = ticks_to_wait - (ticks_end - ticks_start);
    }
    res = xRingbufferSend(p_i2c->tx_ring_buf, data, size, ticks_to_wait);
    if (res == pdFALSE) {
        cnt = 0;
    } else {
        I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
        I2C[i2c_num]->int_clr.tx_fifo_empty = 1;
        I2C[i2c_num]->int_ena.tx_fifo_empty = 1;
        I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
        cnt = size;
    }
    xSemaphoreGive(p_i2c->slv_tx_mux);
    return cnt;
}

static int i2c_slave_read(i2c_port_t i2c_num, uint8_t* data, size_t max_size, TickType_t ticks_to_wait)
{
    i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];
    size_t size = 0;
    uint8_t* pdata = (uint8_t*) xRingbufferReceiveUpTo(p_i2c->rx_ring_buf, &size, ticks_to_wait, max_size);
    if (pdata && size > 0) {
        memcpy(data, pdata, size);
        vRingbufferReturnItem(p_i2c->rx_ring_buf, pdata);
    }
    return size;
}

int i2c_slave_read_buffer(i2c_port_t i2c_num, uint8_t* data, size_t max_size, TickType_t ticks_to_wait)
{
    I2C_CHECK(( i2c_num < I2C_NUM_MAX ), I2C_NUM_ERROR_STR, ESP_FAIL);
    I2C_CHECK((data != NULL), I2C_ADDR_ERROR_STR, ESP_FAIL);
    I2C_CHECK(p_i2c_obj[i2c_num]->mode == I2C_MODE_SLAVE, I2C_MODE_SLAVE_ERR_STR, ESP_FAIL);

    i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];
    portBASE_TYPE res;
    portTickType ticks_start = xTaskGetTickCount();
    res = xSemaphoreTake(p_i2c->slv_rx_mux, ticks_to_wait);
    if (res == pdFALSE) {
        return 0;
    }
    TickType_t ticks_end = xTaskGetTickCount();
    if (ticks_end - ticks_start > ticks_to_wait) {
        ticks_to_wait = 0;
    } else {
        ticks_to_wait = ticks_to_wait - (ticks_end - ticks_start);
    }
    int cnt = i2c_slave_read(i2c_num, data, max_size, ticks_to_wait);
    if (cnt > 0) {
        I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
        I2C[i2c_num]->int_ena.rx_fifo_full = 1;
        I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
        ticks_end = xTaskGetTickCount();
        if (ticks_end - ticks_start > ticks_to_wait) {
            ticks_to_wait = 0;
        } else {
            ticks_to_wait = ticks_to_wait - (ticks_end - ticks_start);
        }
        if (cnt < max_size && ticks_to_wait > 0) {
            cnt += i2c_slave_read(i2c_num, data + cnt, max_size - cnt, ticks_to_wait);
        }
    } else {
        cnt = 0;
    }
    xSemaphoreGive(p_i2c->slv_rx_mux);
    return cnt;
}
