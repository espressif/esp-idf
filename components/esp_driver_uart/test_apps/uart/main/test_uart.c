/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <sys/param.h>
#include "unity.h"
#include "test_utils.h"
#include "unity_test_utils.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"
#include "esp_private/gpio.h"
#include "hal/gpio_ll.h"
#if SOC_LP_GPIO_MATRIX_SUPPORTED
#include "driver/lp_io.h"
#include "driver/rtc_io.h"
#include "hal/rtc_io_ll.h"
#endif
#include "hal/uart_ll.h"
#include "hal/uart_periph.h"
#include "soc/uart_pins.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "test_common.h"
#include "esp_attr.h"

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
        port_param->tx_pin_num = TEST_UART_TX_PIN_NUM;
        port_param->rx_pin_num = TEST_UART_RX_PIN_NUM;
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

typedef struct {
    uart_port_t port_num;
    int test_times;
} uart_write_task_param_t;

static void uart_write_task(void *param)
{
    uart_write_task_param_t *task_param = (uart_write_task_param_t *)param;
    uart_port_t uart_num = task_param->port_num;
    uint8_t *tx_buf = (uint8_t *)malloc(1024);
    TEST_ASSERT_NOT_NULL(tx_buf);
    for (int i = 1; i < 1023; i++) {
        tx_buf[i] = (i & 0xff);
    }
    for (int i = 0; i < task_param->test_times; i++) {
        //d[0] and d[1023] are header
        tx_buf[0] = (i & 0xff);
        tx_buf[1023] = ((~i) & 0xff);
        uart_write_bytes(uart_num, (const char *)tx_buf, 1024);
        uart_wait_tx_done(uart_num, portMAX_DELAY);
    }
    free(tx_buf);
    vTaskDelete(NULL);
}

static void uart_read_data_with_check(uart_port_t uart_num, int test_times)
{
    uint8_t *rd_data = (uint8_t *)malloc(1024);
    TEST_ASSERT_NOT_NULL(rd_data);
    for (int i = 0; i < test_times; i++) {
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
    free(rd_data);
}

TEST_CASE("uart read write test", "[uart]")
{
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));

    uart_port_t uart_num = port_param.port_num;
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

    uart_write_task_param_t task_param = {
        .port_num = uart_num,
        .test_times = 1024,
    };
    xTaskCreate(uart_write_task, "uart_write_task", 8192, (void *)&task_param, 5, NULL);

    uart_read_data_with_check(uart_num, task_param.test_times);

    uart_wait_tx_done(uart_num, portMAX_DELAY);
    uart_driver_delete(uart_num);

    vTaskDelay(2); // wait for uart_write_task to exit
}

TEST_CASE("uart tx with ringbuffer test", "[uart]")
{
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));

    uart_port_t uart_num = port_param.port_num;
    uint8_t *rd_data = (uint8_t *)malloc(1024);
    TEST_ASSERT_NOT_NULL(rd_data);
    uint8_t *wr_data = (uint8_t *)malloc(1024);
    TEST_ASSERT_NOT_NULL(wr_data);
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

    uart_write_bytes(uart_num, (const char *)wr_data, 1024);
    uart_wait_tx_done(uart_num, portMAX_DELAY);

    uart_read_bytes(uart_num, rd_data, 1024, pdMS_TO_TICKS(1000));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(wr_data, rd_data, 1024);

    TEST_ESP_OK(uart_driver_delete(uart_num));
    free(rd_data);
    free(wr_data);
}

