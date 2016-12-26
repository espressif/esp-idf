// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "i2s.h"
#include <string.h>
#include "rom/ets_sys.h"
#include "esp_intr.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/gpio.h"
#include "stdio.h"


#define I2S_BASE_CLK (80000000L)
#define ABS(x) (((x)>0)?(x):(-(x)))

//*****************************************************************************
//
// Make sure all of the definitions in this header have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Set the interrupt function.
 *
 */
static void i2s_intr_func_set(void * isr, i2s_num_t i2sNum)
{
    if (I2S_NUM_I2S0 == i2sNum) {
        intr_matrix_set(0, ETS_I2S0_INTR_SOURCE, ETS_I2S0_INUM);
        ESP_I2S0_INTR_ATTACH(isr, NULL);
        // enable intr in cpu
        ESP_I2S0_INTR_ENABLE();
    } else if (I2S_NUM_I2S1 == i2sNum) {
        intr_matrix_set(0, ETS_I2S1_INTR_SOURCE, ETS_I2S1_INUM);
        ESP_I2S1_INTR_ATTACH(isr, NULL);
        // enable intr in cpu
        ESP_I2S1_INTR_ENABLE();
    } else {
        // To do nothing
    }
}

/**
 * @brief Get I2S ping buffer address.
 *
 */
uint32_t* i2s_dma_ping_buf_get(i2s_dma_attr_t *obj)
{
    if (NULL == obj) {
        return NULL;
    }
    return obj->buf->ping->buffer_addr;
}

/**
 * @brief Get I2S pong buffer address.
 *
 */
uint32_t* i2s_dma_pong_buf_get(i2s_dma_attr_t *obj)
{
    if (NULL == obj) {
        return NULL;
    }
    return obj->buf->pong->buffer_addr;
}

/**
 * @brief Get not working ping-pong buffer address.
 *
 */
uint32_t* i2s_dma_status_get(i2s_num_t i2sNum, i2s_dma_attr_t *obj)
{
    if ((NULL == obj)
        || (i2sNum > I2S_NUM_MAX)) {
        return NULL;
    }
    if ((I2S_MODE_MASTER_RX == obj->mode)
        || (I2S_MODE_SLAVE_RX == obj->mode)) {
        // Data stream receive
        if (READ_PERI_REG(I2S_IN_EOF_DES_ADDR_REG(i2sNum)) == ((uint32_t)obj->buf->ping->last_queue)) {
            return obj->buf->ping->buffer_addr;
        } else if (READ_PERI_REG(I2S_IN_EOF_DES_ADDR_REG(i2sNum)) == ((uint32_t)obj->buf->pong->last_queue)) {
            return obj->buf->pong->buffer_addr;
        }
    } else if ((I2S_MODE_MASTER_TX == obj->mode)
               || (I2S_MODE_SLAVE_TX == obj->mode)) {
        // Data stream send
        if (READ_PERI_REG(I2S_OUT_EOF_DES_ADDR_REG(i2sNum)) == ((uint32_t)obj->buf->ping->last_queue))  {
            return obj->buf->ping->buffer_addr;
        } else if (READ_PERI_REG(I2S_OUT_EOF_DES_ADDR_REG(i2sNum)) == ((uint32_t)obj->buf->pong->last_queue)) {
            return obj->buf->pong->buffer_addr;
        }
    }
    return NULL;

}

/**
 * @brief Set the Ping-Pong buffer to the destination
 *
 */
void i2s_dma_dest_add_set(i2s_num_t i2sNum, i2s_dma_attr_t *obj)
{
    if ((NULL == obj)
        || (i2sNum > I2S_NUM_MAX)) {
        return;
    }
    if ((I2S_MODE_MASTER_RX == obj->mode)
        || (I2S_MODE_SLAVE_RX == obj->mode)) {
        SET_PERI_REG_BITS(I2S_IN_LINK_REG(i2sNum), I2S_INLINK_ADDR, ((uint32_t)(obj->buf->ping->first_queue)), I2S_INLINK_ADDR_S);
    } else if ((I2S_MODE_MASTER_TX == obj->mode)
               || (I2S_MODE_SLAVE_TX == obj->mode)) {
        SET_PERI_REG_BITS(I2S_OUT_LINK_REG(i2sNum), I2S_OUTLINK_ADDR, ((uint32_t)(obj->buf->ping->first_queue)), I2S_OUTLINK_ADDR_S);
    }
}

