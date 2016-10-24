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
#include "rom/ets_sys.h"
#include "soc/uart_struct.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "soc/uart_struct.h"

const char* UART_TAG = "UART";
#define UART_CHECK(a, str) if (!(a)) {                                                 \
        ESP_LOGE(UART_TAG,"%s:%d (%s):%s\n", __FILE__, __LINE__, __FUNCTION__, str);    \
        return ESP_FAIL;                                                                \
        }
#define DEFAULT_EMPTY_THRESH  10
#define DEFAULT_FULL_THRESH  120
#define DEFAULT_TOUT_THRESH   10
#define UART_ENTER_CRITICAL_ISR(mux)    portENTER_CRITICAL_ISR(mux)
#define UART_EXIT_CRITICAL_ISR(mux)     portEXIT_CRITICAL_ISR(mux)
#define UART_ENTER_CRITICAL(mux)    portENTER_CRITICAL(mux)
#define UART_EXIT_CRITICAL(mux)     portEXIT_CRITICAL(mux)

typedef struct {
    uart_port_t uart_num;
    SemaphoreHandle_t tx_fifo_sem;
    SemaphoreHandle_t tx_mutex;
    SemaphoreHandle_t tx_done_sem;
    SemaphoreHandle_t tx_brk_sem;
    SemaphoreHandle_t rx_sem;
    QueueHandle_t xQueueUart;
    int queue_size;
    int intr_num;
    RingbufHandle_t ring_buffer;
    bool buffer_full_flg;
    bool tx_waiting;
    int cur_remain;
    uint8_t* rd_ptr;
    uint8_t* head_ptr;
    uint8_t data_buf[UART_FIFO_LEN];
    uint8_t data_len;
} uart_obj_t;

static uart_obj_t *p_uart_obj[UART_NUM_MAX] = {0};
static uart_dev_t* UART[UART_NUM_MAX] = {&UART0, &UART1, &UART2};
static portMUX_TYPE uart_spinlock[UART_NUM_MAX] = {portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED};

//Fill UART tx_fifo and return a number,
//This function by itself is not thread-safe, always call from within a muxed section.
static int uart_fill_fifo(uart_port_t uart_num, char* buffer, uint32_t len)
{
    uint8_t i = 0;
    uint8_t tx_fifo_cnt = UART[uart_num]->status.txfifo_cnt;
    uint8_t tx_remain_fifo_cnt = (UART_FIFO_LEN - tx_fifo_cnt);
    uint8_t copy_cnt = (len >= tx_remain_fifo_cnt ? tx_remain_fifo_cnt : len);
    for(i = 0; i < copy_cnt; i++) {
        WRITE_PERI_REG(UART_FIFO_AHB_REG(uart_num), buffer[i]);
    }
    return copy_cnt;
}

