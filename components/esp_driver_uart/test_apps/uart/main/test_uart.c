/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <sys/param.h>
#include "unity.h"
#include "test_utils.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"
#if SOC_LP_GPIO_MATRIX_SUPPORTED
#include "driver/lp_io.h"
#include "driver/rtc_io.h"
#include "hal/rtc_io_ll.h"
#endif
#include "soc/uart_periph.h"
#include "soc/uart_pins.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "test_common.h"

#define BUF_SIZE         (100)
#define UART_BAUD_11520  (11520)
#define UART_BAUD_115200 (115200)
#define TOLERANCE        (0.02)    //baud rate error tolerance 2%.

bool port_select(uart_port_param_t *port_param)
{
    char argv[10];
    unity_wait_for_signal_param("select to test 'uart' or 'lp_uart' port", argv, sizeof(argv));

    if (strcmp(argv, "uart") == 0) {
        port_param->port_num = UART_NUM_1;                  // Test HP_UART with UART1 port
        port_param->default_src_clk = UART_SCLK_DEFAULT;
        port_param->tx_pin_num = 4;
        port_param->rx_pin_num = 5;
        port_param->rx_flow_ctrl_thresh = 120;
        return true;
#if SOC_UART_LP_NUM > 0
    } else if (strcmp(argv, "lp_uart") == 0) {
        port_param->port_num = LP_UART_NUM_0;
        port_param->default_src_clk = LP_UART_SCLK_DEFAULT;
        port_param->tx_pin_num = LP_U0TXD_GPIO_NUM;
        port_param->rx_pin_num = LP_U0RXD_GPIO_NUM;
        port_param->rx_flow_ctrl_thresh = 12;
        return true;
#endif
    } else {
        return false;
    }
}

static void uart_config(uart_port_t uart_num, uint32_t baud_rate, uart_sclk_t source_clk)
{
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .source_clk = source_clk,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    TEST_ESP_OK(uart_driver_install(uart_num, BUF_SIZE * 2, BUF_SIZE * 2, 20, NULL, 0));
    TEST_ESP_OK(uart_param_config(uart_num, &uart_config));
    TEST_ESP_OK(uart_set_loop_back(uart_num, true));
}

static volatile bool exit_flag, case_end;

typedef struct {
    uart_port_t port_num;
    SemaphoreHandle_t exit_sem;
} uart_task1_param_t;

static void test_task1(void *pvParameters)
{
    uart_task1_param_t *param = (uart_task1_param_t *)pvParameters;
    char* data = (char *) malloc(256);

    while (exit_flag == false) {
        uart_tx_chars(param->port_num, data, 256);
        // The uart_wait_tx_done() function does not block anything if ticks_to_wait = 0.
        uart_wait_tx_done(param->port_num, 0);
    }

    free(data);
    xSemaphoreGive(param->exit_sem);
    vTaskDelete(NULL);
}

static void test_task2(void *pvParameters)
{
    uart_port_t uart_num = (uart_port_t)pvParameters;
    while (exit_flag == false) {
        // This task obstruct a setting tx_done_sem semaphore in the UART interrupt.
        // It leads to waiting the ticks_to_wait time in uart_wait_tx_done() function.
        uart_disable_tx_intr(uart_num);
    }
    vTaskDelete(NULL);
}

static void test_task3(void *pvParameters)
{
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));

    uart_port_t uart_num = port_param.port_num;
    uart_config(uart_num, UART_BAUD_11520, port_param.default_src_clk);

    SemaphoreHandle_t exit_sema = xSemaphoreCreateBinary();
    uart_task1_param_t task1_param = {
        .port_num = uart_num,
        .exit_sem = exit_sema,
    };
    exit_flag = false;
    case_end = false;

    xTaskCreate(test_task1, "tsk1", 2048, (void *)&task1_param, 5, NULL);
    xTaskCreate(test_task2, "tsk2", 2048, (void *)uart_num,    5, NULL);

    printf("Waiting for 5 sec\n");
    vTaskDelay(pdMS_TO_TICKS(5000));
    exit_flag = true;

    if (xSemaphoreTake(exit_sema, pdMS_TO_TICKS(1000)) == pdTRUE) {
        vSemaphoreDelete(exit_sema);
    } else {
        TEST_FAIL_MESSAGE("uart_wait_tx_done is blocked");
    }
    TEST_ESP_OK(uart_driver_delete(uart_num));

    vTaskDelay(2);  // wait for test_task1 to exit

    case_end = true;
    vTaskDelete(NULL);
}