/**
 * @brief Initialize DMA and create a I2S DMA instance
 *
 */
int i2s_dma_init(i2s_num_t i2sNum, i2s_dma_attr_t *obj, void *isr)
{
    if ((NULL == obj)
        || (i2sNum > I2S_NUM_MAX)) {
        return -1;
    }
    obj->buf = dma_buf_create(obj->buf_size);
    if (NULL == obj->buf) {
        return -2;
    }
    if ((I2S_MODE_MASTER_TX == obj->mode)
        || (I2S_MODE_SLAVE_TX == obj->mode)) {
        //Reset DMA
        SET_PERI_REG_MASK(I2S_LC_CONF_REG(i2sNum), I2S_OUT_RST);
        CLEAR_PERI_REG_MASK(I2S_LC_CONF_REG(i2sNum), I2S_OUT_RST);
        // enable send intr
        i2s_int_enable(i2sNum, I2S_INT_SRC_ONE_BUF_SEND_DONE);
    } else if ((I2S_MODE_MASTER_RX == obj->mode)
               || (I2S_MODE_SLAVE_RX == obj->mode)) {
        //Reset DMA
        SET_PERI_REG_MASK(I2S_LC_CONF_REG(i2sNum), I2S_IN_RST);
        CLEAR_PERI_REG_MASK(I2S_LC_CONF_REG(i2sNum), I2S_IN_RST);
        SET_PERI_REG_BITS(I2S_RXEOF_NUM_REG(i2sNum), I2S_RX_EOF_NUM, (obj->buf_size / 4), I2S_RX_EOF_NUM_S);
        // enable receive intr
        i2s_int_enable(i2sNum, I2S_INT_SRC_ONE_BUF_RECV_DONE /*| I2sIntSrc_RxHalfFull |I2sIntSrc_RxFull*/);
    }

    //Enable and configure DMA
    SET_PERI_REG_MASK(I2S_LC_CONF_REG(i2sNum), I2S_CHECK_OWNER | I2S_OUT_EOF_MODE |
                      I2S_OUT_LOOP_TEST | I2S_IN_LOOP_TEST);
    i2s_intr_func_set(isr, i2sNum);
    return 0;
}

/**
 * @brief Destroy the I2S DMA instance
 *
 */
esp_err_t i2s_dma_uninit(i2s_dma_attr_t *obj)
{
    if (NULL == obj) {
        return ESP_FAIL;
    }
    dma_buf_destroy(obj->buf);
    return ESP_OK;
}

/**
 * @brief Enable the I2S DMA work mode.
 *
 */
void i2s_dma_enable(i2s_num_t i2sNum, i2s_mode_t mode)
{
    if (i2sNum > I2S_NUM_MAX) {
        return;
    }
    if ((I2S_MODE_MASTER_RX == mode)
        || (I2S_MODE_SLAVE_RX == mode)) {
        SET_PERI_REG_MASK(I2S_IN_LINK_REG(i2sNum), I2S_INLINK_START);
    } else if ((I2S_MODE_MASTER_TX == mode)
               || (I2S_MODE_SLAVE_TX == mode)) {
        SET_PERI_REG_MASK(I2S_OUT_LINK_REG(i2sNum), I2S_OUTLINK_START);
    }
}

/**
 * @brief Disable the I2S DMA work mode.
 *
 */
void i2s_dma_disable(i2s_num_t i2sNum, i2s_mode_t mode)
{
    if (i2sNum > I2S_NUM_MAX) {
        return;
    }
    if ((I2S_MODE_MASTER_RX == mode)
        || (I2S_MODE_SLAVE_RX == mode)) {
        SET_PERI_REG_MASK(I2S_IN_LINK_REG(i2sNum), I2S_INLINK_STOP);
    } else if ((I2S_MODE_MASTER_TX == mode)
               || (I2S_MODE_SLAVE_TX == mode)) {
        SET_PERI_REG_MASK(I2S_OUT_LINK_REG(i2sNum), I2S_OUTLINK_STOP);
    }
}

/**
 * @brief Set the I2S sample rate, in HZ
 *
 */