esp_err_t uart_set_word_length(uart_port_t uart_num, uart_word_length_t data_bit)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((data_bit < UART_DATA_MAX_BITS), "data bit error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->conf0.bit_num = data_bit;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

int uart_get_word_length(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    return UART[uart_num]->conf0.bit_num;
}

esp_err_t uart_set_stop_bits(uart_port_t uart_num, uart_stop_bits_t stop_bit)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((stop_bit < UART_STOP_BITS_MAX), "stop bit error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->conf0.stop_bit_num = stop_bit;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

int uart_get_stop_bits(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    return UART[uart_num]->conf0.stop_bit_num;
}

esp_err_t uart_set_parity(uart_port_t uart_num, uart_parity_t parity_mode)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->conf0.parity = parity_mode & 0x1;
    UART[uart_num]->conf0.parity_en = (parity_mode >> 1) & 0x1;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

int uart_get_parity(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    int val = UART[uart_num]->conf0.val;
    if(val & UART_PARITY_EN_M) {
        if(val & UART_PARITY_M) {
            return UART_PARITY_ODD;
        } else {
            return UART_PARITY_EVEN;
        }
    } else {
        return UART_PARITY_DISABLE;
    }
}

esp_err_t uart_set_baudrate(uart_port_t uart_num, uint32_t baud_rate)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((baud_rate < UART_BITRATE_MAX), "baud_rate error");
    uint32_t clk_div = (((UART_CLK_FREQ) << 4) / baud_rate);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->clk_div.div_int = clk_div >> 4;
    UART[uart_num]->clk_div.div_frag = clk_div & 0xf;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

int uart_get_baudrate(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    uint32_t clk_div = (UART[uart_num]->clk_div.div_int << 4) | UART[uart_num]->clk_div.div_frag;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    uint32_t baudrate = ((UART_CLK_FREQ) << 4) / clk_div;
    return baudrate;
}

esp_err_t uart_set_line_inverse(uart_port_t uart_num, uint32_t inverse_mask)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((((inverse_mask & UART_LINE_INV_MASK) == 0) && (inverse_mask != 0)), "inverse_mask error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    CLEAR_PERI_REG_MASK(UART_CONF0_REG(uart_num), UART_LINE_INV_MASK);
    SET_PERI_REG_MASK(UART_CONF0_REG(uart_num), inverse_mask);
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

//only when UART_HW_FLOWCTRL_RTS is set , will the rx_thresh value be set.
esp_err_t uart_set_hw_flow_ctrl(uart_port_t uart_num, uart_hw_flowcontrol_t flow_ctrl, uint8_t rx_thresh)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((rx_thresh < UART_FIFO_LEN), "rx flow thresh error");
    UART_CHECK((flow_ctrl < UART_HW_FLOWCTRL_MAX), "hw_flowctrl mode error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    if(flow_ctrl & UART_HW_FLOWCTRL_RTS) {
        UART[uart_num]->conf1.rx_flow_thrhd = rx_thresh;
        UART[uart_num]->conf1.rx_flow_en = 1;
    } else {
        UART[uart_num]->conf1.rx_flow_en = 0;
    }
    if(flow_ctrl & UART_HW_FLOWCTRL_CTS) {
        UART[uart_num]->conf0.tx_flow_en = 1;
    } else {
        UART[uart_num]->conf0.tx_flow_en = 0;
    }
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

int uart_get_hw_flow_ctrl(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    uart_hw_flowcontrol_t val = UART_HW_FLOWCTRL_DISABLE;
    if(UART[uart_num]->conf1.rx_flow_en) {
        val |= UART_HW_FLOWCTRL_RTS;
    }
    if(UART[uart_num]->conf0.tx_flow_en) {
        val |= UART_HW_FLOWCTRL_CTS;
    }
    return val;
}

static esp_err_t uart_reset_fifo(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->conf0.rxfifo_rst = 1;
    UART[uart_num]->conf0.rxfifo_rst = 0;
    UART[uart_num]->conf0.txfifo_rst = 1;
    UART[uart_num]->conf0.txfifo_rst = 0;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_clear_intr_status(uart_port_t uart_num, uint32_t clr_mask)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    //intr_clr register is write-only
    UART[uart_num]->int_clr.val = clr_mask;
    return ESP_OK;
}

esp_err_t uart_enable_intr_mask(uart_port_t uart_num, uint32_t enable_mask)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    SET_PERI_REG_MASK(UART_INT_CLR_REG(uart_num), enable_mask);
    SET_PERI_REG_MASK(UART_INT_ENA_REG(uart_num), enable_mask);
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_disable_intr_mask(uart_port_t uart_num, uint32_t disable_mask)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    CLEAR_PERI_REG_MASK(UART_INT_ENA_REG(uart_num), disable_mask);
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_enable_rx_intr(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    SET_PERI_REG_MASK(UART_INT_ENA_REG(uart_num), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_TOUT_INT_ENA);
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_disable_rx_intr(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    CLEAR_PERI_REG_MASK(UART_INT_ENA_REG(uart_num), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_TOUT_INT_ENA);
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_disable_tx_intr(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->int_ena.txfifo_empty = 0;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_enable_tx_intr(uart_port_t uart_num, int enable, int thresh)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((thresh < UART_FIFO_LEN), "empty intr threshold error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->int_clr.txfifo_empty = 1;
    UART[uart_num]->conf1.txfifo_empty_thrhd = thresh & UART_TXFIFO_EMPTY_THRHD_V;
    UART[uart_num]->int_ena.txfifo_empty = enable & 0x1;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    ESP_INTR_ENABLE(p_uart_obj[uart_num]->intr_num);
    return ESP_OK;
}

esp_err_t uart_isr_register(uart_port_t uart_num, uint8_t uart_intr_num, void (*fn)(void*), void * arg)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    ESP_INTR_DISABLE(uart_intr_num);
    switch(uart_num) {
        case UART_NUM_1:
            intr_matrix_set(xPortGetCoreID(), ETS_UART1_INTR_SOURCE, uart_intr_num);
            break;
        case UART_NUM_2:
            intr_matrix_set(xPortGetCoreID(), ETS_UART2_INTR_SOURCE, uart_intr_num);
            break;
        case UART_NUM_0:
            default:
            intr_matrix_set(xPortGetCoreID(), ETS_UART0_INTR_SOURCE, uart_intr_num);
            break;
    }
    xt_set_interrupt_handler(uart_intr_num, fn, arg);
    ESP_INTR_ENABLE(uart_intr_num);
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

//internal signal can be output to multiple GPIO pads
//only one GPIO pad can connect with input signal
esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((tx_io_num < 0 || (GPIO_IS_VALID_OUTPUT_GPIO(tx_io_num))), "tx_io_num error");
    UART_CHECK((rx_io_num < 0 || (GPIO_IS_VALID_GPIO(rx_io_num))), "rx_io_num error");
    UART_CHECK((rts_io_num < 0 || (GPIO_IS_VALID_OUTPUT_GPIO(rts_io_num))), "rts_io_num error");
    UART_CHECK((cts_io_num < 0 || (GPIO_IS_VALID_GPIO(cts_io_num))), "cts_io_num error");

    int tx_sig, rx_sig, rts_sig, cts_sig;
    switch(uart_num) {
        case UART_NUM_0:
            tx_sig = U0TXD_OUT_IDX;
            rx_sig = U0RXD_IN_IDX;
            rts_sig = U0RTS_OUT_IDX;
            cts_sig = U0CTS_IN_IDX;
            break;
        case UART_NUM_1:
            tx_sig = U1TXD_OUT_IDX;
            rx_sig = U1RXD_IN_IDX;
            rts_sig = U1RTS_OUT_IDX;
            cts_sig = U1CTS_IN_IDX;
            break;
        case UART_NUM_2:
            tx_sig = U2TXD_OUT_IDX;
            rx_sig = U2RXD_IN_IDX;
            rts_sig = U2RTS_OUT_IDX;
            cts_sig = U2CTS_IN_IDX;
            break;
        case UART_NUM_MAX:
            default:
            tx_sig = U0TXD_OUT_IDX;
            rx_sig = U0RXD_IN_IDX;
            rts_sig = U0RTS_OUT_IDX;
            cts_sig = U0CTS_IN_IDX;
            break;
    }
    if(tx_io_num >= 0) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[tx_io_num], PIN_FUNC_GPIO);
        gpio_set_direction(tx_io_num, GPIO_MODE_OUTPUT);
        gpio_matrix_out(tx_io_num, tx_sig, 0, 0);
    }

    if(rx_io_num >= 0) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[rx_io_num], PIN_FUNC_GPIO);
        gpio_set_direction(rx_io_num, GPIO_MODE_INPUT);
        gpio_matrix_in(rx_io_num, rx_sig, 0);
    }
    if(rts_io_num >= 0) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[rts_io_num], PIN_FUNC_GPIO);
        gpio_set_direction(rts_io_num, GPIO_MODE_OUTPUT);
        gpio_matrix_out(rts_io_num, rts_sig, 0, 0);
    }
    if(cts_io_num >= 0) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[cts_io_num], PIN_FUNC_GPIO);
        gpio_set_direction(cts_io_num, GPIO_MODE_INPUT);
        gpio_matrix_in(cts_io_num, cts_sig, 0);
    }
    return ESP_OK;
}

