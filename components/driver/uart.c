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
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_clk.h"
#include "malloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "soc/dport_reg.h"
#include "soc/uart_struct.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/uart_select.h"

#define XOFF (char)0x13
#define XON (char)0x11

static const char* UART_TAG = "uart";
#define UART_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(UART_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

#define UART_EMPTY_THRESH_DEFAULT  (10)
#define UART_FULL_THRESH_DEFAULT  (120)
#define UART_TOUT_THRESH_DEFAULT   (10)
#define UART_CLKDIV_FRAG_BIT_WIDTH  (3)
#define UART_TOUT_REF_FACTOR_DEFAULT (UART_CLK_FREQ/(REF_CLK_FREQ<<UART_CLKDIV_FRAG_BIT_WIDTH))
#define UART_TX_IDLE_NUM_DEFAULT   (0)
#define UART_PATTERN_DET_QLEN_DEFAULT (10)
#define UART_MIN_WAKEUP_THRESH      (2)

#define UART_ENTER_CRITICAL_ISR(mux)    portENTER_CRITICAL_ISR(mux)
#define UART_EXIT_CRITICAL_ISR(mux)     portEXIT_CRITICAL_ISR(mux)
#define UART_ENTER_CRITICAL(mux)    portENTER_CRITICAL(mux)
#define UART_EXIT_CRITICAL(mux)     portEXIT_CRITICAL(mux)

// Check actual UART mode set
#define UART_IS_MODE_SET(uart_number, mode) ((p_uart_obj[uart_number]->uart_mode == mode))

typedef struct {
    uart_event_type_t type;        /*!< UART TX data type */
    struct {
        int brk_len;
        size_t size;
        uint8_t data[0];
    } tx_data;
} uart_tx_data_t;

typedef struct {
    int wr;
    int rd;
    int len;
    int* data;
} uart_pat_rb_t;

typedef struct {
    uart_port_t uart_num;               /*!< UART port number*/
    int queue_size;                     /*!< UART event queue size*/
    QueueHandle_t xQueueUart;           /*!< UART queue handler*/
    intr_handle_t intr_handle;          /*!< UART interrupt handle*/
    uart_mode_t uart_mode;              /*!< UART controller actual mode set by uart_set_mode() */
    bool coll_det_flg;                  /*!< UART collision detection flag */
    
    //rx parameters
    int rx_buffered_len;                  /*!< UART cached data length */
    SemaphoreHandle_t rx_mux;           /*!< UART RX data mutex*/
    int rx_buf_size;                    /*!< RX ring buffer size */
    RingbufHandle_t rx_ring_buf;        /*!< RX ring buffer handler*/
    bool rx_buffer_full_flg;            /*!< RX ring buffer full flag. */
    int rx_cur_remain;                  /*!< Data number that waiting to be read out in ring buffer item*/
    uint8_t* rx_ptr;                    /*!< pointer to the current data in ring buffer*/
    uint8_t* rx_head_ptr;               /*!< pointer to the head of RX item*/
    uint8_t rx_data_buf[UART_FIFO_LEN]; /*!< Data buffer to stash FIFO data*/
    uint8_t rx_stash_len;               /*!< stashed data length.(When using flow control, after reading out FIFO data, if we fail to push to buffer, we can just stash them.) */
    uart_pat_rb_t rx_pattern_pos;

    //tx parameters
    SemaphoreHandle_t tx_fifo_sem;      /*!< UART TX FIFO semaphore*/
    SemaphoreHandle_t tx_mux;           /*!< UART TX mutex*/
    SemaphoreHandle_t tx_done_sem;      /*!< UART TX done semaphore*/
    SemaphoreHandle_t tx_brk_sem;       /*!< UART TX send break done semaphore*/
    int tx_buf_size;                    /*!< TX ring buffer size */
    RingbufHandle_t tx_ring_buf;        /*!< TX ring buffer handler*/
    bool tx_waiting_fifo;               /*!< this flag indicates that some task is waiting for FIFO empty interrupt, used to send all data without any data buffer*/
    uint8_t* tx_ptr;                    /*!< TX data pointer to push to FIFO in TX buffer mode*/
    uart_tx_data_t* tx_head;            /*!< TX data pointer to head of the current buffer in TX ring buffer*/
    uint32_t tx_len_tot;                /*!< Total length of current item in ring buffer*/
    uint32_t tx_len_cur;
    uint8_t tx_brk_flg;                 /*!< Flag to indicate to send a break signal in the end of the item sending procedure */
    uint8_t tx_brk_len;                 /*!< TX break signal cycle length/number */
    uint8_t tx_waiting_brk;             /*!< Flag to indicate that TX FIFO is ready to send break signal after FIFO is empty, do not push data into TX FIFO right now.*/
    uart_select_notif_callback_t uart_select_notif_callback; /*!< Notification about select() events */
} uart_obj_t;

static uart_obj_t *p_uart_obj[UART_NUM_MAX] = {0};
/* DRAM_ATTR is required to avoid UART array placed in flash, due to accessed from ISR */
static DRAM_ATTR uart_dev_t* const UART[UART_NUM_MAX] = {&UART0, &UART1, &UART2};
static portMUX_TYPE uart_spinlock[UART_NUM_MAX] = {portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED};
static portMUX_TYPE uart_selectlock = portMUX_INITIALIZER_UNLOCKED;

esp_err_t uart_set_word_length(uart_port_t uart_num, uart_word_length_t data_bit)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((data_bit < UART_DATA_BITS_MAX), "data bit error", ESP_FAIL);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->conf0.bit_num = data_bit;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_get_word_length(uart_port_t uart_num, uart_word_length_t* data_bit)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    *(data_bit) = UART[uart_num]->conf0.bit_num;
    return ESP_OK;
}

esp_err_t uart_set_stop_bits(uart_port_t uart_num, uart_stop_bits_t stop_bit)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((stop_bit < UART_STOP_BITS_MAX), "stop bit error", ESP_FAIL);

    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    //workaround for hardware bug, when uart stop bit set as 2-bit mode.
    if (stop_bit == UART_STOP_BITS_2) {
        stop_bit = UART_STOP_BITS_1;
        UART[uart_num]->rs485_conf.dl1_en = 1;
    } else {
        UART[uart_num]->rs485_conf.dl1_en = 0;
    }
    UART[uart_num]->conf0.stop_bit_num = stop_bit;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_get_stop_bits(uart_port_t uart_num, uart_stop_bits_t* stop_bit)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    //workaround for hardware bug, when uart stop bit set as 2-bit mode.
    if (UART[uart_num]->rs485_conf.dl1_en == 1 && UART[uart_num]->conf0.stop_bit_num == UART_STOP_BITS_1) {
        (*stop_bit) = UART_STOP_BITS_2;
    } else {
        (*stop_bit) = UART[uart_num]->conf0.stop_bit_num;
    }
    return ESP_OK;
}

esp_err_t uart_set_parity(uart_port_t uart_num, uart_parity_t parity_mode)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->conf0.parity = parity_mode & 0x1;
    UART[uart_num]->conf0.parity_en = (parity_mode >> 1) & 0x1;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_get_parity(uart_port_t uart_num, uart_parity_t* parity_mode)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    int val = UART[uart_num]->conf0.val;
    if(val & UART_PARITY_EN_M) {
        if(val & UART_PARITY_M) {
            (*parity_mode) = UART_PARITY_ODD;
        } else {
            (*parity_mode) = UART_PARITY_EVEN;
        }
    } else {
        (*parity_mode) = UART_PARITY_DISABLE;
    }
    return ESP_OK;
}

esp_err_t uart_set_baudrate(uart_port_t uart_num, uint32_t baud_rate)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    esp_err_t ret = ESP_OK;
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    int uart_clk_freq;
    if (UART[uart_num]->conf0.tick_ref_always_on == 0) {
        /* this UART has been configured to use REF_TICK */
        uart_clk_freq = REF_CLK_FREQ;
    } else {
        uart_clk_freq = esp_clk_apb_freq();
    }
    uint32_t clk_div = (((uart_clk_freq) << 4) / baud_rate);
    if (clk_div < 16) {
        /* baud rate is too high for this clock frequency */
        ret = ESP_ERR_INVALID_ARG;
    } else {
        UART[uart_num]->clk_div.div_int = clk_div >> 4;
        UART[uart_num]->clk_div.div_frag = clk_div & 0xf;
    }
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ret;
}

