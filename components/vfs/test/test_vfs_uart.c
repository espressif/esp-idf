// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/termios.h>
#include <sys/errno.h>
#include "unity.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/uart.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/uart.h"
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/uart.h"
#include "esp_vfs_dev.h"
#include "esp_vfs.h"
#include "sdkconfig.h"

static void fwrite_str_loopback(const char* str, size_t size)
{
    uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
    UART0.conf0.loopback = 1;
    fwrite(str, 1, size, stdout);
    fflush(stdout);
    uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
    vTaskDelay(2 / portTICK_PERIOD_MS);
    UART0.conf0.loopback = 0;
}

static void flush_stdin_stdout(void)
{
    vTaskDelay(10 / portTICK_PERIOD_MS);
    char bitbucket[UART_FIFO_LEN];
    while (fread(bitbucket, 1, UART_FIFO_LEN, stdin) > 0) {
        ;
    }
    fflush(stdout);
    uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
}

TEST_CASE("can read from stdin", "[vfs]")
{
    flush_stdin_stdout();

    const size_t count = 12;
    srand(count);
    char* data = (char*) calloc(1, count * 8 + 2);
    char* buf = (char*) calloc(1, count * 8 + 2);
    char* p = data;
    for (size_t i = 0; i < count; ++i) {
        p += sprintf(p, "%08x", rand());
    }
    p += sprintf(p, "\n");
    size_t len = p - data;
    fwrite_str_loopback(data, len);
    size_t cb = fread(buf, 1, len, stdin);
    TEST_ASSERT_EQUAL(len, cb);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(data, buf, len);

    free(data);
    free(buf);
}


TEST_CASE("CRs are removed from the stdin correctly", "[vfs]")
{
    esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CRLF);
    esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    flush_stdin_stdout();
    const char* send_str = "1234567890\n\r123\r\n4\n";
    /* with CONFIG_NEWLIB_STDOUT_ADDCR, the following will be sent on the wire.
     * (last character of each part is marked with a hat)
     *
     *                      1234567890\r\n\r123\r\r\n4\r\n
     *                                 ^   ^^            ^
     */
    char buf[128];
    char* dst = buf;

    fwrite_str_loopback(send_str, 11);      // send up to the first \n
    size_t rb = fread(dst, 1, 5, stdin);    // read first 5
    TEST_ASSERT_EQUAL(5, rb);
    dst += rb;

    rb = fread(dst, 1, 6, stdin);           // ask for 6
    TEST_ASSERT_EQUAL(6, rb);               // get 6
    
    TEST_ASSERT_EQUAL_UINT8_ARRAY("1234567890\n", buf, 11);
    dst += rb;

    rb = fread(dst, 1, 2, stdin);           // any more characters?
    TEST_ASSERT_EQUAL(0, rb);               // nothing

    fwrite_str_loopback(send_str + 11, 1);  // send the '\r'
    vTaskDelay(10 / portTICK_PERIOD_MS);

    rb = fread(dst, 1, 2, stdin);           // try to get somthing
    TEST_ASSERT_EQUAL(0, rb);               // still nothing (\r is buffered)

    fwrite_str_loopback(send_str + 12, 1);  // Now send the '1'
    vTaskDelay(10 / portTICK_PERIOD_MS);
    rb = fread(dst, 1, 2, stdin);           // try again
    TEST_ASSERT_EQUAL(2, rb);               // get two characters
    TEST_ASSERT_EQUAL_UINT8_ARRAY("\r1", dst, 2);
    dst += rb;
    
    fwrite_str_loopback(send_str + 13, 6);  // Send the rest
    vTaskDelay(10 / portTICK_PERIOD_MS);

    rb = fread(dst, 1, 4, stdin);           // consume "23\r\n"
    TEST_ASSERT_EQUAL(4, rb);
    TEST_ASSERT_EQUAL_UINT8_ARRAY("23\r\n", dst, 4);
    dst += rb;

    rb = fread(dst, 1, 4, stdin);           // ask for more than the remainder
    TEST_ASSERT_EQUAL(2, rb);
    TEST_ASSERT_EQUAL_UINT8_ARRAY("4\n", dst, 2);
}