esp_err_t uart_set_rts(uart_port_t uart_num, int level)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((UART[uart_num]->conf1.rx_flow_en != 1), "disable hw flowctrl before using sw control\n");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->conf0.sw_rts = level & 0x1;
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_set_dtr(uart_port_t uart_num, int level)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->conf0.sw_dtr = level & 0x1;
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_param_config(uart_port_t uart_num, uart_config_t *uart_config)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((uart_config), "param null\n");
    if(uart_num == UART_NUM_0) {
        periph_module_enable(PERIPH_UART0_MODULE);
    } else if(uart_num == UART_NUM_1) {
        periph_module_enable(PERIPH_UART1_MODULE);
    } else if(uart_num == UART_NUM_2) {
        periph_module_enable(PERIPH_UART2_MODULE);
    }
    uart_set_hw_flow_ctrl(uart_num, uart_config->flow_ctrl, uart_config->rx_flow_ctrl_thresh);
    uart_set_baudrate(uart_num, uart_config->baud_rate);
    UART[uart_num]->conf0.val = (
        (uart_config->parity << UART_PARITY_S)
            | (uart_config->stop_bits << UART_STOP_BIT_NUM_S)
            | (uart_config->data_bits << UART_BIT_NUM_S)
            | ((uart_config->flow_ctrl & UART_HW_FLOWCTRL_CTS) ? UART_TX_FLOW_EN : 0x0)
            | UART_TICK_REF_ALWAYS_ON_M);
    return ESP_OK;
}

