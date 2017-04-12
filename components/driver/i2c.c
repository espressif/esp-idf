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
#include "esp_intr.h"
#include "esp_log.h"
#include "malloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "soc/dport_reg.h"
#include "soc/i2c_struct.h"
#include "soc/i2c_reg.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"

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
#define I2C_ENTER_CRITICAL(mux)    portENTER_CRITICAL(mux)
#define I2C_EXIT_CRITICAL(mux)     portEXIT_CRITICAL(mux)

#define I2C_DRIVER_ERR_STR             "i2c driver install error"
#define I2C_DRIVER_MALLOC_ERR_STR      "i2c driver malloc error"
#define I2C_NUM_ERROR_STR              "i2c number error"
#define I2C_ADDR_ERROR_STR             "i2c null address error"
#define I2C_DRIVER_NOT_INSTALL_ERR_STR "i2c driver not installed"
#define I2C_SLAVE_BUFFER_LEN_ERR_STR   "i2c buffer size too short for slave mode"
#define I2C_EVT_QUEUE_ERR_STR          "i2c evt queue error"
#define I2C_SEM_ERR_STR                "i2c semaphore error"
#define I2C_BUF_ERR_STR                "i2c ringbuffer error"
#define I2C_MASTER_MODE_ERR_STR        "Only allowed in master mode"
#define I2C_MODE_SLAVE_ERR_STR         "Only allowed in slave mode"
#define I2C_CMD_MALLOC_ERR_STR         "i2c command link malloc error"
#define I2C_TRANS_MODE_ERR_STR         "i2c trans mode error"
#define I2C_MODE_ERR_STR               "i2c mode error"
#define I2C_SDA_IO_ERR_STR              "sda gpio number error"
#define I2C_SCL_IO_ERR_STR              "scl gpio number error"
#define I2C_CMD_LINK_INIT_ERR_STR       "i2c command link error"
#define I2C_GPIO_PULLUP_ERR_STR         "this i2c pin do not support internal pull-up"
#define I2C_FIFO_FULL_THRESH_VAL   (28)
#define I2C_FIFO_EMPTY_THRESH_VAL  (5)
#define I2C_IO_INIT_LEVEL          (1)

typedef struct {
    uint8_t byte_num;  /*!< cmd byte number */
    uint8_t ack_en;    /*!< ack check enable */
    uint8_t ack_exp;   /*!< expected ack level to get */
    uint8_t ack_val;   /*!< ack value to send */
    uint8_t* data;     /*!< data address */
    uint8_t byte_cmd;  /*!< to save cmd for one byte command mode */
    i2c_opmode_t op_code; /*!< haredware cmd type */
}i2c_cmd_t;

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
} i2c_status_t;