TEST_CASE("can write to UART while another task is reading", "[vfs]")
{
    struct read_task_arg_t {
        char* out_buffer;
        size_t out_buffer_len;
        SemaphoreHandle_t ready;
        SemaphoreHandle_t done;
    };

    struct write_task_arg_t {
        const char* str;
        SemaphoreHandle_t done;
    };

    void read_task_fn(void* varg)
    {
        struct read_task_arg_t* parg = (struct read_task_arg_t*) varg;
        parg->out_buffer[0] = 0;

        fgets(parg->out_buffer, parg->out_buffer_len, stdin);
        xSemaphoreGive(parg->done);
        vTaskDelete(NULL);
    }

    void write_task_fn(void* varg)
    {
        struct write_task_arg_t* parg = (struct write_task_arg_t*) varg;
        fwrite_str_loopback(parg->str, strlen(parg->str));
        xSemaphoreGive(parg->done);
        vTaskDelete(NULL);
    }

    char out_buffer[32];
    size_t out_buffer_len = sizeof(out_buffer);

    struct read_task_arg_t read_arg = {
            .out_buffer = out_buffer,
            .out_buffer_len = out_buffer_len,
            .done = xSemaphoreCreateBinary()
    };

    struct write_task_arg_t write_arg = {
            .str = "!(@*#&(!*@&#((SDasdkjhadsl\n",
            .done = xSemaphoreCreateBinary()
    };

    flush_stdin_stdout();

    ESP_ERROR_CHECK( uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM,
            256, 0, 0, NULL, 0) );
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);


    xTaskCreate(&read_task_fn, "vfs_read", 4096, &read_arg, 5, NULL);
    vTaskDelay(10);
    xTaskCreate(&write_task_fn, "vfs_write", 4096, &write_arg, 6, NULL);


    int res = xSemaphoreTake(write_arg.done, 100 / portTICK_PERIOD_MS);
    TEST_ASSERT(res);

    res = xSemaphoreTake(read_arg.done, 100 / portTICK_PERIOD_MS);
    TEST_ASSERT(res);

    TEST_ASSERT_EQUAL(0, strcmp(write_arg.str, read_arg.out_buffer));

    esp_vfs_dev_uart_use_nonblocking(CONFIG_ESP_CONSOLE_UART_NUM);
    uart_driver_delete(CONFIG_ESP_CONSOLE_UART_NUM);
    vSemaphoreDelete(read_arg.done);
    vSemaphoreDelete(write_arg.done);
}

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
TEST_CASE("Can use termios for UART", "[vfs]")
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_driver_install(UART_NUM_1, 256, 256, 0, NULL, 0);

    const int uart_fd = open("/dev/uart/1", O_RDWR);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(uart_fd, -1, "Cannot open UART");
    esp_vfs_dev_uart_use_driver(1);

    TEST_ASSERT_EQUAL(-1, tcgetattr(uart_fd, NULL));
    TEST_ASSERT_EQUAL(EINVAL, errno);

    struct termios tios, tios_result;

    TEST_ASSERT_EQUAL(-1, tcgetattr(-1, &tios));
    TEST_ASSERT_EQUAL(EBADF, errno);

    TEST_ASSERT_EQUAL(0, tcgetattr(uart_fd, &tios));

    TEST_ASSERT_EQUAL(0, tcsetattr(uart_fd, TCSADRAIN, &tios));
    TEST_ASSERT_EQUAL(0, tcsetattr(uart_fd, TCSAFLUSH, &tios));

    tios.c_iflag |= IGNCR;
    TEST_ASSERT_EQUAL(0, tcsetattr(uart_fd, TCSANOW, &tios));
    tios.c_iflag &= (~IGNCR);
    TEST_ASSERT_EQUAL(0, tcgetattr(uart_fd, &tios_result));
    TEST_ASSERT_EQUAL(IGNCR, tios_result.c_iflag & IGNCR);
    memset(&tios_result, 0xFF, sizeof(struct termios));

    tios.c_iflag |= ICRNL;
    TEST_ASSERT_EQUAL(0, tcsetattr(uart_fd, TCSANOW, &tios));
    tios.c_iflag &= (~ICRNL);
    TEST_ASSERT_EQUAL(0, tcgetattr(uart_fd, &tios_result));
    TEST_ASSERT_EQUAL(ICRNL, tios_result.c_iflag & ICRNL);
    memset(&tios_result, 0xFF, sizeof(struct termios));

    {
        uart_word_length_t data_bit;
        uart_stop_bits_t stop_bits;
        uart_parity_t parity_mode;

        tios.c_cflag &= (~CSIZE);
        tios.c_cflag &= (~CSTOPB);
        tios.c_cflag &= (~PARENB);
        tios.c_cflag |= CS6;
        TEST_ASSERT_EQUAL(0, tcsetattr(uart_fd, TCSANOW, &tios));
        tios.c_cflag &= (~CSIZE);
        TEST_ASSERT_EQUAL(0, tcgetattr(uart_fd, &tios_result));
        TEST_ASSERT_EQUAL(CS6, tios_result.c_cflag & CS6);
        TEST_ASSERT_EQUAL(ESP_OK, uart_get_word_length(UART_NUM_1, &data_bit));
        TEST_ASSERT_EQUAL(UART_DATA_6_BITS, data_bit);
        TEST_ASSERT_EQUAL(0, tios_result.c_cflag & CSTOPB);
        TEST_ASSERT_EQUAL(ESP_OK, uart_get_stop_bits(UART_NUM_1, &stop_bits));
        TEST_ASSERT_EQUAL(UART_STOP_BITS_1, stop_bits);
        TEST_ASSERT_EQUAL(ESP_OK, uart_get_parity(UART_NUM_1, &parity_mode));
        TEST_ASSERT_EQUAL(UART_PARITY_DISABLE, parity_mode);
        memset(&tios_result, 0xFF, sizeof(struct termios));
    }

    {
        uart_stop_bits_t stop_bits;
        uart_parity_t parity_mode;

        tios.c_cflag |= CSTOPB;
        tios.c_cflag |= (PARENB | PARODD);
        TEST_ASSERT_EQUAL(0, tcsetattr(uart_fd, TCSANOW, &tios));
        tios.c_cflag &= (~(CSTOPB | PARENB | PARODD));
        TEST_ASSERT_EQUAL(0, tcgetattr(uart_fd, &tios_result));
        TEST_ASSERT_EQUAL(CSTOPB, tios_result.c_cflag & CSTOPB);
        TEST_ASSERT_EQUAL(ESP_OK, uart_get_stop_bits(UART_NUM_1, &stop_bits));
        TEST_ASSERT_EQUAL(UART_STOP_BITS_2, stop_bits);
        TEST_ASSERT_EQUAL(ESP_OK, uart_get_parity(UART_NUM_1, &parity_mode));
        TEST_ASSERT_EQUAL(UART_PARITY_ODD, parity_mode);
        memset(&tios_result, 0xFF, sizeof(struct termios));
    }

    {
        uint32_t baudrate;

        tios.c_cflag &= (~BOTHER);
        tios.c_cflag |= CBAUD;
        tios.c_ispeed = tios.c_ospeed = B38400;
        TEST_ASSERT_EQUAL(0, tcsetattr(uart_fd, TCSANOW, &tios));
        TEST_ASSERT_EQUAL(0, tcgetattr(uart_fd, &tios_result));
        TEST_ASSERT_EQUAL(CBAUD, tios_result.c_cflag & CBAUD);
        TEST_ASSERT_EQUAL(B38400, tios_result.c_ispeed);
        TEST_ASSERT_EQUAL(B38400, tios_result.c_ospeed);
        TEST_ASSERT_EQUAL(ESP_OK, uart_get_baudrate(UART_NUM_1, &baudrate));
        TEST_ASSERT_EQUAL(38400, baudrate);

        tios.c_cflag |= CBAUDEX;
        tios.c_ispeed = tios.c_ospeed = B230400;
        TEST_ASSERT_EQUAL(0, tcsetattr(uart_fd, TCSANOW, &tios));
        TEST_ASSERT_EQUAL(0, tcgetattr(uart_fd, &tios_result));
        TEST_ASSERT_EQUAL(BOTHER, tios_result.c_cflag & BOTHER);
        // Setting the speed to 230400 will set it actually to 230423
        TEST_ASSERT_EQUAL(230423, tios_result.c_ispeed);
        TEST_ASSERT_EQUAL(230423, tios_result.c_ospeed);
        TEST_ASSERT_EQUAL(ESP_OK, uart_get_baudrate(UART_NUM_1, &baudrate));
        TEST_ASSERT_EQUAL(230423, baudrate);

        tios.c_cflag |= BOTHER;
        tios.c_ispeed = tios.c_ospeed = 213;
        TEST_ASSERT_EQUAL(0, tcsetattr(uart_fd, TCSANOW, &tios));
        TEST_ASSERT_EQUAL(0, tcgetattr(uart_fd, &tios_result));
        TEST_ASSERT_EQUAL(BOTHER, tios_result.c_cflag & BOTHER);
        TEST_ASSERT_EQUAL(213, tios_result.c_ispeed);
        TEST_ASSERT_EQUAL(213, tios_result.c_ospeed);
        TEST_ASSERT_EQUAL(ESP_OK, uart_get_baudrate(UART_NUM_1, &baudrate));
        TEST_ASSERT_EQUAL(213, baudrate);

        memset(&tios_result, 0xFF, sizeof(struct termios));
    }

    esp_vfs_dev_uart_use_nonblocking(1);
    close(uart_fd);
    uart_driver_delete(UART_NUM_1);
}
#endif // CONFIG_VFS_SUPPORT_TERMIOS