esp_err_t uart_intr_config(uart_port_t uart_num, uart_intr_config_t *p_intr_conf)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((p_intr_conf), "param null\n");
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->int_clr.val = UART_INTR_MASK;
    if(p_intr_conf->intr_enable_mask & UART_RXFIFO_TOUT_INT_ENA_M) {
        UART[uart_num]->conf1.rx_tout_thrhd = ((p_intr_conf->rx_timeout_thresh) & UART_RX_TOUT_THRHD_V);
        UART[uart_num]->conf1.rx_tout_en = 1;
    } else {
        UART[uart_num]->conf1.rx_tout_en = 0;
    }
    if(p_intr_conf->intr_enable_mask & UART_RXFIFO_FULL_INT_ENA_M) {
        UART[uart_num]->conf1.rxfifo_full_thrhd = p_intr_conf->rxfifo_full_thresh;
    }
    if(p_intr_conf->intr_enable_mask & UART_TXFIFO_EMPTY_INT_ENA_M) {
        UART[uart_num]->conf1.txfifo_empty_thrhd = p_intr_conf->txfifo_empty_intr_thresh;
    }
    UART[uart_num]->int_ena.val = p_intr_conf->intr_enable_mask;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_FAIL;
}

//internal isr handler for default driver code.
static void IRAM_ATTR uart_rx_intr_handler_default(void *param)
{
    uart_obj_t *p_uart = (uart_obj_t*) param;
    uint8_t uart_num = p_uart->uart_num;
    uart_dev_t* uart_reg = UART[uart_num];

    uint8_t buf_idx = 0;
    uint32_t uart_intr_status = UART[uart_num]->int_st.val;
    static int rx_fifo_len = 0;
    uart_event_t uart_event;
    portBASE_TYPE HPTaskAwoken = 0;
    while(uart_intr_status != 0x0) {
        buf_idx = 0;
        uart_event.type = UART_EVENT_MAX;
        if(uart_intr_status & UART_TXFIFO_EMPTY_INT_ST_M) {
            UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
            uart_reg->int_ena.txfifo_empty = 0;
            uart_reg->int_clr.txfifo_empty = 1;
            UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
            if(p_uart->tx_waiting == true) {
                p_uart->tx_waiting = false;
                xSemaphoreGiveFromISR(p_uart->tx_fifo_sem, NULL);
            }
        }
        else if((uart_intr_status & UART_RXFIFO_TOUT_INT_ST_M) || (uart_intr_status & UART_RXFIFO_FULL_INT_ST_M)) {
            if(p_uart->buffer_full_flg == false) {
                //Get the buffer from the FIFO
                rx_fifo_len = uart_reg->status.rxfifo_cnt;
                p_uart->data_len = rx_fifo_len;
                memset(p_uart->data_buf, 0, sizeof(p_uart->data_buf));
                while(buf_idx < rx_fifo_len) {
                    p_uart->data_buf[buf_idx++] = uart_reg->fifo.rw_byte;
                }
                //After Copying the Data From FIFO ,Clear intr_status
                UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
                uart_reg->int_clr.rxfifo_tout = 1;
                uart_reg->int_clr.rxfifo_full = 1;
                UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
                uart_event.type = UART_DATA;
                uart_event.data.size = rx_fifo_len;
                if(pdFALSE == xRingbufferSendFromISR(p_uart->ring_buffer, p_uart->data_buf, p_uart->data_len, &HPTaskAwoken)) {
                    UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
                    uart_reg->int_ena.rxfifo_full = 0;
                    uart_reg->int_ena.rxfifo_tout = 0;
                    UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
                    p_uart->buffer_full_flg = true;
                    uart_event.type = UART_BUFFER_FULL;
                } else {
                    uart_event.type = UART_DATA;
                }
            } else {
                UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
                uart_reg->int_ena.rxfifo_full = 0;
                uart_reg->int_ena.rxfifo_tout = 0;
                uart_reg->int_clr.val = UART_RXFIFO_FULL_INT_CLR_M | UART_RXFIFO_TOUT_INT_CLR_M;
                UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
                uart_event.type = UART_BUFFER_FULL;
            }
        } else if(uart_intr_status & UART_RXFIFO_OVF_INT_ST_M) {
            UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
            uart_reg->conf0.rxfifo_rst = 1;
            uart_reg->conf0.rxfifo_rst = 0;
            uart_reg->int_clr.rxfifo_ovf = 1;
            UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
            uart_event.type = UART_FIFO_OVF;
        } else if(uart_intr_status & UART_BRK_DET_INT_ST_M) {
            uart_reg->int_clr.brk_det = 1;
            uart_event.type = UART_BREAK;
        } else if(uart_intr_status & UART_FRM_ERR_INT_ST_M) {
            uart_reg->int_clr.parity_err = 1;
            uart_event.type = UART_FRAME_ERR;
        } else if(uart_intr_status & UART_PARITY_ERR_INT_ST_M) {
            uart_reg->int_clr.frm_err = 1;
            uart_event.type = UART_PARITY_ERR;
        } else if(uart_intr_status & UART_TX_BRK_DONE_INT_ST_M) {
            UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
            uart_reg->conf0.txd_brk = 0;
            uart_reg->int_ena.tx_brk_done = 0;
            uart_reg->int_clr.tx_brk_done = 1;
            UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
            xSemaphoreGiveFromISR(p_uart->tx_brk_sem, &HPTaskAwoken);
        } else if(uart_intr_status & UART_TX_BRK_IDLE_DONE_INT_ST_M) {
            UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
            uart_reg->int_ena.tx_brk_idle_done = 0;
            uart_reg->int_clr.tx_brk_idle_done = 1;
            UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
        } else if(uart_intr_status & UART_TX_DONE_INT_ST_M) {
            UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
            uart_reg->int_ena.tx_done = 0;
            uart_reg->int_clr.tx_done = 1;
            UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
            xSemaphoreGiveFromISR(p_uart_obj[uart_num]->tx_done_sem, &HPTaskAwoken);
        }
        else {
            uart_reg->int_clr.val = uart_intr_status; /*simply clear all other intr status*/
            uart_event.type = UART_EVENT_MAX;
        }

        if(uart_event.type != UART_EVENT_MAX && p_uart->xQueueUart) {
            xQueueSendFromISR(p_uart->xQueueUart, (void * )&uart_event, &HPTaskAwoken);
        }
        uart_intr_status = uart_reg->int_st.val;
    }
}