typedef struct {
    int i2c_num;                     /*!< I2C port number */
    int mode;                        /*!< I2C mode, master or slave */
    intr_handle_t intr_handle;       /*!< I2C interrupt handle*/

    int cmd_idx;                     /*!< record current command index, for master mode */
    int status;                      /*!< record current command status, for master mode */
    int rx_cnt;                      /*!< record current read index, for master mode */
    uint8_t data_buf[I2C_FIFO_LEN];  /*!< a buffer to store i2c fifo data */
    i2c_cmd_desc_t cmd_link;         /*!< I2C command link */
    xSemaphoreHandle cmd_sem;        /*!< semaphore to sync command status */
    xSemaphoreHandle cmd_mux;        /*!< semaphore to lock command process */
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
        p_i2c_obj[i2c_num] = (i2c_obj_t*) calloc(1, sizeof(i2c_obj_t));
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
                p_i2c->tx_ring_buf = NULL;
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
            if (p_i2c->slv_rx_mux == NULL || p_i2c->slv_rx_mux == NULL) {
                ESP_LOGE(I2C_TAG, I2C_SEM_ERR_STR);
                goto err;
            }
            intr_mask |= ( I2C_RXFIFO_FULL_INT_ENA_M | I2C_TRANS_COMPLETE_INT_ENA_M );
        } else {
            //semaphore to sync sending process, because we only have 32 bytes for hardware fifo.
            p_i2c->cmd_sem = xSemaphoreCreateBinary();
            p_i2c->cmd_mux = xSemaphoreCreateMutex();
            if (p_i2c->cmd_sem == NULL || p_i2c->cmd_mux == NULL) {
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
    //hook isr handler
    i2c_isr_register(i2c_num, i2c_isr_handler_default, p_i2c_obj[i2c_num], intr_alloc_flags, &p_i2c_obj[i2c_num]->intr_handle);
    intr_mask |= ( I2C_TRANS_COMPLETE_INT_ENA_M |
                   I2C_TRANS_START_INT_ENA_M |
                   I2C_ARBITRATION_LOST_INT_ENA_M |
                   I2C_ACK_ERR_INT_ENA_M |
                   I2C_RXFIFO_OVF_INT_ENA_M |
                   I2C_SLAVE_TRAN_COMP_INT_ENA_M );
    SET_PERI_REG_MASK(I2C_INT_ENA_REG(i2c_num), intr_mask);
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
        if (p_i2c_obj[i2c_num]->cmd_sem) {
            vSemaphoreDelete(p_i2c_obj[i2c_num]->cmd_sem);
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
    }
    free(p_i2c_obj[i2c_num]);
    return ESP_FAIL;
}

esp_err_t i2c_driver_delete(i2c_port_t i2c_num)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(p_i2c_obj[i2c_num] != NULL, I2C_DRIVER_ERR_STR, ESP_FAIL);

    i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];
    if (p_i2c->cmd_mux) {
        xSemaphoreTake(p_i2c->cmd_mux, portMAX_DELAY);
        vSemaphoreDelete(p_i2c->cmd_mux);
    }
    if (p_i2c->cmd_sem) {
        vSemaphoreDelete(p_i2c->cmd_sem);
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
    uint32_t intr_mask = I2C_MASTER_TRAN_COMP_INT_ENA_M |
                         I2C_TIME_OUT_INT_ENA_M |
                         I2C_TRANS_COMPLETE_INT_ENA_M |
                         I2C_TRANS_START_INT_ENA_M |
                         I2C_TX_SEND_EMPTY_INT_ENA_M |
                         I2C_ARBITRATION_LOST_INT_ENA_M |
                         I2C_ACK_ERR_INT_ENA_M |
                         I2C_RXFIFO_OVF_INT_ENA_M |
                         I2C_RX_REC_FULL_INT_ENA_M |
                         I2C_SLAVE_TRAN_COMP_INT_ENA_M;
    CLEAR_PERI_REG_MASK(I2C_INT_ENA_REG(i2c_num), intr_mask);
    esp_intr_free(p_i2c->intr_handle);
    p_i2c->intr_handle = NULL;
    free(p_i2c_obj[i2c_num]);
    p_i2c_obj[i2c_num] = NULL;
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

static void i2c_isr_handler_default(void* arg)
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
            I2C[i2c_num]->int_clr.time_out = 1;
        } else if (status & I2C_TRANS_COMPLETE_INT_ST_M) {
            I2C[i2c_num]->int_clr.trans_complete = 1;
            if (p_i2c->mode == I2C_MODE_SLAVE) {
                int rx_fifo_cnt = I2C[i2c_num]->status_reg.rx_fifo_cnt;
                for (idx = 0; idx < rx_fifo_cnt; idx++) {
                    p_i2c->data_buf[idx] = I2C[i2c_num]->fifo_data.data;
                }
                xRingbufferSendFromISR(p_i2c->rx_ring_buf, p_i2c->data_buf, rx_fifo_cnt, &HPTaskAwoken);
                if (HPTaskAwoken == pdTRUE) {
                    portYIELD_FROM_ISR();
                }
                I2C[i2c_num]->int_clr.rx_fifo_full = 1;
            } else {
                if (p_i2c->status != I2C_STATUS_ACK_ERROR) {
                    i2c_master_cmd_begin_static(i2c_num);
                }
            }
        } else if (status & I2C_MASTER_TRAN_COMP_INT_ST_M) {
            I2C[i2c_num]->int_clr.master_tran_comp = 1;
        } else if (status & I2C_ARBITRATION_LOST_INT_ST_M) {
            I2C[i2c_num]->int_clr.arbitration_lost = 1;
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
                if (HPTaskAwoken == pdTRUE) {
                    portYIELD_FROM_ISR();
                }
                I2C[i2c_num]->int_ena.tx_fifo_empty = 1;
                I2C[i2c_num]->int_clr.tx_fifo_empty = 1;
            } else {
                I2C[i2c_num]->int_ena.tx_fifo_empty = 0;
                I2C[i2c_num]->int_clr.tx_fifo_empty = 1;
            }
        } else if (status & I2C_RXFIFO_FULL_INT_ST_M) {
            int rx_fifo_cnt = I2C[i2c_num]->status_reg.rx_fifo_cnt;
            for (idx = 0; idx < rx_fifo_cnt; idx++) {
                p_i2c->data_buf[idx] = I2C[i2c_num]->fifo_data.data;
            }
            xRingbufferSendFromISR(p_i2c->rx_ring_buf, p_i2c->data_buf, rx_fifo_cnt, &HPTaskAwoken);
            if (HPTaskAwoken == pdTRUE) {
                portYIELD_FROM_ISR();
            }
            I2C[i2c_num]->int_clr.rx_fifo_full = 1;
        } else {
            I2C[i2c_num]->int_clr.val = status;
        }
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