esp_err_t uart_get_baudrate(uart_port_t uart_num, uint32_t* baudrate)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    uint32_t clk_div = (UART[uart_num]->clk_div.div_int << 4) | UART[uart_num]->clk_div.div_frag;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    uint32_t uart_clk_freq = esp_clk_apb_freq();
    if(UART[uart_num]->conf0.tick_ref_always_on == 0) {
        uart_clk_freq = REF_CLK_FREQ;
    }
    (*baudrate) = ((uart_clk_freq) << 4) / clk_div;
    return ESP_OK;
}

esp_err_t uart_set_line_inverse(uart_port_t uart_num, uint32_t inverse_mask)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((((inverse_mask & ~UART_LINE_INV_MASK) == 0) || (inverse_mask == 0)), "inverse_mask error", ESP_FAIL);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    CLEAR_PERI_REG_MASK(UART_CONF0_REG(uart_num), UART_LINE_INV_MASK);
    SET_PERI_REG_MASK(UART_CONF0_REG(uart_num), inverse_mask);
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_set_sw_flow_ctrl(uart_port_t uart_num, bool enable,  uint8_t rx_thresh_xon,  uint8_t rx_thresh_xoff)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((rx_thresh_xon < UART_FIFO_LEN), "rx flow xon thresh error", ESP_FAIL);
    UART_CHECK((rx_thresh_xoff < UART_FIFO_LEN), "rx flow xon thresh error", ESP_FAIL);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->flow_conf.sw_flow_con_en = enable? 1:0;
    UART[uart_num]->flow_conf.xonoff_del = enable?1:0;
    UART[uart_num]->swfc_conf.xon_threshold =  rx_thresh_xon;
    UART[uart_num]->swfc_conf.xoff_threshold =  rx_thresh_xoff;
    UART[uart_num]->swfc_conf.xon_char = XON;
    UART[uart_num]->swfc_conf.xoff_char = XOFF;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

//only when UART_HW_FLOWCTRL_RTS is set , will the rx_thresh value be set.
esp_err_t uart_set_hw_flow_ctrl(uart_port_t uart_num, uart_hw_flowcontrol_t flow_ctrl, uint8_t rx_thresh)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((rx_thresh < UART_FIFO_LEN), "rx flow thresh error", ESP_FAIL);
    UART_CHECK((flow_ctrl < UART_HW_FLOWCTRL_MAX), "hw_flowctrl mode error", ESP_FAIL);
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

esp_err_t uart_get_hw_flow_ctrl(uart_port_t uart_num, uart_hw_flowcontrol_t* flow_ctrl)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    uart_hw_flowcontrol_t val = UART_HW_FLOWCTRL_DISABLE;
    if(UART[uart_num]->conf1.rx_flow_en) {
        val |= UART_HW_FLOWCTRL_RTS;
    }
    if(UART[uart_num]->conf0.tx_flow_en) {
        val |= UART_HW_FLOWCTRL_CTS;
    }
    (*flow_ctrl) = val;
    return ESP_OK;
}

static esp_err_t uart_reset_rx_fifo(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    //Due to hardware issue, we can not use fifo_rst to reset uart fifo.
    //See description about UART_TXFIFO_RST and UART_RXFIFO_RST in <<esp32_technical_reference_manual>> v2.6 or later.

    // we read the data out and make `fifo_len == 0 && rd_addr == wr_addr`.
    while(UART[uart_num]->status.rxfifo_cnt != 0 || (UART[uart_num]->mem_rx_status.wr_addr != UART[uart_num]->mem_rx_status.rd_addr)) {
        READ_PERI_REG(UART_FIFO_REG(uart_num));
    }
    return ESP_OK;
}

esp_err_t uart_clear_intr_status(uart_port_t uart_num, uint32_t clr_mask)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    //intr_clr register is write-only
    UART[uart_num]->int_clr.val = clr_mask;
    return ESP_OK;
}

esp_err_t uart_enable_intr_mask(uart_port_t uart_num, uint32_t enable_mask)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    SET_PERI_REG_MASK(UART_INT_CLR_REG(uart_num), enable_mask);
    SET_PERI_REG_MASK(UART_INT_ENA_REG(uart_num), enable_mask);
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_disable_intr_mask(uart_port_t uart_num, uint32_t disable_mask)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    CLEAR_PERI_REG_MASK(UART_INT_ENA_REG(uart_num), disable_mask);
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

static esp_err_t uart_pattern_link_free(uart_port_t uart_num)
{
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", ESP_FAIL);
    if (p_uart_obj[uart_num]->rx_pattern_pos.data != NULL) {
        int* pdata = p_uart_obj[uart_num]->rx_pattern_pos.data;
        UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
        p_uart_obj[uart_num]->rx_pattern_pos.data = NULL;
        p_uart_obj[uart_num]->rx_pattern_pos.wr = 0;
        p_uart_obj[uart_num]->rx_pattern_pos.rd = 0;
        UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
        free(pdata);
    }
    return ESP_OK;
}

static esp_err_t uart_pattern_enqueue(uart_port_t uart_num, int pos)
{
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", ESP_FAIL);
    esp_err_t ret = ESP_OK;
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    uart_pat_rb_t* p_pos = &p_uart_obj[uart_num]->rx_pattern_pos;
    int next = p_pos->wr + 1;
    if (next >= p_pos->len) {
        next = 0;
    }
    if (next == p_pos->rd) {
        ESP_EARLY_LOGW(UART_TAG, "Fail to enqueue pattern position, pattern queue is full.");
        ret = ESP_FAIL;
    } else {
        p_pos->data[p_pos->wr] = pos;
        p_pos->wr = next;
        ret = ESP_OK;
    }
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ret;
}

static esp_err_t uart_pattern_dequeue(uart_port_t uart_num)
{
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", ESP_FAIL);
    if(p_uart_obj[uart_num]->rx_pattern_pos.data == NULL) {
        return ESP_ERR_INVALID_STATE;
    } else {
        esp_err_t ret = ESP_OK;
        UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
        uart_pat_rb_t* p_pos = &p_uart_obj[uart_num]->rx_pattern_pos;
        if (p_pos->rd == p_pos->wr) {
            ret = ESP_FAIL;
        } else {
            p_pos->rd++;
        }
        if (p_pos->rd >= p_pos->len) {
            p_pos->rd = 0;
        }
        UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
        return ret;
    }
}

static esp_err_t uart_pattern_queue_update(uart_port_t uart_num, int diff_len)
{
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", ESP_FAIL);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    uart_pat_rb_t* p_pos = &p_uart_obj[uart_num]->rx_pattern_pos;
    int rd = p_pos->rd;
    while(rd != p_pos->wr) {
        p_pos->data[rd] -= diff_len;
        int rd_rec = rd;
        rd ++;
        if (rd >= p_pos->len) {
            rd = 0;
        }
        if (p_pos->data[rd_rec] < 0) {
            p_pos->rd = rd;
        }
    }
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

int uart_pattern_pop_pos(uart_port_t uart_num)
{
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", (-1));
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    uart_pat_rb_t* pat_pos = &p_uart_obj[uart_num]->rx_pattern_pos;
    int pos = -1;
    if (pat_pos != NULL && pat_pos->rd != pat_pos->wr) {
        pos = pat_pos->data[pat_pos->rd];
        uart_pattern_dequeue(uart_num);
    }
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return pos;
}

int uart_pattern_get_pos(uart_port_t uart_num)
{
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", (-1));
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    uart_pat_rb_t* pat_pos = &p_uart_obj[uart_num]->rx_pattern_pos;
    int pos = -1;
    if (pat_pos != NULL && pat_pos->rd != pat_pos->wr) {
        pos = pat_pos->data[pat_pos->rd];
    }
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return pos;
}

esp_err_t uart_pattern_queue_reset(uart_port_t uart_num, int queue_length)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", ESP_ERR_INVALID_STATE);

    int* pdata = (int*) malloc(queue_length * sizeof(int));
    if(pdata == NULL) {
        return ESP_ERR_NO_MEM;
    }
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    int* ptmp = p_uart_obj[uart_num]->rx_pattern_pos.data;
    p_uart_obj[uart_num]->rx_pattern_pos.data = pdata;
    p_uart_obj[uart_num]->rx_pattern_pos.len = queue_length;
    p_uart_obj[uart_num]->rx_pattern_pos.rd = 0;
    p_uart_obj[uart_num]->rx_pattern_pos.wr = 0;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    free(ptmp);
    return ESP_OK;
}