TEST_CASE("uart tx ring buffer free space test", "[uart]")
{
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));
    // This is a test on the driver API, no need to test for both HP/LP uart port, call port_select() to be compatible with pytest
    // Let's only test on HP UART
    if (port_param.port_num < SOC_UART_HP_NUM) {
        uart_port_t uart_num = port_param.port_num;
        uint8_t *rd_data = (uint8_t *)malloc(1024);
        TEST_ASSERT_NOT_NULL(rd_data);
        uint8_t *wr_data = (uint8_t *)malloc(2048);
        TEST_ASSERT_NOT_NULL(wr_data);
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
        TEST_ESP_OK(uart_driver_install(uart_num, 256, 1024 * 2, 20, NULL, 0));
        // Let CTS be high, so that transmission is blocked
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, uart_periph_signal[uart_num].pins[SOC_UART_PERIPH_SIGNAL_CTS].signal, false);

        // When nothing pushed to the TX ring buffer, the free space should be the full capacity
        size_t tx_buffer_free_space;
        uart_get_tx_buffer_free_size(uart_num, &tx_buffer_free_space);
        TEST_ASSERT_EQUAL_INT(2020, tx_buffer_free_space); // no-split ring buffer: 2048 - 20 (data description item) - 8 (header)

        // Push 1024 bytes to the TX ring buffer
        uart_write_bytes(uart_num, (const char *)wr_data, 1024); // two chunks
        vTaskDelay(pdMS_TO_TICKS(500));
        uart_get_tx_buffer_free_size(uart_num, &tx_buffer_free_space);
        TEST_ASSERT_LESS_THAN(2020, tx_buffer_free_space); // tx buffer has content
        TEST_ASSERT_GREATER_OR_EQUAL(952, tx_buffer_free_space);

        // Fill the remaining space in the TX ring buffer
        uart_write_bytes(uart_num, (const char *)wr_data, tx_buffer_free_space);
        uart_get_tx_buffer_free_size(uart_num, &tx_buffer_free_space);
        TEST_ASSERT_EQUAL_INT(0, tx_buffer_free_space); // tx buffer is full

        // Let CTS be low, so that transmission is unblocked
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, uart_periph_signal[uart_num].pins[SOC_UART_PERIPH_SIGNAL_CTS].signal, false);
        uart_wait_tx_done(uart_num, portMAX_DELAY);
        uart_get_tx_buffer_free_size(uart_num, &tx_buffer_free_space);
        TEST_ASSERT_EQUAL_INT(2020, tx_buffer_free_space); // tx buffer is back to full capacity

        TEST_ESP_OK(uart_driver_delete(uart_num));
        free(rd_data);
        free(wr_data);
    }
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

    const int uart_tx_signal = uart_periph_signal[uart_num].pins[SOC_UART_PERIPH_SIGNAL_TX].signal;
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
        gpio_func_sel(uart_rx, PIN_FUNC_GPIO);
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
        const int uart_rx_signal = uart_periph_signal[uart_num].pins[SOC_UART_PERIPH_SIGNAL_RX].signal;
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

TEST_CASE("uart in one-wire mode", "[uart]")
{
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));
    port_param.tx_pin_num = port_param.rx_pin_num; // let tx and rx use the same pin

    uart_port_t uart_num = port_param.port_num;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = port_param.default_src_clk,
    };

    TEST_ESP_OK(uart_driver_install(uart_num, BUF_SIZE * 2, 0, 20, NULL, 0));
    TEST_ESP_OK(uart_param_config(uart_num, &uart_config));
    esp_err_t err = uart_set_pin(uart_num, port_param.tx_pin_num, port_param.rx_pin_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (uart_num < SOC_UART_HP_NUM) {
        TEST_ESP_OK(err);
#if SOC_UART_LP_NUM > 0
    } else {
#if !SOC_LP_GPIO_MATRIX_SUPPORTED
        TEST_ESP_ERR(ESP_FAIL, err); // For LP UART port, if no LP GPIO Matrix, unable to be used in one-wire mode
#else
        TEST_ESP_OK(err);
#endif
#endif // SOC_UART_LP_NUM > 0
    }

    // If configured successfully in one-wire mode
    if (err == ESP_OK) {
        TEST_ESP_OK(uart_wait_tx_done(uart_num, portMAX_DELAY));
        vTaskDelay(pdMS_TO_TICKS(20)); // make sure last byte has flushed from TX FIFO
        TEST_ESP_OK(uart_flush_input(uart_num));

        const char *wr_data = "ECHO!";
        const int len = strlen(wr_data);
        uint8_t *rd_data = (uint8_t *)calloc(1, 1024);
        TEST_ASSERT_NOT_NULL(rd_data);

        uart_write_bytes(uart_num, wr_data, len);
        int bytes_received = uart_read_bytes(uart_num, rd_data, BUF_SIZE, pdMS_TO_TICKS(20));
        TEST_ASSERT_EQUAL(len, bytes_received);
        TEST_ASSERT_EQUAL_STRING_LEN(wr_data, rd_data, bytes_received);

        free(rd_data);
    }

    TEST_ESP_OK(uart_driver_delete(uart_num));
}

