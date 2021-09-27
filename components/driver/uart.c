/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "malloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "hal/uart_hal.h"
#include "hal/gpio_hal.h"
#include "soc/uart_periph.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/uart_select.h"
#include "driver/periph_ctrl.h"
#include "sdkconfig.h"
#include "esp_rom_gpio.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/clk.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/clk.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/clk.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/clk.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/clk.h"
#endif

#ifdef CONFIG_UART_ISR_IN_IRAM
#define UART_ISR_ATTR     IRAM_ATTR
#define UART_MALLOC_CAPS  (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define UART_ISR_ATTR
#define UART_MALLOC_CAPS  MALLOC_CAP_DEFAULT
#endif

#define XOFF (0x13)
#define XON (0x11)

static const char *UART_TAG = "uart";

#define UART_EMPTY_THRESH_DEFAULT       (10)
#define UART_FULL_THRESH_DEFAULT        (120)
#define UART_TOUT_THRESH_DEFAULT        (10)
#define UART_CLKDIV_FRAG_BIT_WIDTH      (3)
#define UART_TX_IDLE_NUM_DEFAULT        (0)
#define UART_PATTERN_DET_QLEN_DEFAULT   (10)
#define UART_MIN_WAKEUP_THRESH          (UART_LL_MIN_WAKEUP_THRESH)

#define UART_INTR_CONFIG_FLAG ((UART_INTR_RXFIFO_FULL) \
                            | (UART_INTR_RXFIFO_TOUT) \
                            | (UART_INTR_RXFIFO_OVF) \
                            | (UART_INTR_BRK_DET) \
                            | (UART_INTR_PARITY_ERR))

#define UART_ENTER_CRITICAL_ISR(mux)    portENTER_CRITICAL_ISR(mux)
#define UART_EXIT_CRITICAL_ISR(mux)     portEXIT_CRITICAL_ISR(mux)
#define UART_ENTER_CRITICAL(mux)    portENTER_CRITICAL(mux)
#define UART_EXIT_CRITICAL(mux)     portEXIT_CRITICAL(mux)


// Check actual UART mode set
#define UART_IS_MODE_SET(uart_number, mode) ((p_uart_obj[uart_number]->uart_mode == mode))

#define UART_CONTEX_INIT_DEF(uart_num) {\
    .hal.dev = UART_LL_GET_HW(uart_num),\
    .spinlock = portMUX_INITIALIZER_UNLOCKED,\
    .hw_enabled = false,\
}

#if SOC_UART_SUPPORT_RTC_CLK
#define RTC_ENABLED(uart_num)    (BIT(uart_num))
#endif

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
    int *data;
} uart_pat_rb_t;

typedef struct {
    uart_port_t uart_num;               /*!< UART port number*/
    int event_queue_size;               /*!< UART event queue size*/
    intr_handle_t intr_handle;          /*!< UART interrupt handle*/
    uart_mode_t uart_mode;              /*!< UART controller actual mode set by uart_set_mode() */
    bool coll_det_flg;                  /*!< UART collision detection flag */
    bool rx_always_timeout_flg;         /*!< UART always detect rx timeout flag */
    int rx_buffered_len;                /*!< UART cached data length */
    int rx_buf_size;                    /*!< RX ring buffer size */
    bool rx_buffer_full_flg;            /*!< RX ring buffer full flag. */
    uint32_t rx_cur_remain;             /*!< Data number that waiting to be read out in ring buffer item*/
    uint8_t *rx_ptr;                    /*!< pointer to the current data in ring buffer*/
    uint8_t *rx_head_ptr;               /*!< pointer to the head of RX item*/
    uint8_t rx_data_buf[SOC_UART_FIFO_LEN]; /*!< Data buffer to stash FIFO data*/
    uint8_t rx_stash_len;               /*!< stashed data length.(When using flow control, after reading out FIFO data, if we fail to push to buffer, we can just stash them.) */
    uart_pat_rb_t rx_pattern_pos;
    int tx_buf_size;                    /*!< TX ring buffer size */
    bool tx_waiting_fifo;               /*!< this flag indicates that some task is waiting for FIFO empty interrupt, used to send all data without any data buffer*/
    uint8_t *tx_ptr;                    /*!< TX data pointer to push to FIFO in TX buffer mode*/
    uart_tx_data_t *tx_head;            /*!< TX data pointer to head of the current buffer in TX ring buffer*/
    uint32_t tx_len_tot;                /*!< Total length of current item in ring buffer*/
    uint32_t tx_len_cur;
    uint8_t tx_brk_flg;                 /*!< Flag to indicate to send a break signal in the end of the item sending procedure */
    uint8_t tx_brk_len;                 /*!< TX break signal cycle length/number */
    uint8_t tx_waiting_brk;             /*!< Flag to indicate that TX FIFO is ready to send break signal after FIFO is empty, do not push data into TX FIFO right now.*/
    uart_select_notif_callback_t uart_select_notif_callback; /*!< Notification about select() events */
    QueueHandle_t event_queue;          /*!< UART event queue handler*/
    RingbufHandle_t rx_ring_buf;        /*!< RX ring buffer handler*/
    RingbufHandle_t tx_ring_buf;        /*!< TX ring buffer handler*/
    SemaphoreHandle_t rx_mux;           /*!< UART RX data mutex*/
    SemaphoreHandle_t tx_mux;           /*!< UART TX mutex*/
    SemaphoreHandle_t tx_fifo_sem;      /*!< UART TX FIFO semaphore*/
    SemaphoreHandle_t tx_done_sem;      /*!< UART TX done semaphore*/
    SemaphoreHandle_t tx_brk_sem;       /*!< UART TX send break done semaphore*/
#if CONFIG_UART_ISR_IN_IRAM
    void *event_queue_storage;
    void *event_queue_struct;
    void *rx_ring_buf_storage;
    void *rx_ring_buf_struct;
    void *tx_ring_buf_storage;
    void *tx_ring_buf_struct;
    void *rx_mux_struct;
    void *tx_mux_struct;
    void *tx_fifo_sem_struct;
    void *tx_done_sem_struct;
    void *tx_brk_sem_struct;
#endif
} uart_obj_t;

typedef struct {
    uart_hal_context_t hal;        /*!< UART hal context*/
    portMUX_TYPE spinlock;
    bool hw_enabled;
} uart_context_t;

static uart_obj_t *p_uart_obj[UART_NUM_MAX] = {0};

static uart_context_t uart_context[UART_NUM_MAX] = {
    UART_CONTEX_INIT_DEF(UART_NUM_0),
    UART_CONTEX_INIT_DEF(UART_NUM_1),
#if UART_NUM_MAX > 2
    UART_CONTEX_INIT_DEF(UART_NUM_2),
#endif
};

static portMUX_TYPE uart_selectlock = portMUX_INITIALIZER_UNLOCKED;

#if SOC_UART_SUPPORT_RTC_CLK

static uint8_t rtc_enabled = 0;
static portMUX_TYPE rtc_num_spinlock = portMUX_INITIALIZER_UNLOCKED;

static void rtc_clk_enable(uart_port_t uart_num)
{
    portENTER_CRITICAL(&rtc_num_spinlock);
    if (!(rtc_enabled & RTC_ENABLED(uart_num))) {
        rtc_enabled |= RTC_ENABLED(uart_num);
    }
    SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN_M);
    portEXIT_CRITICAL(&rtc_num_spinlock);
}

static void rtc_clk_disable(uart_port_t uart_num)
{
    assert(rtc_enabled & RTC_ENABLED(uart_num));

    portENTER_CRITICAL(&rtc_num_spinlock);
    rtc_enabled &= ~RTC_ENABLED(uart_num);
    if (rtc_enabled == 0) {
        CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN_M);
    }
    portEXIT_CRITICAL(&rtc_num_spinlock);
}
#endif

static void uart_module_enable(uart_port_t uart_num)
{
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    if (uart_context[uart_num].hw_enabled != true) {
        periph_module_enable(uart_periph_signal[uart_num].module);
        if (uart_num != CONFIG_ESP_CONSOLE_UART_NUM) {
            // Workaround for ESP32C3: enable core reset
            // before enabling uart module clock
            // to prevent uart output garbage value.
#if SOC_UART_REQUIRE_CORE_RESET
            uart_hal_set_reset_core(&(uart_context[uart_num].hal), true);
            periph_module_reset(uart_periph_signal[uart_num].module);
            uart_hal_set_reset_core(&(uart_context[uart_num].hal), false);
#else
            periph_module_reset(uart_periph_signal[uart_num].module);
#endif
        }
        uart_context[uart_num].hw_enabled = true;
    }
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
}

static void uart_module_disable(uart_port_t uart_num)
{
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    if (uart_context[uart_num].hw_enabled != false) {
        if (uart_num != CONFIG_ESP_CONSOLE_UART_NUM ) {
            periph_module_disable(uart_periph_signal[uart_num].module);
        }
        uart_context[uart_num].hw_enabled = false;
    }
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
}

esp_err_t uart_set_word_length(uart_port_t uart_num, uart_word_length_t data_bit)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((data_bit < UART_DATA_BITS_MAX), ESP_FAIL, UART_TAG, "data bit error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_data_bit_num(&(uart_context[uart_num].hal), data_bit);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_get_word_length(uart_port_t uart_num, uart_word_length_t *data_bit)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    uart_hal_get_data_bit_num(&(uart_context[uart_num].hal), data_bit);
    return ESP_OK;
}

