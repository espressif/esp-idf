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
    };
    uart_config.source_clk = use_ref_tick ? UART_SCLK_REF_TICK : UART_SCLK_APB;
    uart_driver_install(UART_NUM1, BUF_SIZE * 2, BUF_SIZE * 2, 20, NULL, 0);
    uart_param_config(UART_NUM1, &uart_config);
    TEST_ESP_OK(uart_set_loop_back(UART_NUM1, true));
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
        uart_disable_tx_intr(UART_NUM1);
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
    TEST_ASSERT_UINT32_WITHIN(UART_BAUD_11520 * TOLERANCE, UART_BAUD_11520, baud_rate1);
    TEST_ASSERT_UINT32_WITHIN(UART_BAUD_115200 * TOLERANCE, UART_BAUD_115200, baud_rate2);
    ESP_LOGI(UART_TAG, "get baud-rate test passed  ....\n");
}

TEST_CASE("test uart tx data with break", "[uart]")
{
    const int buf_len = 200;
    const int send_len = 128;
    const int brk_len = 10;
    char *psend = (char *)malloc(buf_len);
    TEST_ASSERT_NOT_NULL(psend);
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

static void uart_word_len_set_get_test(int uart_num)
{
    printf("uart word len set and get test\n");
    uart_word_length_t word_length_set = 0;
    uart_word_length_t word_length_get = 0;
    for (int i = 0; i < UART_DATA_BITS_MAX; i++) {
        word_length_set = UART_DATA_5_BITS + i;
        TEST_ESP_OK(uart_set_word_length(uart_num, word_length_set));
        TEST_ESP_OK(uart_get_word_length(uart_num, &word_length_get));
        TEST_ASSERT_EQUAL(word_length_set, word_length_get);
    }
}

static void uart_stop_bit_set_get_test(int uart_num)
{
    printf("uart stop bit set and get test\n");
    uart_stop_bits_t stop_bit_set = 0;
    uart_stop_bits_t stop_bit_get = 0;
    for (int i = UART_STOP_BITS_1; i < UART_STOP_BITS_MAX; i++) {
        stop_bit_set = i;
        TEST_ESP_OK(uart_set_stop_bits(uart_num, stop_bit_set));
        TEST_ESP_OK(uart_get_stop_bits(uart_num, &stop_bit_get));
        TEST_ASSERT_EQUAL(stop_bit_set, stop_bit_get);
    }
}

static void uart_parity_set_get_test(int uart_num)
{
    printf("uart parity set and get test\n");
    uart_parity_t parity_set[3] = {
        UART_PARITY_DISABLE,
        UART_PARITY_EVEN,
        UART_PARITY_ODD,
    };
    uart_parity_t parity_get = 0;
    for (int i = 0; i < 3; i++) {
        TEST_ESP_OK(uart_set_parity(uart_num, parity_set[i]));
        TEST_ESP_OK(uart_get_parity(uart_num, &parity_get));
        TEST_ASSERT_EQUAL(parity_set[i], parity_get);
    }
}

static void uart_hw_flow_set_get_test(int uart_num)
{
    printf("uart hw flow control set and get test\n");
    uart_hw_flowcontrol_t flowcontrol_set = 0;
    uart_hw_flowcontrol_t flowcontrol_get = 0;
    for (int i = 0; i < UART_HW_FLOWCTRL_DISABLE; i++) {
        TEST_ESP_OK(uart_set_hw_flow_ctrl(uart_num, flowcontrol_set, 20));
        TEST_ESP_OK(uart_get_hw_flow_ctrl(uart_num, &flowcontrol_get));
        TEST_ASSERT_EQUAL(flowcontrol_set, flowcontrol_get);
    }
}

static void uart_wakeup_set_get_test(int uart_num)
{
    printf("uart wake up set and get test\n");
    int wake_up_set = 0;
    int wake_up_get = 0;
    for (int i = 3; i < 0x3ff; i++) {
        wake_up_set = i;
        TEST_ESP_OK(uart_set_wakeup_threshold(uart_num, wake_up_set));
        TEST_ESP_OK(uart_get_wakeup_threshold(uart_num, &wake_up_get));
        TEST_ASSERT_EQUAL(wake_up_set, wake_up_get);
    }
}

TEST_CASE("uart general API test", "[uart]")
{
    const int uart_num = UART_NUM1;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_param_config(uart_num, &uart_config);
    uart_word_len_set_get_test(uart_num);
    uart_stop_bit_set_get_test(uart_num);
    uart_parity_set_get_test(uart_num);
    uart_hw_flow_set_get_test(uart_num);
    uart_wakeup_set_get_test(uart_num);
}

static void uart_write_task(void *param)
{
    int uart_num = (int)param;
    uint8_t *tx_buf = (uint8_t *)malloc(1024);
    if(tx_buf == NULL) {
        TEST_FAIL_MESSAGE("tx buffer malloc fail");
    }
    for(int i = 1; i < 1023; i++) {
        tx_buf[i] = (i & 0xff);
    }
    for(int i = 0; i < 1024; i++) {
        //d[0] and d[1023] are header
        tx_buf[0] = (i & 0xff);
        tx_buf[1023] = ((~i) & 0xff);
        uart_write_bytes(uart_num, (const char*)tx_buf, 1024);
        uart_wait_tx_done(uart_num, (TickType_t)portMAX_DELAY);
    }
    free(tx_buf);
    vTaskDelete(NULL);
}

TEST_CASE("uart read write test", "[uart]")
{
    const int uart_num = UART_NUM1;
    uint8_t *rd_data = (uint8_t *)malloc(1024);
    if(rd_data == NULL) {
        TEST_FAIL_MESSAGE("rx buffer malloc fail");
    }
    uart_config_t uart_config = {
        .baud_rate = 2000000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .source_clk = UART_SCLK_APB,
        .rx_flow_ctrl_thresh = 120
    };
    TEST_ESP_OK(uart_driver_install(uart_num, BUF_SIZE * 2, 0, 20, NULL, 0));
    TEST_ESP_OK(uart_param_config(uart_num, &uart_config));
    TEST_ESP_OK(uart_set_loop_back(uart_num, true));

    TEST_ESP_OK(uart_wait_tx_done(uart_num, portMAX_DELAY));
    vTaskDelay(1 / portTICK_PERIOD_MS); // make sure last byte has flushed from TX FIFO
    TEST_ESP_OK(uart_flush_input(uart_num));

    xTaskCreate(uart_write_task, "uart_write_task", 2048 * 4, (void *)uart_num, UNITY_FREERTOS_PRIORITY - 1, NULL);
    for (int i = 0; i < 1024; i++) {
        int bytes_remaining = 1024;
        memset(rd_data, 0, 1024);
        while (bytes_remaining) {
            int bytes_received = uart_read_bytes(uart_num, rd_data + 1024 - bytes_remaining, bytes_remaining, (TickType_t)1000);
            if (bytes_received < 0) {
                TEST_FAIL_MESSAGE("read timeout, uart read write test fail");
            }
            bytes_remaining -= bytes_received;
        }
        int check_fail_cnt = 0;
        if (rd_data[0] != (i & 0xff)) {
            printf("packet %d index check error at offset 0, expected 0x%02x\n", i, i);
            ++check_fail_cnt;
        }
        if (rd_data[1023] != ((~i) & 0xff)) {
            printf("packet %d index check error at offset 1023, expected 0x%02x\n", i, ((~i) & 0xff));
            ++check_fail_cnt;
        }
        for (int j = 1; j < 1023; j++) {
            if (rd_data[j] != (j & 0xff)) {
                printf("data mismatch in packet %d offset %d, expected 0x%02x got 0x%02x\n", i, j, (j & 0xff), rd_data[j]);
                ++check_fail_cnt;
            }
            if (check_fail_cnt > 10) {
                printf("(further checks skipped)\n");
                break;
            }
        }
        if (check_fail_cnt > 0) {
            ESP_LOG_BUFFER_HEX("rd_data", rd_data, 1024);
            TEST_FAIL();
        }
    }
    uart_wait_tx_done(uart_num, (TickType_t)portMAX_DELAY);
    uart_driver_delete(uart_num);
    free(rd_data);
}

TEST_CASE("uart tx with ringbuffer test", "[uart]")
{
    const int uart_num = UART_NUM1;
    uint8_t *rd_data = (uint8_t *)malloc(1024);
    uint8_t *wr_data = (uint8_t *)malloc(1024);
    if(rd_data == NULL || wr_data == NULL) {
        TEST_FAIL_MESSAGE("buffer malloc fail");
    }
    uart_config_t uart_config = {
        .baud_rate = 2000000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 120,
        .source_clk = UART_SCLK_APB,
    };
    TEST_ESP_OK(uart_param_config(uart_num, &uart_config));
    TEST_ESP_OK(uart_driver_install(uart_num, 1024 * 2, 1024 *2, 20, NULL, 0));
    TEST_ESP_OK(uart_set_loop_back(uart_num, true));
    for (int i = 0; i < 1024; i++) {
        wr_data[i] = i;
        rd_data[i] = 0;
    }
    uart_write_bytes(uart_num, (const char*)wr_data, 1024);
    uart_wait_tx_done(uart_num, (TickType_t)portMAX_DELAY);
    uart_read_bytes(uart_num, rd_data, 1024, (TickType_t)1000);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(wr_data, rd_data, 1024);
    TEST_ESP_OK(uart_driver_delete(uart_num));
    free(rd_data);
    free(wr_data);
}
