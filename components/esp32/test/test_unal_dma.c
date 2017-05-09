
#include <esp_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rom/ets_sys.h"
#include "rom/lldesc.h"
#include "rom/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"

#include "soc/uart_reg.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_reg.h"
#include "soc/i2s_reg.h"


#define DPORT_I2S0_CLK_EN   (BIT(4))
#define DPORT_I2S0_RST   (BIT(4))

static volatile lldesc_t dmaDesc[2];


//hacked up routine to essentially do a memcpy() using dma. Supports max 4K-1 bytes.
static void dmaMemcpy(void *in, void *out, int len)
{
    volatile int i;
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_I2S0_CLK_EN);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_I2S0_RST);

    //Init pins to i2s functions
    SET_PERI_REG_MASK(GPIO_ENABLE_W1TS_REG, (1 << 11) | (1 << 3) | (1 << 0) | (1 << 2) | (1 << 5) | (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19) | (1 << 20)); //ENABLE GPIO oe_enable

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, 0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, 0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, 0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO16_U, 0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO17_U, 0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO18_U, 0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO19_U, 0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO20_U, 0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CMD_U, 2); //11
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO26_U, 0); //RS

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

    WRITE_PERI_REG(I2S_CONF_REG(0), 0);//I2S_I2S_SIG_LOOPBACK);
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

//--
    //Fill DMA descriptor
    dmaDesc[0].length = len;
    dmaDesc[0].size = len;
    dmaDesc[0].owner = 1;
    dmaDesc[0].sosf = 0;
    dmaDesc[0].buf = (uint8_t *)in;
    dmaDesc[0].offset = 0; //unused in hw
    dmaDesc[0].empty = 0;
    dmaDesc[0].eof = 1;
    dmaDesc[1].length = len;
    dmaDesc[1].size = len;
    dmaDesc[1].owner = 1;
    dmaDesc[1].sosf = 0;
    dmaDesc[1].buf = (uint8_t *)out;
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
//--
    //No need to finish if no DMA transfer going on
    if (!(READ_PERI_REG(I2S_FIFO_CONF_REG(0))&I2S_DSCR_EN)) {
        return;
    }

    //Wait till fifo done
    while (!(READ_PERI_REG(I2S_INT_RAW_REG(0))&I2S_TX_REMPTY_INT_RAW)) ;
    //Wait for last bytes to leave i2s xmit thing
    //ToDo: poll bit in next hw
    for (i = 0; i < (1 << 8); i++);
    while (!(READ_PERI_REG(I2S_STATE_REG(0))&I2S_TX_IDLE));

    //Reset I2S for next transfer
    CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_START | I2S_RX_START);
    CLEAR_PERI_REG_MASK(I2S_OUT_LINK_REG(0), I2S_OUTLINK_START | I2S_INLINK_START);

    SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_RESET | I2S_TX_FIFO_RESET | I2S_RX_RESET | I2S_RX_FIFO_RESET);
    CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_TX_RESET | I2S_TX_FIFO_RESET | I2S_RX_RESET | I2S_RX_FIFO_RESET);

//  for (i=0; i<(1<<8); i++);
    while ((READ_PERI_REG(I2S_STATE_REG(0))&I2S_TX_FIFO_RESET_BACK));

}


int mymemcmp(char *a, char *b, int len)
{
    int x;
    for (x = 0; x < len; x++) {
        if (a[x] != b[x]) {
            printf("Not equal at byte %d. a=%x, b=%x\n", x, (int)a[x], (int)b[x]);
            return 1;
        }
    }
    return 0;
}



TEST_CASE("Unaligned DMA test (needs I2S)", "[hw][ignore]")
{
    int x;
    char src[2049], dest[2049];
    for (x = 0; x < sizeof(src); x++) {
        src[x] = x & 0xff;
    }

    printf("Aligned dma\n");
    memset(dest, 0, 2049);
    dmaMemcpy(src, dest, 2048 + 1);
    TEST_ASSERT(mymemcmp(src, dest, 2048) == 0);
    printf("Src unaligned\n");
    dmaMemcpy(src + 1, dest, 2048 + 1);
    TEST_ASSERT(mymemcmp(src + 1, dest, 2048) == 0);
    printf("Dst unaligned\n");
    dmaMemcpy(src, dest + 1, 2048 + 2);
    TEST_ASSERT(mymemcmp(src, dest + 1, 2048) == 0);
}