esp_err_t uart_set_stop_bits(uart_port_t uart_num, uart_stop_bits_t stop_bit)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((stop_bit < UART_STOP_BITS_MAX), ESP_FAIL, UART_TAG, "stop bit error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_stop_bits(&(uart_context[uart_num].hal), stop_bit);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_get_stop_bits(uart_port_t uart_num, uart_stop_bits_t *stop_bit)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_get_stop_bits(&(uart_context[uart_num].hal), stop_bit);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_set_parity(uart_port_t uart_num, uart_parity_t parity_mode)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_parity(&(uart_context[uart_num].hal), parity_mode);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_get_parity(uart_port_t uart_num, uart_parity_t *parity_mode)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_get_parity(&(uart_context[uart_num].hal), parity_mode);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_set_baudrate(uart_port_t uart_num, uint32_t baud_rate)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_baudrate(&(uart_context[uart_num].hal), baud_rate);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_get_baudrate(uart_port_t uart_num, uint32_t *baudrate)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_get_baudrate(&(uart_context[uart_num].hal), baudrate);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_set_line_inverse(uart_port_t uart_num, uint32_t inverse_mask)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_inverse_signal(&(uart_context[uart_num].hal), inverse_mask);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_set_sw_flow_ctrl(uart_port_t uart_num, bool enable,  uint8_t rx_thresh_xon,  uint8_t rx_thresh_xoff)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((rx_thresh_xon < SOC_UART_FIFO_LEN), ESP_FAIL, UART_TAG, "rx flow xon thresh error");
    ESP_RETURN_ON_FALSE((rx_thresh_xoff < SOC_UART_FIFO_LEN), ESP_FAIL, UART_TAG, "rx flow xoff thresh error");
    uart_sw_flowctrl_t sw_flow_ctl = {
        .xon_char = XON,
        .xoff_char = XOFF,
        .xon_thrd = rx_thresh_xon,
        .xoff_thrd = rx_thresh_xoff,
    };
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_sw_flow_ctrl(&(uart_context[uart_num].hal), &sw_flow_ctl, enable);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_set_hw_flow_ctrl(uart_port_t uart_num, uart_hw_flowcontrol_t flow_ctrl, uint8_t rx_thresh)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((rx_thresh < SOC_UART_FIFO_LEN), ESP_FAIL, UART_TAG, "rx flow thresh error");
    ESP_RETURN_ON_FALSE((flow_ctrl < UART_HW_FLOWCTRL_MAX), ESP_FAIL, UART_TAG, "hw_flowctrl mode error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_hw_flow_ctrl(&(uart_context[uart_num].hal), flow_ctrl, rx_thresh);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_get_hw_flow_ctrl(uart_port_t uart_num, uart_hw_flowcontrol_t *flow_ctrl)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_get_hw_flow_ctrl(&(uart_context[uart_num].hal), flow_ctrl);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t UART_ISR_ATTR uart_clear_intr_status(uart_port_t uart_num, uint32_t clr_mask)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), clr_mask);
    return ESP_OK;
}

esp_err_t uart_enable_intr_mask(uart_port_t uart_num, uint32_t enable_mask)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), enable_mask);
    uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), enable_mask);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_disable_intr_mask(uart_port_t uart_num, uint32_t disable_mask)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_disable_intr_mask(&(uart_context[uart_num].hal), disable_mask);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

static esp_err_t uart_pattern_link_free(uart_port_t uart_num)
{
    int *pdata = NULL;
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    if (p_uart_obj[uart_num]->rx_pattern_pos.data != NULL) {
        pdata = p_uart_obj[uart_num]->rx_pattern_pos.data;
        p_uart_obj[uart_num]->rx_pattern_pos.data = NULL;
        p_uart_obj[uart_num]->rx_pattern_pos.wr = 0;
        p_uart_obj[uart_num]->rx_pattern_pos.rd = 0;
    }
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    free(pdata);
    return ESP_OK;
}

static esp_err_t UART_ISR_ATTR uart_pattern_enqueue(uart_port_t uart_num, int pos)
{
    esp_err_t ret = ESP_OK;
    uart_pat_rb_t *p_pos = &p_uart_obj[uart_num]->rx_pattern_pos;
    int next = p_pos->wr + 1;
    if (next >= p_pos->len) {
        next = 0;
    }
    if (next == p_pos->rd) {
#ifndef CONFIG_UART_ISR_IN_IRAM     //Only log if ISR is not in IRAM
        ESP_EARLY_LOGW(UART_TAG, "Fail to enqueue pattern position, pattern queue is full.");
#endif
        ret = ESP_FAIL;
    } else {
        p_pos->data[p_pos->wr] = pos;
        p_pos->wr = next;
        ret = ESP_OK;
    }
    return ret;
}

static esp_err_t uart_pattern_dequeue(uart_port_t uart_num)
{
    if (p_uart_obj[uart_num]->rx_pattern_pos.data == NULL) {
        return ESP_ERR_INVALID_STATE;
    } else {
        esp_err_t ret = ESP_OK;
        uart_pat_rb_t *p_pos = &p_uart_obj[uart_num]->rx_pattern_pos;
        if (p_pos->rd == p_pos->wr) {
            ret = ESP_FAIL;
        } else {
            p_pos->rd++;
        }
        if (p_pos->rd >= p_pos->len) {
            p_pos->rd = 0;
        }
        return ret;
    }
}

static esp_err_t uart_pattern_queue_update(uart_port_t uart_num, int diff_len)
{
    uart_pat_rb_t *p_pos = &p_uart_obj[uart_num]->rx_pattern_pos;
    int rd = p_pos->rd;
    while (rd != p_pos->wr) {
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
    return ESP_OK;
}

int uart_pattern_pop_pos(uart_port_t uart_num)
{
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num]), (-1), UART_TAG, "uart driver error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_pat_rb_t *pat_pos = &p_uart_obj[uart_num]->rx_pattern_pos;
    int pos = -1;
    if (pat_pos != NULL && pat_pos->rd != pat_pos->wr) {
        pos = pat_pos->data[pat_pos->rd];
        uart_pattern_dequeue(uart_num);
    }
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return pos;
}

int uart_pattern_get_pos(uart_port_t uart_num)
{
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num]), (-1), UART_TAG, "uart driver error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_pat_rb_t *pat_pos = &p_uart_obj[uart_num]->rx_pattern_pos;
    int pos = -1;
    if (pat_pos != NULL && pat_pos->rd != pat_pos->wr) {
        pos = pat_pos->data[pat_pos->rd];
    }
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return pos;
}

esp_err_t uart_pattern_queue_reset(uart_port_t uart_num, int queue_length)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num]), ESP_ERR_INVALID_STATE, UART_TAG, "uart driver error");

    int *pdata = (int *) malloc(queue_length * sizeof(int));
    if (pdata == NULL) {
        return ESP_ERR_NO_MEM;
    }
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    int *ptmp = p_uart_obj[uart_num]->rx_pattern_pos.data;
    p_uart_obj[uart_num]->rx_pattern_pos.data = pdata;
    p_uart_obj[uart_num]->rx_pattern_pos.len = queue_length;
    p_uart_obj[uart_num]->rx_pattern_pos.rd = 0;
    p_uart_obj[uart_num]->rx_pattern_pos.wr = 0;
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    free(ptmp);
    return ESP_OK;
}

#if CONFIG_IDF_TARGET_ESP32
esp_err_t uart_enable_pattern_det_intr(uart_port_t uart_num, char pattern_chr, uint8_t chr_num, int chr_tout, int post_idle, int pre_idle)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE(chr_tout >= 0 && chr_tout <= UART_RX_GAP_TOUT_V, ESP_FAIL, UART_TAG, "uart pattern set error\n");
    ESP_RETURN_ON_FALSE(post_idle >= 0 && post_idle <= UART_POST_IDLE_NUM_V, ESP_FAIL, UART_TAG, "uart pattern set error\n");
    ESP_RETURN_ON_FALSE(pre_idle >= 0 && pre_idle <= UART_PRE_IDLE_NUM_V, ESP_FAIL, UART_TAG, "uart pattern set error\n");
    uart_at_cmd_t at_cmd = {0};
    at_cmd.cmd_char = pattern_chr;
    at_cmd.char_num = chr_num;
    at_cmd.gap_tout = chr_tout;
    at_cmd.pre_idle = pre_idle;
    at_cmd.post_idle = post_idle;
    uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_CMD_CHAR_DET);
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_at_cmd_char(&(uart_context[uart_num].hal), &at_cmd);
    uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), UART_INTR_CMD_CHAR_DET);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}
#endif

esp_err_t uart_enable_pattern_det_baud_intr(uart_port_t uart_num, char pattern_chr, uint8_t chr_num, int chr_tout, int post_idle, int pre_idle)
{
    ESP_RETURN_ON_FALSE(uart_num < UART_NUM_MAX, ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE(chr_tout >= 0 && chr_tout <= UART_RX_GAP_TOUT_V, ESP_FAIL, UART_TAG, "uart pattern set error\n");
    ESP_RETURN_ON_FALSE(post_idle >= 0 && post_idle <= UART_POST_IDLE_NUM_V, ESP_FAIL, UART_TAG, "uart pattern set error\n");
    ESP_RETURN_ON_FALSE(pre_idle >= 0 && pre_idle <= UART_PRE_IDLE_NUM_V, ESP_FAIL, UART_TAG, "uart pattern set error\n");
    uart_at_cmd_t at_cmd = {0};
    at_cmd.cmd_char = pattern_chr;
    at_cmd.char_num = chr_num;

#if CONFIG_IDF_TARGET_ESP32
    int apb_clk_freq = 0;
    uint32_t uart_baud = 0;
    uint32_t uart_div = 0;
    uart_get_baudrate(uart_num, &uart_baud);
    apb_clk_freq = esp_clk_apb_freq();
    uart_div = apb_clk_freq / uart_baud;

    at_cmd.gap_tout = chr_tout * uart_div;
    at_cmd.pre_idle = pre_idle * uart_div;
    at_cmd.post_idle = post_idle * uart_div;
#else
    at_cmd.gap_tout = chr_tout;
    at_cmd.pre_idle = pre_idle;
    at_cmd.post_idle = post_idle;
#endif
    uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_CMD_CHAR_DET);
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_at_cmd_char(&(uart_context[uart_num].hal), &at_cmd);
    uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), UART_INTR_CMD_CHAR_DET);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}