/**************************************************************/
esp_err_t uart_driver_install(uart_port_t uart_num, int buffer_size, int queue_size, int uart_intr_num, void* uart_queue)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    if(p_uart_obj[uart_num] == NULL) {
        ESP_INTR_DISABLE(uart_intr_num);
        p_uart_obj[uart_num] = (uart_obj_t*) malloc(sizeof(uart_obj_t));
        if(p_uart_obj[uart_num] == NULL) {
            ESP_LOGE(UART_TAG, "UART driver malloc error\n");
            return ESP_FAIL;
        }
        p_uart_obj[uart_num]->uart_num = uart_num;
        p_uart_obj[uart_num]->tx_fifo_sem = xSemaphoreCreateBinary();
        xSemaphoreGive(p_uart_obj[uart_num]->tx_fifo_sem);
        p_uart_obj[uart_num]->tx_done_sem = xSemaphoreCreateBinary();
        xSemaphoreGive(p_uart_obj[uart_num]->tx_done_sem);
        p_uart_obj[uart_num]->tx_brk_sem = xSemaphoreCreateBinary();

        p_uart_obj[uart_num]->tx_mutex = xSemaphoreCreateMutex();
        p_uart_obj[uart_num]->rx_sem = xSemaphoreCreateMutex();
        p_uart_obj[uart_num]->intr_num = uart_intr_num;
        p_uart_obj[uart_num]->queue_size = queue_size;

        if(uart_queue) {
            p_uart_obj[uart_num]->xQueueUart = xQueueCreate(queue_size, sizeof(uart_event_t));
            *((QueueHandle_t*) uart_queue) = p_uart_obj[uart_num]->xQueueUart;
            ESP_LOGI(UART_TAG, "queue free spaces: %d\n", uxQueueSpacesAvailable(p_uart_obj[uart_num]->xQueueUart));
        } else {
            p_uart_obj[uart_num]->xQueueUart = NULL;
        }
        p_uart_obj[uart_num]->buffer_full_flg = false;
        p_uart_obj[uart_num]->tx_waiting = false;
        p_uart_obj[uart_num]->rd_ptr = NULL;
        p_uart_obj[uart_num]->cur_remain = 0;
        p_uart_obj[uart_num]->head_ptr = NULL;
        p_uart_obj[uart_num]->ring_buffer = xRingbufferCreate(buffer_size, 0);
    } else {
        ESP_LOGE(UART_TAG, "UART driver already installed\n");
        return ESP_FAIL;
    }
    uart_isr_register(uart_num, uart_intr_num, uart_rx_intr_handler_default, p_uart_obj[uart_num]);
    uart_intr_config_t uart_intr = {
        .intr_enable_mask = UART_RXFIFO_FULL_INT_ENA_M
                            | UART_RXFIFO_TOUT_INT_ENA_M
                            | UART_FRM_ERR_INT_ENA_M
                            | UART_RXFIFO_OVF_INT_ENA_M
                            | UART_BRK_DET_INT_ENA_M,
        .rxfifo_full_thresh = DEFAULT_FULL_THRESH,
        .rx_timeout_thresh = DEFAULT_TOUT_THRESH,
        .txfifo_empty_intr_thresh = DEFAULT_EMPTY_THRESH
    };
    uart_intr_config(uart_num, &uart_intr);
    ESP_INTR_ENABLE(uart_intr_num);
    return ESP_OK;
}