esp_err_t uart_enable_pattern_det_intr(uart_port_t uart_num, char pattern_chr, uint8_t chr_num, int chr_tout, int post_idle, int pre_idle)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK(chr_tout >= 0 && chr_tout <= UART_RX_GAP_TOUT_V, "uart pattern set error\n", ESP_FAIL);
    UART_CHECK(post_idle >= 0 && post_idle <= UART_POST_IDLE_NUM_V, "uart pattern set error\n", ESP_FAIL);
    UART_CHECK(pre_idle >= 0 && pre_idle <= UART_PRE_IDLE_NUM_V, "uart pattern set error\n", ESP_FAIL);
    UART[uart_num]->at_cmd_char.data = pattern_chr;
    UART[uart_num]->at_cmd_char.char_num = chr_num;
    UART[uart_num]->at_cmd_gaptout.rx_gap_tout = chr_tout;
    UART[uart_num]->at_cmd_postcnt.post_idle_num = post_idle;
    UART[uart_num]->at_cmd_precnt.pre_idle_num = pre_idle;
    return uart_enable_intr_mask(uart_num, UART_AT_CMD_CHAR_DET_INT_ENA_M);
}

esp_err_t uart_disable_pattern_det_intr(uart_port_t uart_num)
{
    return uart_disable_intr_mask(uart_num, UART_AT_CMD_CHAR_DET_INT_ENA_M);
}

esp_err_t uart_enable_rx_intr(uart_port_t uart_num)
{
    return uart_enable_intr_mask(uart_num, UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_TOUT_INT_ENA);
}

esp_err_t uart_disable_rx_intr(uart_port_t uart_num)
{
    return uart_disable_intr_mask(uart_num, UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_TOUT_INT_ENA);
}

esp_err_t uart_disable_tx_intr(uart_port_t uart_num)
{
    return uart_disable_intr_mask(uart_num, UART_TXFIFO_EMPTY_INT_ENA);
}

esp_err_t uart_enable_tx_intr(uart_port_t uart_num, int enable, int thresh)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((thresh < UART_FIFO_LEN), "empty intr threshold error", ESP_FAIL);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->int_clr.txfifo_empty = 1;
    UART[uart_num]->conf1.txfifo_empty_thrhd = thresh & UART_TXFIFO_EMPTY_THRHD_V;
    UART[uart_num]->int_ena.txfifo_empty = enable & 0x1;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_isr_register(uart_port_t uart_num, void (*fn)(void*), void * arg, int intr_alloc_flags,  uart_isr_handle_t *handle)
{
    int ret;
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    switch(uart_num) {
        case UART_NUM_1:
            ret=esp_intr_alloc(ETS_UART1_INTR_SOURCE, intr_alloc_flags, fn, arg, handle);
            break;
        case UART_NUM_2:
            ret=esp_intr_alloc(ETS_UART2_INTR_SOURCE, intr_alloc_flags, fn, arg, handle);
            break;
        case UART_NUM_0:
            default:
            ret=esp_intr_alloc(ETS_UART0_INTR_SOURCE, intr_alloc_flags, fn, arg, handle);
            break;
    }
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ret;
}


esp_err_t uart_isr_free(uart_port_t uart_num)
{
    esp_err_t ret;
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    if (p_uart_obj[uart_num]->intr_handle==NULL) return ESP_ERR_INVALID_ARG;
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    ret=esp_intr_free(p_uart_obj[uart_num]->intr_handle);
    p_uart_obj[uart_num]->intr_handle=NULL;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ret;
}

//internal signal can be output to multiple GPIO pads
//only one GPIO pad can connect with input signal
esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((tx_io_num < 0 || (GPIO_IS_VALID_OUTPUT_GPIO(tx_io_num))), "tx_io_num error", ESP_FAIL);
    UART_CHECK((rx_io_num < 0 || (GPIO_IS_VALID_GPIO(rx_io_num))), "rx_io_num error", ESP_FAIL);
    UART_CHECK((rts_io_num < 0 || (GPIO_IS_VALID_OUTPUT_GPIO(rts_io_num))), "rts_io_num error", ESP_FAIL);
    UART_CHECK((cts_io_num < 0 || (GPIO_IS_VALID_GPIO(cts_io_num))), "cts_io_num error", ESP_FAIL);

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
        gpio_set_level(tx_io_num, 1);
        gpio_matrix_out(tx_io_num, tx_sig, 0, 0);
    }

    if(rx_io_num >= 0) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[rx_io_num], PIN_FUNC_GPIO);
        gpio_set_pull_mode(rx_io_num, GPIO_PULLUP_ONLY);
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
        gpio_set_pull_mode(cts_io_num, GPIO_PULLUP_ONLY);
        gpio_set_direction(cts_io_num, GPIO_MODE_INPUT);
        gpio_matrix_in(cts_io_num, cts_sig, 0);
    }
    return ESP_OK;
}