esp_err_t i2c_param_config(i2c_port_t i2c_num, i2c_config_t* i2c_conf)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(i2c_conf != NULL, I2C_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(i2c_conf->mode < I2C_MODE_MAX, I2C_MODE_ERR_STR, ESP_ERR_INVALID_ARG);

    esp_err_t ret = i2c_set_pin(i2c_num, i2c_conf->sda_io_num, i2c_conf->scl_io_num,
                                i2c_conf->sda_pullup_en, i2c_conf->scl_pullup_en, i2c_conf->mode);
    if (ret != ESP_OK) {
        return ret;
    }
    if (i2c_num == I2C_NUM_0) {
        periph_module_enable(PERIPH_I2C0_MODULE);
    } else if (i2c_num == I2C_NUM_1) {
        periph_module_enable(PERIPH_I2C1_MODULE);
    }

    I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
    I2C[i2c_num]->ctr.rx_lsb_first = I2C_DATA_MODE_MSB_FIRST; //set rx data msb first
    I2C[i2c_num]->ctr.tx_lsb_first = I2C_DATA_MODE_MSB_FIRST; //set tx data msb first
    I2C[i2c_num]->ctr.ms_mode = i2c_conf->mode; //mode for master or slave
    I2C[i2c_num]->ctr.sda_force_out = 1; // set open-drain output mode
    I2C[i2c_num]->ctr.scl_force_out = 1; // set open-drain output mode
    I2C[i2c_num]->ctr.sample_scl_level = 0; //sample at high level of clock

    if (i2c_conf->mode == I2C_MODE_SLAVE) {  //slave mode
        I2C[i2c_num]->slave_addr.addr = i2c_conf->slave.slave_addr;
        I2C[i2c_num]->slave_addr.en_10bit = i2c_conf->slave.addr_10bit_en;
        I2C[i2c_num]->fifo_conf.nonfifo_en = 0;
        I2C[i2c_num]->fifo_conf.fifo_addr_cfg_en = 0;
        I2C[i2c_num]->fifo_conf.rx_fifo_full_thrhd = I2C_FIFO_FULL_THRESH_VAL;
        I2C[i2c_num]->fifo_conf.tx_fifo_empty_thrhd = I2C_FIFO_EMPTY_THRESH_VAL;
        I2C[i2c_num]->int_ena.rx_fifo_full = 1;
        I2C[i2c_num]->ctr.trans_start = 0;
    } else {
        I2C[i2c_num]->fifo_conf.nonfifo_en = 0;
    }
    //set frequency
    int half_cycle = ( I2C_APB_CLK_FREQ / i2c_conf->master.clk_speed ) / 2;
    I2C[i2c_num]->scl_low_period.period = half_cycle - 1;
    I2C[i2c_num]->scl_high_period.period = ( I2C_APB_CLK_FREQ / i2c_conf->master.clk_speed ) - half_cycle - 1;
    //set timing for start signal
    I2C[i2c_num]->scl_start_hold.time = half_cycle;
    I2C[i2c_num]->scl_rstart_setup.time = half_cycle;
    //set timing for stop signal
    I2C[i2c_num]->scl_stop_hold.time = half_cycle;
    I2C[i2c_num]->scl_stop_setup.time = half_cycle;
    //set timing for data
    I2C[i2c_num]->sda_hold.time = half_cycle / 2;
    I2C[i2c_num]->sda_sample.time = half_cycle / 2;
    //set timeout of receving data
    I2C[i2c_num]->timeout.tout = 200000;
    I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
    return ESP_OK;
}