TEST_CASE("test uart_wait_tx_done is not blocked when ticks_to_wait=0", "[uart]")
{
    xTaskCreate(test_task3, "tsk3", 4096, NULL, 5, NULL);
    while (!case_end);
    vTaskDelay(2);  // wait for test_task3 to exit
}

TEST_CASE("test uart get baud-rate", "[uart]")
{
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));

    uart_port_t uart_num = port_param.port_num;
    soc_module_clk_t uart_clk_srcs[] = SOC_UART_CLKS;
    uint32_t uart_clk_srcs_num = sizeof(uart_clk_srcs) / sizeof(uart_clk_srcs[0]);

    soc_module_clk_t *clk_srcs = uart_clk_srcs;
    uint32_t clk_srcs_num = uart_clk_srcs_num;

#if SOC_UART_LP_NUM > 0
    soc_module_clk_t lp_uart_clk_srcs[] = SOC_LP_UART_CLKS;
    uint32_t lp_uart_clk_srcs_num = sizeof(lp_uart_clk_srcs) / sizeof(lp_uart_clk_srcs[0]);

    if (uart_num >= SOC_UART_HP_NUM) {
        clk_srcs = lp_uart_clk_srcs;
        clk_srcs_num = lp_uart_clk_srcs_num;
    }
#endif

    uart_config(uart_num, UART_BAUD_115200, port_param.default_src_clk);

    const uint32_t test_baudrate_vals[] = {UART_BAUD_11520, UART_BAUD_115200};
    for (size_t i = 0; i < sizeof(test_baudrate_vals) / sizeof(test_baudrate_vals[0]); i++) {
        for (size_t j = 0; j < clk_srcs_num; j++) {
            uart_config_t uart_config = {
                .baud_rate = test_baudrate_vals[i],
                .source_clk = clk_srcs[j],
                .data_bits = UART_DATA_8_BITS,
                .parity = UART_PARITY_DISABLE,
                .stop_bits = UART_STOP_BITS_1,
                .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            };
            TEST_ESP_OK(uart_param_config(uart_num, &uart_config));

            uint32_t actual_baudrate = 0;
            uart_get_baudrate(uart_num, &actual_baudrate);
            TEST_ASSERT_UINT32_WITHIN(test_baudrate_vals[i] * TOLERANCE, test_baudrate_vals[i], actual_baudrate);
        }
    }

    uart_driver_delete(uart_num);
}

TEST_CASE("test uart tx data with break", "[uart]")
{
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));

    uart_port_t uart_num = port_param.port_num;
    const int buf_len = 200;
    const int send_len = 128;
    const int brk_len = 10;
    char *psend = (char *)malloc(buf_len);
    TEST_ASSERT_NOT_NULL(psend);
    memset(psend, '0', buf_len);
    uart_config(uart_num, UART_BAUD_115200, port_param.default_src_clk);
    printf("Uart%d send %d bytes with break\n", uart_num, send_len);
    uart_write_bytes_with_break(uart_num, (const char *)psend, send_len, brk_len);
    uart_wait_tx_done(uart_num, portMAX_DELAY);
    //If the code is running here, it means the test passed, otherwise it will crash due to the interrupt wdt timeout.
    printf("Send data with break test passed\n");
    free(psend);
    uart_driver_delete(uart_num);
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
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));

    uart_port_t uart_num = port_param.port_num;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = port_param.default_src_clk,
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
    uart_port_t uart_num = (uart_port_t)param;
    uint8_t *tx_buf = (uint8_t *)malloc(1024);
    if (tx_buf == NULL) {
        TEST_FAIL_MESSAGE("tx buffer malloc fail");
    }
    for (int i = 1; i < 1023; i++) {
        tx_buf[i] = (i & 0xff);
    }
    for (int i = 0; i < 1024; i++) {
        //d[0] and d[1023] are header
        tx_buf[0] = (i & 0xff);
        tx_buf[1023] = ((~i) & 0xff);
        uart_write_bytes(uart_num, (const char *)tx_buf, 1024);
        uart_wait_tx_done(uart_num, portMAX_DELAY);
    }
    free(tx_buf);
    vTaskDelete(NULL);
}