void i2s_clk_set(int rate, int bits)
{
    uint32_t i2sNum = 0 ;
    int bckdiv, factor = 32;
    int clkmInteger, clkmDecimals;
    float denom = (float)1 / 16;
    // BCLK = Fs * bits * 2;
    // MCLK = factor * BCLK; m = 2;
    // clkm = 160MHz / MCLK;
    ets_printf("rate=%d, bits=%d \r\n", rate, bits);
    float clkmdiv = (float)I2S_BASE_CLK / (rate * factor * bits * 2);
    ets_printf("clkmdiv=%f\r\n", clkmdiv);
    if (clkmdiv > 256) {
        factor++;
        clkmdiv = (float)I2S_BASE_CLK / (rate * factor * bits * 2);
    }
    ets_printf("clkmdiv=%f\r\n", clkmdiv);
    clkmInteger = clkmdiv;
    ets_printf("clkmInteger=%d\r\n", clkmInteger);
    clkmDecimals = (clkmdiv - clkmInteger) / denom;
    ets_printf("clkmDecimals=%d\r\n", clkmDecimals);
    float mclk = clkmInteger + denom * clkmDecimals;
    ets_printf("MCLK=%f\r\n", (float)I2S_BASE_CLK / mclk);
    rtc_plla_ena(1,10,2);
#if 0
    CLEAR_PERI_REG_MASK(I2S_CLKM_CONF_REG, (I2S_CLKM_DIV_NUM_M
                                            | I2S_CLKM_DIV_A_M
                                            | I2S_CLKM_DIV_B_M));
    SET_PERI_REG_MASK(I2S_CLKM_CONF_REG, I2S_CLK_EN | ((0 & I2S_CLKM_DIV_A_V) << I2S_CLKM_DIV_A_S)
                      | ((0) << I2S_CLKM_DIV_B_S) | (20 & I2S_CLKM_DIV_NUM_V));

    CLEAR_PERI_REG_MASK(I2S_SAMPLE_RATE_CONF_REG, I2S_RX_BCK_DIV_NUM_M | I2S_TX_BCK_DIV_NUM_M);
    SET_PERI_REG_MASK(I2S_SAMPLE_RATE_CONF_REG, ((4 & I2S_RX_BCK_DIV_NUM_V) << I2S_RX_BCK_DIV_NUM_S)
                      | ((4 & I2S_TX_BCK_DIV_NUM_V) << I2S_TX_BCK_DIV_NUM_S));

#else
#if 0 // Fs= 48kHz
    CLEAR_PERI_REG_MASK(I2S_CLKM_CONF_REG, (I2S_CLKM_DIV_NUM_M
                                            | I2S_CLKM_DIV_A_M
                                            | I2S_CLKM_DIV_B_M));
    I2S_SET_CLKA_ENA(1);
    I2S_SET_CLKM_DIV_A(63);
    I2S_SET_CLKM_DIV_B(32);
    I2S_SET_CLKM_DIV_NUM(6);

    CLEAR_PERI_REG_MASK(I2S_SAMPLE_RATE_CONF_REG, I2S_RX_BCK_DIV_NUM_M | I2S_TX_BCK_DIV_NUM_M);
    I2S_SET_RX_BCK_DIV_NUM(4);
    I2S_SET_TX_BCK_DIV_NUM(2);
    I2S_SET_TX_PDM_FS(480);
#else
    // Fs = 44.1kHz
    CLEAR_PERI_REG_MASK(I2S_CLKM_CONF_REG(i2sNum), (I2S_CLKM_DIV_NUM_M | I2S_CLKM_DIV_A_M | I2S_CLKM_DIV_B_M));
    SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(i2sNum), I2S_CLKM_DIV_A_V, 64, I2S_CLKM_DIV_A_S);
    SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(i2sNum), I2S_CLKM_DIV_B_V,8, I2S_CLKM_DIV_B_S);
    SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(i2sNum), I2S_CLKM_DIV_NUM_V, 78, I2S_CLKM_DIV_NUM_S);
    
    CLEAR_PERI_REG_MASK(I2S_SAMPLE_RATE_CONF_REG(i2sNum), I2S_RX_BCK_DIV_NUM_M | I2S_TX_BCK_DIV_NUM_M);
    SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(i2sNum), I2S_RX_BCK_DIV_NUM_V, 4, I2S_RX_BCK_DIV_NUM_S);
    SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(i2sNum), I2S_TX_BCK_DIV_NUM_V, 2, I2S_TX_BCK_DIV_NUM_S);
    SET_PERI_REG_BITS(I2S_PDM_FREQ_CONF_REG(i2sNum), I2S_TX_PDM_FS_V, 441, I2S_TX_PDM_FS_S);
