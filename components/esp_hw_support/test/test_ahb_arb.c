/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32

#include <esp_types.h>
#include <stdio.h>
#include <stdlib.h>
#include "esp32/rom/lldesc.h"
#include "esp_private/periph_ctrl.h"
#include "hal/gpio_hal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"

#include "unity.h"

#include "soc/uart_periph.h"
#include "soc/dport_reg.h"
#include "soc/gpio_periph.h"
#include "soc/i2s_periph.h"



#define DPORT_I2S0_CLK_EN   (BIT(4))
#define DPORT_I2S0_RST   (BIT(4))


/*
This test tests the s32c1i instruction when the AHB bus is also used. To create some AHB traffic, we use the I2S interface
to copy bytes over from one memory location to another. DO NOT USE the i2s routines inhere, they've been trial-and-error'ed until
the point where they happened to do what I want.
*/

static void lcdIfaceInit(void)
{
    periph_module_enable(PERIPH_I2S0_MODULE);

    //Init pins to i2s functions
    SET_PERI_REG_MASK(GPIO_ENABLE_W1TS_REG, (1 << 11) | (1 << 3) | (1 << 0) | (1 << 2) | (1 << 5) | (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19) | (1 << 20)); //ENABLE GPIO oe_enable

    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_GPIO0_U, 0);
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_GPIO2_U, 0);
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_GPIO5_U, 0);
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_GPIO16_U, 0);
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_GPIO17_U, 0);
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_GPIO18_U, 0);
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_GPIO19_U, 0);
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_GPIO20_U, 0);
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_SD_CMD_U, 2); //11
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_GPIO26_U, 0); //RS

    WRITE_PERI_REG(GPIO_FUNC0_OUT_SEL_CFG_REG, (148 << GPIO_FUNC0_OUT_SEL_S));
    WRITE_PERI_REG(GPIO_FUNC2_OUT_SEL_CFG_REG, (149 << GPIO_FUNC0_OUT_SEL_S));
    WRITE_PERI_REG(GPIO_FUNC5_OUT_SEL_CFG_REG, (150 << GPIO_FUNC0_OUT_SEL_S));
    WRITE_PERI_REG(GPIO_FUNC16_OUT_SEL_CFG_REG, (151 << GPIO_FUNC0_OUT_SEL_S));
    WRITE_PERI_REG(GPIO_FUNC17_OUT_SEL_CFG_REG, (152 << GPIO_FUNC0_OUT_SEL_S));
    WRITE_PERI_REG(GPIO_FUNC18_OUT_SEL_CFG_REG, (153 << GPIO_FUNC0_OUT_SEL_S));
    WRITE_PERI_REG(GPIO_FUNC19_OUT_SEL_CFG_REG, (154 << GPIO_FUNC0_OUT_SEL_S));
    WRITE_PERI_REG(GPIO_FUNC20_OUT_SEL_CFG_REG, (155 << GPIO_FUNC0_OUT_SEL_S));
    WRITE_PERI_REG(GPIO_FUNC26_OUT_SEL_CFG_REG, (156 << GPIO_FUNC0_OUT_SEL_S)); //RS
    WRITE_PERI_REG(GPIO_FUNC11_OUT_SEL_CFG_REG, (I2S0O_WS_OUT_IDX << GPIO_FUNC0_OUT_SEL_S));
//  WRITE_PERI_REG(GPIO_FUNC11_OUT_SEL_CFG, (I2S0O_BCK_OUT_IDX<<GPIO_GPIO_FUNC0_OUT_SEL_S));

    //GPIO_SET_GPIO_FUNC11_OUT_INV_SEL(1); //old
    WRITE_PERI_REG(GPIO_FUNC11_OUT_SEL_CFG_REG, READ_PERI_REG(GPIO_FUNC11_OUT_SEL_CFG_REG) | GPIO_FUNC11_OUT_INV_SEL);

    //Reset I2S subsystem
    CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_RX_RESET | I2S_TX_RESET);
    SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_RX_RESET | I2S_TX_RESET);
    CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_RX_RESET | I2S_TX_RESET);

    WRITE_PERI_REG(I2S_CONF_REG(0), 0);//I2S_SIG_LOOPBACK);
    WRITE_PERI_REG(I2S_CONF2_REG(0), 0);

    WRITE_PERI_REG(I2S_SAMPLE_RATE_CONF_REG(0),
                   (16 << I2S_RX_BITS_MOD_S) |
                   (16 << I2S_TX_BITS_MOD_S) |
                   (1 << I2S_RX_BCK_DIV_NUM_S) |
                   (1 << I2S_TX_BCK_DIV_NUM_S));
    WRITE_PERI_REG(I2S_CLKM_CONF_REG(0),
                   I2S_CLKA_ENA | I2S_CLK_EN |
                   (1 << I2S_CLKM_DIV_A_S) |
                   (1 << I2S_CLKM_DIV_B_S) |
                   (1 << I2S_CLKM_DIV_NUM_S));
    WRITE_PERI_REG(I2S_FIFO_CONF_REG(0),
                   (32 << I2S_TX_DATA_NUM_S) |     //Low watermark for IRQ
                   (32 << I2S_RX_DATA_NUM_S));

    WRITE_PERI_REG(I2S_CONF1_REG(0), I2S_RX_PCM_BYPASS | I2S_TX_PCM_BYPASS);

    WRITE_PERI_REG(I2S_CONF_CHAN_REG(0), (2 << I2S_TX_CHAN_MOD_S) | (2 << I2S_RX_CHAN_MOD_S));

    //Invert WS to active-low
    SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_RIGHT_FIRST | I2S_RX_RIGHT_FIRST);
    WRITE_PERI_REG(I2S_TIMING_REG(0), 0);
}