esp_err_t uart_set_rts(uart_port_t uart_num, int level)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((UART[uart_num]->conf1.rx_flow_en != 1), "disable hw flowctrl before using sw control", ESP_FAIL);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->conf0.sw_rts = level & 0x1;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_set_dtr(uart_port_t uart_num, int level)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->conf0.sw_dtr = level & 0x1;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_set_tx_idle_num(uart_port_t uart_num, uint16_t idle_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((idle_num <= UART_TX_IDLE_NUM_V), "uart idle num error", ESP_FAIL);

    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->idle_conf.tx_idle_num = idle_num;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_param_config(uart_port_t uart_num, const uart_config_t *uart_config)
{
    esp_err_t r;
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((uart_config), "param null", ESP_FAIL);
    if(uart_num == UART_NUM_0) {
        periph_module_enable(PERIPH_UART0_MODULE);
    } else if(uart_num == UART_NUM_1) {
        periph_module_enable(PERIPH_UART1_MODULE);
    } else if(uart_num == UART_NUM_2) {
        periph_module_enable(PERIPH_UART2_MODULE);
    }
    r = uart_set_hw_flow_ctrl(uart_num, uart_config->flow_ctrl, uart_config->rx_flow_ctrl_thresh);
    if (r != ESP_OK) return r;

    UART[uart_num]->conf0.val =
          (uart_config->parity << UART_PARITY_S)
        | (uart_config->data_bits << UART_BIT_NUM_S)
        | ((uart_config->flow_ctrl & UART_HW_FLOWCTRL_CTS) ? UART_TX_FLOW_EN : 0x0)
        | (uart_config->use_ref_tick ? 0 : UART_TICK_REF_ALWAYS_ON_M);

    r = uart_set_baudrate(uart_num, uart_config->baud_rate);
    if (r != ESP_OK) return r;
    r = uart_set_tx_idle_num(uart_num, UART_TX_IDLE_NUM_DEFAULT);
    if (r != ESP_OK) return r;
    r = uart_set_stop_bits(uart_num, uart_config->stop_bits);
    //A hardware reset does not reset the fifo,
    //so we need to reset the fifo manually.
    uart_reset_rx_fifo(uart_num);
    return r;
}

esp_err_t uart_intr_config(uart_port_t uart_num, const uart_intr_config_t *intr_conf)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((intr_conf), "param null", ESP_FAIL);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->int_clr.val = UART_INTR_MASK;
    if(intr_conf->intr_enable_mask & UART_RXFIFO_TOUT_INT_ENA_M) {
        //Hardware issue workaround: when using ref_tick, the rx timeout threshold needs increase to 10 times.
        //T_ref = T_apb * APB_CLK/(REF_TICK << CLKDIV_FRAG_BIT_WIDTH)
        if(UART[uart_num]->conf0.tick_ref_always_on == 0) {
            UART[uart_num]->conf1.rx_tout_thrhd = ((intr_conf->rx_timeout_thresh * UART_TOUT_REF_FACTOR_DEFAULT) & UART_RX_TOUT_THRHD_V);
        } else {
            UART[uart_num]->conf1.rx_tout_thrhd = ((intr_conf->rx_timeout_thresh) & UART_RX_TOUT_THRHD_V);
        }
        UART[uart_num]->conf1.rx_tout_en = 1;
    } else {
        UART[uart_num]->conf1.rx_tout_en = 0;
    }
    if(intr_conf->intr_enable_mask & UART_RXFIFO_FULL_INT_ENA_M) {
        UART[uart_num]->conf1.rxfifo_full_thrhd = intr_conf->rxfifo_full_thresh;
    }
    if(intr_conf->intr_enable_mask & UART_TXFIFO_EMPTY_INT_ENA_M) {
        UART[uart_num]->conf1.txfifo_empty_thrhd = intr_conf->txfifo_empty_intr_thresh;
    }
    UART[uart_num]->int_ena.val = intr_conf->intr_enable_mask;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

static int uart_find_pattern_from_last(uint8_t* buf, int length, uint8_t pat_chr, int pat_num)
{
    int cnt = 0;
    int len = length;
    while (len >= 0) {
        if (buf[len] == pat_chr) {
            cnt++;
        } else {
            cnt = 0;
        }
        if (cnt >= pat_num) {
            break;
        }
        len --;
    }
    return len;
}

//internal isr handler for default driver code.
static void uart_rx_intr_handler_default(void *param)
{
    uart_obj_t *p_uart = (uart_obj_t*) param;
    uint8_t uart_num = p_uart->uart_num;
    uart_dev_t* uart_reg = UART[uart_num];
    int rx_fifo_len = uart_reg->status.rxfifo_cnt;
    uint8_t buf_idx = 0;
    uint32_t uart_intr_status = UART[uart_num]->int_st.val;
    uart_event_t uart_event;
    portBASE_TYPE HPTaskAwoken = 0;
    static uint8_t pat_flg = 0;
    while(uart_intr_status != 0x0) {
        buf_idx = 0;
        uart_event.type = UART_EVENT_MAX;
        if(uart_intr_status & UART_TXFIFO_EMPTY_INT_ST_M) {
            uart_clear_intr_status(uart_num, UART_TXFIFO_EMPTY_INT_CLR_M);
            uart_disable_intr_mask(uart_num, UART_TXFIFO_EMPTY_INT_ENA_M);
            if(p_uart->tx_waiting_brk) {
                continue;
            }
            //TX semaphore will only be used when tx_buf_size is zero.
            if(p_uart->tx_waiting_fifo == true && p_uart->tx_buf_size == 0) {
                p_uart->tx_waiting_fifo = false;
                xSemaphoreGiveFromISR(p_uart->tx_fifo_sem, &HPTaskAwoken);
                if(HPTaskAwoken == pdTRUE) {
                    portYIELD_FROM_ISR();
                }
            } else {
                //We don't use TX ring buffer, because the size is zero.
                if(p_uart->tx_buf_size == 0) {
                    continue;
                }
                int tx_fifo_rem = UART_FIFO_LEN - UART[uart_num]->status.txfifo_cnt;
                bool en_tx_flg = false;
                //We need to put a loop here, in case all the buffer items are very short.
                //That would cause a watch_dog reset because empty interrupt happens so often.
                //Although this is a loop in ISR, this loop will execute at most 128 turns.
                while(tx_fifo_rem) {
                    if(p_uart->tx_len_tot == 0 || p_uart->tx_ptr == NULL || p_uart->tx_len_cur == 0) {
                        size_t size;
                        p_uart->tx_head = (uart_tx_data_t*) xRingbufferReceiveFromISR(p_uart->tx_ring_buf, &size);
                        if(p_uart->tx_head) {
                            //The first item is the data description
                            //Get the first item to get the data information
                            if(p_uart->tx_len_tot == 0) {
                                p_uart->tx_ptr = NULL;
                                p_uart->tx_len_tot = p_uart->tx_head->tx_data.size;
                                if(p_uart->tx_head->type == UART_DATA_BREAK) {
                                    p_uart->tx_brk_flg = 1;
                                    p_uart->tx_brk_len = p_uart->tx_head->tx_data.brk_len;
                                }
                                //We have saved the data description from the 1st item, return buffer.
                                vRingbufferReturnItemFromISR(p_uart->tx_ring_buf, p_uart->tx_head, &HPTaskAwoken);
                                if(HPTaskAwoken == pdTRUE) {
                                    portYIELD_FROM_ISR();
                                }
                            }else if(p_uart->tx_ptr == NULL) {
                                //Update the TX item pointer, we will need this to return item to buffer.
                                p_uart->tx_ptr =  (uint8_t*) p_uart->tx_head;
                                en_tx_flg = true;
                                p_uart->tx_len_cur = size;
                            }
                        }
                        else {
                            //Can not get data from ring buffer, return;
                            break;
                        }
                    }
                    if (p_uart->tx_len_tot > 0 && p_uart->tx_ptr && p_uart->tx_len_cur > 0) {
                        //To fill the TX FIFO.
                        int send_len = p_uart->tx_len_cur > tx_fifo_rem ? tx_fifo_rem : p_uart->tx_len_cur;
                        // Set RS485 RTS pin before transmission if the half duplex mode is enabled
                        if (UART_IS_MODE_SET(uart_num, UART_MODE_RS485_HALF_DUPLEX)) {
                            UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
                            uart_reg->conf0.sw_rts = 0;
                            uart_reg->int_ena.tx_done = 1;
                            UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
                        }
                        for (buf_idx = 0; buf_idx < send_len; buf_idx++) {
                            WRITE_PERI_REG(UART_FIFO_AHB_REG(uart_num),
                                    *(p_uart->tx_ptr++) & 0xff);
                        }
                        p_uart->tx_len_tot -= send_len;
                        p_uart->tx_len_cur -= send_len;
                        tx_fifo_rem -= send_len;
                        if (p_uart->tx_len_cur == 0) {
                            //Return item to ring buffer.
                            vRingbufferReturnItemFromISR(p_uart->tx_ring_buf, p_uart->tx_head, &HPTaskAwoken);
                            if(HPTaskAwoken == pdTRUE) {
                                portYIELD_FROM_ISR();
                            }
                            p_uart->tx_head = NULL;
                            p_uart->tx_ptr = NULL;
                            //Sending item done, now we need to send break if there is a record.
                            //Set TX break signal after FIFO is empty
                            if(p_uart->tx_len_tot == 0 && p_uart->tx_brk_flg == 1) {
                                UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
                                uart_reg->int_ena.tx_brk_done = 0;
                                uart_reg->idle_conf.tx_brk_num = p_uart->tx_brk_len;
                                uart_reg->conf0.txd_brk = 1;
                                uart_reg->int_clr.tx_brk_done = 1;
                                uart_reg->int_ena.tx_brk_done = 1;
                                UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
                                p_uart->tx_waiting_brk = 1;
                                //do not enable TX empty interrupt
                                en_tx_flg = false;
                            } else {
                                //enable TX empty interrupt
                                en_tx_flg = true;
                            }
                        } else {
                            //enable TX empty interrupt
                            en_tx_flg = true;
                        }
                    }
                }
                if (en_tx_flg) {
                    uart_clear_intr_status(uart_num, UART_TXFIFO_EMPTY_INT_CLR_M);
                    uart_enable_intr_mask(uart_num, UART_TXFIFO_EMPTY_INT_ENA_M);
                }
            }
        }
        else if ((uart_intr_status & UART_RXFIFO_TOUT_INT_ST_M)
                || (uart_intr_status & UART_RXFIFO_FULL_INT_ST_M)
                || (uart_intr_status & UART_AT_CMD_CHAR_DET_INT_ST_M)
                ) {
            rx_fifo_len = uart_reg->status.rxfifo_cnt;
            if(pat_flg == 1) {
                uart_intr_status |= UART_AT_CMD_CHAR_DET_INT_ST_M;
                pat_flg = 0;
            }
            if (p_uart->rx_buffer_full_flg == false) {
                //We have to read out all data in RX FIFO to clear the interrupt signal
                while (buf_idx < rx_fifo_len) {
                    p_uart->rx_data_buf[buf_idx++] = uart_reg->fifo.rw_byte;
                }
                uint8_t pat_chr = uart_reg->at_cmd_char.data;
                int pat_num = uart_reg->at_cmd_char.char_num;
                int pat_idx = -1;

                //Get the buffer from the FIFO
                if (uart_intr_status & UART_AT_CMD_CHAR_DET_INT_ST_M) {
                    uart_clear_intr_status(uart_num, UART_AT_CMD_CHAR_DET_INT_CLR_M);
                    uart_event.type = UART_PATTERN_DET;
                    uart_event.size = rx_fifo_len;
                    pat_idx = uart_find_pattern_from_last(p_uart->rx_data_buf, rx_fifo_len - 1, pat_chr, pat_num);
                } else {
                    //After Copying the Data From FIFO ,Clear intr_status
                    uart_clear_intr_status(uart_num, UART_RXFIFO_TOUT_INT_CLR_M | UART_RXFIFO_FULL_INT_CLR_M);
                    uart_event.type = UART_DATA;
                    uart_event.size = rx_fifo_len;
                    UART_ENTER_CRITICAL_ISR(&uart_selectlock);
                    if (p_uart->uart_select_notif_callback) {
                        p_uart->uart_select_notif_callback(uart_num, UART_SELECT_READ_NOTIF, &HPTaskAwoken);
                    }
                    UART_EXIT_CRITICAL_ISR(&uart_selectlock);
                }
                p_uart->rx_stash_len = rx_fifo_len;
                //If we fail to push data to ring buffer, we will have to stash the data, and send next time.
                //Mainly for applications that uses flow control or small ring buffer.
                if(pdFALSE == xRingbufferSendFromISR(p_uart->rx_ring_buf, p_uart->rx_data_buf, p_uart->rx_stash_len, &HPTaskAwoken)) {
                    p_uart->rx_buffer_full_flg = true;
                    uart_disable_intr_mask(uart_num, UART_RXFIFO_TOUT_INT_ENA_M | UART_RXFIFO_FULL_INT_ENA_M);
                    if (uart_event.type == UART_PATTERN_DET) {
                        if (rx_fifo_len < pat_num) {
                            //some of the characters are read out in last interrupt
                            uart_pattern_enqueue(uart_num, p_uart->rx_buffered_len - (pat_num - rx_fifo_len));
                        } else {
                            uart_pattern_enqueue(uart_num,
                                    pat_idx <= -1 ?
                                            //can not find the pattern in buffer,
                                            p_uart->rx_buffered_len + p_uart->rx_stash_len :
                                            // find the pattern in buffer
                                            p_uart->rx_buffered_len + pat_idx);
                        }
                        if ((p_uart->xQueueUart != NULL) && (pdFALSE == xQueueSendFromISR(p_uart->xQueueUart, (void * )&uart_event, &HPTaskAwoken))) {
                            ESP_EARLY_LOGV(UART_TAG, "UART event queue full");
                        }
                    }
                    uart_event.type = UART_BUFFER_FULL;
                } else {
                    UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
                    if (uart_intr_status & UART_AT_CMD_CHAR_DET_INT_ST_M) {
                        if (rx_fifo_len < pat_num) {
                            //some of the characters are read out in last interrupt
                            uart_pattern_enqueue(uart_num, p_uart->rx_buffered_len - (pat_num - rx_fifo_len));
                        } else if(pat_idx >= 0) {
                            // find pattern in statsh buffer.
                            uart_pattern_enqueue(uart_num, p_uart->rx_buffered_len + pat_idx);
                        }
                    }
                    p_uart->rx_buffered_len += p_uart->rx_stash_len;
                    UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
                }
                if(HPTaskAwoken == pdTRUE) {
                    portYIELD_FROM_ISR();
                }
            } else {
                uart_disable_intr_mask(uart_num, UART_RXFIFO_FULL_INT_ENA_M | UART_RXFIFO_TOUT_INT_ENA_M);
                uart_clear_intr_status(uart_num, UART_RXFIFO_FULL_INT_CLR_M | UART_RXFIFO_TOUT_INT_CLR_M);
                if(uart_intr_status & UART_AT_CMD_CHAR_DET_INT_ST_M) {
                    uart_reg->int_clr.at_cmd_char_det = 1;
                    uart_event.type = UART_PATTERN_DET;
                    uart_event.size = rx_fifo_len;
                    pat_flg = 1;
                }
            }
        } else if(uart_intr_status & UART_RXFIFO_OVF_INT_ST_M) {
            // When fifo overflows, we reset the fifo.
            UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
            uart_reset_rx_fifo(uart_num);
            uart_reg->int_clr.rxfifo_ovf = 1;
            UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
            uart_event.type = UART_FIFO_OVF;
            UART_ENTER_CRITICAL_ISR(&uart_selectlock);
            if (p_uart->uart_select_notif_callback) {
                p_uart->uart_select_notif_callback(uart_num, UART_SELECT_ERROR_NOTIF, &HPTaskAwoken);
            }
            UART_EXIT_CRITICAL_ISR(&uart_selectlock);
        } else if(uart_intr_status & UART_BRK_DET_INT_ST_M) {
            uart_reg->int_clr.brk_det = 1;
            uart_event.type = UART_BREAK;
        } else if(uart_intr_status & UART_FRM_ERR_INT_ST_M) {
            uart_reg->int_clr.frm_err = 1;
            uart_event.type = UART_FRAME_ERR;
            UART_ENTER_CRITICAL_ISR(&uart_selectlock);
            if (p_uart->uart_select_notif_callback) {
                p_uart->uart_select_notif_callback(uart_num, UART_SELECT_ERROR_NOTIF, &HPTaskAwoken);
            }
            UART_EXIT_CRITICAL_ISR(&uart_selectlock);
        } else if(uart_intr_status & UART_PARITY_ERR_INT_ST_M) {
            uart_reg->int_clr.parity_err = 1;
            uart_event.type = UART_PARITY_ERR;
            UART_ENTER_CRITICAL_ISR(&uart_selectlock);
            if (p_uart->uart_select_notif_callback) {
                p_uart->uart_select_notif_callback(uart_num, UART_SELECT_ERROR_NOTIF, &HPTaskAwoken);
            }
            UART_EXIT_CRITICAL_ISR(&uart_selectlock);
        } else if(uart_intr_status & UART_TX_BRK_DONE_INT_ST_M) {
            UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
            uart_reg->conf0.txd_brk = 0;
            uart_reg->int_ena.tx_brk_done = 0;
            uart_reg->int_clr.tx_brk_done = 1;
            if(p_uart->tx_brk_flg == 1) {
                uart_reg->int_ena.txfifo_empty = 1;
            }
            UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
            if(p_uart->tx_brk_flg == 1) {
                p_uart->tx_brk_flg = 0;
                p_uart->tx_waiting_brk = 0;
            } else {
                xSemaphoreGiveFromISR(p_uart->tx_brk_sem, &HPTaskAwoken);
                if(HPTaskAwoken == pdTRUE) {
                    portYIELD_FROM_ISR();
                }
            }
        } else if(uart_intr_status & UART_TX_BRK_IDLE_DONE_INT_ST_M) {
            uart_disable_intr_mask(uart_num, UART_TX_BRK_IDLE_DONE_INT_ENA_M);
            uart_clear_intr_status(uart_num, UART_TX_BRK_IDLE_DONE_INT_CLR_M);
        } else if(uart_intr_status & UART_AT_CMD_CHAR_DET_INT_ST_M) {
            uart_reg->int_clr.at_cmd_char_det = 1;
            uart_event.type = UART_PATTERN_DET;
        } else if ((uart_intr_status & UART_RS485_CLASH_INT_ST_M)
                || (uart_intr_status & UART_RS485_FRM_ERR_INT_ENA)
                || (uart_intr_status & UART_RS485_PARITY_ERR_INT_ENA)) {
            // RS485 collision or frame error interrupt triggered
            uart_clear_intr_status(uart_num, UART_RS485_CLASH_INT_CLR_M);
            UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
            uart_reset_rx_fifo(uart_num);
            // Set collision detection flag
            p_uart_obj[uart_num]->coll_det_flg = true; 
            UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
            uart_event.type = UART_EVENT_MAX;
        } else if(uart_intr_status & UART_TX_DONE_INT_ST_M) {
            uart_disable_intr_mask(uart_num, UART_TX_DONE_INT_ENA_M);
            uart_clear_intr_status(uart_num, UART_TX_DONE_INT_CLR_M);
            // If RS485 half duplex mode is enable then reset FIFO and 
            // reset RTS pin to start receiver driver
            if (UART_IS_MODE_SET(uart_num, UART_MODE_RS485_HALF_DUPLEX)) {
                UART_ENTER_CRITICAL_ISR(&uart_spinlock[uart_num]);
                uart_reset_rx_fifo(uart_num); // Allows to avoid hardware issue with the RXFIFO reset
                uart_reg->conf0.sw_rts = 1;
                UART_EXIT_CRITICAL_ISR(&uart_spinlock[uart_num]);
            }
            xSemaphoreGiveFromISR(p_uart_obj[uart_num]->tx_done_sem, &HPTaskAwoken);
            if (HPTaskAwoken == pdTRUE) {
                portYIELD_FROM_ISR();
            }
        } else {
            uart_reg->int_clr.val = uart_intr_status; /*simply clear all other intr status*/
            uart_event.type = UART_EVENT_MAX;
        }

        if(uart_event.type != UART_EVENT_MAX && p_uart->xQueueUart) {
            if (pdFALSE == xQueueSendFromISR(p_uart->xQueueUart, (void * )&uart_event, &HPTaskAwoken)) {
                ESP_EARLY_LOGV(UART_TAG, "UART event queue full");
            }
            if(HPTaskAwoken == pdTRUE) {
                portYIELD_FROM_ISR();
            }
        }
        uart_intr_status = uart_reg->int_st.val;
    }
}

/**************************************************************/
esp_err_t uart_wait_tx_done(uart_port_t uart_num, TickType_t ticks_to_wait)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", ESP_FAIL);
    BaseType_t res;
    portTickType ticks_end = xTaskGetTickCount() + ticks_to_wait;
    //Take tx_mux
    res = xSemaphoreTake(p_uart_obj[uart_num]->tx_mux, (portTickType)ticks_to_wait);
    if(res == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }
    ticks_to_wait = ticks_end - xTaskGetTickCount();
    xSemaphoreTake(p_uart_obj[uart_num]->tx_done_sem, 0);
    ticks_to_wait = ticks_end - xTaskGetTickCount();
    if(UART[uart_num]->status.txfifo_cnt == 0) {
        xSemaphoreGive(p_uart_obj[uart_num]->tx_mux);
        return ESP_OK;
    }
    uart_enable_intr_mask(uart_num, UART_TX_DONE_INT_ENA_M);
    //take 2nd tx_done_sem, wait given from ISR
    res = xSemaphoreTake(p_uart_obj[uart_num]->tx_done_sem, (portTickType)ticks_to_wait);
    if(res == pdFALSE) {
        uart_disable_intr_mask(uart_num, UART_TX_DONE_INT_ENA_M);
        xSemaphoreGive(p_uart_obj[uart_num]->tx_mux);
        return ESP_ERR_TIMEOUT;
    }
    xSemaphoreGive(p_uart_obj[uart_num]->tx_mux);
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

//Fill UART tx_fifo and return a number,
//This function by itself is not thread-safe, always call from within a muxed section.
static int uart_fill_fifo(uart_port_t uart_num, const char* buffer, uint32_t len)
{
    uint8_t i = 0;
    uint8_t tx_fifo_cnt = UART[uart_num]->status.txfifo_cnt;
    uint8_t tx_remain_fifo_cnt = (UART_FIFO_LEN - tx_fifo_cnt);
    uint8_t copy_cnt = (len >= tx_remain_fifo_cnt ? tx_remain_fifo_cnt : len);
    // Set the RTS pin if RS485 mode is enabled
    if (UART_IS_MODE_SET(uart_num, UART_MODE_RS485_HALF_DUPLEX)) {
        UART[uart_num]->conf0.sw_rts = 0;
        UART[uart_num]->int_ena.tx_done = 1;
    }
    for (i = 0; i < copy_cnt; i++) {
        WRITE_PERI_REG(UART_FIFO_AHB_REG(uart_num), buffer[i]);
    }
    return copy_cnt;
}

int uart_tx_chars(uart_port_t uart_num, const char* buffer, uint32_t len)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", (-1));
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", (-1));
    UART_CHECK(buffer, "buffer null", (-1));
    if(len == 0) {
        return 0;
    }
    xSemaphoreTake(p_uart_obj[uart_num]->tx_mux, (portTickType)portMAX_DELAY);
    int tx_len = uart_fill_fifo(uart_num, (const char*) buffer, len);
    xSemaphoreGive(p_uart_obj[uart_num]->tx_mux);
    return tx_len;
}

