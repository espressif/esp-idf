
#include <esp_types.h>
#include <stdio.h>
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/uart_reg.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"


/*
This test tests the 'fast' peripherial bus at 0x3ff40000. This bus is connected directly to the core, and as such
can receive 'speculative' reads, that is, reads that may or may not actually be executed in the code flow. This
may mess with any FIFOs mapped in the region: if a byte gets dropped due to a missed speculative read, the fifo
may advance to the next byte anyway.

This code tests reading/writing from the UART1 FIFO, using both cores. For this to work, it's required that the
UARTs RX and TX lines are connected.
*/


void test_fastbus_cp(int fifo_addr, unsigned char *buf, int len, int *dummy);

static volatile int state = 0;
static volatile int xor = 0;
static unsigned char res[128];

static void tskOne(void *pvParameters)
{
    int run = 0, err = 0;
    int x;
    int ct[256];
    volatile int w;
    int dummy;
    while (1) {
        state = 1;
        for (x = 0; x < 64; x++) {
            WRITE_PERI_REG(UART_FIFO_REG(1), x ^ xor);
        }
        for (w = 0; w < (1 << 14); w++); //delay
        state = 2;
        test_fastbus_cp(UART_FIFO_REG(1), &res[0], 64, &dummy);
        for (w = 0; w < (1 << 10); w++);    //delay
        for (x = 0; x < 255; x++) {
            ct[x] = 0;    //zero ctrs
        }
        for (x = 0; x < 128; x++) {
            ct[(int)res[x]^xor]++;    //count values
        }
        for (x = 0; x < 255; x++) {         //check counts
            if (ct[x] != (x < 128 ? 1 : 0)) {
                //Disregard first few loops; there may be crap in the fifo.
                if (run > 2) {
                    err++;
                    printf("Error! Received value %d %d times!\n", x, ct[x]);
                }
            }
        }
        run++;
        if ((run & 255) == 0) {
            printf("Loop %d errct %d\n", run, err);
        }
        xor = (xor + 1) & 0xff;
    }
}

#define FB2ADDR 0x40098000

static void tskTwo(void *pvParameters)
{
    int x;
    int dummy;
    int *p = (int *)FB2ADDR;
    int *s = (int *)test_fastbus_cp;
    for (x = 0; x < 100; x++) {
        *p++ = *s++;
    }
    void (*test_fastbus_cp2)(int fifo_addr, unsigned char * buf, int len, int * dummy) = (void *)FB2ADDR;


    while (1) {
        while (state != 1) ;
        for (x = 64; x < 128; x++) {
            WRITE_PERI_REG(UART_FIFO_REG(1), x ^ xor);
        }
        while (state != 2);
        test_fastbus_cp2(UART_FIFO_REG(1), &res[64], 64, &dummy);
    }
}


// TODO: split this thing into separate orthogonal tests
TEST_CASE("Fast I/O bus test", "[hw][ignore]")
{
    int i;
    if ((REG_UART_BASE(0) >> 16) != 0x3ff4) {
        printf("Error! Uart base isn't on fast bus.\n");
        TEST_ASSERT(0);
    }

    PIN_PULLUP_DIS(PERIPHS_IO_MUX_SD_DATA3_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA2_U, FUNC_SD_DATA2_U1RXD);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA3_U, FUNC_SD_DATA3_U1TXD);

    int reg_val = (1 << UART_RXFIFO_FULL_THRHD_S);
    WRITE_PERI_REG(UART_CONF1_REG(1), reg_val);
    WRITE_PERI_REG(UART_CLKDIV_REG(1), 0x30); //semi-random
//  CLEAR_PERI_REG_MASK(UART_INT_ENA_REG(1), UART_TXFIFO_EMPTY_INT_ENA|UART_RXFIFO_TOUT_INT_ENA);

    TaskHandle_t th[2];
    printf("Creating tasks\n");
    xTaskCreatePinnedToCore(tskOne  , "tskone"  , 2048, NULL, 3, &th[0], 0);
    xTaskCreatePinnedToCore(tskTwo  , "tsktwo"  , 2048, NULL, 3, &th[1], 1);

    // Let stuff run for 20s
    while (1) {
        vTaskDelay(20000 / portTICK_PERIOD_MS);
    }

    //Shut down all the tasks
    for (i = 0; i < 2; i++) {
        vTaskDelete(th[i]);
    }
    xt_ints_off(1 << ETS_UART0_INUM);
}

