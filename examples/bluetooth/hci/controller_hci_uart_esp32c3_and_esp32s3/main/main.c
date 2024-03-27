/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include "esp_private/periph_ctrl.h" // for enabling UHCI module, remove it after UHCI driver is released
#include "driver/gpio.h"
#include "driver/uart.h"
#include "soc/lldesc.h"
#include "esp_private/gdma.h"
#include "hal/uhci_ll.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_log.h"

static const char *tag = "UHCI";

#define UART_HCI_NUM       (1)

#define UART_RX_THRS       (120)

#define GPIO_UART_TXD_OUT  (4)
#define GPIO_UART_RXD_IN   (5)
#define GPIO_UART_RTS_OUT  (6)
#define GPIO_UART_CTS_IN   (7)

#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_UART_TXD_OUT) | (1ULL<<GPIO_UART_RTS_OUT))
#define GPIO_INPUT_PIN_SEL   ((1ULL<<GPIO_UART_RXD_IN) | (1ULL<<GPIO_UART_CTS_IN))

// Operation functions for HCI UART Transport Layer
static bool hci_uart_tl_init(void);
static void hci_uart_tl_deinit(void);
static void hci_uart_tl_recv_async(uint8_t *buf, uint32_t size, esp_bt_hci_tl_callback_t callback, void *arg);
static void hci_uart_tl_send_async(uint8_t *buf, uint32_t size, esp_bt_hci_tl_callback_t callback, void *arg);
static void hci_uart_tl_flow_on(void);
static bool hci_uart_tl_flow_off(void);
static void hci_uart_tl_finish_transfers(void);

struct uart_txrxchannel {
    esp_bt_hci_tl_callback_t callback;
    void *arg;
    lldesc_t link;
};

struct uart_env_tag {
    struct uart_txrxchannel tx;
    struct uart_txrxchannel rx;
};

struct uart_env_tag uart_env;

static volatile uhci_dev_t *s_uhci_hw = &UHCI0;
static gdma_channel_handle_t s_rx_channel;
static gdma_channel_handle_t s_tx_channel;

static esp_bt_hci_tl_t s_hci_uart_tl_funcs = {
    ._magic = ESP_BT_HCI_TL_MAGIC_VALUE,
    ._version = ESP_BT_HCI_TL_VERSION,
    ._reserved = 0,
    ._open = (void *)hci_uart_tl_init,
    ._close = (void *)hci_uart_tl_deinit,
    ._finish_transfers = (void *)hci_uart_tl_finish_transfers,
    ._recv = (void *)hci_uart_tl_recv_async,
    ._send = (void *)hci_uart_tl_send_async,
    ._flow_on = (void *)hci_uart_tl_flow_on,
    ._flow_off = (void *)hci_uart_tl_flow_off,
};

static bool hci_uart_tl_init(void)
{
    return true;
}

static void hci_uart_tl_deinit(void)
{
}

static IRAM_ATTR void hci_uart_tl_recv_async(uint8_t *buf, uint32_t size, esp_bt_hci_tl_callback_t callback, void *arg)
{
    assert(buf != NULL);
    assert(size != 0);
    assert(callback != NULL);
    uart_env.rx.callback = callback;
    uart_env.rx.arg = arg;

    memset(&uart_env.rx.link, 0, sizeof(lldesc_t));
    uart_env.rx.link.buf = buf;
    uart_env.rx.link.size = size;

    s_uhci_hw->pkt_thres.thrs = size;

    gdma_start(s_rx_channel, (intptr_t)(&uart_env.rx.link));
}

static IRAM_ATTR void hci_uart_tl_send_async(uint8_t *buf, uint32_t size, esp_bt_hci_tl_callback_t callback, void *arg)
{
    assert(buf != NULL);
    assert(size != 0);
    assert(callback != NULL);

    uart_env.tx.callback = callback;
    uart_env.tx.arg = arg;

    memset(&uart_env.tx.link, 0, sizeof(lldesc_t));
    uart_env.tx.link.length = size;
    uart_env.tx.link.buf = buf;
    uart_env.tx.link.eof = 1;

    gdma_start(s_tx_channel, (intptr_t)(&uart_env.tx.link));
}

static void hci_uart_tl_flow_on(void)
{
}

static bool hci_uart_tl_flow_off(void)
{
    return true;
}

static void hci_uart_tl_finish_transfers(void)
{
}

static IRAM_ATTR bool hci_uart_tl_rx_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    assert(dma_chan == s_rx_channel);
    assert(uart_env.rx.callback != NULL);
    esp_bt_hci_tl_callback_t callback = uart_env.rx.callback;
    void *arg = uart_env.rx.arg;

    // clear callback pointer
    uart_env.rx.callback = NULL;
    uart_env.rx.arg = NULL;

    // call handler
    callback(arg, ESP_BT_HCI_TL_STATUS_OK);

    // send notification to Bluetooth Controller task
    esp_bt_h4tl_eif_io_event_notify(1);

    return true;
}