TEST_CASE("uart read write test", "[uart]")
{
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));

    uart_port_t uart_num = port_param.port_num;
    uint8_t *rd_data = (uint8_t *)malloc(1024);
    if (rd_data == NULL) {
        TEST_FAIL_MESSAGE("rx buffer malloc fail");
    }
    uart_config_t uart_config = {
        .baud_rate = 2000000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .source_clk = port_param.default_src_clk,
        .rx_flow_ctrl_thresh = port_param.rx_flow_ctrl_thresh,
    };
    TEST_ESP_OK(uart_driver_install(uart_num, BUF_SIZE * 2, 0, 20, NULL, 0));
    TEST_ESP_OK(uart_param_config(uart_num, &uart_config));
    // Use loop back feature to connect TX signal to RX signal, CTS signal to RTS signal internally. Then no need to configure uart pins.
    TEST_ESP_OK(uart_set_loop_back(uart_num, true));

    TEST_ESP_OK(uart_wait_tx_done(uart_num, portMAX_DELAY));
    vTaskDelay(pdMS_TO_TICKS(20)); // make sure last byte has flushed from TX FIFO
    TEST_ESP_OK(uart_flush_input(uart_num));
    xTaskCreate(uart_write_task, "uart_write_task", 8192, (void *)uart_num, 5, NULL);
    for (int i = 0; i < 1024; i++) {
        int bytes_remaining = 1024;
        memset(rd_data, 0, 1024);
        while (bytes_remaining) {
            int bytes_received = uart_read_bytes(uart_num, rd_data + 1024 - bytes_remaining, bytes_remaining, pdMS_TO_TICKS(100));
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
    uart_wait_tx_done(uart_num, portMAX_DELAY);
    uart_driver_delete(uart_num);
    free(rd_data);

    vTaskDelay(2); // wait for uart_write_task to exit
}

TEST_CASE("uart tx with ringbuffer test", "[uart]")
{
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));

    uart_port_t uart_num = port_param.port_num;
    uint8_t *rd_data = (uint8_t *)malloc(1024);
    uint8_t *wr_data = (uint8_t *)malloc(1024);
    if (rd_data == NULL || wr_data == NULL) {
        TEST_FAIL_MESSAGE("buffer malloc fail");
    }
    uart_config_t uart_config = {
        .baud_rate = 2000000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = port_param.rx_flow_ctrl_thresh,
        .source_clk = port_param.default_src_clk,
    };
    uart_wait_tx_idle_polling(uart_num);
    TEST_ESP_OK(uart_param_config(uart_num, &uart_config));
    TEST_ESP_OK(uart_driver_install(uart_num, 1024 * 2, 1024 * 2, 20, NULL, 0));
    // Use loop back feature to connect TX signal to RX signal, CTS signal to RTS signal internally. Then no need to configure uart pins.
    TEST_ESP_OK(uart_set_loop_back(uart_num, true));

    for (int i = 0; i < 1024; i++) {
        wr_data[i] = i;
        rd_data[i] = 0;
    }

    size_t tx_buffer_free_space;
    uart_get_tx_buffer_free_size(uart_num, &tx_buffer_free_space);
    TEST_ASSERT_EQUAL_INT(2048, tx_buffer_free_space); // full tx buffer space is free
    uart_write_bytes(uart_num, (const char *)wr_data, 1024);
    uart_get_tx_buffer_free_size(uart_num, &tx_buffer_free_space);
    TEST_ASSERT_LESS_THAN(2048, tx_buffer_free_space); // tx transmit in progress: tx buffer has content
    TEST_ASSERT_GREATER_OR_EQUAL(1024, tx_buffer_free_space);
    uart_wait_tx_done(uart_num, portMAX_DELAY);
    uart_get_tx_buffer_free_size(uart_num, &tx_buffer_free_space);
    TEST_ASSERT_EQUAL_INT(2048, tx_buffer_free_space); // tx done: tx buffer back to empty
    uart_read_bytes(uart_num, rd_data, 1024, pdMS_TO_TICKS(1000));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(wr_data, rd_data, 1024);
    TEST_ESP_OK(uart_driver_delete(uart_num));
    free(rd_data);
    free(wr_data);
}