//Make sure no other tasks are still using UART before you call this function
esp_err_t uart_driver_delete(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    if(p_uart_obj[uart_num] == NULL) {
        ESP_LOGI(UART_TAG, "ALREADY NULL\n");
        return ESP_OK;
    }
    ESP_INTR_DISABLE(p_uart_obj[uart_num]->intr_num);
    uart_disable_rx_intr(uart_num);
    uart_disable_tx_intr(uart_num);
    uart_isr_register(uart_num, p_uart_obj[uart_num]->intr_num, NULL, NULL);

    if(p_uart_obj[uart_num]->tx_fifo_sem) {
        vSemaphoreDelete(p_uart_obj[uart_num]->tx_fifo_sem);
        p_uart_obj[uart_num]->tx_fifo_sem = NULL;
    }
    if(p_uart_obj[uart_num]->tx_done_sem) {
        vSemaphoreDelete(p_uart_obj[uart_num]->tx_done_sem);
        p_uart_obj[uart_num]->tx_done_sem = NULL;
    }
    if(p_uart_obj[uart_num]->tx_brk_sem) {
        vSemaphoreDelete(p_uart_obj[uart_num]->tx_brk_sem);
        p_uart_obj[uart_num]->tx_brk_sem = NULL;
    }
    if(p_uart_obj[uart_num]->tx_mutex) {
        vSemaphoreDelete(p_uart_obj[uart_num]->tx_mutex);
        p_uart_obj[uart_num]->tx_mutex = NULL;
    }
    if(p_uart_obj[uart_num]->rx_sem) {
        vSemaphoreDelete(p_uart_obj[uart_num]->rx_sem);
        p_uart_obj[uart_num]->rx_sem = NULL;
    }
    if(p_uart_obj[uart_num]->xQueueUart) {
        vQueueDelete(p_uart_obj[uart_num]->xQueueUart);
        p_uart_obj[uart_num]->xQueueUart = NULL;
    }
    if(p_uart_obj[uart_num]->ring_buffer) {
        vRingbufferDelete(p_uart_obj[uart_num]->ring_buffer);
        p_uart_obj[uart_num]->ring_buffer = NULL;
    }
    free(p_uart_obj[uart_num]);
    p_uart_obj[uart_num] = NULL;
    return ESP_OK;
}

esp_err_t uart_wait_tx_fifo_empty(uart_port_t uart_num, TickType_t ticks_to_wait)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error");
    BaseType_t res;
    portTickType ticks_end = xTaskGetTickCount() + ticks_to_wait;
    //Take tx_mutex
    res = xSemaphoreTake(p_uart_obj[uart_num]->tx_mutex, (portTickType)ticks_to_wait);
    if(res == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }
    ticks_to_wait = ticks_end - xTaskGetTickCount();
    //take 1st tx_done_sem
    res = xSemaphoreTake(p_uart_obj[uart_num]->tx_done_sem, (portTickType)ticks_to_wait);
    if(res == pdFALSE) {
        ESP_LOGE(UART_TAG, "take uart done sem error, should not get here.\n");
        xSemaphoreGive(p_uart_obj[uart_num]->tx_done_sem);
        xSemaphoreGive(p_uart_obj[uart_num]->tx_mutex);
        return ESP_ERR_TIMEOUT;
    }
    ticks_to_wait = ticks_end - xTaskGetTickCount();
    if(UART[uart_num]->status.txfifo_cnt == 0) {
        xSemaphoreGive(p_uart_obj[uart_num]->tx_done_sem);
        xSemaphoreGive(p_uart_obj[uart_num]->tx_mutex);
        return ESP_OK;
    }
    uart_enable_intr_mask(uart_num, UART_TX_DONE_INT_ENA_M);
    //take 2nd tx_done_sem, wait given from ISR
    res = xSemaphoreTake(p_uart_obj[uart_num]->tx_done_sem, (portTickType)ticks_to_wait);
    if(res == pdFALSE) {
        uart_disable_intr_mask(uart_num, UART_TX_DONE_INT_ENA_M);
        xSemaphoreGive(p_uart_obj[uart_num]->tx_done_sem);
        xSemaphoreGive(p_uart_obj[uart_num]->tx_mutex);
        return ESP_ERR_TIMEOUT;
    }
    xSemaphoreGive(p_uart_obj[uart_num]->tx_done_sem);
    xSemaphoreGive(p_uart_obj[uart_num]->tx_mutex);
    return ESP_OK;
}