static void uart_console_write_task(void *arg)
{
    while (1) {
        printf("This is a sentence used to detect uart baud rate...\nThis is a sentence used to detect uart baud rate...\nThis is a sentence used to detect uart baud rate...\nThis is a sentence used to detect uart baud rate...\nThis is a sentence used to detect uart baud rate...\n");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

TEST_CASE("uart auto baud rate detection", "[uart]")
{
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));
    // This is indeed a standalone feature, no need to specify the uart port, call port_select() to be compatible with pytest
    // And this test case no need to be tested twice on HP/LP uart ports both exist targets (also, LP UART does not support auto baud rate detection functionality)
    if (port_param.port_num < SOC_UART_HP_NUM) {
        TaskHandle_t console_write_task = NULL;
        xTaskCreate(uart_console_write_task, "uart_console_write_task", 2048, NULL, 5, &console_write_task);
        vTaskDelay(20);

        // Measure console uart baudrate
        uint32_t actual_baudrate = 0;
        uint32_t detected_baudrate = 0;
        uart_bitrate_detect_config_t conf = {
            .rx_io_num = uart_periph_signal[CONFIG_CONSOLE_UART_NUM].pins[SOC_UART_PERIPH_SIGNAL_TX].default_gpio,
            .source_clk = UART_SCLK_DEFAULT,
        };
        uart_bitrate_res_t res = {};

        uart_get_baudrate(CONFIG_CONSOLE_UART_NUM, &actual_baudrate);
        TEST_ESP_OK(uart_detect_bitrate_start(UART_NUM_1, &conf)); // acquire a new uart port
        vTaskDelay(pdMS_TO_TICKS(500));
        TEST_ESP_OK(uart_detect_bitrate_stop(UART_NUM_1, false, &res)); // no releasing
        detected_baudrate = res.clk_freq_hz * 2 / res.pos_period; // assume the wave has a slow falling slew rate
        TEST_ASSERT_INT32_WITHIN(actual_baudrate * 0.03, actual_baudrate, detected_baudrate); // allow 3% error

        // Temporarily change console baudrate
        uart_set_baudrate(CONFIG_CONSOLE_UART_NUM, 38400);

        uart_get_baudrate(CONFIG_CONSOLE_UART_NUM, &actual_baudrate);
        TEST_ESP_OK(uart_detect_bitrate_start(UART_NUM_1, NULL)); // use the previously acquired uart port
        vTaskDelay(pdMS_TO_TICKS(500));
        TEST_ESP_OK(uart_detect_bitrate_stop(UART_NUM_1, true, &res)); // release the uart port
        detected_baudrate = res.clk_freq_hz * 2 / res.pos_period;
        TEST_ASSERT_INT32_WITHIN(actual_baudrate * 0.03, actual_baudrate, detected_baudrate);

        // Set back to original console baudrate, test again
        uart_set_baudrate(CONFIG_CONSOLE_UART_NUM, CONFIG_CONSOLE_UART_BAUDRATE);

        uart_get_baudrate(CONFIG_CONSOLE_UART_NUM, &actual_baudrate);
        TEST_ESP_OK(uart_detect_bitrate_start(UART_NUM_1, &conf)); // acquire a new uart port again
        vTaskDelay(pdMS_TO_TICKS(500));
        TEST_ESP_OK(uart_detect_bitrate_stop(UART_NUM_1, true, &res)); // release it
        detected_baudrate = res.clk_freq_hz * 2 / res.pos_period;
        TEST_ASSERT_INT32_WITHIN(actual_baudrate * 0.03, actual_baudrate, detected_baudrate);

        vTaskDelete(console_write_task);
    }
}