static volatile lldesc_t dmaDesc[2];

static void finishDma(void)
{
    //No need to finish if no DMA transfer going on
    if (!(READ_PERI_REG(I2S_FIFO_CONF_REG(0))&I2S_DSCR_EN)) {
        return;
    }

    //Wait till fifo done
    while (!(READ_PERI_REG(I2S_INT_RAW_REG(0))&I2S_TX_REMPTY_INT_RAW)) ;
    //Wait for last bytes to leave i2s xmit thing
    //ToDo: poll bit in next hw
//  for (i=0; i<(1<<8); i++);
    while (!(READ_PERI_REG(I2S_STATE_REG(0))&I2S_TX_IDLE));

    //Reset I2S for next transfer
    CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_START | I2S_RX_START);
    CLEAR_PERI_REG_MASK(I2S_OUT_LINK_REG(0), I2S_OUTLINK_START | I2S_INLINK_START);

    SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_RESET | I2S_TX_FIFO_RESET | I2S_RX_RESET | I2S_RX_FIFO_RESET);
    CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_RESET | I2S_TX_FIFO_RESET | I2S_RX_RESET | I2S_RX_FIFO_RESET);

//  for (i=0; i<(1<<8); i++);
    while ((READ_PERI_REG(I2S_STATE_REG(0))&I2S_TX_FIFO_RESET_BACK));
}


/*
This is a very, very, very hacked up LCD routine which ends up basically doing a memcpy from sbuf to rbuf.
*/
static void sendRecvBufDma(uint16_t *sbuf, uint16_t *rbuf, int len)
{
    //Fill DMA descriptor
    dmaDesc[0].length = len * 2;
    dmaDesc[0].size = len * 2;
    dmaDesc[0].owner = 1;
    dmaDesc[0].sosf = 0;
    dmaDesc[0].buf = (uint8_t *)sbuf;
    dmaDesc[0].offset = 0; //unused in hw
    dmaDesc[0].empty = 0;
    dmaDesc[0].eof = 1;
    dmaDesc[1].length = len * 2;
    dmaDesc[1].size = len * 2;
    dmaDesc[1].owner = 1;
    dmaDesc[1].sosf = 0;
    dmaDesc[1].buf = (uint8_t *)rbuf;
    dmaDesc[1].offset = 0; //unused in hw
    dmaDesc[1].empty = 0;
    dmaDesc[1].eof = 1;

    //Reset DMA
    SET_PERI_REG_MASK(I2S_LC_CONF_REG(0), I2S_IN_RST | I2S_OUT_RST | I2S_AHBM_RST | I2S_AHBM_FIFO_RST);
    CLEAR_PERI_REG_MASK(I2S_LC_CONF_REG(0), I2S_IN_RST | I2S_OUT_RST | I2S_AHBM_RST | I2S_AHBM_FIFO_RST);

    //Reset I2S FIFO
    SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_RX_RESET | I2S_TX_RESET | I2S_TX_FIFO_RESET | I2S_RX_FIFO_RESET);
    CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_RX_RESET | I2S_TX_RESET | I2S_TX_FIFO_RESET | I2S_RX_FIFO_RESET);

    //Set desc addr
    CLEAR_PERI_REG_MASK(I2S_OUT_LINK_REG(0), I2S_OUTLINK_ADDR);
    SET_PERI_REG_MASK(I2S_OUT_LINK_REG(0), ((uint32_t)(&dmaDesc[0]))&I2S_OUTLINK_ADDR);
    CLEAR_PERI_REG_MASK(I2S_IN_LINK_REG(0), I2S_INLINK_ADDR);
    SET_PERI_REG_MASK(I2S_IN_LINK_REG(0), ((uint32_t)(&dmaDesc[1]))&I2S_INLINK_ADDR);

    SET_PERI_REG_MASK(I2S_FIFO_CONF_REG(0), I2S_DSCR_EN); //Enable DMA mode

    WRITE_PERI_REG(I2S_RXEOF_NUM_REG(0), len);

    //Enable and configure DMA
    WRITE_PERI_REG(I2S_LC_CONF_REG(0), I2S_OUT_DATA_BURST_EN  |
                   I2S_OUT_EOF_MODE | I2S_OUTDSCR_BURST_EN | I2S_OUT_DATA_BURST_EN |
                   I2S_INDSCR_BURST_EN | I2S_MEM_TRANS_EN);

    //Start transmission
    SET_PERI_REG_MASK(I2S_OUT_LINK_REG(0), I2S_OUTLINK_START);
    SET_PERI_REG_MASK(I2S_IN_LINK_REG(0), I2S_INLINK_START);

    SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_START | I2S_RX_START);
    //Clear int flags
    WRITE_PERI_REG(I2S_INT_CLR_REG(0), 0xFFFFFFFF);
}