TEST_CASE("uart int state restored after flush", "[uart]")
{
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));

    uart_port_t uart_num = port_param.port_num;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = port_param.default_src_clk,
    };

    const int uart_tx_signal = uart_periph_signal[uart_num].pins[SOC_UART_TX_PIN_IDX].signal;
    const int uart_tx = port_param.tx_pin_num;
    const int uart_rx = port_param.rx_pin_num;
    const int buf_size = 256;
    const int intr_alloc_flags = 0;

    TEST_ESP_OK(uart_driver_install(uart_num, buf_size * 2, 0, 0, NULL, intr_alloc_flags));
    TEST_ESP_OK(uart_param_config(uart_num, &uart_config));
    TEST_ESP_OK(uart_set_pin(uart_num, uart_tx, uart_rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    /* Make sure UART's TX signal is connected to RX pin
     * This creates a loop that lets us receive anything we send on the UART */
    if (uart_num < SOC_UART_HP_NUM) {
        esp_rom_gpio_connect_out_signal(uart_rx, uart_tx_signal, false, false);
#if SOC_UART_LP_NUM > 0
    } else {
        // LP_UART
#if SOC_LP_GPIO_MATRIX_SUPPORTED
        // Need to route TX signal to RX signal with the help of LP_GPIO matrix, TX signal connect to the RX IO directly
        // This means RX IO should also only use LP_GPIO matrix to connect the RX signal
        // In case the selected RX IO is the LP UART IOMUX IO, and the IO has been configured to IOMUX function in the driver
        // Do the following:
        TEST_ESP_OK(rtc_gpio_iomux_func_sel(uart_rx, RTCIO_LL_PIN_FUNC));
        const int uart_rx_signal = uart_periph_signal[uart_num].pins[SOC_UART_RX_PIN_IDX].signal;
        TEST_ESP_OK(lp_gpio_connect_in_signal(uart_rx, uart_rx_signal, false));
        TEST_ESP_OK(lp_gpio_connect_out_signal(uart_rx, uart_tx_signal, false, false));
#else
        // The only way is to use loop back feature
        TEST_ESP_OK(uart_set_loop_back(uart_num, true));
#endif
#endif // SOC_UART_LP_NUM > 0
    }

    uint8_t *data = (uint8_t *)malloc(buf_size);
    TEST_ASSERT_NOT_NULL(data);
    uart_write_bytes(uart_num, (const char *)data, buf_size);

    /* As we set up a loopback, we can read them back on RX */
    int len = uart_read_bytes(uart_num, data, buf_size, pdMS_TO_TICKS(1000));
    printf("len is %d\n", len);
    TEST_ASSERT_EQUAL(len, buf_size);

    /**
     * The first goal of this test is to make sure that when our RX FIFO is full,
     * we can continue receiving back data after flushing
     * For more details, check IDF-4374 */

    /* Fill the RX buffer, this should disable the RX interrupts */
    int written = uart_write_bytes(uart_num, (const char *)data, buf_size);
    TEST_ASSERT_NOT_EQUAL(-1, written);
    written = uart_write_bytes(uart_num, (const char *)data, buf_size);
    TEST_ASSERT_NOT_EQUAL(-1, written);
    written = uart_write_bytes(uart_num, (const char *)data, buf_size);
    TEST_ASSERT_NOT_EQUAL(-1, written);

    /* Flush the input buffer, RX interrupts should be re-enabled */
    uart_flush_input(uart_num);
    written = uart_write_bytes(uart_num, (const char *)data, buf_size);
    TEST_ASSERT_NOT_EQUAL(-1, written);
    len = uart_read_bytes(uart_num, data, buf_size, pdMS_TO_TICKS(1000));
    /* len equals buf_size bytes if interrupts were indeed re-enabled */
    TEST_ASSERT_EQUAL(len, buf_size);

    /**
     * Second test, make sure that if we explicitly disable the RX interrupts,
     * they are NOT re-enabled after flushing
     * To do so, start by cleaning the RX FIFO, disable the RX interrupts,
     * flush again, send data to the UART and check that we haven't received
     * any of the bytes */

    uart_flush_input(uart_num);
    uart_disable_rx_intr(uart_num);
    uart_flush_input(uart_num);
    written = uart_write_bytes(uart_num, (const char *)data, buf_size);
    TEST_ASSERT_NOT_EQUAL(-1, written);
    len = uart_read_bytes(uart_num, data, buf_size, pdMS_TO_TICKS(250));
    TEST_ASSERT_EQUAL(len, 0);

    TEST_ESP_OK(uart_driver_delete(uart_num));
    free(data);
}