static esp_err_t uart_set_break(uart_port_t uart_num, int break_num)
{
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->idle_conf.tx_brk_num = break_num;
    UART[uart_num]->conf0.txd_brk = 1;
    UART[uart_num]->int_clr.tx_brk_done = 1;
    UART[uart_num]->int_ena.tx_brk_done = 1;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

int uart_tx_chars(uart_port_t uart_num, char* buffer, uint32_t len)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error");
    UART_CHECK(buffer, "buffer null");
    if(len == 0) {
        return 0;
    }
    xSemaphoreTake(p_uart_obj[uart_num]->tx_mutex, (portTickType)portMAX_DELAY);
    int tx_len = uart_fill_fifo(uart_num, buffer, len);
    xSemaphoreGive(p_uart_obj[uart_num]->tx_mutex);
    return tx_len;
}

static int uart_tx_all(uart_port_t uart_num, const char* src, size_t size, bool brk_en, int brk_len)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error");
    UART_CHECK(src, "buffer null");
    if(size == 0) {
        return 0;
    }
    //lock for uart_tx
    xSemaphoreTake(p_uart_obj[uart_num]->tx_mutex, (portTickType)portMAX_DELAY);
    size_t original_size = size;
    while(size) {
        //semaphore for tx_fifo available
        if(pdTRUE == xSemaphoreTake(p_uart_obj[uart_num]->tx_fifo_sem, (portTickType)portMAX_DELAY)) {
            size_t sent = uart_fill_fifo(uart_num, (char*) src, size);
            if(sent < size) {
                p_uart_obj[uart_num]->tx_waiting = true;
                uart_enable_tx_intr(uart_num, 1, DEFAULT_EMPTY_THRESH);
            }
            size -= sent;
            src += sent;
        }
    }
    if(brk_en) {
        uart_set_break(uart_num, brk_len);
        xSemaphoreTake(p_uart_obj[uart_num]->tx_brk_sem, (portTickType)portMAX_DELAY);
    }
    xSemaphoreGive(p_uart_obj[uart_num]->tx_fifo_sem);
    xSemaphoreGive(p_uart_obj[uart_num]->tx_mutex);
    return original_size;
}

int uart_tx_all_chars(uart_port_t uart_num, const char* src, size_t size)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error");
    UART_CHECK(src, "buffer null");
    return uart_tx_all(uart_num, src, size, 0, 0);
}

int uart_tx_all_chars_with_break(uart_port_t uart_num, const char* src, size_t size, int brk_len)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error");
    UART_CHECK((size > 0), "uart size error");
    UART_CHECK((src), "uart data null");
    UART_CHECK((brk_len > 0 && brk_len < 256), "break_num error");
    return uart_tx_all(uart_num, src,  size, 1, brk_len);
}

int uart_read_char(uart_port_t uart_num, TickType_t ticks_to_wait)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error");
    uint8_t* data;
    size_t size;
    int val;
    portTickType ticks_end = xTaskGetTickCount() + ticks_to_wait;
    if(xSemaphoreTake(p_uart_obj[uart_num]->rx_sem,(portTickType)ticks_to_wait) != pdTRUE) {
        return -1;
    }
    if(p_uart_obj[uart_num]->cur_remain == 0) {
        ticks_to_wait = ticks_end - xTaskGetTickCount();
        data = (uint8_t*) xRingbufferReceive(p_uart_obj[uart_num]->ring_buffer, &size, (portTickType) ticks_to_wait);
        if(data) {
            p_uart_obj[uart_num]->head_ptr = data;
            p_uart_obj[uart_num]->rd_ptr = data;
            p_uart_obj[uart_num]->cur_remain = size;
        } else {
            xSemaphoreGive(p_uart_obj[uart_num]->rx_sem);
            return -1;
        }
    }
    val = *(p_uart_obj[uart_num]->rd_ptr);
    p_uart_obj[uart_num]->rd_ptr++;
    p_uart_obj[uart_num]->cur_remain--;
    if(p_uart_obj[uart_num]->cur_remain == 0) {
        vRingbufferReturnItem(p_uart_obj[uart_num]->ring_buffer, p_uart_obj[uart_num]->head_ptr);
        p_uart_obj[uart_num]->head_ptr = NULL;
        p_uart_obj[uart_num]->rd_ptr = NULL;
        if(p_uart_obj[uart_num]->buffer_full_flg) {
            BaseType_t res = xRingbufferSend(p_uart_obj[uart_num]->ring_buffer, p_uart_obj[uart_num]->data_buf, p_uart_obj[uart_num]->data_len, 1);
            if(res == pdTRUE) {
                p_uart_obj[uart_num]->buffer_full_flg = false;
                uart_enable_rx_intr(p_uart_obj[uart_num]->uart_num);
            }
        }
    }
    xSemaphoreGive(p_uart_obj[uart_num]->rx_sem);
    return val;
}