#endif

#endif
    CLEAR_PERI_REG_MASK(0x60009000, (0xf));
    CLEAR_PERI_REG_MASK(I2S_PDM_CONF_REG(i2sNum), 0x0f);


#if 0 // Fs= 48kHz
        CLEAR_PERI_REG_MASK(I2S_CLKM_CONF_REG, (I2S_CLKM_DIV_NUM_M
                        |I2S_CLKM_DIV_A_M
                        |I2S_CLKM_DIV_B_M));
        I2S_SET_CLKA_ENA(1);
        I2S_SET_CLKM_DIV_A(63);
        I2S_SET_CLKM_DIV_B(32);
        I2S_SET_CLKM_DIV_NUM(6);

        CLEAR_PERI_REG_MASK(I2S_SAMPLE_RATE_CONF_REG, I2S_RX_BCK_DIV_NUM_M |I2S_TX_BCK_DIV_NUM_M);
        I2S_SET_RX_BCK_DIV_NUM(4);
        I2S_SET_TX_BCK_DIV_NUM(2);
        I2S_SET_TX_PDM_FS(480);
#else
    // Fs = 44.1kHz
        CLEAR_PERI_REG_MASK(I2S_CLKM_CONF_REG(i2sNum), (I2S_CLKM_DIV_NUM_M
                    |I2S_CLKM_DIV_A_M
                    |I2S_CLKM_DIV_B_M));
        SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(i2sNum),I2S_CLKM_DIV_A_V,64,I2S_CLKM_DIV_A_S);
        SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(i2sNum),I2S_CLKM_DIV_B_V,8,I2S_CLKM_DIV_B_S);
        SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(i2sNum),I2S_CLKM_DIV_NUM_V,78,I2S_CLKM_DIV_NUM_S);

        CLEAR_PERI_REG_MASK(I2S_SAMPLE_RATE_CONF_REG(i2sNum), I2S_RX_BCK_DIV_NUM_M |I2S_TX_BCK_DIV_NUM_M);
        SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(i2sNum),I2S_RX_BCK_DIV_NUM_V,4,I2S_RX_BCK_DIV_NUM_S);
        SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(i2sNum),I2S_TX_BCK_DIV_NUM_V,8,I2S_TX_BCK_DIV_NUM_S);
        SET_PERI_REG_BITS(I2S_PDM_FREQ_CONF_REG(i2sNum),I2S_TX_PDM_FS_V,441,I2S_TX_PDM_FS_S);

#endif
        CLEAR_PERI_REG_MASK(0x60009000,(0xf));
        CLEAR_PERI_REG_MASK(I2S_PDM_CONF_REG(i2sNum),0x0f);
}

/**
 * @brief Reset I2s with a given module.
 *
 */
void i2s_reset(i2s_num_t i2sNum)
{
    SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_TX_RESET_M);
    SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_RX_RESET_M);
    SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_TX_FIFO_RESET_M);
    SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_RX_FIFO_RESET_M);

    CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_TX_RESET_M);
    CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_RX_RESET_M);
    CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_TX_FIFO_RESET_M);
    CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_RX_FIFO_RESET_M);
}

/**
 * @brief Initialize the I2S module
 *
 */
