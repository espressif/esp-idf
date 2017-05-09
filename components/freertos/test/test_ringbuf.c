/*
 Test for multicore FreeRTOS ringbuffer.
*/

#include <esp_types.h>
#include <stdio.h>
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/uart_reg.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"

#include <string.h>
#include <stdio.h>

void ets_isr_unmask(uint32_t unmask);

static RingbufHandle_t rb;
typedef enum {
    TST_MOSTLYFILLED,
    TST_MOSTLYEMPTY,
    TST_INTTOTASK,
    TST_TASKTOINT,
} testtype_t;

static volatile testtype_t testtype;

static void task1(void *arg)
{
    testtype_t oldtest;
    char buf[100];
    int i = 0;
    int x, r;
    while (1) {
        oldtest = testtype;
        if (testtype == TST_MOSTLYFILLED || testtype == TST_MOSTLYEMPTY) {
            for (x = 0; x < 10; x++) {
                sprintf(buf, "This is test %d item %d.", (int)testtype,  i++);
                ets_printf("TSK w");
                xRingbufferPrintInfo(rb);
                r = xRingbufferSend(rb, buf, strlen(buf) + 1, 2000 / portTICK_PERIOD_MS);
                if (!r) {
                    printf("Test %d: Timeout on send!\n", (int)testtype);
                }
                if (testtype == TST_MOSTLYEMPTY) {
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }
            }
            //Send NULL event to stop other side.
            r = xRingbufferSend(rb, NULL, 0, 10000 / portTICK_PERIOD_MS);
        }
        while (oldtest == testtype) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

static void task2(void *arg)
{
    testtype_t oldtest;
    char *buf;
    size_t len;
    while (1) {
        oldtest = testtype;
        if (testtype == TST_MOSTLYFILLED || testtype == TST_MOSTLYEMPTY) {
            while (1) {
                ets_printf("TSK r");
                xRingbufferPrintInfo(rb);
                buf = xRingbufferReceive(rb, &len, 2000 / portTICK_PERIOD_MS);
                if (buf == NULL) {
                    printf("Test %d: Timeout on recv!\n", (int)testtype);
                } else if (len == 0) {
                    printf("End packet received.\n");
                    vRingbufferReturnItem(rb, buf);
                    break;
                } else {
                    printf("Received: %s (%d bytes, %p)\n", buf, len, buf);
                    vRingbufferReturnItem(rb, buf);
                }
                if (testtype == TST_MOSTLYFILLED) {
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }
            }
        }
        while (oldtest == testtype) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}



static void uartIsrHdl(void *arg)
{
    char c;
    char buf[50];
    char *item;
    int r;
    size_t len;
    BaseType_t xHigherPriorityTaskWoken;
    SET_PERI_REG_MASK(UART_INT_CLR_REG(0), UART_RXFIFO_FULL_INT_CLR);
    while (READ_PERI_REG(UART_STATUS_REG(0)) & (UART_RXFIFO_CNT << UART_RXFIFO_CNT_S)) {
        c = READ_PERI_REG(UART_FIFO_REG(0));
        if (c == 'r') {
            ets_printf("ISR r");
            xRingbufferPrintInfo(rb);
            item = xRingbufferReceiveFromISR(rb, &len);
            if (item == NULL) {
                ets_printf("ISR recv fail!\n");
            } else if (len == 0) {
                ets_printf("ISR recv NULL!\n");
                vRingbufferReturnItemFromISR(rb, item, &xHigherPriorityTaskWoken);
            } else {
                ets_printf("ISR recv '%s' (%d bytes, %p)\n", buf, len, buf);
                vRingbufferReturnItemFromISR(rb, item, &xHigherPriorityTaskWoken);
            }
        } else {
            sprintf(buf, "UART: %c", c);
            ets_printf("ISR w");
            xRingbufferPrintInfo(rb);
            r = xRingbufferSendFromISR(rb, buf, strlen(buf) + 1, &xHigherPriorityTaskWoken);
            if (!r) {
                ets_printf("ISR send fail\n");
            }
        }
    }
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

static void uartRxInit()
{
    uint32_t reg_val;
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_U0RXD);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD_U0TXD);

//  reg_val = READ_PERI_REG(UART_CONF1(0));
    reg_val = (1 << UART_RXFIFO_FULL_THRHD_S);
    WRITE_PERI_REG(UART_CONF1_REG(0), reg_val);
    CLEAR_PERI_REG_MASK(UART_INT_ENA_REG(0), UART_TXFIFO_EMPTY_INT_ENA | UART_RXFIFO_TOUT_INT_ENA);
    SET_PERI_REG_MASK(UART_INT_ENA_REG(0), UART_RXFIFO_FULL_INT_ENA);

    printf("Enabling int %d\n", ETS_UART0_INUM);
    DPORT_REG_SET_FIELD(DPORT_PRO_UART_INTR_MAP_REG, DPORT_PRO_UART_INTR_MAP, ETS_UART0_INUM);
    DPORT_REG_SET_FIELD(DPORT_PRO_UART1_INTR_MAP_REG, DPORT_PRO_UART1_INTR_MAP, ETS_UART0_INUM);

    xt_set_interrupt_handler(ETS_UART0_INUM, uartIsrHdl, NULL);
    xt_ints_on(1 << ETS_UART0_INUM);

}

static void testRingbuffer(int type)
{
    TaskHandle_t th[2];
    int i;
    rb = xRingbufferCreate(32 * 3, type);

    testtype = TST_MOSTLYFILLED;

    xTaskCreatePinnedToCore(task1  , "tskone"  , 2048, NULL, 3, &th[0], 0);
    xTaskCreatePinnedToCore(task2  , "tsktwo"  , 2048, NULL, 3, &th[1], 0);
    uartRxInit();

    printf("Press 'r' to read an event in isr, any other key to write one.\n");
    printf("Test: mostlyfilled; putting 10 items in ringbuff ASAP, reading 1 a second\n");
    vTaskDelay(15000 / portTICK_PERIOD_MS);
    printf("Test: mostlyempty; putting 10 items in ringbuff @ 1/sec, reading as fast as possible\n");
    testtype = TST_MOSTLYEMPTY;
    vTaskDelay(15000 / portTICK_PERIOD_MS);

    //Shut down all the tasks
    for (i = 0; i < 2; i++) {
        vTaskDelete(th[i]);
    }
    xt_ints_off(1 << ETS_UART0_INUM);
}

// TODO: split this thing into separate orthogonal tests
TEST_CASE("FreeRTOS ringbuffer test, no splitting items", "[freertos][ignore]")
{
    testRingbuffer(0);
}

TEST_CASE("FreeRTOS ringbuffer test, w/ splitting items", "[freertos][ignore]")
{
    testRingbuffer(1);
}