#define DMALEN (2048-2)

static void tskLcd(void *pvParameters)
{
    uint16_t *sbuf = malloc(DMALEN * 2);
    uint16_t *rbuf = malloc(DMALEN * 2);
    uint16_t xorval = 0;
    int x;
    lcdIfaceInit();
//  lcdFlush();
    while (1) {
        for (x = 0; x < DMALEN; x++) {
            sbuf[x] = x ^ xorval;
        }
        for (x = 0; x < DMALEN; x++) {
            rbuf[x] = 0;    //clear rbuf
        }
        sendRecvBufDma(sbuf, rbuf, DMALEN);
        vTaskDelay(20 / portTICK_PERIOD_MS);
        finishDma();
        for (x = 0; x < DMALEN; x++) if (rbuf[x] != (x ^ xorval)) {
                printf("Rxbuf err! pos %d val %x xor %x", x, (int)rbuf[x], (int)xorval);
            }
        printf(".");
        fflush(stdout);
        xorval++;
    }
}



void test_s32c1i_lock(volatile int *lockvar, int lockval, int unlockval, volatile int *ctr);

static volatile int ctr = 0, state = 0;
static volatile int lock = 0;

static void tskOne(void *pvParameters)
{
    int x;
    int err = 0, run = 0;
    while (1) {
        ctr = 0; lock = 0;
        state = 1;
        for (x = 0; x < 16 * 1024; x++) {
            test_s32c1i_lock(&lock, 1, 0, &ctr);
        }
        vTaskDelay(60 / portTICK_PERIOD_MS);
        state = 2;
        if (ctr != 16 * 1024 * 2) {
            printf("Lock malfunction detected! Ctr=0x%x instead of %x\n", ctr, 16 * 1024 * 2);
            err++;
        }
        run++;
        printf("Run %d err %d\n", run, err);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

#define FB2ADDR 0x40098000

static void tskTwo(void *pvParameters)
{
    int x;
    int *p = (int *)FB2ADDR;
    int *s = (int *)test_s32c1i_lock;
    void (*test_s32c1i_lock2)(volatile int * lockvar, int lockval, int unlockval, volatile int * ctr) = (void *)FB2ADDR;
    volatile int w;
    int delay;
    for (x = 0; x < 100; x++) {
        *p++ = *s++;    //copy routine to different pool
    }

    while (1) {
        while (state != 1) ;
        for (x = 0; x < 16 * 1024; x++) {
            test_s32c1i_lock2(&lock, 2, 0, &ctr);
            //Some random delay to increase chance of weirdness
            if ((x & 0x1f) == 0) {
                delay = rand() & 0x1f;
                for (w = 0; w < delay; w++);
            }
        }
        while (state != 2);
    }
}


TEST_CASE("S32C1I vs AHB test (needs I2S)", "[hw][ignore]")
{
    int i;
    TaskHandle_t th[3];
    state = 0;

    printf("Creating tasks\n");
    xTaskCreatePinnedToCore(tskTwo  , "tsktwo"  , 2048, NULL, 3, &th[1], 1);
    xTaskCreatePinnedToCore(tskOne  , "tskone"  , 2048, NULL, 3, &th[0], 0);
    xTaskCreatePinnedToCore(tskLcd  , "tsklcd"  , 2048, NULL, 3, &th[2], 0);

    // Let stuff run for 20s
    while (1) {
        vTaskDelay(20000 / portTICK_PERIOD_MS);
    }

    //Shut down all the tasks
    for (i = 0; i < 3; i++) {
        vTaskDelete(th[i]);
    }
}

#endif // CONFIG_IDF_TARGET_ESP32