static int uart_tx_all(uart_port_t uart_num, const char* src, size_t size, bool brk_en, int brk_len)
{
    if(size == 0) {
        return 0;
    }
    size_t original_size = size;

    //lock for uart_tx
    xSemaphoreTake(p_uart_obj[uart_num]->tx_mux, (portTickType)portMAX_DELAY);
    p_uart_obj[uart_num]->coll_det_flg = false;
    if(p_uart_obj[uart_num]->tx_buf_size > 0) {
        int max_size = xRingbufferGetMaxItemSize(p_uart_obj[uart_num]->tx_ring_buf);
        int offset = 0;
        uart_tx_data_t evt;
        evt.tx_data.size = size;
        evt.tx_data.brk_len = brk_len;
        if(brk_en) {
            evt.type = UART_DATA_BREAK;
        } else {
            evt.type = UART_DATA;
        }
        xRingbufferSend(p_uart_obj[uart_num]->tx_ring_buf, (void*) &evt, sizeof(uart_tx_data_t), portMAX_DELAY);
        while(size > 0) {
            int send_size = size > max_size / 2 ? max_size / 2 : size;
            xRingbufferSend(p_uart_obj[uart_num]->tx_ring_buf, (void*) (src + offset), send_size, portMAX_DELAY);
            size -= send_size;
            offset += send_size;
            uart_enable_tx_intr(uart_num, 1, UART_EMPTY_THRESH_DEFAULT);
        }
    } else {
        while(size) {
            //semaphore for tx_fifo available
            if(pdTRUE == xSemaphoreTake(p_uart_obj[uart_num]->tx_fifo_sem, (portTickType)portMAX_DELAY)) {
                size_t sent = uart_fill_fifo(uart_num, (char*) src, size);
                if(sent < size) {
                    p_uart_obj[uart_num]->tx_waiting_fifo = true;
                    uart_enable_tx_intr(uart_num, 1, UART_EMPTY_THRESH_DEFAULT);
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
    }
    xSemaphoreGive(p_uart_obj[uart_num]->tx_mux);
    return original_size;
}

int uart_write_bytes(uart_port_t uart_num, const char* src, size_t size)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", (-1));
    UART_CHECK((p_uart_obj[uart_num] != NULL), "uart driver error", (-1));
    UART_CHECK(src, "buffer null", (-1));
    return uart_tx_all(uart_num, src, size, 0, 0);
}

int uart_write_bytes_with_break(uart_port_t uart_num, const char* src, size_t size, int brk_len)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", (-1));
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", (-1));
    UART_CHECK((size > 0), "uart size error", (-1));
    UART_CHECK((src), "uart data null", (-1));
    UART_CHECK((brk_len > 0 && brk_len < 256), "break_num error", (-1));
    return uart_tx_all(uart_num, src, size, 1, brk_len);
}

static bool uart_check_buf_full(uart_port_t uart_num)
{
    if(p_uart_obj[uart_num]->rx_buffer_full_flg) {
        BaseType_t res = xRingbufferSend(p_uart_obj[uart_num]->rx_ring_buf, p_uart_obj[uart_num]->rx_data_buf, p_uart_obj[uart_num]->rx_stash_len, 1);
        if(res == pdTRUE) {
            UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
            p_uart_obj[uart_num]->rx_buffered_len += p_uart_obj[uart_num]->rx_stash_len;
            p_uart_obj[uart_num]->rx_buffer_full_flg = false;
            UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
            uart_enable_rx_intr(p_uart_obj[uart_num]->uart_num);
            return true;
        }
    }
    return false;
}

int uart_read_bytes(uart_port_t uart_num, uint8_t* buf, uint32_t length, TickType_t ticks_to_wait)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", (-1));
    UART_CHECK((buf), "uart data null", (-1));
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", (-1));
    uint8_t* data = NULL;
    size_t size;
    size_t copy_len = 0;
    int len_tmp;
    if(xSemaphoreTake(p_uart_obj[uart_num]->rx_mux,(portTickType)ticks_to_wait) != pdTRUE) {
        return -1;
    }
    while(length) {
        if(p_uart_obj[uart_num]->rx_cur_remain == 0) {
            data = (uint8_t*) xRingbufferReceive(p_uart_obj[uart_num]->rx_ring_buf, &size, (portTickType) ticks_to_wait);
            if(data) {
                p_uart_obj[uart_num]->rx_head_ptr = data;
                p_uart_obj[uart_num]->rx_ptr = data;
                p_uart_obj[uart_num]->rx_cur_remain = size;
            } else {
                //When using dual cores, `rx_buffer_full_flg` may read and write on different cores at same time,
                //which may lose synchronization. So we also need to call `uart_check_buf_full` once when ringbuffer is empty
                //to solve the possible asynchronous issues.
                if(uart_check_buf_full(uart_num)) {
                    //This condition will never be true if `uart_read_bytes`
                    //and `uart_rx_intr_handler_default` are scheduled on the same core.
                    continue;
                } else {
                    xSemaphoreGive(p_uart_obj[uart_num]->rx_mux);
                    return copy_len;
                }
            }
        }
        if(p_uart_obj[uart_num]->rx_cur_remain > length) {
            len_tmp = length;
        } else {
            len_tmp = p_uart_obj[uart_num]->rx_cur_remain;
        }
        memcpy(buf + copy_len, p_uart_obj[uart_num]->rx_ptr, len_tmp);
        UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
        p_uart_obj[uart_num]->rx_buffered_len -= len_tmp;
        uart_pattern_queue_update(uart_num, len_tmp);
        p_uart_obj[uart_num]->rx_ptr += len_tmp;
        UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
        p_uart_obj[uart_num]->rx_cur_remain -= len_tmp;
        copy_len += len_tmp;
        length -= len_tmp;
        if(p_uart_obj[uart_num]->rx_cur_remain == 0) {
            vRingbufferReturnItem(p_uart_obj[uart_num]->rx_ring_buf, p_uart_obj[uart_num]->rx_head_ptr);
            p_uart_obj[uart_num]->rx_head_ptr = NULL;
            p_uart_obj[uart_num]->rx_ptr = NULL;
            uart_check_buf_full(uart_num);
        }
    }

    xSemaphoreGive(p_uart_obj[uart_num]->rx_mux);
    return copy_len;
}