esp_err_t uart_disable_pattern_det_intr(uart_port_t uart_num)
{
    return uart_disable_intr_mask(uart_num, UART_INTR_CMD_CHAR_DET);
}

esp_err_t uart_enable_rx_intr(uart_port_t uart_num)
{
    return uart_enable_intr_mask(uart_num, UART_INTR_RXFIFO_FULL | UART_INTR_RXFIFO_TOUT);
}

esp_err_t uart_disable_rx_intr(uart_port_t uart_num)
{
    return uart_disable_intr_mask(uart_num, UART_INTR_RXFIFO_FULL | UART_INTR_RXFIFO_TOUT);
}

esp_err_t uart_disable_tx_intr(uart_port_t uart_num)
{
    return uart_disable_intr_mask(uart_num, UART_INTR_TXFIFO_EMPTY);
}

esp_err_t uart_enable_tx_intr(uart_port_t uart_num, int enable, int thresh)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((thresh < SOC_UART_FIFO_LEN), ESP_FAIL, UART_TAG, "empty intr threshold error");
    uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_TXFIFO_EMPTY);
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_txfifo_empty_thr(&(uart_context[uart_num].hal), thresh);
    uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), UART_INTR_TXFIFO_EMPTY);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_isr_register(uart_port_t uart_num, void (*fn)(void *), void *arg, int intr_alloc_flags,  uart_isr_handle_t *handle)
{
    int ret;
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    ret = esp_intr_alloc(uart_periph_signal[uart_num].irq, intr_alloc_flags, fn, arg, handle);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ret;
}

esp_err_t uart_isr_free(uart_port_t uart_num)
{
    esp_err_t ret;
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num]), ESP_FAIL, UART_TAG, "uart driver error");
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num]->intr_handle != NULL), ESP_ERR_INVALID_ARG, UART_TAG, "uart driver error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    ret = esp_intr_free(p_uart_obj[uart_num]->intr_handle);
    p_uart_obj[uart_num]->intr_handle = NULL;
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ret;
}

static bool uart_try_set_iomux_pin(uart_port_t uart_num, int io_num, uint32_t idx)
{
    /* Store a pointer to the default pin, to optimize access to its fields. */
    const uart_periph_sig_t *upin = &uart_periph_signal[uart_num].pins[idx];

    /* In theory, if default_gpio is -1, iomux_func should also be -1, but
     * let's be safe and test both. */
    if (upin->iomux_func == -1 || upin->default_gpio == -1 || upin->default_gpio != io_num) {
        return false;
    }

    /* Assign the correct funct to the GPIO. */
    assert (upin->iomux_func != -1);
    gpio_iomux_out(io_num, upin->iomux_func, false);

    /* If the pin is input, we also have to redirect the signal,
     * in order to bypasse the GPIO matrix. */
    if (upin->input) {
        gpio_iomux_in(io_num, upin->signal);
    }

    return true;
}

//internal signal can be output to multiple GPIO pads
//only one GPIO pad can connect with input signal
esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num)
{
    ESP_RETURN_ON_FALSE((uart_num >= 0), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((tx_io_num < 0 || (GPIO_IS_VALID_OUTPUT_GPIO(tx_io_num))), ESP_FAIL, UART_TAG, "tx_io_num error");
    ESP_RETURN_ON_FALSE((rx_io_num < 0 || (GPIO_IS_VALID_GPIO(rx_io_num))), ESP_FAIL, UART_TAG, "rx_io_num error");
    ESP_RETURN_ON_FALSE((rts_io_num < 0 || (GPIO_IS_VALID_OUTPUT_GPIO(rts_io_num))), ESP_FAIL, UART_TAG, "rts_io_num error");
    ESP_RETURN_ON_FALSE((cts_io_num < 0 || (GPIO_IS_VALID_GPIO(cts_io_num))), ESP_FAIL, UART_TAG, "cts_io_num error");

    /* In the following statements, if the io_num is negative, no need to configure anything. */
    if (tx_io_num >= 0 && !uart_try_set_iomux_pin(uart_num, tx_io_num, SOC_UART_TX_PIN_IDX)) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[tx_io_num], PIN_FUNC_GPIO);
        gpio_set_level(tx_io_num, 1);
        esp_rom_gpio_connect_out_signal(tx_io_num, UART_PERIPH_SIGNAL(uart_num, SOC_UART_TX_PIN_IDX), 0, 0);
    }

    if (rx_io_num >= 0 && !uart_try_set_iomux_pin(uart_num, rx_io_num, SOC_UART_RX_PIN_IDX)) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[rx_io_num], PIN_FUNC_GPIO);
        gpio_set_pull_mode(rx_io_num, GPIO_PULLUP_ONLY);
        gpio_set_direction(rx_io_num, GPIO_MODE_INPUT);
        esp_rom_gpio_connect_in_signal(rx_io_num, UART_PERIPH_SIGNAL(uart_num, SOC_UART_RX_PIN_IDX), 0);
    }

    if (rts_io_num >= 0 && !uart_try_set_iomux_pin(uart_num, rts_io_num, SOC_UART_RTS_PIN_IDX)) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[rts_io_num], PIN_FUNC_GPIO);
        gpio_set_direction(rts_io_num, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(rts_io_num, UART_PERIPH_SIGNAL(uart_num, SOC_UART_RTS_PIN_IDX), 0, 0);
    }

    if (cts_io_num >= 0  && !uart_try_set_iomux_pin(uart_num, cts_io_num, SOC_UART_CTS_PIN_IDX)) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[cts_io_num], PIN_FUNC_GPIO);
        gpio_set_pull_mode(cts_io_num, GPIO_PULLUP_ONLY);
        gpio_set_direction(cts_io_num, GPIO_MODE_INPUT);
        esp_rom_gpio_connect_in_signal(cts_io_num, UART_PERIPH_SIGNAL(uart_num, SOC_UART_CTS_PIN_IDX), 0);
    }

    return ESP_OK;
}

esp_err_t uart_set_rts(uart_port_t uart_num, int level)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((!uart_hal_is_hw_rts_en(&(uart_context[uart_num].hal))), ESP_FAIL, UART_TAG, "disable hw flowctrl before using sw control");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_rts(&(uart_context[uart_num].hal), level);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_set_dtr(uart_port_t uart_num, int level)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_dtr(&(uart_context[uart_num].hal), level);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_set_tx_idle_num(uart_port_t uart_num, uint16_t idle_num)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((idle_num <= UART_TX_IDLE_NUM_V), ESP_FAIL, UART_TAG, "uart idle num error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_tx_idle_num(&(uart_context[uart_num].hal), idle_num);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_param_config(uart_port_t uart_num, const uart_config_t *uart_config)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((uart_config), ESP_FAIL, UART_TAG, "param null");
    ESP_RETURN_ON_FALSE((uart_config->rx_flow_ctrl_thresh < SOC_UART_FIFO_LEN), ESP_FAIL, UART_TAG, "rx flow thresh error");
    ESP_RETURN_ON_FALSE((uart_config->flow_ctrl < UART_HW_FLOWCTRL_MAX), ESP_FAIL, UART_TAG, "hw_flowctrl mode error");
    ESP_RETURN_ON_FALSE((uart_config->data_bits < UART_DATA_BITS_MAX), ESP_FAIL, UART_TAG, "data bit error");
    uart_module_enable(uart_num);
#if SOC_UART_SUPPORT_RTC_CLK
    if (uart_config->source_clk == UART_SCLK_RTC) {
        rtc_clk_enable(uart_num);
    }
#endif
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_init(&(uart_context[uart_num].hal), uart_num);
    uart_hal_set_sclk(&(uart_context[uart_num].hal), uart_config->source_clk);
    uart_hal_set_baudrate(&(uart_context[uart_num].hal), uart_config->baud_rate);
    uart_hal_set_parity(&(uart_context[uart_num].hal), uart_config->parity);
    uart_hal_set_data_bit_num(&(uart_context[uart_num].hal), uart_config->data_bits);
    uart_hal_set_stop_bits(&(uart_context[uart_num].hal), uart_config->stop_bits);
    uart_hal_set_tx_idle_num(&(uart_context[uart_num].hal), UART_TX_IDLE_NUM_DEFAULT);
    uart_hal_set_hw_flow_ctrl(&(uart_context[uart_num].hal), uart_config->flow_ctrl, uart_config->rx_flow_ctrl_thresh);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_rxfifo_rst(&(uart_context[uart_num].hal));
    uart_hal_txfifo_rst(&(uart_context[uart_num].hal));
    return ESP_OK;
}

esp_err_t uart_intr_config(uart_port_t uart_num, const uart_intr_config_t *intr_conf)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((intr_conf), ESP_FAIL, UART_TAG, "param null");
    uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_LL_INTR_MASK);
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    if (intr_conf->intr_enable_mask & UART_INTR_RXFIFO_TOUT) {
        uart_hal_set_rx_timeout(&(uart_context[uart_num].hal), intr_conf->rx_timeout_thresh);
    } else {
        //Disable rx_tout intr
        uart_hal_set_rx_timeout(&(uart_context[uart_num].hal), 0);
    }
    if (intr_conf->intr_enable_mask & UART_INTR_RXFIFO_FULL) {
        uart_hal_set_rxfifo_full_thr(&(uart_context[uart_num].hal), intr_conf->rxfifo_full_thresh);
    }
    if (intr_conf->intr_enable_mask & UART_INTR_TXFIFO_EMPTY) {
        uart_hal_set_txfifo_empty_thr(&(uart_context[uart_num].hal), intr_conf->txfifo_empty_intr_thresh);
    }
    uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), intr_conf->intr_enable_mask);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