void i2s_init(i2s_num_t i2sNum, i2s_attr_t *pAttr)
{
    if ((i2sNum > I2S_NUM_MAX)
        || (NULL == pAttr)) {
        return ;
    }
    if (i2sNum == I2S_NUM_I2S0) {
        SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_I2S0_CLK_EN);
        CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_I2S0_RST);
    } else if (i2sNum == I2S_NUM_I2S1) {
        SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_I2S1_CLK_EN);
        CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_I2S1_RST);
    }

    // configure I2S data port interface.
    CLEAR_PERI_REG_MASK(I2S_CONF2_REG(i2sNum), I2S_LCD_EN);
    CLEAR_PERI_REG_MASK(I2S_PDM_CONF_REG(i2sNum), I2S_PCM2PDM_CONV_EN | I2S_PDM2PCM_CONV_EN);
    SET_PERI_REG_BITS(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL, 0x1, RTC_CNTL_SOC_CLK_SEL_S);

    if (0 != pAttr->rx_mode.mode) {
        if (I2S_IFACE_TYPE_DMA == pAttr->rx_mode.iface) {
            SET_PERI_REG_MASK(I2S_FIFO_CONF_REG(i2sNum), I2S_DSCR_EN);
        } else {
            CLEAR_PERI_REG_MASK(I2S_FIFO_CONF_REG(i2sNum), I2S_DSCR_EN);
        }
        // Working mode
        if (I2S_MODE_MASTER_RX == pAttr->rx_mode.mode) {
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_RX_SLAVE_MOD);
            // configure I2S communication standard format
            i2s_clk_set(pAttr->rx_mode.rate, pAttr->rx_mode.bits);
        } else if (I2S_MODE_SLAVE_RX == pAttr->rx_mode.mode) {
            SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_RX_SLAVE_MOD);
        } else {
            // To do nothing.
        }
        // configure I2S communication standard format
        if (I2S_STD_FORMAT_I2S == pAttr->rx_mode.std) {
            SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_RX_MSB_SHIFT);
            CLEAR_PERI_REG_MASK(I2S_CONF1_REG(i2sNum), I2S_RX_SHORT_SYNC);
        } else if (I2S_STD_FORMAT_MSB == pAttr->rx_mode.std) {
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_RX_MSB_SHIFT);
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_RX_SHORT_SYNC);
        } else if (I2S_STD_FORMAT_LSB == pAttr->rx_mode.std) {

        } else if (I2S_STD_FORMAT_PCM_SHORT == pAttr->rx_mode.std) {
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_RX_MSB_SHIFT);
            SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_RX_SHORT_SYNC);
        } else if (I2S_STD_FORMAT_PCM_LONG == pAttr->rx_mode.std) {
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_RX_MSB_SHIFT);
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_RX_SHORT_SYNC);
        } else {
            // To do nothing.
        }
        // configure bit width format.
        SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(i2sNum), I2S_RX_BITS_MOD, pAttr->rx_mode.bits, I2S_RX_BITS_MOD_S);

        SET_PERI_REG_MASK(I2S_FIFO_CONF_REG(i2sNum), I2S_RX_FIFO_MOD_FORCE_EN);
        // setting channel mode.
        if (I2S_CHANNEL_FMT_RIGHT_LEFT == pAttr->rx_mode.channel) {
            // set rx,tx channel mode, both are "two channel" here
            SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(i2sNum), I2S_RX_FIFO_MOD_V, 0, I2S_RX_FIFO_MOD_S); // 0-right&left channel;1-one channel
            SET_PERI_REG_BITS(I2S_CONF_CHAN_REG(i2sNum), I2S_RX_CHAN_MOD, I2S_CHANNEL_FMT_RIGHT_LEFT, I2S_RX_CHAN_MOD_S);// 0-two channel;1-right;2-left
        } else if (I2S_CHANNEL_FMT_ALL_RIGHT == pAttr->rx_mode.channel) {
            SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(i2sNum), I2S_RX_FIFO_MOD_V, 0, I2S_RX_FIFO_MOD_S); // 0-right&left channel;1-one channel
            SET_PERI_REG_BITS(I2S_CONF_CHAN_REG(i2sNum), I2S_RX_CHAN_MOD, I2S_CHANNEL_FMT_ALL_RIGHT, I2S_RX_CHAN_MOD_S);
        } else if (I2S_CHANNEL_FMT_ALL_LEFT == pAttr->rx_mode.channel) {
            SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(i2sNum), I2S_RX_FIFO_MOD_V, 0, I2S_RX_FIFO_MOD_S); // 0-right&left channel;1-one channel
            SET_PERI_REG_BITS(I2S_CONF_CHAN_REG(i2sNum), I2S_RX_CHAN_MOD, I2S_CHANNEL_FMT_ALL_LEFT, I2S_RX_CHAN_MOD_S);
        } else if (I2S_CHANNEL_FMT_ONLY_RIGHT == pAttr->rx_mode.channel) {
            SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(i2sNum), I2S_RX_FIFO_MOD_V, 1, I2S_RX_FIFO_MOD_S); // 0-right&left channel;1-one channel
            SET_PERI_REG_BITS(I2S_CONF_CHAN_REG(i2sNum), I2S_RX_CHAN_MOD, 1, I2S_RX_CHAN_MOD_S);
        } else if (I2S_CHANNEL_FMT_ONLY_LEFT == pAttr->rx_mode.channel) {
            SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(i2sNum), I2S_RX_FIFO_MOD_V, 1, I2S_RX_FIFO_MOD_S); // 0-right&left channel;1-one channel
            SET_PERI_REG_BITS(I2S_CONF_CHAN_REG(i2sNum), I2S_RX_CHAN_MOD, 2, I2S_RX_CHAN_MOD_S);
        } else {
            // To do nothing.
        }

        if (I2S_MODULE_WORK_TYPE_PDM == pAttr->rx_mode.type) {
            if (PDM_SAMPLE_RATE_RATIO_64 == pAttr->rx_mode.ratio) {
                CLEAR_PERI_REG_MASK(I2S_PDM_CONF_REG(i2sNum), I2S_RX_PDM_SINC_DSR_16_EN); // Clear:64*fs;set:128*fs
            } else if (PDM_SAMPLE_RATE_RATIO_128 == pAttr->rx_mode.ratio) {
                SET_PERI_REG_MASK(I2S_PDM_CONF_REG(i2sNum), I2S_RX_PDM_SINC_DSR_16_EN); // Clear:64*fs;set:128*fs
            }
            if (PDM_PCM_CONV_ENABLE == pAttr->rx_mode.conv) {
                SET_PERI_REG_MASK(I2S_PDM_CONF_REG(i2sNum), I2S_PDM2PCM_CONV_EN);
            } else if (PDM_PCM_CONV_DISABLE == pAttr->rx_mode.conv) {
                CLEAR_PERI_REG_MASK(I2S_PDM_CONF_REG(i2sNum), I2S_PDM2PCM_CONV_EN);
            }
        }
    }

    if (0 != pAttr->tx_mode.mode) {
        if (I2S_IFACE_TYPE_DMA == pAttr->tx_mode.iface) {
            SET_PERI_REG_MASK(I2S_FIFO_CONF_REG(i2sNum), I2S_DSCR_EN);
        } else {
            CLEAR_PERI_REG_MASK(I2S_FIFO_CONF_REG(i2sNum), I2S_DSCR_EN);
        }

        if (I2S_MODE_MASTER_TX == pAttr->tx_mode.mode) {
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_TX_SLAVE_MOD);
            // configure I2S communication standard format
            i2s_clk_set(pAttr->tx_mode.rate, pAttr->tx_mode.bits);
          //  rtc_plla_ena(1, 621, 14);
          //  SET_PERI_REG_MASK(I2S_CLKM_CONF_REG(i2sNum), I2S_CLKA_ENA);
            printf("rtc_plla_ena is ok\r\n");
        } else if (I2S_MODE_SLAVE_RX == pAttr->tx_mode.mode) {
            SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_TX_SLAVE_MOD);
        } else {
            // To do nothing.
        }
        // configure I2S communication standard format
        if (I2S_STD_FORMAT_I2S == pAttr->tx_mode.std) {
            SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_TX_MSB_SHIFT);
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_TX_SHORT_SYNC);
        } else if (I2S_STD_FORMAT_MSB == pAttr->tx_mode.std) {
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_TX_MSB_SHIFT);
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_TX_SHORT_SYNC);
        } else if (I2S_STD_FORMAT_LSB == pAttr->tx_mode.std) {

        } else if (I2S_STD_FORMAT_PCM_SHORT == pAttr->tx_mode.std) {
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_TX_MSB_SHIFT);
            SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_TX_SHORT_SYNC);
        } else if (I2S_STD_FORMAT_PCM_LONG == pAttr->tx_mode.std) {
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_TX_MSB_SHIFT);
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum),  I2S_TX_SHORT_SYNC);
        } else {
            // To do nothing.
        }
        // configure bit width format.
        SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(i2sNum), I2S_TX_BITS_MOD, pAttr->tx_mode.bits, I2S_TX_BITS_MOD_S);
        SET_PERI_REG_MASK(I2S_FIFO_CONF_REG(i2sNum), I2S_TX_FIFO_MOD_FORCE_EN);

        if (I2S_CHANNEL_FMT_RIGHT_LEFT == pAttr->tx_mode.channel) {
            // set rx,tx channel mode, both are "two channel" here
            SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(i2sNum), I2S_TX_FIFO_MOD_V, 0, I2S_TX_FIFO_MOD_S); // 0-right&left channel;1-one channel
            SET_PERI_REG_BITS(I2S_CONF_CHAN_REG(i2sNum), I2S_TX_CHAN_MOD, I2S_CHANNEL_FMT_RIGHT_LEFT, I2S_TX_CHAN_MOD_S);// 0-two channel;1-right;2-left
        } else if (I2S_CHANNEL_FMT_ALL_RIGHT == pAttr->tx_mode.channel) {
            SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(i2sNum), I2S_TX_FIFO_MOD_V, 0, I2S_TX_FIFO_MOD_S); // 0-right&left channel;1-one channel
            SET_PERI_REG_BITS(I2S_CONF_CHAN_REG(i2sNum), I2S_TX_CHAN_MOD, I2S_CHANNEL_FMT_ALL_RIGHT, I2S_TX_CHAN_MOD_S);
        } else if (I2S_CHANNEL_FMT_ALL_LEFT == pAttr->tx_mode.channel) {
            SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(i2sNum), I2S_TX_FIFO_MOD_V, 0, I2S_TX_FIFO_MOD_S); // 0-right&left channel;1-one channel
            SET_PERI_REG_BITS(I2S_CONF_CHAN_REG(i2sNum), I2S_TX_CHAN_MOD, I2S_CHANNEL_FMT_ALL_LEFT, I2S_TX_CHAN_MOD_S);
        } else if (I2S_CHANNEL_FMT_ONLY_RIGHT == pAttr->tx_mode.channel) {
            SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(i2sNum), I2S_TX_FIFO_MOD_V, 1, I2S_TX_FIFO_MOD_S); // 0-right&left channel;1-one channel
            SET_PERI_REG_BITS(I2S_CONF_CHAN_REG(i2sNum), I2S_TX_CHAN_MOD, 1, I2S_TX_CHAN_MOD_S);
        } else if (I2S_CHANNEL_FMT_ONLY_LEFT == pAttr->tx_mode.channel) {
            SET_PERI_REG_BITS(I2S_FIFO_CONF_REG(i2sNum), I2S_TX_FIFO_MOD_V, 1, I2S_TX_FIFO_MOD_S); // 0-right&left channel;1-one channel
            SET_PERI_REG_BITS(I2S_CONF_CHAN_REG(i2sNum), I2S_TX_CHAN_MOD, 2, I2S_TX_CHAN_MOD_S);
        } else {
            // To do nothing.
        }
        if (I2S_MODULE_WORK_TYPE_PDM == pAttr->tx_mode.type) {
            if (PDM_SAMPLE_RATE_RATIO_64 == pAttr->tx_mode.ratio) {
                SET_PERI_REG_BITS(I2S_PDM_CONF_REG(i2sNum), I2S_TX_PDM_SINC_OSR2_M, 1, I2S_TX_PDM_SINC_OSR2_S); // TX: 2---128*fs;1---64*fs
            } else if (PDM_SAMPLE_RATE_RATIO_128 == pAttr->tx_mode.ratio) {
                SET_PERI_REG_BITS(I2S_PDM_CONF_REG(i2sNum), I2S_TX_PDM_SINC_OSR2_M, 2, I2S_TX_PDM_SINC_OSR2_S); // TX: 2---128*fs;1---64*fs
            }
            if (PDM_PCM_CONV_ENABLE == pAttr->rx_mode.conv) {
                SET_PERI_REG_MASK(I2S_PDM_CONF_REG(i2sNum), I2S_PDM2PCM_CONV_EN);
            } else if (PDM_PCM_CONV_DISABLE == pAttr->rx_mode.conv) {
                CLEAR_PERI_REG_MASK(I2S_PDM_CONF_REG(i2sNum), I2S_PDM2PCM_CONV_EN);
            }
        }
    }
    // Clear all of the interrupt sources.
    WRITE_PERI_REG(I2S_INT_CLR_REG(i2sNum), 0xffffffff);
}