int uart_read_bytes(uart_port_t uart_num, uint8_t* buf, uint32_t length, TickType_t ticks_to_wait)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((buf), "uart_num error");
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error");

    uint8_t* data = NULL;
    size_t size;
    size_t copy_len = 0;
    int len_tmp;
    if(xSemaphoreTake(p_uart_obj[uart_num]->rx_sem,(portTickType)ticks_to_wait) != pdTRUE) {
        return -1;
    }
    while(length) {
        if(p_uart_obj[uart_num]->cur_remain == 0) {
            data = (uint8_t*) xRingbufferReceive(p_uart_obj[uart_num]->ring_buffer, &size, (portTickType) ticks_to_wait);
            if(data) {
                p_uart_obj[uart_num]->head_ptr = data;
                p_uart_obj[uart_num]->rd_ptr = data;
                p_uart_obj[uart_num]->cur_remain = size;
            } else {
                xSemaphoreGive(p_uart_obj[uart_num]->rx_sem);
                return copy_len;
            }
        }
        if(p_uart_obj[uart_num]->cur_remain > length) {
            len_tmp = length;
        } else {
            len_tmp = p_uart_obj[uart_num]->cur_remain;
        }
        memcpy(buf + copy_len, p_uart_obj[uart_num]->rd_ptr, len_tmp);
        p_uart_obj[uart_num]->rd_ptr += len_tmp;
        p_uart_obj[uart_num]->cur_remain -= len_tmp;
        copy_len += len_tmp;
        length -= len_tmp;
        if(p_uart_obj[uart_num]->cur_remain == 0) {
            vRingbufferReturnItem(p_uart_obj[uart_num]->ring_buffer, p_uart_obj[uart_num]->head_ptr);
            p_uart_obj[uart_num]->head_ptr = NULL;
            p_uart_obj[uart_num]->rd_ptr = NULL;
            if(p_uart_obj[uart_num]->buffer_full_flg) {
                BaseType_t res = xRingbufferSend(p_uart_obj[uart_num]->ring_buffer, p_uart_obj[uart_num]->data_buf, p_uart_obj[uart_num]->data_len, 1);
                if(res == pdTRUE) {
                    p_uart_obj[uart_num]->buffer_full_flg = false;
                    uart_enable_rx_intr(p_uart_obj[uart_num]->uart_num);
                }
            }
        }
    }
    xSemaphoreGive(p_uart_obj[uart_num]->rx_sem);
    return copy_len;
}

esp_err_t uart_flush(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error");

    uart_obj_t* p_uart = p_uart_obj[uart_num];
    uint8_t* data;
    size_t size;
    //rx sem protect the ring buffer read related functions
    xSemaphoreTake(p_uart->rx_sem, (portTickType)portMAX_DELAY);
    while(true) {
        if(p_uart->head_ptr) {
            vRingbufferReturnItem(p_uart->ring_buffer, p_uart->head_ptr);
            p_uart->rd_ptr = NULL;
            p_uart->cur_remain = 0;
            p_uart->head_ptr = NULL;
        }
        data = (uint8_t*) xRingbufferReceive(p_uart->ring_buffer, &size, (portTickType) 0);
        if(data == NULL) {
            break;
        }
        vRingbufferReturnItem(p_uart->ring_buffer, data);
    }
    p_uart->rd_ptr = NULL;
    p_uart->cur_remain = 0;
    p_uart->head_ptr = NULL;
    xSemaphoreGive(p_uart->rx_sem);
    uart_wait_tx_fifo_empty(uart_num, portMAX_DELAY);
    uart_reset_fifo(uart_num);
    return ESP_OK;
}

//-----------------------------------
//Should not enable hw flow control the debug print port.
//Use uart_tx_all_chars() as a thread-safe function to send data.
static int s_uart_print_nport = UART_NUM_0;
static void uart2_write_char(char chr)
{
    uart_tx_all_chars(UART_NUM_2, (const char*)&chr, 1);
}

static void uart1_write_char(char chr)
{
    uart_tx_all_chars(UART_NUM_1, (const char*)&chr, 1);
}

static void uart0_write_char(char chr)
{
    uart_tx_all_chars(UART_NUM_0, (const char*)&chr, 1);
}

static void uart_ignore_char(char chr)
{

}

//Only effective to ets_printf function, not ESP_LOGX macro.
esp_err_t uart_set_print_port(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error");
    UART_CHECK((p_uart_obj[uart_num]), "UART driver error");

    s_uart_print_nport = uart_num;
    switch(s_uart_print_nport) {
        case UART_NUM_0:
            ets_install_putc1(uart0_write_char);
            break;
        case UART_NUM_1:
            ets_install_putc1(uart1_write_char);
            break;
        case UART_NUM_2:
            ets_install_putc1(uart2_write_char);
            break;
        case UART_NUM_MAX:
            default:
            ets_install_putc1(uart_ignore_char);
            break;
    }
    return ESP_OK;
}

int uart_get_print_port()
{
    return s_uart_print_nport;
}

