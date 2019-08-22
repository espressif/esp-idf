#include <string.h>
#include <sys/param.h>
#include "unity.h"
#include "test_utils.h"             // unity_send_signal
#include "driver/uart.h"            // for the uart driver access
#include "esp_log.h"
#include "esp_system.h"             // for uint32_t esp_random()

#define UART_TAG         "Uart"
#define UART_NUM1        (UART_NUM_1)
#define BUF_SIZE         (100)
#define UART1_RX_PIN     (22)
#define UART1_TX_PIN     (23)
#define UART_BAUD_11520  (11520)
#define UART_BAUD_115200 (115200)
#define TOLERANCE        (0.02)    //baud rate error tolerance 2%.

#define UART_TOLERANCE_CHECK(val, uper_limit, lower_limit)   ( (val) <= (uper_limit) && (val) >= (lower_limit) )

// RTS for RS485 Half-Duplex Mode manages DE/~RE
#define UART1_RTS_PIN    (18)

// Number of packets to be send during test
#define PACKETS_NUMBER  (10)

// Wait timeout for uart driver
#define PACKET_READ_TICS    (1000 / portTICK_RATE_MS)

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

static volatile bool exit_flag;

static void test_task(void *pvParameters)
{
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;
    char* data = (char *) malloc(256);

    while (exit_flag == false) {
        uart_tx_chars(UART_NUM1, data, 256);
        // The uart_wait_tx_done() function does not block anything if ticks_to_wait = 0.
        uart_wait_tx_done(UART_NUM1, 0);
    }

    free(data);
    xSemaphoreGive(*sema);
    vTaskDelete(NULL);
}

static void test_task2(void *pvParameters)
{
    while (exit_flag == false) {
        // This task obstruct a setting tx_done_sem semaphore in the UART interrupt.
        // It leads to waiting the ticks_to_wait time in uart_wait_tx_done() function.
        uart_disable_intr_mask(UART_NUM1, UART_TX_DONE_INT_ENA_M);
    }
    vTaskDelete(NULL);
}

TEST_CASE("test uart_wait_tx_done is not blocked when ticks_to_wait=0", "[uart]")
{
    uart_config(UART_BAUD_11520, false);

    xSemaphoreHandle exit_sema = xSemaphoreCreateBinary();
    exit_flag = false;

    xTaskCreate(test_task,  "tsk1", 2048, &exit_sema, 5, NULL);
    xTaskCreate(test_task2, "tsk2", 2048, NULL,       5, NULL);

    printf("Waiting for 5 sec\n");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    exit_flag = true;

    if (xSemaphoreTake(exit_sema, 1000 / portTICK_PERIOD_MS) == pdTRUE) {
        vSemaphoreDelete(exit_sema);
    } else {
        TEST_FAIL_MESSAGE("uart_wait_tx_done is blocked");
    }
    TEST_ESP_OK(uart_driver_delete(UART_NUM1));
}

TEST_CASE("test uart get baud-rate", "[uart]")
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

TEST_CASE("test uart tx data with break", "[uart]")
{
    const int buf_len = 200;
    const int send_len = 128;
    const int brk_len = 10;
    char *psend = (char *)malloc(buf_len);
    TEST_ASSERT(psend != NULL);
    memset(psend, '0', buf_len);
    uart_config(UART_BAUD_115200, false);
    printf("Uart%d send %d bytes with break\n", UART_NUM1, send_len);
    uart_write_bytes_with_break(UART_NUM1, (const char *)psend, send_len, brk_len);
    uart_wait_tx_done(UART_NUM1, (portTickType)portMAX_DELAY);
    //If the code is running here, it means the test passed, otherwise it will crash due to the interrupt wdt timeout.
    printf("Send data with break test passed\n");
    free(psend);
    uart_driver_delete(UART_NUM1);
}