static int UART_ISR_ATTR uart_find_pattern_from_last(uint8_t *buf, int length, uint8_t pat_chr, uint8_t pat_num)
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
static void UART_ISR_ATTR uart_rx_intr_handler_default(void *param)
{
    uart_obj_t *p_uart = (uart_obj_t *) param;
    uint8_t uart_num = p_uart->uart_num;
    int rx_fifo_len = 0;
    uint32_t uart_intr_status = 0;
    uart_event_t uart_event;
    portBASE_TYPE HPTaskAwoken = 0;
    static uint8_t pat_flg = 0;
    while (1) {
        // The `continue statement` may cause the interrupt to loop infinitely
        // we exit the interrupt here
        uart_intr_status = uart_hal_get_intsts_mask(&(uart_context[uart_num].hal));
        //Exit form while loop
        if (uart_intr_status == 0) {
            break;
        }
        uart_event.type = UART_EVENT_MAX;
        if (uart_intr_status & UART_INTR_TXFIFO_EMPTY) {
            UART_ENTER_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
            uart_hal_disable_intr_mask(&(uart_context[uart_num].hal), UART_INTR_TXFIFO_EMPTY);
            UART_EXIT_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
            uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_TXFIFO_EMPTY);
            if (p_uart->tx_waiting_brk) {
                continue;
            }
            //TX semaphore will only be used when tx_buf_size is zero.
            if (p_uart->tx_waiting_fifo == true && p_uart->tx_buf_size == 0) {
                p_uart->tx_waiting_fifo = false;
                xSemaphoreGiveFromISR(p_uart->tx_fifo_sem, &HPTaskAwoken);
            } else {
                //We don't use TX ring buffer, because the size is zero.
                if (p_uart->tx_buf_size == 0) {
                    continue;
                }
                bool en_tx_flg = false;
                uint32_t tx_fifo_rem = uart_hal_get_txfifo_len(&(uart_context[uart_num].hal));
                //We need to put a loop here, in case all the buffer items are very short.
                //That would cause a watch_dog reset because empty interrupt happens so often.
                //Although this is a loop in ISR, this loop will execute at most 128 turns.
                while (tx_fifo_rem) {
                    if (p_uart->tx_len_tot == 0 || p_uart->tx_ptr == NULL || p_uart->tx_len_cur == 0) {
                        size_t size;
                        p_uart->tx_head = (uart_tx_data_t *) xRingbufferReceiveFromISR(p_uart->tx_ring_buf, &size);
                        if (p_uart->tx_head) {
                            //The first item is the data description
                            //Get the first item to get the data information
                            if (p_uart->tx_len_tot == 0) {
                                p_uart->tx_ptr = NULL;
                                p_uart->tx_len_tot = p_uart->tx_head->tx_data.size;
                                if (p_uart->tx_head->type == UART_DATA_BREAK) {
                                    p_uart->tx_brk_flg = 1;
                                    p_uart->tx_brk_len = p_uart->tx_head->tx_data.brk_len;
                                }
                                //We have saved the data description from the 1st item, return buffer.
                                vRingbufferReturnItemFromISR(p_uart->tx_ring_buf, p_uart->tx_head, &HPTaskAwoken);
                            } else if (p_uart->tx_ptr == NULL) {
                                //Update the TX item pointer, we will need this to return item to buffer.
                                p_uart->tx_ptr = (uint8_t *)p_uart->tx_head;
                                en_tx_flg = true;
                                p_uart->tx_len_cur = size;
                            }
                        } else {
                            //Can not get data from ring buffer, return;
                            break;
                        }
                    }
                    if (p_uart->tx_len_tot > 0 && p_uart->tx_ptr && p_uart->tx_len_cur > 0) {
                        //To fill the TX FIFO.
                        uint32_t send_len = 0;
                        // Set RS485 RTS pin before transmission if the half duplex mode is enabled
                        if (UART_IS_MODE_SET(uart_num, UART_MODE_RS485_HALF_DUPLEX)) {
                            UART_ENTER_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                            uart_hal_set_rts(&(uart_context[uart_num].hal), 0);
                            uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), UART_INTR_TX_DONE);
                            UART_EXIT_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                        }
                        uart_hal_write_txfifo(&(uart_context[uart_num].hal),
                                              (const uint8_t *)p_uart->tx_ptr,
                                              (p_uart->tx_len_cur > tx_fifo_rem) ? tx_fifo_rem : p_uart->tx_len_cur,
                                              &send_len);
                        p_uart->tx_ptr += send_len;
                        p_uart->tx_len_tot -= send_len;
                        p_uart->tx_len_cur -= send_len;
                        tx_fifo_rem -= send_len;
                        if (p_uart->tx_len_cur == 0) {
                            //Return item to ring buffer.
                            vRingbufferReturnItemFromISR(p_uart->tx_ring_buf, p_uart->tx_head, &HPTaskAwoken);
                            p_uart->tx_head = NULL;
                            p_uart->tx_ptr = NULL;
                            //Sending item done, now we need to send break if there is a record.
                            //Set TX break signal after FIFO is empty
                            if (p_uart->tx_len_tot == 0 && p_uart->tx_brk_flg == 1) {
                                uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_TX_BRK_DONE);
                                UART_ENTER_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                                uart_hal_tx_break(&(uart_context[uart_num].hal), p_uart->tx_brk_len);
                                uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), UART_INTR_TX_BRK_DONE);
                                UART_EXIT_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
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
                    uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_TXFIFO_EMPTY);
                    UART_ENTER_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                    uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), UART_INTR_TXFIFO_EMPTY);
                    UART_EXIT_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                }
            }
        } else if ((uart_intr_status & UART_INTR_RXFIFO_TOUT)
                   || (uart_intr_status & UART_INTR_RXFIFO_FULL)
                   || (uart_intr_status & UART_INTR_CMD_CHAR_DET)
                  ) {
            if (pat_flg == 1) {
                uart_intr_status |= UART_INTR_CMD_CHAR_DET;
                pat_flg = 0;
            }
            if (p_uart->rx_buffer_full_flg == false) {
                rx_fifo_len = uart_hal_get_rxfifo_len(&(uart_context[uart_num].hal));
                if ((p_uart_obj[uart_num]->rx_always_timeout_flg) && !(uart_intr_status & UART_INTR_RXFIFO_TOUT)) {
                    rx_fifo_len--; // leave one byte in the fifo in order to trigger uart_intr_rxfifo_tout
                }
                uart_hal_read_rxfifo(&(uart_context[uart_num].hal), p_uart->rx_data_buf, &rx_fifo_len);
                uint8_t pat_chr = 0;
                uint8_t pat_num = 0;
                int pat_idx = -1;
                uart_hal_get_at_cmd_char(&(uart_context[uart_num].hal), &pat_chr, &pat_num);

                //Get the buffer from the FIFO
                if (uart_intr_status & UART_INTR_CMD_CHAR_DET) {
                    uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_CMD_CHAR_DET);
                    uart_event.type = UART_PATTERN_DET;
                    uart_event.size = rx_fifo_len;
                    pat_idx = uart_find_pattern_from_last(p_uart->rx_data_buf, rx_fifo_len - 1, pat_chr, pat_num);
                } else {
                    //After Copying the Data From FIFO ,Clear intr_status
                    uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_RXFIFO_TOUT | UART_INTR_RXFIFO_FULL);
                    uart_event.type = UART_DATA;
                    uart_event.size = rx_fifo_len;
                    uart_event.timeout_flag = (uart_intr_status & UART_INTR_RXFIFO_TOUT) ? true : false;
                    UART_ENTER_CRITICAL_ISR(&uart_selectlock);
                    if (p_uart->uart_select_notif_callback) {
                        p_uart->uart_select_notif_callback(uart_num, UART_SELECT_READ_NOTIF, &HPTaskAwoken);
                    }
                    UART_EXIT_CRITICAL_ISR(&uart_selectlock);
                }
                p_uart->rx_stash_len = rx_fifo_len;
                //If we fail to push data to ring buffer, we will have to stash the data, and send next time.
                //Mainly for applications that uses flow control or small ring buffer.
                if (pdFALSE == xRingbufferSendFromISR(p_uart->rx_ring_buf, p_uart->rx_data_buf, p_uart->rx_stash_len, &HPTaskAwoken)) {
                    p_uart->rx_buffer_full_flg = true;
                    UART_ENTER_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                    uart_hal_disable_intr_mask(&(uart_context[uart_num].hal), UART_INTR_RXFIFO_TOUT | UART_INTR_RXFIFO_FULL);
                    UART_EXIT_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                    if (uart_event.type == UART_PATTERN_DET) {
                        UART_ENTER_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
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
                        UART_EXIT_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                        if ((p_uart->event_queue != NULL) && (pdFALSE == xQueueSendFromISR(p_uart->event_queue, (void * )&uart_event, &HPTaskAwoken))) {
#ifndef CONFIG_UART_ISR_IN_IRAM     //Only log if ISR is not in IRAM
                            ESP_EARLY_LOGV(UART_TAG, "UART event queue full");
#endif
                        }
                    }
                    uart_event.type = UART_BUFFER_FULL;
                } else {
                    UART_ENTER_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                    if (uart_intr_status & UART_INTR_CMD_CHAR_DET) {
                        if (rx_fifo_len < pat_num) {
                            //some of the characters are read out in last interrupt
                            uart_pattern_enqueue(uart_num, p_uart->rx_buffered_len - (pat_num - rx_fifo_len));
                        } else if (pat_idx >= 0) {
                            // find the pattern in stash buffer.
                            uart_pattern_enqueue(uart_num, p_uart->rx_buffered_len + pat_idx);
                        }
                    }
                    p_uart->rx_buffered_len += p_uart->rx_stash_len;
                    UART_EXIT_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                }
            } else {
                UART_ENTER_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                uart_hal_disable_intr_mask(&(uart_context[uart_num].hal), UART_INTR_RXFIFO_FULL | UART_INTR_RXFIFO_TOUT);
                UART_EXIT_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_RXFIFO_FULL | UART_INTR_RXFIFO_TOUT);
                if (uart_intr_status & UART_INTR_CMD_CHAR_DET) {
                    uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_CMD_CHAR_DET);
                    uart_event.type = UART_PATTERN_DET;
                    uart_event.size = rx_fifo_len;
                    pat_flg = 1;
                }
            }
        } else if (uart_intr_status & UART_INTR_RXFIFO_OVF) {
            // When fifo overflows, we reset the fifo.
            UART_ENTER_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
            uart_hal_rxfifo_rst(&(uart_context[uart_num].hal));
            UART_EXIT_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
            UART_ENTER_CRITICAL_ISR(&uart_selectlock);
            if (p_uart->uart_select_notif_callback) {
                p_uart->uart_select_notif_callback(uart_num, UART_SELECT_ERROR_NOTIF, &HPTaskAwoken);
            }
            UART_EXIT_CRITICAL_ISR(&uart_selectlock);
            uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_RXFIFO_OVF);
            uart_event.type = UART_FIFO_OVF;
        } else if (uart_intr_status & UART_INTR_BRK_DET) {
            uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_BRK_DET);
            uart_event.type = UART_BREAK;
        } else if (uart_intr_status & UART_INTR_FRAM_ERR) {
            UART_ENTER_CRITICAL_ISR(&uart_selectlock);
            if (p_uart->uart_select_notif_callback) {
                p_uart->uart_select_notif_callback(uart_num, UART_SELECT_ERROR_NOTIF, &HPTaskAwoken);
            }
            UART_EXIT_CRITICAL_ISR(&uart_selectlock);
            uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_FRAM_ERR);
            uart_event.type = UART_FRAME_ERR;
        } else if (uart_intr_status & UART_INTR_PARITY_ERR) {
            UART_ENTER_CRITICAL_ISR(&uart_selectlock);
            if (p_uart->uart_select_notif_callback) {
                p_uart->uart_select_notif_callback(uart_num, UART_SELECT_ERROR_NOTIF, &HPTaskAwoken);
            }
            UART_EXIT_CRITICAL_ISR(&uart_selectlock);
            uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_PARITY_ERR);
            uart_event.type = UART_PARITY_ERR;
        } else if (uart_intr_status & UART_INTR_TX_BRK_DONE) {
            UART_ENTER_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
            uart_hal_tx_break(&(uart_context[uart_num].hal), 0);
            uart_hal_disable_intr_mask(&(uart_context[uart_num].hal), UART_INTR_TX_BRK_DONE);
            if (p_uart->tx_brk_flg == 1) {
                uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), UART_INTR_TXFIFO_EMPTY);
            }
            UART_EXIT_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
            uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_TX_BRK_DONE);
            if (p_uart->tx_brk_flg == 1) {
                p_uart->tx_brk_flg = 0;
                p_uart->tx_waiting_brk = 0;
            } else {
                xSemaphoreGiveFromISR(p_uart->tx_brk_sem, &HPTaskAwoken);
            }
        } else if (uart_intr_status & UART_INTR_TX_BRK_IDLE) {
            UART_ENTER_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
            uart_hal_disable_intr_mask(&(uart_context[uart_num].hal), UART_INTR_TX_BRK_IDLE);
            UART_EXIT_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
            uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_TX_BRK_IDLE);
        } else if (uart_intr_status & UART_INTR_CMD_CHAR_DET) {
            uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_CMD_CHAR_DET);
            uart_event.type = UART_PATTERN_DET;
        } else if ((uart_intr_status & UART_INTR_RS485_PARITY_ERR)
                   || (uart_intr_status & UART_INTR_RS485_FRM_ERR)
                   || (uart_intr_status & UART_INTR_RS485_CLASH)) {
            // RS485 collision or frame error interrupt triggered
            UART_ENTER_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
            uart_hal_rxfifo_rst(&(uart_context[uart_num].hal));
            // Set collision detection flag
            p_uart_obj[uart_num]->coll_det_flg = true;
            UART_EXIT_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
            uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_RS485_CLASH | UART_INTR_RS485_FRM_ERR | UART_INTR_RS485_PARITY_ERR);
            uart_event.type = UART_EVENT_MAX;
        } else if (uart_intr_status & UART_INTR_TX_DONE) {
            if (UART_IS_MODE_SET(uart_num, UART_MODE_RS485_HALF_DUPLEX) && uart_hal_is_tx_idle(&(uart_context[uart_num].hal)) != true) {
                // The TX_DONE interrupt is triggered but transmit is active
                // then postpone interrupt processing for next interrupt
                uart_event.type = UART_EVENT_MAX;
            } else {
                // Workaround for RS485: If the RS485 half duplex mode is active
                // and transmitter is in idle state then reset received buffer and reset RTS pin
                // skip this behavior for other UART modes
                UART_ENTER_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                uart_hal_disable_intr_mask(&(uart_context[uart_num].hal), UART_INTR_TX_DONE);
                if (UART_IS_MODE_SET(uart_num, UART_MODE_RS485_HALF_DUPLEX)) {
                    uart_hal_rxfifo_rst(&(uart_context[uart_num].hal));
                    uart_hal_set_rts(&(uart_context[uart_num].hal), 1);
                }
                UART_EXIT_CRITICAL_ISR(&(uart_context[uart_num].spinlock));
                uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_TX_DONE);
                xSemaphoreGiveFromISR(p_uart_obj[uart_num]->tx_done_sem, &HPTaskAwoken);
            }
        } else {
            uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), uart_intr_status); /*simply clear all other intr status*/
            uart_event.type = UART_EVENT_MAX;
        }

        if (uart_event.type != UART_EVENT_MAX && p_uart->event_queue) {
            if (pdFALSE == xQueueSendFromISR(p_uart->event_queue, (void * )&uart_event, &HPTaskAwoken)) {
#ifndef CONFIG_UART_ISR_IN_IRAM     //Only log if ISR is not in IRAM
                ESP_EARLY_LOGV(UART_TAG, "UART event queue full");
#endif
            }
        }
    }
    if (HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/**************************************************************/
esp_err_t uart_wait_tx_done(uart_port_t uart_num, TickType_t ticks_to_wait)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num]), ESP_FAIL, UART_TAG, "uart driver error");
    BaseType_t res;
    portTickType ticks_start = xTaskGetTickCount();
    //Take tx_mux
    res = xSemaphoreTake(p_uart_obj[uart_num]->tx_mux, (portTickType)ticks_to_wait);
    if (res == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }
    xSemaphoreTake(p_uart_obj[uart_num]->tx_done_sem, 0);
    if (uart_hal_is_tx_idle(&(uart_context[uart_num].hal))) {
        xSemaphoreGive(p_uart_obj[uart_num]->tx_mux);
        return ESP_OK;
    }
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), UART_INTR_TX_DONE);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));

    TickType_t ticks_end = xTaskGetTickCount();
    if (ticks_end - ticks_start > ticks_to_wait) {
        ticks_to_wait = 0;
    } else {
        ticks_to_wait = ticks_to_wait - (ticks_end - ticks_start);
    }
    //take 2nd tx_done_sem, wait given from ISR
    res = xSemaphoreTake(p_uart_obj[uart_num]->tx_done_sem, (portTickType)ticks_to_wait);
    if (res == pdFALSE) {
        // The TX_DONE interrupt will be disabled in ISR
        xSemaphoreGive(p_uart_obj[uart_num]->tx_mux);
        return ESP_ERR_TIMEOUT;
    }
    xSemaphoreGive(p_uart_obj[uart_num]->tx_mux);
    return ESP_OK;
}