static IRAM_ATTR bool hci_uart_tl_tx_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    assert(dma_chan == s_tx_channel);
    assert(uart_env.tx.callback != NULL);
    esp_bt_hci_tl_callback_t callback = uart_env.tx.callback;
    void *arg = uart_env.tx.arg;

    // clear callback pointer
    uart_env.tx.callback = NULL;
    uart_env.tx.arg = NULL;

    // call handler
    callback(arg, ESP_BT_HCI_TL_STATUS_OK);

    // send notification to Bluetooth Controller task
    esp_bt_h4tl_eif_io_event_notify(1);

    return true;
}

static void uart_gpio_set(void)
{
    gpio_config_t io_output_conf = {
        .intr_type = GPIO_INTR_DISABLE,    //disable interrupt
        .mode = GPIO_MODE_OUTPUT,    // output mode
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,    // bit mask of the output pins
        .pull_down_en = 0,    // disable pull-down mode
        .pull_up_en = 0,    // disable pull-up mode
    };
    gpio_config(&io_output_conf);

    gpio_config_t io_input_conf = {
        .intr_type = GPIO_INTR_DISABLE,    //disable interrupt
        .mode = GPIO_MODE_INPUT,    // input mode
        .pin_bit_mask = GPIO_INPUT_PIN_SEL,  // bit mask of the input pins
        .pull_down_en = 0,    // disable pull-down mode
        .pull_up_en = 0,    // disable pull-down mode
    };
    gpio_config(&io_input_conf);

    uart_set_pin(UART_HCI_NUM, GPIO_UART_TXD_OUT, GPIO_UART_RXD_IN, GPIO_UART_RTS_OUT, GPIO_UART_CTS_IN);
}

void uhci_uart_install(void)
{
    periph_module_enable(PERIPH_UHCI0_MODULE);
    periph_module_reset(PERIPH_UHCI0_MODULE);

    periph_module_enable(PERIPH_UART1_MODULE);
    periph_module_reset(PERIPH_UART1_MODULE);

    uart_gpio_set();

    // configure UART1
    uart_config_t uart_config = {
        .baud_rate = CONFIG_EXAMPLE_HCI_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
#ifdef CONFIG_EXAMPLE_HCI_UART_FLOW_CTRL_ENABLE
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
#else
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
#endif
        .rx_flow_ctrl_thresh = UART_RX_THRS,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_HCI_NUM, &uart_config));

    // install DMA driver
    gdma_channel_alloc_config_t tx_channel_config = {
        .flags.reserve_sibling = 1,
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };
    ESP_ERROR_CHECK(gdma_new_channel(&tx_channel_config, &s_tx_channel));
    gdma_channel_alloc_config_t rx_channel_config = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
        .sibling_chan = s_tx_channel,
    };
    ESP_ERROR_CHECK(gdma_new_channel(&rx_channel_config, &s_rx_channel));

    gdma_connect(s_tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_UHCI, 0));
    gdma_connect(s_rx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_UHCI, 0));

    gdma_strategy_config_t strategy_config = {
        .auto_update_desc = false,
        .owner_check = false
    };
    gdma_apply_strategy(s_tx_channel, &strategy_config);
    gdma_apply_strategy(s_rx_channel, &strategy_config);

    gdma_rx_event_callbacks_t rx_cbs = {
        .on_recv_eof = hci_uart_tl_rx_eof_callback
    };
    gdma_register_rx_event_callbacks(s_rx_channel, &rx_cbs, NULL);

    gdma_tx_event_callbacks_t tx_cbs = {
        .on_trans_eof = hci_uart_tl_tx_eof_callback
    };
    gdma_register_tx_event_callbacks(s_tx_channel, &tx_cbs, NULL);

    // configure UHCI
    uhci_ll_init(s_uhci_hw);
    uhci_ll_set_eof_mode(s_uhci_hw, UHCI_RX_LEN_EOF);
    // disable software flow control
    s_uhci_hw->escape_conf.val = 0;
    uhci_ll_attach_uart_port(s_uhci_hw, 1);
}


void app_main(void)
{
    esp_err_t ret;

    /* Initialize NVS — it is used to store PHY calibration data */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    uhci_uart_install();

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    bt_cfg.hci_tl_funcs = &s_hci_uart_tl_funcs;

    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Bluetooth Controller initialize failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Bluetooth Controller initialize failed: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(tag, "HCI messages can be communicated over UART%d:\n"
             "--PINs: TxD %d, RxD %d, RTS %d, CTS %d\n"
             "--Baudrate: %d", UART_HCI_NUM,
             GPIO_UART_TXD_OUT, GPIO_UART_RXD_IN, GPIO_UART_RTS_OUT, GPIO_UART_CTS_IN,
             CONFIG_EXAMPLE_HCI_UART_BAUDRATE);

}