IRAM_ATTR static void uart_signal_inject_glitch_task(void *param)
{
    uart_port_param_t *port_param = (uart_port_param_t *)param;
    uart_port_t uart_num = port_param->port_num;
    uint32_t tx_pin = port_param->tx_pin_num;
    // while sending, frequently disconnect from UART TX signal and set level to high to create glitches
    gpio_ll_set_level(&GPIO, tx_pin, 1);
#if SOC_UART_LP_NUM > 0 && SOC_LP_GPIO_MATRIX_SUPPORTED
    uint32_t rtc_gpio_num = rtc_io_number_get(tx_pin);
    rtcio_ll_set_level(rtc_gpio_num, 1);
#endif

    // esp_rom_delay_us(1000); // wait for uart write task to start sending data

    while (1) {
        // make sure the glitch is always less than 5us
        portDISABLE_INTERRUPTS();
        if (uart_num < SOC_UART_HP_NUM) {
            esp_rom_gpio_connect_out_signal(tx_pin, SIG_GPIO_OUT_IDX, false, false);
            gpio_ll_set_output_enable_ctrl(&GPIO, tx_pin, false, false);
            esp_rom_gpio_connect_out_signal(tx_pin, UART_PERIPH_SIGNAL(uart_num, SOC_UART_PERIPH_SIGNAL_TX), false, false);
#if SOC_UART_LP_NUM > 0 && SOC_LP_GPIO_MATRIX_SUPPORTED
        } else {
            rtcio_ll_matrix_out(rtc_gpio_num, SIG_LP_GPIO_OUT_IDX, false, false);
            LP_GPIO.func_out_sel_cfg[rtc_gpio_num].oe_sel = 1;
            rtcio_ll_matrix_out(rtc_gpio_num, UART_PERIPH_SIGNAL(uart_num, SOC_UART_PERIPH_SIGNAL_TX), false, false);
#endif
        }
        portENABLE_INTERRUPTS();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

TEST_CASE("uart rx glitch filter (read write test + auto baud rate detection test)", "[uart]")
{
    // The test will be constructed as:
    // Configuring TX and RX signal on the same GPIO pad
    // And while the TX data is sending, the TX output configuration will be manipulated to generate glitches on the signal
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));
    port_param.tx_pin_num = port_param.rx_pin_num; // let tx and rx use the same pin

    uart_port_t uart_num = port_param.port_num;
    // High speed clock source may not able to filter a 5us glitch, therefore, lower the source clock frequency
    if (uart_num < SOC_UART_HP_NUM) {
#if SOC_UART_SUPPORT_XTAL_CLK
        port_param.default_src_clk = UART_SCLK_XTAL;
#endif
    }
    uart_config_t uart_config = {
        .baud_rate = 50000,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = port_param.default_src_clk,
#if !UART_LL_GLITCH_FILT_ONLY_ON_AUTOBAUD
        .rx_glitch_filt_thresh = 5000, // filter all glitches with width less than 5us
#endif
    };

    TEST_ESP_OK(uart_driver_install(uart_num, BUF_SIZE * 2, 0, 20, NULL, 0));
    TEST_ESP_OK(uart_param_config(uart_num, &uart_config));
    esp_err_t err = uart_set_pin(uart_num, port_param.tx_pin_num, port_param.rx_pin_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (uart_num < SOC_UART_HP_NUM) {
        TEST_ESP_OK(err);
#if SOC_UART_LP_NUM > 0
    } else {
#if !SOC_LP_GPIO_MATRIX_SUPPORTED
        TEST_ESP_ERR(ESP_FAIL, err); // For LP UART port, if no LP GPIO Matrix, unable to be used in one-wire mode
#else
        TEST_ESP_OK(err);
#endif
#endif // SOC_UART_LP_NUM > 0
    }

    // If successfully route TX and RX on the same pad, then we can continue with the test
    if (err == ESP_OK) {
        TEST_ESP_OK(uart_wait_tx_done(uart_num, portMAX_DELAY));
        vTaskDelay(pdMS_TO_TICKS(20)); // make sure last byte has flushed from TX FIFO
        TEST_ESP_OK(uart_flush_input(uart_num));

        uart_write_task_param_t task_param = {
            .port_num = uart_num,
            .test_times = 10,
        };
        TaskHandle_t inject_glitch_task_handle;
        xTaskCreate(uart_signal_inject_glitch_task, "uart_signal_inject_glitch_task", 1024, (void *)&port_param, 6, &inject_glitch_task_handle);

        // 1. read write test
#if !UART_LL_GLITCH_FILT_ONLY_ON_AUTOBAUD
        xTaskCreate(uart_write_task, "uart_write_task", 8192, (void *)&task_param, 5, NULL);
        uart_read_data_with_check(uart_num, task_param.test_times);
        uart_wait_tx_done(uart_num, portMAX_DELAY);
#endif

        // 2. auto baud rate detection test
        if (uart_num < SOC_UART_HP_NUM) {
            TaskHandle_t write_task_handle;
            task_param.test_times = 5;
            xTaskCreate(uart_write_task, "uart_write_task", 8192, (void *)&task_param, 5, &write_task_handle);
            uart_bitrate_res_t res = {};
#if UART_LL_GLITCH_FILT_ONLY_ON_AUTOBAUD
            // Set the glitch filter threshold for auto baud rate detection on ESP32 and ESP32S2
            // And since the reference tick for filter is APB clock (80MHz) for the two targets, so set the threshold to 3us to avoid exceeding the register value limit
            // The glitch is much less than 3us
            uart_bitrate_detect_config_t conf = {
                .rx_glitch_filt_thresh = 3000,
            };
            TEST_ESP_OK(uart_detect_bitrate_start(uart_num, &conf));
#else
            TEST_ESP_OK(uart_detect_bitrate_start(uart_num, NULL));
#endif
            vTaskDelay(pdMS_TO_TICKS(10));
            TEST_ESP_OK(uart_detect_bitrate_stop(uart_num, false, &res));
            uint32_t detected_baudrate = res.clk_freq_hz * 2 / res.pos_period; // assume the wave has a slow falling slew rate
            uint32_t actual_baudrate = 0;
            uart_get_baudrate(uart_num, &actual_baudrate);
            TEST_ASSERT_INT32_WITHIN(actual_baudrate * 0.03, actual_baudrate, detected_baudrate);
            // wait for write task to finish and self deleted
            while (eTaskGetState(write_task_handle) != eDeleted) {
                vTaskDelay(1);
            }
        }

        unity_utils_task_delete(inject_glitch_task_handle);
    }

    uart_driver_delete(uart_num);

    vTaskDelay(2); // wait for tasks to exit
}