esp_err_t uart_get_buffered_data_len(uart_port_t uart_num, size_t* size)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", ESP_FAIL);
    *size = p_uart_obj[uart_num]->rx_buffered_len;
    return ESP_OK;
}

esp_err_t uart_flush(uart_port_t uart_num) __attribute__((alias("uart_flush_input")));

esp_err_t uart_flush_input(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", ESP_FAIL);
    uart_obj_t* p_uart = p_uart_obj[uart_num];
    uint8_t* data;
    size_t size;

    //rx sem protect the ring buffer read related functions
    xSemaphoreTake(p_uart->rx_mux, (portTickType)portMAX_DELAY);
    uart_disable_rx_intr(p_uart_obj[uart_num]->uart_num);
    while(true) {
        if(p_uart->rx_head_ptr) {
            vRingbufferReturnItem(p_uart->rx_ring_buf, p_uart->rx_head_ptr);
            UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
            p_uart_obj[uart_num]->rx_buffered_len -= p_uart->rx_cur_remain;
            uart_pattern_queue_update(uart_num, p_uart->rx_cur_remain);
            UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
            p_uart->rx_ptr = NULL;
            p_uart->rx_cur_remain = 0;
            p_uart->rx_head_ptr = NULL;
        }
        data = (uint8_t*) xRingbufferReceive(p_uart->rx_ring_buf, &size, (portTickType) 0);
        if(data == NULL) {
            if( p_uart_obj[uart_num]->rx_buffered_len != 0 ) {
                ESP_LOGE(UART_TAG, "rx_buffered_len error");
                p_uart_obj[uart_num]->rx_buffered_len = 0;
            }
            //We also need to clear the `rx_buffer_full_flg` here.
            UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
            p_uart_obj[uart_num]->rx_buffer_full_flg = false;
            UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
            break;
        }
        UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
        p_uart_obj[uart_num]->rx_buffered_len -= size;
        uart_pattern_queue_update(uart_num, size);
        UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
        vRingbufferReturnItem(p_uart->rx_ring_buf, data);
        if(p_uart_obj[uart_num]->rx_buffer_full_flg) {
            BaseType_t res = xRingbufferSend(p_uart_obj[uart_num]->rx_ring_buf, p_uart_obj[uart_num]->rx_data_buf, p_uart_obj[uart_num]->rx_stash_len, 1);
            if(res == pdTRUE) {
                UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
                p_uart_obj[uart_num]->rx_buffered_len += p_uart_obj[uart_num]->rx_stash_len;
                p_uart_obj[uart_num]->rx_buffer_full_flg = false;
                UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
            }
        }
    }
    p_uart->rx_ptr = NULL;
    p_uart->rx_cur_remain = 0;
    p_uart->rx_head_ptr = NULL;
    uart_reset_rx_fifo(uart_num);
    uart_enable_rx_intr(p_uart_obj[uart_num]->uart_num);
    xSemaphoreGive(p_uart->rx_mux);
    return ESP_OK;
}