int uart_tx_chars(uart_port_t uart_num, const char *buffer, uint32_t len)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), (-1), UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num]), (-1), UART_TAG, "uart driver error");
    ESP_RETURN_ON_FALSE(buffer, (-1), UART_TAG, "buffer null");
    if (len == 0) {
        return 0;
    }
    int tx_len = 0;
    xSemaphoreTake(p_uart_obj[uart_num]->tx_mux, (portTickType)portMAX_DELAY);
    if (UART_IS_MODE_SET(uart_num, UART_MODE_RS485_HALF_DUPLEX)) {
        UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
        uart_hal_set_rts(&(uart_context[uart_num].hal), 0);
        uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), UART_INTR_TX_DONE);
        UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    }
    uart_hal_write_txfifo(&(uart_context[uart_num].hal), (const uint8_t *) buffer, len, (uint32_t *)&tx_len);
    xSemaphoreGive(p_uart_obj[uart_num]->tx_mux);
    return tx_len;
}

static int uart_tx_all(uart_port_t uart_num, const char *src, size_t size, bool brk_en, int brk_len)
{
    if (size == 0) {
        return 0;
    }
    size_t original_size = size;

    //lock for uart_tx
    xSemaphoreTake(p_uart_obj[uart_num]->tx_mux, (portTickType)portMAX_DELAY);
    p_uart_obj[uart_num]->coll_det_flg = false;
    if (p_uart_obj[uart_num]->tx_buf_size > 0) {
        size_t max_size = xRingbufferGetMaxItemSize(p_uart_obj[uart_num]->tx_ring_buf);
        int offset = 0;
        uart_tx_data_t evt;
        evt.tx_data.size = size;
        evt.tx_data.brk_len = brk_len;
        if (brk_en) {
            evt.type = UART_DATA_BREAK;
        } else {
            evt.type = UART_DATA;
        }
        xRingbufferSend(p_uart_obj[uart_num]->tx_ring_buf, (void *) &evt, sizeof(uart_tx_data_t), portMAX_DELAY);
        while (size > 0) {
            size_t send_size = size > max_size / 2 ? max_size / 2 : size;
            xRingbufferSend(p_uart_obj[uart_num]->tx_ring_buf, (void *) (src + offset), send_size, portMAX_DELAY);
            size -= send_size;
            offset += send_size;
            uart_enable_tx_intr(uart_num, 1, UART_EMPTY_THRESH_DEFAULT);
        }
    } else {
        while (size) {
            //semaphore for tx_fifo available
            if (pdTRUE == xSemaphoreTake(p_uart_obj[uart_num]->tx_fifo_sem, (portTickType)portMAX_DELAY)) {
                uint32_t sent = 0;
                if (UART_IS_MODE_SET(uart_num, UART_MODE_RS485_HALF_DUPLEX)) {
                    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
                    uart_hal_set_rts(&(uart_context[uart_num].hal), 0);
                    uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), UART_INTR_TX_DONE);
                    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
                }
                uart_hal_write_txfifo(&(uart_context[uart_num].hal), (const uint8_t *)src, size, &sent);
                if (sent < size) {
                    p_uart_obj[uart_num]->tx_waiting_fifo = true;
                    uart_enable_tx_intr(uart_num, 1, UART_EMPTY_THRESH_DEFAULT);
                }
                size -= sent;
                src += sent;
            }
        }
        if (brk_en) {
            uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_INTR_TX_BRK_DONE);
            UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
            uart_hal_tx_break(&(uart_context[uart_num].hal), brk_len);
            uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), UART_INTR_TX_BRK_DONE);
            UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
            xSemaphoreTake(p_uart_obj[uart_num]->tx_brk_sem, (portTickType)portMAX_DELAY);
        }
        xSemaphoreGive(p_uart_obj[uart_num]->tx_fifo_sem);
    }
    xSemaphoreGive(p_uart_obj[uart_num]->tx_mux);
    return original_size;
}