/**
 * @brief Enable the interrupt source.
 *
 */
void i2s_int_enable(i2s_num_t i2sNum, i2s_int_src_t intSrc)
{
    SET_PERI_REG_MASK(I2S_INT_ENA_REG(i2sNum), intSrc);
}

/**
 * @brief Disable the interrupt source.
 *
 */
void i2s_int_disable(i2s_num_t i2sNum, i2s_int_src_t intSrc)
{
    CLEAR_PERI_REG_MASK(I2S_INT_ENA_REG(i2sNum), intSrc);
}

/**
 * @brief Clear the interrupt source.
 *
 */
void i2s_int_clear(i2s_num_t i2sNum, i2s_int_src_t intSrc)
{
    SET_PERI_REG_MASK(I2S_INT_CLR_REG(i2sNum), intSrc);
}

/**
 * @brief Get the I2S interrupt status
 */
uint32_t i2s_int_status_get(i2s_num_t i2sNum)
{
    return READ_PERI_REG(I2S_INT_ST_REG(i2sNum));
}

/**
 * @brief Start I2S work
 *
 */
void i2s_start(i2s_module_work_type_t type, i2s_num_t i2sNum, i2s_mode_t mode)
{
    if (i2sNum > I2S_NUM_MAX) {
        return;
    }
    if (I2S_MODULE_WORK_TYPE_I2S == type) {
        if ((I2S_MODE_MASTER_RX == mode)
            || (I2S_MODE_SLAVE_RX == mode)) {
            SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_RX_START);
            SET_PERI_REG_MASK(GPIO_ENABLE_REG, BIT(0));
        } else if ((I2S_MODE_MASTER_TX == mode)
                   || (I2S_MODE_SLAVE_TX == mode)) {
            SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_TX_START);
            SET_PERI_REG_MASK(GPIO_ENABLE_REG, BIT(0));
        }
    } else if (I2S_MODULE_WORK_TYPE_PDM == type) {
        if ((I2S_MODE_MASTER_RX == mode)
            || (I2S_MODE_SLAVE_RX == mode)) {
            SET_PERI_REG_MASK(I2S_PDM_CONF_REG(i2sNum), I2S_RX_PDM_EN);
            SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_RX_START);
        } else if ((I2S_MODE_MASTER_TX == mode)
                   || (I2S_MODE_SLAVE_TX == mode)) {
            SET_PERI_REG_MASK(I2S_PDM_CONF_REG(i2sNum), I2S_TX_PDM_EN);
            SET_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_TX_START);
        }
    }
}