esp_err_t uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t *uart_queue, int intr_alloc_flags)
{
    esp_err_t r;
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    UART_CHECK((rx_buffer_size > UART_FIFO_LEN), "uart rx buffer length error(>128)", ESP_FAIL);
    UART_CHECK((tx_buffer_size > UART_FIFO_LEN) || (tx_buffer_size == 0), "uart tx buffer length error(>128 or 0)", ESP_FAIL);
    UART_CHECK((intr_alloc_flags & ESP_INTR_FLAG_IRAM) == 0, "ESP_INTR_FLAG_IRAM set in intr_alloc_flags", ESP_FAIL); /* uart_rx_intr_handler_default is not in IRAM */

    if(p_uart_obj[uart_num] == NULL) {
        p_uart_obj[uart_num] = (uart_obj_t*) calloc(1, sizeof(uart_obj_t));
        if(p_uart_obj[uart_num] == NULL) {
            ESP_LOGE(UART_TAG, "UART driver malloc error");
            return ESP_FAIL;
        }
        p_uart_obj[uart_num]->uart_num = uart_num;
        p_uart_obj[uart_num]->uart_mode = UART_MODE_UART;
        p_uart_obj[uart_num]->coll_det_flg = false;
        p_uart_obj[uart_num]->tx_fifo_sem = xSemaphoreCreateBinary();
        xSemaphoreGive(p_uart_obj[uart_num]->tx_fifo_sem);
        p_uart_obj[uart_num]->tx_done_sem = xSemaphoreCreateBinary();
        p_uart_obj[uart_num]->tx_brk_sem = xSemaphoreCreateBinary();
        p_uart_obj[uart_num]->tx_mux = xSemaphoreCreateMutex();
        p_uart_obj[uart_num]->rx_mux = xSemaphoreCreateMutex();
        p_uart_obj[uart_num]->queue_size = queue_size;
        p_uart_obj[uart_num]->tx_ptr = NULL;
        p_uart_obj[uart_num]->tx_head = NULL;
        p_uart_obj[uart_num]->tx_len_tot = 0;
        p_uart_obj[uart_num]->tx_brk_flg = 0;
        p_uart_obj[uart_num]->tx_brk_len = 0;
        p_uart_obj[uart_num]->tx_waiting_brk = 0;
        p_uart_obj[uart_num]->rx_buffered_len = 0;
        uart_pattern_queue_reset(uart_num, UART_PATTERN_DET_QLEN_DEFAULT);

        if(uart_queue) {
            p_uart_obj[uart_num]->xQueueUart = xQueueCreate(queue_size, sizeof(uart_event_t));
            *uart_queue = p_uart_obj[uart_num]->xQueueUart;
            ESP_LOGI(UART_TAG, "queue free spaces: %d", uxQueueSpacesAvailable(p_uart_obj[uart_num]->xQueueUart));
        } else {
            p_uart_obj[uart_num]->xQueueUart = NULL;
        }
        p_uart_obj[uart_num]->rx_buffer_full_flg = false;
        p_uart_obj[uart_num]->tx_waiting_fifo = false;
        p_uart_obj[uart_num]->rx_ptr = NULL;
        p_uart_obj[uart_num]->rx_cur_remain = 0;
        p_uart_obj[uart_num]->rx_head_ptr = NULL;
        p_uart_obj[uart_num]->rx_ring_buf = xRingbufferCreate(rx_buffer_size, RINGBUF_TYPE_BYTEBUF);
        if(tx_buffer_size > 0) {
            p_uart_obj[uart_num]->tx_ring_buf = xRingbufferCreate(tx_buffer_size, RINGBUF_TYPE_NOSPLIT);
            p_uart_obj[uart_num]->tx_buf_size = tx_buffer_size;
        } else {
            p_uart_obj[uart_num]->tx_ring_buf = NULL;
            p_uart_obj[uart_num]->tx_buf_size = 0;
        }
        p_uart_obj[uart_num]->uart_select_notif_callback = NULL;
    } else {
        ESP_LOGE(UART_TAG, "UART driver already installed");
        return ESP_FAIL;
    }

    r=uart_isr_register(uart_num, uart_rx_intr_handler_default, p_uart_obj[uart_num], intr_alloc_flags, &p_uart_obj[uart_num]->intr_handle);
    if (r!=ESP_OK) goto err;
    uart_intr_config_t uart_intr = {
        .intr_enable_mask = UART_RXFIFO_FULL_INT_ENA_M
                            | UART_RXFIFO_TOUT_INT_ENA_M
                            | UART_FRM_ERR_INT_ENA_M
                            | UART_RXFIFO_OVF_INT_ENA_M
                            | UART_BRK_DET_INT_ENA_M
                            | UART_PARITY_ERR_INT_ENA_M,
        .rxfifo_full_thresh = UART_FULL_THRESH_DEFAULT,
        .rx_timeout_thresh = UART_TOUT_THRESH_DEFAULT,
        .txfifo_empty_intr_thresh = UART_EMPTY_THRESH_DEFAULT
    };
    r=uart_intr_config(uart_num, &uart_intr);
    if (r!=ESP_OK) goto err;
    return r;

err:
    uart_driver_delete(uart_num);
    return r;
}