int uart_write_bytes(uart_port_t uart_num, const void *src, size_t size)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), (-1), UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num] != NULL), (-1), UART_TAG, "uart driver error");
    ESP_RETURN_ON_FALSE(src, (-1), UART_TAG, "buffer null");
    return uart_tx_all(uart_num, src, size, 0, 0);
}

int uart_write_bytes_with_break(uart_port_t uart_num, const void *src, size_t size, int brk_len)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), (-1), UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num]), (-1), UART_TAG, "uart driver error");
    ESP_RETURN_ON_FALSE((size > 0), (-1), UART_TAG, "uart size error");
    ESP_RETURN_ON_FALSE((src), (-1), UART_TAG, "uart data null");
    ESP_RETURN_ON_FALSE((brk_len > 0 && brk_len < 256), (-1), UART_TAG, "break_num error");
    return uart_tx_all(uart_num, src, size, 1, brk_len);
}

static bool uart_check_buf_full(uart_port_t uart_num)
{
    if (p_uart_obj[uart_num]->rx_buffer_full_flg) {
        BaseType_t res = xRingbufferSend(p_uart_obj[uart_num]->rx_ring_buf, p_uart_obj[uart_num]->rx_data_buf, p_uart_obj[uart_num]->rx_stash_len, 1);
        if (res == pdTRUE) {
            UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
            p_uart_obj[uart_num]->rx_buffered_len += p_uart_obj[uart_num]->rx_stash_len;
            p_uart_obj[uart_num]->rx_buffer_full_flg = false;
            UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
            uart_enable_rx_intr(p_uart_obj[uart_num]->uart_num);
            return true;
        }
    }
    return false;
}

int uart_read_bytes(uart_port_t uart_num, void *buf, uint32_t length, TickType_t ticks_to_wait)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), (-1), UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((buf), (-1), UART_TAG, "uart data null");
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num]), (-1), UART_TAG, "uart driver error");
    uint8_t *data = NULL;
    size_t size;
    size_t copy_len = 0;
    int len_tmp;
    if (xSemaphoreTake(p_uart_obj[uart_num]->rx_mux, (portTickType)ticks_to_wait) != pdTRUE) {
        return -1;
    }
    while (length) {
        if (p_uart_obj[uart_num]->rx_cur_remain == 0) {
            data = (uint8_t *) xRingbufferReceive(p_uart_obj[uart_num]->rx_ring_buf, &size, (portTickType) ticks_to_wait);
            if (data) {
                p_uart_obj[uart_num]->rx_head_ptr = data;
                p_uart_obj[uart_num]->rx_ptr = data;
                p_uart_obj[uart_num]->rx_cur_remain = size;
            } else {
                //When using dual cores, `rx_buffer_full_flg` may read and write on different cores at same time,
                //which may lose synchronization. So we also need to call `uart_check_buf_full` once when ringbuffer is empty
                //to solve the possible asynchronous issues.
                if (uart_check_buf_full(uart_num)) {
                    //This condition will never be true if `uart_read_bytes`
                    //and `uart_rx_intr_handler_default` are scheduled on the same core.
                    continue;
                } else {
                    xSemaphoreGive(p_uart_obj[uart_num]->rx_mux);
                    return copy_len;
                }
            }
        }
        if (p_uart_obj[uart_num]->rx_cur_remain > length) {
            len_tmp = length;
        } else {
            len_tmp = p_uart_obj[uart_num]->rx_cur_remain;
        }
        memcpy((uint8_t *)buf + copy_len, p_uart_obj[uart_num]->rx_ptr, len_tmp);
        UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
        p_uart_obj[uart_num]->rx_buffered_len -= len_tmp;
        uart_pattern_queue_update(uart_num, len_tmp);
        p_uart_obj[uart_num]->rx_ptr += len_tmp;
        UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
        p_uart_obj[uart_num]->rx_cur_remain -= len_tmp;
        copy_len += len_tmp;
        length -= len_tmp;
        if (p_uart_obj[uart_num]->rx_cur_remain == 0) {
            vRingbufferReturnItem(p_uart_obj[uart_num]->rx_ring_buf, p_uart_obj[uart_num]->rx_head_ptr);
            p_uart_obj[uart_num]->rx_head_ptr = NULL;
            p_uart_obj[uart_num]->rx_ptr = NULL;
            uart_check_buf_full(uart_num);
        }
    }

    xSemaphoreGive(p_uart_obj[uart_num]->rx_mux);
    return copy_len;
}

esp_err_t uart_get_buffered_data_len(uart_port_t uart_num, size_t *size)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num]), ESP_FAIL, UART_TAG, "uart driver error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    *size = p_uart_obj[uart_num]->rx_buffered_len;
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_flush(uart_port_t uart_num) __attribute__((alias("uart_flush_input")));

static esp_err_t uart_disable_intr_mask_and_return_prev(uart_port_t uart_num, uint32_t disable_mask, uint32_t *prev_mask)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    *prev_mask = uart_hal_get_intr_ena_status(&uart_context[uart_num].hal) & disable_mask;
    uart_hal_disable_intr_mask(&(uart_context[uart_num].hal), disable_mask);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_flush_input(uart_port_t uart_num)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num]), ESP_FAIL, UART_TAG, "uart driver error");
    uart_obj_t *p_uart = p_uart_obj[uart_num];
    uint8_t *data;
    size_t size;
    uint32_t prev_mask;

    //rx sem protect the ring buffer read related functions
    xSemaphoreTake(p_uart->rx_mux, (portTickType)portMAX_DELAY);
    uart_disable_intr_mask_and_return_prev(uart_num, UART_INTR_RXFIFO_FULL | UART_INTR_RXFIFO_TOUT, &prev_mask);
    while (true) {
        if (p_uart->rx_head_ptr) {
            vRingbufferReturnItem(p_uart->rx_ring_buf, p_uart->rx_head_ptr);
            UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
            p_uart_obj[uart_num]->rx_buffered_len -= p_uart->rx_cur_remain;
            uart_pattern_queue_update(uart_num, p_uart->rx_cur_remain);
            UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
            p_uart->rx_ptr = NULL;
            p_uart->rx_cur_remain = 0;
            p_uart->rx_head_ptr = NULL;
        }
        data = (uint8_t*) xRingbufferReceive(p_uart->rx_ring_buf, &size, (portTickType) 0);
        if(data == NULL) {
            bool error = false;
            UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
            if( p_uart_obj[uart_num]->rx_buffered_len != 0 ) {
                p_uart_obj[uart_num]->rx_buffered_len = 0;
                error = true;
            }
            //We also need to clear the `rx_buffer_full_flg` here.
            p_uart_obj[uart_num]->rx_buffer_full_flg = false;
            UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
            if (error) {
                // this must be called outside the critical section
                ESP_LOGE(UART_TAG, "rx_buffered_len error");
            }
            break;
        }
        UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
        p_uart_obj[uart_num]->rx_buffered_len -= size;
        uart_pattern_queue_update(uart_num, size);
        UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
        vRingbufferReturnItem(p_uart->rx_ring_buf, data);
        if (p_uart_obj[uart_num]->rx_buffer_full_flg) {
            BaseType_t res = xRingbufferSend(p_uart_obj[uart_num]->rx_ring_buf, p_uart_obj[uart_num]->rx_data_buf, p_uart_obj[uart_num]->rx_stash_len, 1);
            if (res == pdTRUE) {
                UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
                p_uart_obj[uart_num]->rx_buffered_len += p_uart_obj[uart_num]->rx_stash_len;
                p_uart_obj[uart_num]->rx_buffer_full_flg = false;
                UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
            }
        }
    }
    p_uart->rx_ptr = NULL;
    p_uart->rx_cur_remain = 0;
    p_uart->rx_head_ptr = NULL;
    uart_hal_rxfifo_rst(&(uart_context[uart_num].hal));
    uart_enable_intr_mask(uart_num, prev_mask);
    xSemaphoreGive(p_uart->rx_mux);
    return ESP_OK;
}