/**
 * @brief Stop I2S work
 *
 */
void i2s_stop(i2s_module_work_type_t type, i2s_num_t i2sNum, i2s_mode_t mode)
{
    if (i2sNum > I2S_NUM_MAX) {
        return;
    }
    if (I2S_MODULE_WORK_TYPE_I2S == type) {
        if ((I2S_MODE_MASTER_RX == mode)
            || (I2S_MODE_SLAVE_RX == mode)) {
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_RX_START);
            SET_PERI_REG_MASK(GPIO_ENABLE_REG, BIT(0));
        } else if ((I2S_MODE_MASTER_TX == mode)
                   || (I2S_MODE_SLAVE_TX == mode)) {
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_TX_START);
        }
    } else if (I2S_MODULE_WORK_TYPE_PDM == type) {
        if ((I2S_MODE_MASTER_RX == mode)
            || (I2S_MODE_SLAVE_RX == mode)) {
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_RX_START);
            CLEAR_PERI_REG_MASK(I2S_PDM_CONF_REG(i2sNum), I2S_RX_PDM_EN);
        } else if ((I2S_MODE_MASTER_TX == mode)
                   || (I2S_MODE_SLAVE_TX == mode)) {
            CLEAR_PERI_REG_MASK(I2S_CONF_REG(i2sNum), I2S_TX_START);
            CLEAR_PERI_REG_MASK(I2S_PDM_CONF_REG(i2sNum), I2S_TX_PDM_EN);
        }
    }
}

#ifdef __cplusplus
}
#endif
