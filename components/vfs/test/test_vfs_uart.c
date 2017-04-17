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
#include "unity.h"
#include "rom/uart.h"
#include "soc/uart_struct.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

static void fwrite_str_loopback(const char* str, size_t size)
{
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);
    UART0.conf0.loopback = 1;
    fwrite(str, 1, size, stdout);
    fflush(stdout);
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);
    vTaskDelay(2 / portTICK_PERIOD_MS);
    UART0.conf0.loopback = 0;
}

static void flush_stdin_stdout()
{
    vTaskDelay(10 / portTICK_PERIOD_MS);
    char *bitbucket = (char*) 0x3f000000;
    while (fread(bitbucket, 1, 128, stdin) > 0) {
        ;
    }
    fflush(stdout);
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);
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


#if CONFIG_NEWLIB_STDOUT_ADDCR

TEST_CASE("CRs are removed from the stdin correctly", "[vfs]")
{
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

#endif //CONFIG_NEWLIB_STDOUT_ADDCR