static void uart_free_driver_obj(uart_obj_t *uart_obj)
{
    if (uart_obj->tx_fifo_sem) {
        vSemaphoreDelete(uart_obj->tx_fifo_sem);
    }
    if (uart_obj->tx_done_sem) {
        vSemaphoreDelete(uart_obj->tx_done_sem);
    }
    if (uart_obj->tx_brk_sem) {
        vSemaphoreDelete(uart_obj->tx_brk_sem);
    }
    if (uart_obj->tx_mux) {
        vSemaphoreDelete(uart_obj->tx_mux);
    }
    if (uart_obj->rx_mux) {
        vSemaphoreDelete(uart_obj->rx_mux);
    }
    if (uart_obj->event_queue) {
        vQueueDelete(uart_obj->event_queue);
    }
    if (uart_obj->rx_ring_buf) {
        vRingbufferDelete(uart_obj->rx_ring_buf);
    }
    if (uart_obj->tx_ring_buf) {
        vRingbufferDelete(uart_obj->tx_ring_buf);
    }
#if CONFIG_UART_ISR_IN_IRAM
    free(uart_obj->event_queue_storage);
    free(uart_obj->event_queue_struct);
    free(uart_obj->tx_ring_buf_storage);
    free(uart_obj->tx_ring_buf_struct);
    free(uart_obj->rx_ring_buf_storage);
    free(uart_obj->rx_ring_buf_struct);
    free(uart_obj->rx_mux_struct);
    free(uart_obj->tx_mux_struct);
    free(uart_obj->tx_brk_sem_struct);
    free(uart_obj->tx_done_sem_struct);
    free(uart_obj->tx_fifo_sem_struct);
#endif
    free(uart_obj);
}

static uart_obj_t *uart_alloc_driver_obj(int event_queue_size, int tx_buffer_size, int rx_buffer_size)
{
    uart_obj_t *uart_obj = heap_caps_calloc(1, sizeof(uart_obj_t), UART_MALLOC_CAPS);
    if (!uart_obj) {
        return NULL;
    }
#if CONFIG_UART_ISR_IN_IRAM
    if (event_queue_size > 0) {
        uart_obj->event_queue_storage = heap_caps_calloc(event_queue_size, sizeof(uart_event_t), UART_MALLOC_CAPS);
        uart_obj->event_queue_struct = heap_caps_calloc(1, sizeof(StaticQueue_t), UART_MALLOC_CAPS);
        if (!uart_obj->event_queue_storage || !uart_obj->event_queue_struct) {
            goto err;
        }
    }
    if (tx_buffer_size > 0) {
        uart_obj->tx_ring_buf_storage = heap_caps_calloc(1, tx_buffer_size, UART_MALLOC_CAPS);
        uart_obj->tx_ring_buf_struct = heap_caps_calloc(1, sizeof(StaticRingbuffer_t), UART_MALLOC_CAPS);
        if (!uart_obj->tx_ring_buf_storage || !uart_obj->tx_ring_buf_struct) {
            goto err;
        }
    }
    uart_obj->rx_ring_buf_storage = heap_caps_calloc(1, rx_buffer_size, UART_MALLOC_CAPS);
    uart_obj->rx_ring_buf_struct = heap_caps_calloc(1, sizeof(StaticRingbuffer_t), UART_MALLOC_CAPS);
    uart_obj->rx_mux_struct = heap_caps_calloc(1, sizeof(StaticSemaphore_t), UART_MALLOC_CAPS);
    uart_obj->tx_mux_struct = heap_caps_calloc(1, sizeof(StaticSemaphore_t), UART_MALLOC_CAPS);
    uart_obj->tx_brk_sem_struct = heap_caps_calloc(1, sizeof(StaticSemaphore_t), UART_MALLOC_CAPS);
    uart_obj->tx_done_sem_struct = heap_caps_calloc(1, sizeof(StaticSemaphore_t), UART_MALLOC_CAPS);
    uart_obj->tx_fifo_sem_struct = heap_caps_calloc(1, sizeof(StaticSemaphore_t), UART_MALLOC_CAPS);
    if (!uart_obj->rx_ring_buf_storage || !uart_obj->rx_ring_buf_struct || !uart_obj->rx_mux_struct ||
            !uart_obj->tx_mux_struct || !uart_obj->tx_brk_sem_struct || !uart_obj->tx_done_sem_struct ||
            !uart_obj->tx_fifo_sem_struct) {
        goto err;
    }
    if (event_queue_size > 0) {
        uart_obj->event_queue = xQueueCreateStatic(event_queue_size, sizeof(uart_event_t),
                                uart_obj->event_queue_storage, uart_obj->event_queue_struct);
        if (!uart_obj->event_queue) {
            goto err;
        }
    }
    if (tx_buffer_size > 0) {
        uart_obj->tx_ring_buf = xRingbufferCreateStatic(tx_buffer_size, RINGBUF_TYPE_NOSPLIT,
                                uart_obj->tx_ring_buf_storage, uart_obj->tx_ring_buf_struct);
        if (!uart_obj->tx_ring_buf) {
            goto err;
        }
    }
    uart_obj->rx_ring_buf = xRingbufferCreateStatic(rx_buffer_size, RINGBUF_TYPE_BYTEBUF,
                            uart_obj->rx_ring_buf_storage, uart_obj->rx_ring_buf_struct);
    uart_obj->rx_mux = xSemaphoreCreateMutexStatic(uart_obj->rx_mux_struct);
    uart_obj->tx_mux = xSemaphoreCreateMutexStatic(uart_obj->tx_mux_struct);
    uart_obj->tx_brk_sem = xSemaphoreCreateBinaryStatic(uart_obj->tx_brk_sem_struct);
    uart_obj->tx_done_sem = xSemaphoreCreateBinaryStatic(uart_obj->tx_done_sem_struct);
    uart_obj->tx_fifo_sem = xSemaphoreCreateBinaryStatic(uart_obj->tx_fifo_sem_struct);
    if (!uart_obj->rx_ring_buf || !uart_obj->rx_mux || !uart_obj->tx_mux || !uart_obj->tx_brk_sem ||
            !uart_obj->tx_done_sem || !uart_obj->tx_fifo_sem) {
        goto err;
    }
#else
    if (event_queue_size > 0) {
        uart_obj->event_queue = xQueueCreate(event_queue_size, sizeof(uart_event_t));
        if (!uart_obj->event_queue) {
            goto err;
        }
    }
    if (tx_buffer_size > 0) {
        uart_obj->tx_ring_buf = xRingbufferCreate(tx_buffer_size, RINGBUF_TYPE_NOSPLIT);
        if (!uart_obj->tx_ring_buf) {
            goto err;
        }
    }
    uart_obj->rx_ring_buf = xRingbufferCreate(rx_buffer_size, RINGBUF_TYPE_BYTEBUF);
    uart_obj->tx_mux = xSemaphoreCreateMutex();
    uart_obj->rx_mux = xSemaphoreCreateMutex();
    uart_obj->tx_brk_sem = xSemaphoreCreateBinary();
    uart_obj->tx_done_sem = xSemaphoreCreateBinary();
    uart_obj->tx_fifo_sem = xSemaphoreCreateBinary();
    if (!uart_obj->rx_ring_buf || !uart_obj->rx_mux || !uart_obj->tx_mux || !uart_obj->tx_brk_sem ||
            !uart_obj->tx_done_sem || !uart_obj->tx_fifo_sem) {
        goto err;
    }
#endif
    return uart_obj;

err:
    uart_free_driver_obj(uart_obj);
    return NULL;
}

esp_err_t uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int event_queue_size, QueueHandle_t *uart_queue, int intr_alloc_flags)
{
    esp_err_t r;
#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    ESP_RETURN_ON_FALSE((uart_num != CONFIG_ESP_CONSOLE_UART_NUM), ESP_FAIL, UART_TAG, "UART used by GDB-stubs! Please disable GDB in menuconfig.");
#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((rx_buffer_size > SOC_UART_FIFO_LEN), ESP_FAIL, UART_TAG, "uart rx buffer length error");
    ESP_RETURN_ON_FALSE((tx_buffer_size > SOC_UART_FIFO_LEN) || (tx_buffer_size == 0), ESP_FAIL, UART_TAG, "uart tx buffer length error");
#if CONFIG_UART_ISR_IN_IRAM
    if ((intr_alloc_flags & ESP_INTR_FLAG_IRAM) == 0) {
        ESP_LOGI(UART_TAG, "ESP_INTR_FLAG_IRAM flag not set while CONFIG_UART_ISR_IN_IRAM is enabled, flag updated");
        intr_alloc_flags |= ESP_INTR_FLAG_IRAM;
    }
#else
    if ((intr_alloc_flags & ESP_INTR_FLAG_IRAM) != 0) {
        ESP_LOGW(UART_TAG, "ESP_INTR_FLAG_IRAM flag is set while CONFIG_UART_ISR_IN_IRAM is not enabled, flag updated");
        intr_alloc_flags &= ~ESP_INTR_FLAG_IRAM;
    }
#endif

    if (p_uart_obj[uart_num] == NULL) {
        p_uart_obj[uart_num] = uart_alloc_driver_obj(event_queue_size, tx_buffer_size, rx_buffer_size);
        if (p_uart_obj[uart_num] == NULL) {
            ESP_LOGE(UART_TAG, "UART driver malloc error");
            return ESP_FAIL;
        }
        p_uart_obj[uart_num]->uart_num = uart_num;
        p_uart_obj[uart_num]->uart_mode = UART_MODE_UART;
        p_uart_obj[uart_num]->coll_det_flg = false;
        p_uart_obj[uart_num]->rx_always_timeout_flg = false;
        p_uart_obj[uart_num]->event_queue_size = event_queue_size;
        p_uart_obj[uart_num]->tx_ptr = NULL;
        p_uart_obj[uart_num]->tx_head = NULL;
        p_uart_obj[uart_num]->tx_len_tot = 0;
        p_uart_obj[uart_num]->tx_brk_flg = 0;
        p_uart_obj[uart_num]->tx_brk_len = 0;
        p_uart_obj[uart_num]->tx_waiting_brk = 0;
        p_uart_obj[uart_num]->rx_buffered_len = 0;
        p_uart_obj[uart_num]->rx_buffer_full_flg = false;
        p_uart_obj[uart_num]->tx_waiting_fifo = false;
        p_uart_obj[uart_num]->rx_ptr = NULL;
        p_uart_obj[uart_num]->rx_cur_remain = 0;
        p_uart_obj[uart_num]->rx_head_ptr = NULL;
        p_uart_obj[uart_num]->tx_buf_size = tx_buffer_size;
        p_uart_obj[uart_num]->uart_select_notif_callback = NULL;
        xSemaphoreGive(p_uart_obj[uart_num]->tx_fifo_sem);
        uart_pattern_queue_reset(uart_num, UART_PATTERN_DET_QLEN_DEFAULT);
        if (uart_queue) {
            *uart_queue = p_uart_obj[uart_num]->event_queue;
            ESP_LOGI(UART_TAG, "queue free spaces: %d", uxQueueSpacesAvailable(p_uart_obj[uart_num]->event_queue));
        }
    } else {
        ESP_LOGE(UART_TAG, "UART driver already installed");
        return ESP_FAIL;
    }

    uart_intr_config_t uart_intr = {
        .intr_enable_mask = UART_INTR_CONFIG_FLAG,
        .rxfifo_full_thresh = UART_FULL_THRESH_DEFAULT,
        .rx_timeout_thresh = UART_TOUT_THRESH_DEFAULT,
        .txfifo_empty_intr_thresh = UART_EMPTY_THRESH_DEFAULT,
    };
    uart_module_enable(uart_num);
    uart_hal_disable_intr_mask(&(uart_context[uart_num].hal), UART_LL_INTR_MASK);
    uart_hal_clr_intsts_mask(&(uart_context[uart_num].hal), UART_LL_INTR_MASK);
    r = uart_isr_register(uart_num, uart_rx_intr_handler_default, p_uart_obj[uart_num], intr_alloc_flags, &p_uart_obj[uart_num]->intr_handle);
    if (r != ESP_OK) {
        goto err;
    }
    r = uart_intr_config(uart_num, &uart_intr);
    if (r != ESP_OK) {
        goto err;
    }
    return r;

err:
    uart_driver_delete(uart_num);
    return r;
}

