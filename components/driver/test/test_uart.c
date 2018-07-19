#include <string.h>
#include "unity.h"
#include "driver/uart.h"
#include "esp_log.h"

#define UART_TAG         "Uart"
#define UART_NUM1        (UART_NUM_1)
#define BUF_SIZE         (100)
#define UART1_RX_PIN     (22)
#define UART1_TX_PIN     (23)
#define UART_BAUD_11520  (11520)
#define UART_BAUD_115200 (115200)
#define TOLERANCE        (0.02)    //baud rate error tolerance 2%.

#define UART_TOLERANCE_CHECK(val, uper_limit, lower_limit)   ( (val) <= (uper_limit) && (val) >= (lower_limit) )

static void uart_config(uint32_t baud_rate, bool use_ref_tick)
{
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .use_ref_tick = use_ref_tick,
    };
    uart_param_config(UART_NUM1, &uart_config);
    uart_set_pin(UART_NUM1, UART1_TX_PIN, UART1_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM1, BUF_SIZE * 2, BUF_SIZE * 2, 20, NULL, 0);
}

TEST_CASE("test uart get baud-rate","[uart]")
{
    uint32_t baud_rate1 = 0;
    uint32_t baud_rate2 = 0;
    printf("init uart%d, use reftick, baud rate : %d\n", (int)UART_NUM1, (int)UART_BAUD_11520);
    uart_config(UART_BAUD_11520, true);
    uart_get_baudrate(UART_NUM1, &baud_rate1);
    printf("init uart%d, unuse reftick, baud rate : %d\n", (int)UART_NUM1, (int)UART_BAUD_115200);
    uart_config(UART_BAUD_115200, false);
    uart_get_baudrate(UART_NUM1, &baud_rate2);
    printf("get  baud rate when use reftick: %d\n", (int)baud_rate1);
    printf("get  baud rate when don't use reftick: %d\n", (int)baud_rate2);
    uart_driver_delete(UART_NUM1);
    TEST_ASSERT(UART_TOLERANCE_CHECK(baud_rate1, (1.0 + TOLERANCE)*UART_BAUD_11520, (1.0 - TOLERANCE)*UART_BAUD_11520))
    TEST_ASSERT(UART_TOLERANCE_CHECK(baud_rate2, (1.0 + TOLERANCE)*UART_BAUD_115200, (1.0 - TOLERANCE)*UART_BAUD_115200))
    ESP_LOGI(UART_TAG, "get baud-rate test passed  ....\n");
}

TEST_CASE("test uart tx data with break","[uart]")
{
    const int buf_len = 200;
    const int send_len = 128;
    const int brk_len = 10;
    char *psend = (char *)malloc(buf_len);
    TEST_ASSERT( psend != NULL);
    memset(psend, '0', buf_len);
    uart_config(UART_BAUD_115200, false);
    printf("Uart%d send %d bytes with break\n", UART_NUM1, send_len);
    uart_write_bytes_with_break(UART_NUM1, (const char *)psend, send_len, brk_len);
    uart_wait_tx_done(UART_NUM1, (portTickType)portMAX_DELAY);
    //If the code is running here, it means the test passed, otherwise it will crash due to the interrupt wdt timeout.
    printf("Send data with break test passed\n");
    free(psend);
}