//Make sure no other tasks are still using UART before you call this function
esp_err_t uart_driver_delete(uart_port_t uart_num)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_FAIL);
    if(p_uart_obj[uart_num] == NULL) {
        ESP_LOGI(UART_TAG, "ALREADY NULL");
        return ESP_OK;
    }
    esp_intr_free(p_uart_obj[uart_num]->intr_handle);
    uart_disable_rx_intr(uart_num);
    uart_disable_tx_intr(uart_num);
    uart_pattern_link_free(uart_num);

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
    if(p_uart_obj[uart_num]->tx_mux) {
        vSemaphoreDelete(p_uart_obj[uart_num]->tx_mux);
        p_uart_obj[uart_num]->tx_mux = NULL;
    }
    if(p_uart_obj[uart_num]->rx_mux) {
        vSemaphoreDelete(p_uart_obj[uart_num]->rx_mux);
        p_uart_obj[uart_num]->rx_mux = NULL;
    }
    if(p_uart_obj[uart_num]->xQueueUart) {
        vQueueDelete(p_uart_obj[uart_num]->xQueueUart);
        p_uart_obj[uart_num]->xQueueUart = NULL;
    }
    if(p_uart_obj[uart_num]->rx_ring_buf) {
        vRingbufferDelete(p_uart_obj[uart_num]->rx_ring_buf);
        p_uart_obj[uart_num]->rx_ring_buf = NULL;
    }
    if(p_uart_obj[uart_num]->tx_ring_buf) {
        vRingbufferDelete(p_uart_obj[uart_num]->tx_ring_buf);
        p_uart_obj[uart_num]->tx_ring_buf = NULL;
    }

    free(p_uart_obj[uart_num]);
    p_uart_obj[uart_num] = NULL;

    if (uart_num != CONFIG_CONSOLE_UART_NUM ) {
       if(uart_num == UART_NUM_0) {
           periph_module_disable(PERIPH_UART0_MODULE);
       } else if(uart_num == UART_NUM_1) {
           periph_module_disable(PERIPH_UART1_MODULE);
       } else if(uart_num == UART_NUM_2) {
           periph_module_disable(PERIPH_UART2_MODULE);
       }
    }
    return ESP_OK;
}

void uart_set_select_notif_callback(uart_port_t uart_num, uart_select_notif_callback_t uart_select_notif_callback)
{
    if (uart_num < UART_NUM_MAX && p_uart_obj[uart_num]) {
        p_uart_obj[uart_num]->uart_select_notif_callback = (uart_select_notif_callback_t) uart_select_notif_callback;
    }
}

portMUX_TYPE *uart_get_selectlock()
{
    return &uart_selectlock;
}
// Set UART mode
esp_err_t uart_set_mode(uart_port_t uart_num, uart_mode_t mode) 
{
    UART_CHECK((p_uart_obj[uart_num]), "uart driver error", ESP_ERR_INVALID_STATE);
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_ERR_INVALID_ARG);
    if ((mode == UART_MODE_RS485_COLLISION_DETECT) || (mode == UART_MODE_RS485_APP_CTRL) 
            || (mode == UART_MODE_RS485_HALF_DUPLEX)) {
        UART_CHECK((UART[uart_num]->conf1.rx_flow_en != 1),
                "disable hw flowctrl before using RS485 mode", ESP_ERR_INVALID_ARG);
    }
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    UART[uart_num]->rs485_conf.en = 0;
    UART[uart_num]->rs485_conf.tx_rx_en = 0;
    UART[uart_num]->rs485_conf.rx_busy_tx_en = 0;
    UART[uart_num]->conf0.irda_en = 0;
    UART[uart_num]->conf0.sw_rts = 0;
    switch (mode) {
    case UART_MODE_UART:
        break;
    case UART_MODE_RS485_COLLISION_DETECT:
        // This mode allows read while transmitting that allows collision detection
        p_uart_obj[uart_num]->coll_det_flg = false;
        // Transmitters output signal loop back to the receivers input signal
        UART[uart_num]->rs485_conf.tx_rx_en = 0 ;
        // Transmitter should send data when its receiver is busy
        UART[uart_num]->rs485_conf.rx_busy_tx_en = 1;
        UART[uart_num]->rs485_conf.en = 1;
        // Enable collision detection interrupts
        uart_enable_intr_mask(uart_num, UART_RXFIFO_TOUT_INT_ENA
                                        | UART_RXFIFO_FULL_INT_ENA
                                        | UART_RS485_CLASH_INT_ENA
                                        | UART_RS485_FRM_ERR_INT_ENA
                                        | UART_RS485_PARITY_ERR_INT_ENA);
        break;
    case UART_MODE_RS485_APP_CTRL:
        // Application software control, remove echo
        UART[uart_num]->rs485_conf.rx_busy_tx_en = 1;
        UART[uart_num]->rs485_conf.en = 1;
        break;
    case UART_MODE_RS485_HALF_DUPLEX:
        // Enable receiver, sw_rts = 1  generates low level on RTS pin
        UART[uart_num]->conf0.sw_rts = 1;
        UART[uart_num]->rs485_conf.en = 1;
        // Must be set to 0 to automatically remove echo
        UART[uart_num]->rs485_conf.tx_rx_en = 0;
        // This is to void collision
        UART[uart_num]->rs485_conf.rx_busy_tx_en = 1;
        break;
    case UART_MODE_IRDA:
        UART[uart_num]->conf0.irda_en = 1;
        break;
    default:
        UART_CHECK(1, "unsupported uart mode", ESP_ERR_INVALID_ARG);
        break;
    }
    p_uart_obj[uart_num]->uart_mode = mode;
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_set_rx_timeout(uart_port_t uart_num, const uint8_t tout_thresh) 
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_ERR_INVALID_ARG);
    UART_CHECK((tout_thresh < 127), "tout_thresh max value is 126", ESP_ERR_INVALID_ARG);
    UART_ENTER_CRITICAL(&uart_spinlock[uart_num]);
    // The tout_thresh = 1, defines TOUT interrupt timeout equal to  
    // transmission time of one symbol (~11 bit) on current baudrate  
    if (tout_thresh > 0) {
        UART[uart_num]->conf1.rx_tout_thrhd = (tout_thresh & UART_RX_TOUT_THRHD_V);
        UART[uart_num]->conf1.rx_tout_en = 1;
    } else {
        UART[uart_num]->conf1.rx_tout_en = 0;
    }
    UART_EXIT_CRITICAL(&uart_spinlock[uart_num]);
    return ESP_OK;
}

esp_err_t uart_get_collision_flag(uart_port_t uart_num, bool* collision_flag)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_ERR_INVALID_ARG);
    UART_CHECK((collision_flag != NULL), "wrong parameter pointer", ESP_ERR_INVALID_ARG);
    UART_CHECK((UART_IS_MODE_SET(uart_num, UART_MODE_RS485_HALF_DUPLEX) 
                    || UART_IS_MODE_SET(uart_num, UART_MODE_RS485_COLLISION_DETECT)), 
                    "wrong mode", ESP_ERR_INVALID_ARG);
    *collision_flag = p_uart_obj[uart_num]->coll_det_flg;
    return ESP_OK;
}

esp_err_t uart_set_wakeup_threshold(uart_port_t uart_num, int wakeup_threshold)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_ERR_INVALID_ARG);
    UART_CHECK((wakeup_threshold <= UART_ACTIVE_THRESHOLD_V &&
                wakeup_threshold > UART_MIN_WAKEUP_THRESH),
                "wakeup_threshold out of bounds", ESP_ERR_INVALID_ARG);

    UART[uart_num]->sleep_conf.active_threshold = wakeup_threshold - UART_MIN_WAKEUP_THRESH;
    return ESP_OK;
}

esp_err_t uart_get_wakeup_threshold(uart_port_t uart_num, int* out_wakeup_threshold)
{
    UART_CHECK((uart_num < UART_NUM_MAX), "uart_num error", ESP_ERR_INVALID_ARG);
    UART_CHECK((out_wakeup_threshold != NULL), "argument is NULL", ESP_ERR_INVALID_ARG);

    *out_wakeup_threshold = UART[uart_num]->sleep_conf.active_threshold + UART_MIN_WAKEUP_THRESH;
    return ESP_OK;
}