esp_err_t i2c_set_period(i2c_port_t i2c_num, int high_period, int low_period)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
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

esp_err_t i2c_set_start_timing(i2c_port_t i2c_num, int setup_time, int hold_time)
{
    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C[i2c_num]->scl_start_hold.time = hold_time;
    I2C[i2c_num]->scl_rstart_setup.time = setup_time;
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
    I2C[i2c_num]->scl_stop_hold.time = hold_time;
    I2C[i2c_num]->scl_stop_setup.time = setup_time;
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
    I2C[i2c_num]->sda_hold.time = hold_time;
    I2C[i2c_num]->sda_sample.time = sample_time;
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

esp_err_t i2c_set_pin(i2c_port_t i2c_num, gpio_num_t sda_io_num, gpio_num_t scl_io_num, gpio_pullup_t sda_pullup_en, gpio_pullup_t scl_pullup_en, i2c_mode_t mode)
{
    I2C_CHECK(( i2c_num < I2C_NUM_MAX ), I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(((GPIO_IS_VALID_OUTPUT_GPIO(sda_io_num))), I2C_SDA_IO_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((GPIO_IS_VALID_OUTPUT_GPIO(scl_io_num)) ||
              (GPIO_IS_VALID_GPIO(scl_io_num) && mode == I2C_MODE_SLAVE),
              I2C_SCL_IO_ERR_STR,
              ESP_ERR_INVALID_ARG);
    I2C_CHECK((sda_pullup_en == GPIO_PULLUP_ENABLE && GPIO_IS_VALID_OUTPUT_GPIO(sda_io_num)) ||
               sda_pullup_en == GPIO_PULLUP_DISABLE, I2C_GPIO_PULLUP_ERR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK((scl_pullup_en == GPIO_PULLUP_ENABLE && GPIO_IS_VALID_OUTPUT_GPIO(scl_io_num)) ||
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

i2c_cmd_handle_t i2c_cmd_link_create()
{
    i2c_cmd_desc_t* cmd_desc = (i2c_cmd_desc_t*) calloc(1, sizeof(i2c_cmd_desc_t));
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
        cmd_desc->head = (i2c_cmd_link_t*) malloc(sizeof(i2c_cmd_link_t));
        if (cmd_desc->head == NULL) {
            ESP_LOGE(I2C_TAG, I2C_CMD_MALLOC_ERR_STR);
            goto err;
        }
        cmd_desc->cur = cmd_desc->head;
        cmd_desc->free = cmd_desc->head;
    } else {
        cmd_desc->cur->next = (i2c_cmd_link_t*) malloc(sizeof(i2c_cmd_link_t));
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

esp_err_t i2c_master_read(i2c_cmd_handle_t cmd_handle, uint8_t* data, size_t data_len, int ack)
{
    I2C_CHECK((data != NULL), I2C_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);

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

esp_err_t i2c_master_read_byte(i2c_cmd_handle_t cmd_handle, uint8_t* data, int ack)
{
    I2C_CHECK((data != NULL), I2C_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);
    i2c_cmd_t cmd;
    cmd.ack_en = 0;
    cmd.ack_exp = 0;
    cmd.ack_val = ack & 0x1;
    cmd.byte_num = 1;
    cmd.op_code = I2C_CMD_READ;
    cmd.data = data;
    return i2c_cmd_link_append(cmd_handle, &cmd);
}

static void IRAM_ATTR i2c_master_cmd_begin_static(i2c_port_t i2c_num)
{
    i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];
    portBASE_TYPE HPTaskAwoken = pdFALSE;
    //This should never happen
    if (p_i2c->mode == I2C_MODE_SLAVE) {
        return;
    }
    if (p_i2c->status == I2C_STATUS_DONE) {
        return;
    } else if (p_i2c->status == I2C_STATUS_ACK_ERROR) {
        I2C[i2c_num]->int_ena.end_detect = 0;
        I2C[i2c_num]->int_clr.end_detect = 1;
        I2C[i2c_num]->int_ena.master_tran_comp = 0;
        xSemaphoreGiveFromISR(p_i2c->cmd_sem, &HPTaskAwoken);
        if (HPTaskAwoken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
        return;
    } else if (p_i2c->status == I2C_STATUS_READ) {
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
        xSemaphoreGiveFromISR(p_i2c->cmd_sem, &HPTaskAwoken);
        if (HPTaskAwoken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
        return;
    }
    while (p_i2c->cmd_link.head) {
        i2c_cmd_t *cmd = &p_i2c->cmd_link.head->cmd;
        I2C[i2c_num]->command[p_i2c->cmd_idx].val = 0;
        I2C[i2c_num]->command[p_i2c->cmd_idx].ack_en = cmd->ack_en;
        I2C[i2c_num]->command[p_i2c->cmd_idx].ack_exp = cmd->ack_exp;
        I2C[i2c_num]->command[p_i2c->cmd_idx].ack_val = cmd->ack_val;
        I2C[i2c_num]->command[p_i2c->cmd_idx].byte_num = cmd->byte_num;
        I2C[i2c_num]->command[p_i2c->cmd_idx].op_code = cmd->op_code;
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
            //Workaround for register field operation.
            I2C[i2c_num]->command[p_i2c->cmd_idx].byte_num = wr_filled;
            I2C[i2c_num]->command[p_i2c->cmd_idx + 1].val = 0;
            I2C[i2c_num]->command[p_i2c->cmd_idx + 1].op_code = I2C_CMD_END;
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
            I2C[i2c_num]->command[p_i2c->cmd_idx].byte_num = p_i2c->rx_cnt;
            I2C[i2c_num]->command[p_i2c->cmd_idx].ack_val = cmd->ack_val;
            I2C[i2c_num]->command[p_i2c->cmd_idx + 1].val = 0;
            I2C[i2c_num]->command[p_i2c->cmd_idx + 1].op_code = I2C_CMD_END;
            p_i2c->status = I2C_STATUS_READ;
            break;
        } else {
        }
        p_i2c->cmd_idx++;
        p_i2c->cmd_link.head = p_i2c->cmd_link.head->next;
        if (p_i2c->cmd_link.head == NULL || p_i2c->cmd_idx >= 15) {
            p_i2c->tx_fifo_remain = I2C_FIFO_LEN;
            p_i2c->cmd_idx = 0;
            p_i2c->status = I2C_STATUS_IDLE;
            break;
        }
    }
    I2C[i2c_num]->int_clr.end_detect = 1;
    I2C[i2c_num]->int_clr.master_tran_comp = 1;
    I2C[i2c_num]->int_ena.end_detect = 1;
    I2C[i2c_num]->int_ena.master_tran_comp = 1;
    I2C[i2c_num]->ctr.trans_start = 0;
    I2C[i2c_num]->ctr.trans_start = 1;
    return;
}

esp_err_t i2c_master_cmd_begin(i2c_port_t i2c_num, i2c_cmd_handle_t cmd_handle, portBASE_TYPE ticks_to_wait)
{
    I2C_CHECK(( i2c_num < I2C_NUM_MAX ), I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(p_i2c_obj[i2c_num] != NULL, I2C_DRIVER_NOT_INSTALL_ERR_STR, ESP_ERR_INVALID_STATE);
    I2C_CHECK(p_i2c_obj[i2c_num]->mode == I2C_MODE_MASTER, I2C_MASTER_MODE_ERR_STR, ESP_ERR_INVALID_STATE);
    I2C_CHECK(cmd_handle != NULL, I2C_CMD_LINK_INIT_ERR_STR, ESP_ERR_INVALID_ARG);

    esp_err_t ret;
    i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];
    portTickType ticks_end = xTaskGetTickCount() + ticks_to_wait;
    portBASE_TYPE res = xSemaphoreTake(p_i2c->cmd_mux, ticks_to_wait);
    if (res == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }
    xSemaphoreTake(p_i2c->cmd_sem, 0);
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

    //start send commands, at most 32 bytes one time, isr handler will process the remaining commands.
    i2c_master_cmd_begin_static(i2c_num);
    ticks_to_wait = ticks_end - xTaskGetTickCount();
    res = xSemaphoreTake(p_i2c->cmd_sem, ticks_to_wait);
    if (res == pdFALSE) {
        ret = ESP_ERR_TIMEOUT;
    } else if (p_i2c->status == I2C_STATUS_ACK_ERROR) {
        ret = ESP_FAIL;
    } else {
        ret = ESP_OK;
    }
    p_i2c->status = I2C_STATUS_DONE;
    xSemaphoreGive(p_i2c->cmd_mux);
    return ret;
}

int i2c_slave_write_buffer(i2c_port_t i2c_num, uint8_t* data, int size, portBASE_TYPE ticks_to_wait)
{
    I2C_CHECK(( i2c_num < I2C_NUM_MAX ), I2C_NUM_ERROR_STR, ESP_FAIL);
    I2C_CHECK((data != NULL), I2C_ADDR_ERROR_STR, ESP_FAIL);
    I2C_CHECK(p_i2c_obj[i2c_num]->mode == I2C_MODE_SLAVE, I2C_MODE_SLAVE_ERR_STR, ESP_FAIL);
    i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];

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
        I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
        I2C[i2c_num]->int_clr.tx_fifo_empty = 1;
        I2C[i2c_num]->int_ena.tx_fifo_empty = 1;
        I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
        cnt = size;
    }
    xSemaphoreGive(p_i2c->slv_tx_mux);
    return cnt;
}

static int i2c_slave_read(i2c_port_t i2c_num, uint8_t* data, size_t max_size, portBASE_TYPE ticks_to_wait)
{
    i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];
    size_t size;
    uint8_t* pdata = (uint8_t*) xRingbufferReceiveUpTo(p_i2c->rx_ring_buf, &size, ticks_to_wait, max_size);
    if (pdata && size > 0) {
        memcpy(data, pdata, size);
        vRingbufferReturnItem(p_i2c->rx_ring_buf, pdata);
    }
    return size;
}

int i2c_slave_read_buffer(i2c_port_t i2c_num, uint8_t* data, size_t max_size, portBASE_TYPE ticks_to_wait)
{
    I2C_CHECK(( i2c_num < I2C_NUM_MAX ), I2C_NUM_ERROR_STR, ESP_FAIL);
    I2C_CHECK((data != NULL), I2C_ADDR_ERROR_STR, ESP_FAIL);
    I2C_CHECK(p_i2c_obj[i2c_num]->mode == I2C_MODE_SLAVE, I2C_MODE_SLAVE_ERR_STR, ESP_FAIL);

    i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];
    portBASE_TYPE res;
    portTickType ticks_end = xTaskGetTickCount() + ticks_to_wait;
    res = xSemaphoreTake(p_i2c->slv_rx_mux, ticks_to_wait);
    if (res == pdFALSE) {
        return 0;
    }
    ticks_to_wait = ticks_end - xTaskGetTickCount();
    int cnt = i2c_slave_read(i2c_num, data, max_size, ticks_to_wait);
    if (cnt > 0) {
        I2C_ENTER_CRITICAL(&i2c_spinlock[i2c_num]);
        I2C[i2c_num]->int_ena.rx_fifo_full = 1;
        I2C_EXIT_CRITICAL(&i2c_spinlock[i2c_num]);
        ticks_to_wait = ticks_end - xTaskGetTickCount();
        if (cnt < max_size && ticks_to_wait > 0) {
            cnt += i2c_slave_read(i2c_num, data + cnt, max_size - cnt, ticks_to_wait);
        }
    } else {
        cnt = 0;
    }
    xSemaphoreGive(p_i2c->slv_rx_mux);
    return cnt;
}