//Make sure no other tasks are still using UART before you call this function
esp_err_t uart_driver_delete(uart_port_t uart_num)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_FAIL, UART_TAG, "uart_num error");
    if (p_uart_obj[uart_num] == NULL) {
        ESP_LOGI(UART_TAG, "ALREADY NULL");
        return ESP_OK;
    }
    esp_intr_free(p_uart_obj[uart_num]->intr_handle);
    uart_disable_rx_intr(uart_num);
    uart_disable_tx_intr(uart_num);
    uart_pattern_link_free(uart_num);
    uart_free_driver_obj(p_uart_obj[uart_num]);
    p_uart_obj[uart_num] = NULL;

#if SOC_UART_SUPPORT_RTC_CLK
    uart_sclk_t sclk = 0;
    uart_hal_get_sclk(&(uart_context[uart_num].hal), &sclk);
    if (sclk == UART_SCLK_RTC) {
        rtc_clk_disable(uart_num);
    }
#endif
    uart_module_disable(uart_num);
    return ESP_OK;
}

bool uart_is_driver_installed(uart_port_t uart_num)
{
    return uart_num < UART_NUM_MAX && (p_uart_obj[uart_num] != NULL);
}

void uart_set_select_notif_callback(uart_port_t uart_num, uart_select_notif_callback_t uart_select_notif_callback)
{
    if (uart_num < UART_NUM_MAX && p_uart_obj[uart_num]) {
        p_uart_obj[uart_num]->uart_select_notif_callback = (uart_select_notif_callback_t) uart_select_notif_callback;
    }
}

portMUX_TYPE *uart_get_selectlock(void)
{
    return &uart_selectlock;
}

// Set UART mode
esp_err_t uart_set_mode(uart_port_t uart_num, uart_mode_t mode)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_ERR_INVALID_ARG, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num]), ESP_ERR_INVALID_STATE, UART_TAG, "uart driver error");
    if ((mode == UART_MODE_RS485_COLLISION_DETECT) || (mode == UART_MODE_RS485_APP_CTRL)
            || (mode == UART_MODE_RS485_HALF_DUPLEX)) {
        ESP_RETURN_ON_FALSE((!uart_hal_is_hw_rts_en(&(uart_context[uart_num].hal))), ESP_ERR_INVALID_ARG, UART_TAG,
                            "disable hw flowctrl before using RS485 mode");
    }
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_mode(&(uart_context[uart_num].hal), mode);
    if (mode ==  UART_MODE_RS485_COLLISION_DETECT) {
        // This mode allows read while transmitting that allows collision detection
        p_uart_obj[uart_num]->coll_det_flg = false;
        // Enable collision detection interrupts
        uart_hal_ena_intr_mask(&(uart_context[uart_num].hal), UART_INTR_RXFIFO_TOUT
                               | UART_INTR_RXFIFO_FULL
                               | UART_INTR_RS485_CLASH
                               | UART_INTR_RS485_FRM_ERR
                               | UART_INTR_RS485_PARITY_ERR);
    }
    p_uart_obj[uart_num]->uart_mode = mode;
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_set_rx_full_threshold(uart_port_t uart_num, int threshold)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_ERR_INVALID_ARG, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((threshold < UART_RXFIFO_FULL_THRHD_V) && (threshold > 0), ESP_ERR_INVALID_ARG, UART_TAG,
                        "rx fifo full threshold value error");
    if (p_uart_obj[uart_num] == NULL) {
        ESP_LOGE(UART_TAG, "call uart_driver_install API first");
        return ESP_ERR_INVALID_STATE;
    }
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    if (uart_hal_get_intr_ena_status(&(uart_context[uart_num].hal)) & UART_INTR_RXFIFO_FULL) {
        uart_hal_set_rxfifo_full_thr(&(uart_context[uart_num].hal), threshold);
    }
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_set_tx_empty_threshold(uart_port_t uart_num, int threshold)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_ERR_INVALID_ARG, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((threshold < UART_TXFIFO_EMPTY_THRHD_V) && (threshold > 0), ESP_ERR_INVALID_ARG, UART_TAG,
                        "tx fifo empty threshold value error");
    if (p_uart_obj[uart_num] == NULL) {
        ESP_LOGE(UART_TAG, "call uart_driver_install API first");
        return ESP_ERR_INVALID_STATE;
    }
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    if (uart_hal_get_intr_ena_status(&(uart_context[uart_num].hal)) & UART_INTR_TXFIFO_EMPTY) {
        uart_hal_set_txfifo_empty_thr(&(uart_context[uart_num].hal), threshold);
    }
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_set_rx_timeout(uart_port_t uart_num, const uint8_t tout_thresh)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_ERR_INVALID_ARG, UART_TAG, "uart_num error");
    // get maximum timeout threshold
    uint16_t tout_max_thresh = uart_hal_get_max_rx_timeout_thrd(&(uart_context[uart_num].hal));
    if (tout_thresh > tout_max_thresh) {
        ESP_LOGE(UART_TAG, "tout_thresh = %d > maximum value = %d", tout_thresh, tout_max_thresh);
        return ESP_ERR_INVALID_ARG;
    }
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_rx_timeout(&(uart_context[uart_num].hal), tout_thresh);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_get_collision_flag(uart_port_t uart_num, bool *collision_flag)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_ERR_INVALID_ARG, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((p_uart_obj[uart_num]), ESP_FAIL, UART_TAG, "uart driver error");
    ESP_RETURN_ON_FALSE((collision_flag != NULL), ESP_ERR_INVALID_ARG, UART_TAG, "wrong parameter pointer");
    ESP_RETURN_ON_FALSE((UART_IS_MODE_SET(uart_num, UART_MODE_RS485_HALF_DUPLEX) || UART_IS_MODE_SET(uart_num, UART_MODE_RS485_COLLISION_DETECT)),
                        ESP_ERR_INVALID_ARG, UART_TAG, "wrong mode");
    *collision_flag = p_uart_obj[uart_num]->coll_det_flg;
    return ESP_OK;
}

esp_err_t uart_set_wakeup_threshold(uart_port_t uart_num, int wakeup_threshold)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_ERR_INVALID_ARG, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((wakeup_threshold <= UART_ACTIVE_THRESHOLD_V && wakeup_threshold > UART_MIN_WAKEUP_THRESH), ESP_ERR_INVALID_ARG, UART_TAG,
                        "wakeup_threshold out of bounds");
    UART_ENTER_CRITICAL(&(uart_context[uart_num].spinlock));
    uart_hal_set_wakeup_thrd(&(uart_context[uart_num].hal), wakeup_threshold);
    UART_EXIT_CRITICAL(&(uart_context[uart_num].spinlock));
    return ESP_OK;
}

esp_err_t uart_get_wakeup_threshold(uart_port_t uart_num, int *out_wakeup_threshold)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_ERR_INVALID_ARG, UART_TAG, "uart_num error");
    ESP_RETURN_ON_FALSE((out_wakeup_threshold != NULL), ESP_ERR_INVALID_ARG, UART_TAG, "argument is NULL");
    uart_hal_get_wakeup_thrd(&(uart_context[uart_num].hal), (uint32_t *)out_wakeup_threshold);
    return ESP_OK;
}

esp_err_t uart_wait_tx_idle_polling(uart_port_t uart_num)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_ERR_INVALID_ARG, UART_TAG, "uart_num error");
    while (!uart_hal_is_tx_idle(&(uart_context[uart_num].hal)));
    return ESP_OK;
}

esp_err_t uart_set_loop_back(uart_port_t uart_num, bool loop_back_en)
{
    ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), ESP_ERR_INVALID_ARG, UART_TAG, "uart_num error");
    uart_hal_set_loop_back(&(uart_context[uart_num].hal), loop_back_en);
    return ESP_OK;
}

void uart_set_always_rx_timeout(uart_port_t uart_num, bool always_rx_timeout)
{
    uint16_t rx_tout = uart_hal_get_rx_tout_thr(&(uart_context[uart_num].hal));
    if (rx_tout) {
        p_uart_obj[uart_num]->rx_always_timeout_flg = always_rx_timeout;
    } else {
        p_uart_obj[uart_num]->rx_always_timeout_flg = false;
    }
}
