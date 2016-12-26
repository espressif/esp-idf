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

#include <string.h>
#include "spi.h"
#include "soc/spi_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "rom/ets_sys.h"
#include "esp_intr.h"
#include "soc/dport_reg.h"

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
 * @brief Defines slave commands.Default value based on slave ESP8266 & ESP32.
 */
#define MASTER_WRITE_DATA_TO_SLAVE_CMD                      2
#define MASTER_READ_DATA_FROM_SLAVE_CMD                     3
#define MASTER_WRITE_STATUS_TO_SLAVE_CMD                    1
#define MASTER_READ_STATUS_FROM_SLAVE_CMD                   4


static void spi_intr_func_set(void * isr, spi_num_t spiNum)
{
    if (SPI_NUM_SPI1 == spiNum) {
        intr_matrix_set(0, ETS_SPI1_DMA_INTR_SOURCE, ETS_SPI1_INUM);
        ESP_SPI1_INTR_ATTACH(isr, NULL);
        // enable intr in cpu
        ESP_SPI1_INTR_ENABLE();
    } else if (SPI_NUM_SPI2 == spiNum) {
        intr_matrix_set(0, ETS_SPI2_DMA_INTR_SOURCE, ETS_SPI2_INUM);
        ESP_SPI2_INTR_ATTACH(isr, NULL);
        // enable intr in cpu
        ESP_SPI2_INTR_ENABLE();
    } else if (SPI_NUM_SPI3 == spiNum) {
        intr_matrix_set(0, ETS_SPI3_DMA_INTR_SOURCE, ETS_SPI3_INUM);
        ESP_SPI3_INTR_ATTACH(isr, NULL);
        // enable intr in cpu
        ESP_SPI3_INTR_ENABLE();
    } else {
        // To do nothing
    }
}
#if 1

/**
 * @brief Get SPI ping buffer address.
 *
 */
uint32_t* spi_dma_ping_buf_get(spi_dma_attr_t *obj)
{
    if (NULL == obj) {
        return NULL;
    }
    return obj->buf->ping->buffer_addr;
}

/**
 * @brief Get SPI ping buffer address.
 *
 */
uint32_t* spi_dma_pong_buf_get(spi_dma_attr_t *obj)
{
    if (NULL == obj) {
        return NULL;
    }
    return obj->buf->pong->buffer_addr;
}

/**
 * @brief Get without work ping-pong buffer address.
 *
 */
uint32_t* spi_dma_status_get(spi_dma_attr_t *obj)
{
    if ((NULL == obj)
        || (obj->spi_num > SPI_NUM_MAX)) {
        return NULL;
    }
    switch (obj->dir) {
    case SPI_DMA_DIR_OUT:
        if (READ_PERI_REG(SPI_OUT_EOF_DES_ADDR_REG(obj->spi_num)) == ((uint32_t)obj->buf->ping->last_queue))  {
            return obj->buf->ping->buffer_addr;
        } else if (READ_PERI_REG(SPI_OUT_EOF_DES_ADDR_REG(obj->spi_num)) == ((uint32_t)obj->buf->pong->last_queue)) {
            return obj->buf->pong->buffer_addr;
        }
        break;
    case SPI_DMA_DIR_IN:
        if (READ_PERI_REG(SPI_IN_SUC_EOF_DES_ADDR_REG(obj->spi_num)) == ((uint32_t)obj->buf->ping->last_queue)) {
            return obj->buf->ping->buffer_addr;
        } else if (READ_PERI_REG(SPI_IN_SUC_EOF_DES_ADDR_REG(obj->spi_num)) == ((uint32_t)obj->buf->pong->last_queue)) {
            return obj->buf->pong->buffer_addr;
        }
        break;
    default:
        break;
    }
    return NULL;
}

/**
 * @brief Configrate the Ping-Pong buffer to the destination
 *
 */
void spi_dma_dest_add_set(spi_dma_attr_t *obj)
{
    if ((NULL == obj)
        || (obj->spi_num > SPI_NUM_MAX)) {
        return;
    }
    if (SPI_DMA_DIR_IN == obj->dir) {
        SET_PERI_REG_BITS(SPI_DMA_IN_LINK_REG(obj->spi_num), SPI_INLINK_ADDR, ((uint32_t)(obj->buf->ping->first_queue)), SPI_INLINK_ADDR_S);
    } else if (SPI_DMA_DIR_OUT == obj->dir) {
        SET_PERI_REG_BITS(SPI_DMA_OUT_LINK_REG(obj->spi_num), SPI_OUTLINK_ADDR, ((uint32_t)(obj->buf->ping->first_queue)), SPI_OUTLINK_ADDR_S);
    }
}

/**
 * @brief Reset SPI ping buffer address.
 *
 */
void spi_dma_rest(spi_dma_attr_t *obj)
{
    if ((NULL == obj)
        || (obj->spi_num > SPI_NUM_MAX)) {
        return;
    }
    dma_buf_len_reset(obj->buf);
}

/**
 * @brief Initialize DMA and create a SPI DMA instance.
 *
 */
int spi_dma_init(spi_dma_attr_t *obj, void *isr)
{
    if ((NULL == obj)
        || (obj->spi_num > SPI_NUM_MAX)) {
        return -1;
    }
    obj->buf = dma_buf_create(obj->buf_size);
    if (NULL == obj->buf) {
        return -2;
    }
    // Reset DMA
    SET_PERI_REG_MASK(SPI_DMA_CONF_REG(obj->spi_num), SPI_OUT_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);
    CLEAR_PERI_REG_MASK(SPI_DMA_OUT_LINK_REG(obj->spi_num), SPI_OUTLINK_START);
    CLEAR_PERI_REG_MASK(SPI_DMA_IN_LINK_REG(obj->spi_num), SPI_INLINK_START);
    CLEAR_PERI_REG_MASK(SPI_DMA_CONF_REG(obj->spi_num), SPI_OUT_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);

    // Select DMA channel.
    SET_PERI_REG_BITS(DPORT_SPI_DMA_CHAN_SEL_REG, 3, obj->channel, ((obj->spi_num - 1) * 2));

    SET_PERI_REG_MASK(SPI_USER_REG(obj->spi_num), SPI_USR_MOSI);//////add
    if ((SPI_MODE_MASTER == obj->mode)) {
        // enable send intr
        SET_PERI_REG_MASK(SPI_DMA_INT_ENA_REG(obj->spi_num ), SPI_INT_SRC_ONE_BUF_SEND_DONE);        
        SET_PERI_REG_MASK(SPI_DMA_INT_ENA_REG(obj->spi_num ), SPI_INT_SRC_ONE_BUF_RECV_DONE);
        
    } else if ((SPI_MODE_SLAVE == obj->mode)) {
        SET_PERI_REG_MASK(SPI_SLV_RDBUF_DLEN_REG(obj->spi_num), ((obj->buf_size << 3) - 1));
        SET_PERI_REG_MASK(SPI_SLV_WRBUF_DLEN_REG(obj->spi_num), ((obj->buf_size << 3) - 1));
        // enable receive intr
        SET_PERI_REG_MASK(SPI_DMA_INT_ENA_REG(obj->spi_num), SPI_INT_SRC_ONE_BUF_RECV_DONE);
    }
    // Clear all of interrupt source
    spi_int_clear(obj->spi_num);

    spi_intr_func_set(isr, obj->spi_num);
    return 0;
}

/**
 * @brief Destroy the SPI DMA instance
 *
 */
int spi_dma_uninit(spi_dma_attr_t *obj)
{
    if (NULL == obj) {
        return -1;
    }
    dma_buf_destroy(obj->buf);
    return 0;
}

/**
 * @brief Enable the SPI DMA work mode.
 *
 */
static void spi_dma_enable(spi_dma_attr_t *obj)
{
    if ((NULL == obj)
        || (obj->spi_num > SPI_NUM_MAX)) {
        return;
    }
    
    if (SPI_DMA_DIR_IN == obj->dir) {
        WRITE_PERI_REG(0x3ff000c4,1<<22);
        WRITE_PERI_REG(0x3ff000c4,0);
        SET_PERI_REG_MASK(SPI_DMA_OUT_LINK_REG(obj->spi_num), SPI_OUTLINK_START);
        SET_PERI_REG_MASK(SPI_DMA_IN_LINK_REG(obj->spi_num), SPI_INLINK_START);
       // LOGD("recv-01:length=%x",READ_PERI_REG(SPI_DMA_TSTATUS_REG(obj->spi_num))>>18);
       // while (((READ_PERI_REG(SPI_DMA_TSTATUS_REG(obj->spi_num))>>18)&0x3)!=0x3){
       //LOGD("recv-01:length=%x",READ_PERI_REG(SPI_DMA_TSTATUS_REG(obj->spi_num))>>18);
       //  }
    } else if (SPI_DMA_DIR_OUT == obj->dir) {
        SET_PERI_REG_MASK(SPI_DMA_IN_LINK_REG(obj->spi_num), SPI_INLINK_START);
        SET_PERI_REG_MASK(SPI_DMA_OUT_LINK_REG(obj->spi_num), SPI_OUTLINK_START);
        // Waiting DMA controller fill TX FIFO
        while ((READ_PERI_REG(SPI_DMA_RSTATUS_REG(obj->spi_num))&0x80000000));
    }
}

/**
 * @brief Disable the SPI DMA work mode.
 *
 */
static void spi_dma_disable(spi_dma_attr_t *obj)
{
    if ((NULL == obj)
        || (obj->spi_num > SPI_NUM_MAX)) {
        return;
    }
    if (SPI_DMA_DIR_IN == obj->dir) {
        CLEAR_PERI_REG_MASK(SPI_DMA_IN_LINK_REG(obj->spi_num), SPI_INLINK_STOP);
    } else if (SPI_DMA_DIR_OUT == obj->dir) {
        CLEAR_PERI_REG_MASK(SPI_DMA_OUT_LINK_REG(obj->spi_num), SPI_OUTLINK_STOP);
    }
}

/**
 * @brief Enable SPI DMA interrupt source.
 *
 */
void  spi_dma_int_enable(spi_num_t spiNum, spi_int_src_t intSrc)
{
    if (spiNum > SPI_NUM_MAX) {
        return;
    }
    SET_PERI_REG_MASK(SPI_DMA_INT_ENA_REG(spiNum), intSrc);
}

/**
 * @brief Disable SPI DMA interrupt source.
 *
 */
void  spi_dma_int_disable(spi_num_t spiNum, spi_int_src_t intSrc)
{
    if (spiNum > SPI_NUM_MAX) {
        return;
    }
    SET_PERI_REG_MASK(SPI_DMA_INT_ENA_REG(spiNum), intSrc);
}

/**
 * @brief Clear all of SPI DMA interrupt source.
 *
 */
void  spi_dma_int_clear(spi_num_t spiNum)
{
    if (spiNum > SPI_NUM_MAX) {
        return;
    }
    SET_PERI_REG_MASK(SPI_DMA_INT_CLR_REG(spiNum), 0x1FF);
}

/**
 * @brief Get the SPI DMA interrupt status.
 *
 */
int32_t spi_dma_int_status_get(spi_num_t spiNum)
{
    if (spiNum > SPI_NUM_MAX) {
        return -1;
    }
    return READ_PERI_REG(SPI_DMA_INT_ST_REG(spiNum));
}

/**
 * @brief Start SPI work by DMA
 *
 */
void spi_dma_start(spi_dma_attr_t *obj, uint32_t len)
{
    if ((NULL == obj)
        || (obj->spi_num > SPI_NUM_MAX)) {
        return;
    }
    // Reset DMA controller

    SET_PERI_REG_MASK(SPI_DMA_CONF_REG(obj->spi_num), SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);
    CLEAR_PERI_REG_MASK(SPI_DMA_CONF_REG(obj->spi_num), SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);
    
    if (SPI_MODE_MASTER == obj->mode) {
        CLEAR_PERI_REG_MASK(SPI_DMA_CONF_REG(obj->spi_num), SPI_DMA_CONTINUE);
        if (obj->dir == SPI_DMA_DIR_IN) {
            SET_PERI_REG_BITS(SPI_MISO_DLEN_REG(obj->spi_num), SPI_USR_MISO_DBITLEN, ((len << 3) - 1), SPI_USR_MISO_DBITLEN_S);
            SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(obj->spi_num), SPI_USR_MOSI_DBITLEN, ((len << 3) - 1), SPI_USR_MOSI_DBITLEN_S); 
        }else{
            SET_PERI_REG_BITS(SPI_MISO_DLEN_REG(obj->spi_num), SPI_USR_MISO_DBITLEN, ((len << 3) - 1), SPI_USR_MISO_DBITLEN_S);
            SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(obj->spi_num), SPI_USR_MOSI_DBITLEN, ((len << 3) - 1), SPI_USR_MOSI_DBITLEN_S);  
        }
    } else if (SPI_MODE_SLAVE == obj->mode) {
        if (0 == len) {
            SET_PERI_REG_MASK(SPI_DMA_CONF_REG(obj->spi_num), SPI_DMA_CONTINUE);
        } else {
            CLEAR_PERI_REG_MASK(SPI_DMA_CONF_REG(obj->spi_num), SPI_DMA_CONTINUE);
        }
    }
    // 1.Enable DMA


    spi_dma_enable(obj);
    //LOGD("recv start before:%08x\n",obj->buf->ping->first_queue->data_length);

    // LOGD("SPI_DMA_STATUS_REG=%x",READ_PERI_REG(SPI_DMA_STATUS_REG(obj->spi_num)));

    // 2.Start SPI
    SET_PERI_REG_MASK(SPI_CMD_REG(obj->spi_num), SPI_USR);
    //LOGD("recv start after:%08x\n",obj->buf->ping->first_queue->data_length);    

    
}

/**
 * @brief Stop SPI work by DMA
 *
 */
void spi_dma_stop(spi_dma_attr_t *obj)
{
    if ((NULL == obj)
        || (obj->spi_num > SPI_NUM_MAX)) {
        return;
    }
    if (SPI_MODE_MASTER == obj->mode) {
        SET_PERI_REG_MASK(SPI_DMA_CONF_REG(obj->spi_num), SPI_DMA_TX_STOP);
    } else if (SPI_MODE_SLAVE == obj->mode) {
        SET_PERI_REG_MASK(SPI_DMA_CONF_REG(obj->spi_num), SPI_DMA_RX_STOP);
    }
    spi_dma_disable(obj);
}
#endif

/**
 * @brief Based on pAttr initialize SPI module.
 *
 */
void spi_init(spi_num_t spiNum, spi_attr_t* pAttr)
{
    if ((spiNum > SPI_NUM_MAX)
        || (NULL == pAttr)) {
        return;
    }

    SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI_CLK_EN);
    CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI_RST);
    SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI_CLK_EN_1);
    CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI_RST_1);
    SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI_CLK_EN_2);
    CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI_RST_2);

    CLEAR_PERI_REG_MASK(SPI_SLAVE_REG(spiNum), SPI_TRANS_DONE << 5);
    SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_CS_SETUP);
    // By default clear command
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_COMMAND);

    // SPI_CPOL & SPI_CPHA
    switch (pAttr->sub_mode) {
    case SPI_SUBMODE_1:
        CLEAR_PERI_REG_MASK(SPI_PIN_REG(spiNum), SPI_CK_IDLE_EDGE);
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum),  SPI_CK_OUT_EDGE); // CHPA_FALLING_EDGE_SAMPLE
        break;
    case SPI_SUBMODE_2:
        SET_PERI_REG_MASK(SPI_PIN_REG(spiNum), SPI_CK_IDLE_EDGE);
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum),  SPI_CK_OUT_EDGE); // CHPA_FALLING_EDGE_SAMPLE
        break;
    case SPI_SUBMODE_3:
        SET_PERI_REG_MASK(SPI_PIN_REG(spiNum), SPI_CK_IDLE_EDGE);
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum),  SPI_CK_OUT_EDGE);
        break;
    case SPI_SUBMODE_0:
    default:
        CLEAR_PERI_REG_MASK(SPI_PIN_REG(spiNum), SPI_CK_IDLE_EDGE);
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum),  SPI_CK_OUT_EDGE);
        // To do nothing
        break;
    }

    // SPI bit order
    if (SPI_BIT_ORDER_MSB_FIRST == pAttr->bit_order) {
        CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spiNum), SPI_WR_BIT_ORDER);
        CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spiNum), SPI_RD_BIT_ORDER);
    } else if (SPI_BIT_ORDER_LSB_FIRST == pAttr->bit_order) {
        SET_PERI_REG_MASK(SPI_CTRL_REG(spiNum), SPI_WR_BIT_ORDER);
        SET_PERI_REG_MASK(SPI_CTRL_REG(spiNum), SPI_RD_BIT_ORDER);
    } else {
        // To do nothing
    }

    // SPI bit order
    if (SPI_WORK_MODE_HALF == pAttr->half_mode) {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_DOUTDIN);
    } else if (SPI_WORK_MODE_FULL == pAttr->half_mode) {
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_DOUTDIN);
    }
    // May be not must to do.
    WRITE_PERI_REG(SPI_USER1_REG(spiNum), 0);
    // SPI mode type
    if (SPI_MODE_MASTER == pAttr->mode) {
        // SPI mode type
        SET_PERI_REG_BITS(SPI_CTRL2_REG(spiNum), SPI_MISO_DELAY_MODE, 0, SPI_MISO_DELAY_MODE_S); ////??????
        // SPI_SET_MISO_DELAY_NUM(spiNum,0);////???????
        //SET_PERI_REG_BITS(SPI_CTRL2_REG(spiNum), SPI_MISO_DELAY_NUM,0,SPI_MISO_DELAY_NUM_S);////??????

        CLEAR_PERI_REG_MASK(SPI_SLAVE_REG(spiNum), SPI_SLAVE_MODE);
        // SPI Send buffer
        // CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MISO_HIGHPART );// By default slave send buffer C0-C7
        // SPI Speed
        if (1 < (pAttr->speed)) {
            uint8_t i, k;
            i = (pAttr->speed / 40) ? (pAttr->speed / 40) : 1;

            k = pAttr->speed / i;
            CLEAR_PERI_REG_MASK(SPI_CLOCK_REG(spiNum), SPI_CLK_EQU_SYSCLK);
            WRITE_PERI_REG(SPI_CLOCK_REG(spiNum),
                           (((i - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
                           (((k - 1) & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |
                           ((((k + 1) / 2 - 1) & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |
                           (((k - 1) & SPI_CLKCNT_L) << SPI_CLKCNT_L_S)); //clear bit 31,set SPI clock div
        } else {
            WRITE_PERI_REG(SPI_CLOCK_REG(spiNum), SPI_CLK_EQU_SYSCLK); // 80Mhz speed
        }
        // Enable MOSI
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_CS_SETUP | SPI_CS_HOLD | SPI_USR_MOSI);

        // CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_CS_HOLD);/////////////add
        SET_PERI_REG_MASK(SPI_CTRL2_REG(spiNum), ((0x4 & SPI_MISO_DELAY_NUM) << SPI_MISO_DELAY_NUM_S)); //delay num

    } else if (SPI_MODE_SLAVE == pAttr->mode) {

        // SPI mode type
        SET_PERI_REG_MASK(SPI_SLAVE_REG(spiNum), SPI_SLAVE_MODE);
        // SPI mode type
        SET_PERI_REG_MASK(SPI_SLAVE_REG(spiNum), SPI_SLV_WR_RD_BUF_EN);
        // SPI Send buffer
        // SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MISO_HIGHPART);// By default slave send buffer C8-C15

        // If do not set delay cycles, slave not working,master cann't get the data.
        SET_PERI_REG_MASK(SPI_CTRL2_REG(spiNum), ((0x2 & SPI_MOSI_DELAY_NUM) << SPI_MOSI_DELAY_NUM_S)); //delay num
        // SPI Speed
        WRITE_PERI_REG(SPI_CLOCK_REG(spiNum), 0);

        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_CS_SETUP);/////////////add
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MOSI);

        // By default format::CMD(8bits)+ADDR(8bits)+DATA(32bytes)
        // set pAttr->cmd_len bit slave recieve command length
        // set 1 bytes status buffer length
        // set pAttr->addr_len bit slave recieve read address length
        // set pAttr->addr_len bit slave recieve write address length
        // set 32 bytes slave recieve buffer length
        SET_PERI_REG_BITS(SPI_USER2_REG(spiNum), SPI_USR_COMMAND_BITLEN,
                          (7), SPI_USR_COMMAND_BITLEN_S);
        SET_PERI_REG_BITS(SPI_SLAVE1_REG(spiNum), SPI_SLV_STATUS_BITLEN,
                          (7), SPI_SLV_STATUS_BITLEN_S);
        SET_PERI_REG_BITS(SPI_SLAVE1_REG(spiNum), SPI_SLV_WR_ADDR_BITLEN,
                          (7), SPI_SLV_WR_ADDR_BITLEN_S);
        SET_PERI_REG_BITS(SPI_SLAVE1_REG(spiNum), SPI_SLV_RD_ADDR_BITLEN,
                          (7), SPI_SLV_RD_ADDR_BITLEN_S);
        SET_PERI_REG_BITS(SPI_SLV_WRBUF_DLEN_REG(spiNum), SPI_SLV_WRBUF_DBITLEN,
                          (32 * 8 - 1), SPI_SLV_WRBUF_DBITLEN_S);
        SET_PERI_REG_BITS(SPI_SLV_RDBUF_DLEN_REG(spiNum), SPI_SLV_RDBUF_DBITLEN,
                          (32 * 8 - 1), SPI_SLV_RDBUF_DBITLEN_S);
    } else {
        // To do nothing
    }

    char i;
    for (i = 0; i < 16; ++i) {
        WRITE_PERI_REG((SPI_W0_REG(spiNum) + (i << 2)), 0);
    }
}

/**
 * @brief Set address value by master mode.
 *
 */
void spi_master_cfg_addr(spi_num_t spiNum, uint32_t addr)
{
    if (spiNum > SPI_NUM_MAX) {
        return;
    }
    // Set address
    SET_PERI_REG_BITS(SPI_ADDR_REG(spiNum), SPI_USR_ADDR_VALUE, addr, SPI_USR_ADDR_VALUE_S);
}

/**
 * @brief Set command value by master mode.
 *
 */
void  spi_master_cfg_cmd(spi_num_t spiNum, uint32_t cmd)
{
    if (spiNum > SPI_NUM_MAX) {
        return;
    }
    // SPI_USER2 bit28-31 is cmd length,cmd bit length is value(0-15)+1,
    // bit15-0 is cmd value.
    SET_PERI_REG_BITS(SPI_USER2_REG(spiNum), SPI_USR_COMMAND_VALUE, cmd, SPI_USR_COMMAND_VALUE_S);
}

/**
 * @brief Send data to slave.
 *
 */
int  spi_master_send_data(spi_num_t spiNum, spi_data_t* pInData)
{
    char idx = 0;
    if ((spiNum > SPI_NUM_MAX)
        || (NULL == pInData)
        || (64 < pInData->tx_data_len)) {
        return -1;
    }
    uint32_t *value = pInData->tx_data;
    while (READ_PERI_REG(SPI_CMD_REG(spiNum))&SPI_USR);
    // Set command by user.
    if (pInData->cmd_len != 0) {
        // Max command length 16 bits.
        SET_PERI_REG_BITS(SPI_USER2_REG(spiNum), SPI_USR_COMMAND_BITLEN,
                          ((pInData->cmd_len << 3) - 1), SPI_USR_COMMAND_BITLEN_S);
        // Enable command
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_COMMAND);
        // Load command
        spi_master_cfg_cmd(spiNum, pInData->cmd);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_COMMAND);
        SET_PERI_REG_BITS(SPI_USER2_REG(spiNum), SPI_USR_COMMAND_BITLEN,
                          0, SPI_USR_COMMAND_BITLEN_S);
    }
    // Set Address by user.
    if (pInData->addr_len == 0) {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_ADDR);
        SET_PERI_REG_BITS(SPI_USER1_REG(spiNum), SPI_USR_ADDR_BITLEN,
                          0, SPI_USR_ADDR_BITLEN_S);
    } else {
        if (NULL == pInData->addr) {
            return -1;
        }
        SET_PERI_REG_BITS(SPI_USER1_REG(spiNum), SPI_USR_ADDR_BITLEN,
                          ((pInData->addr_len << 3) - 1), SPI_USR_ADDR_BITLEN_S);
        // Enable address
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_ADDR);
        // Load address
        spi_master_cfg_addr(spiNum, *pInData->addr);
    }
    // Set data by user.
    if (pInData->tx_data_len != 0) {
        if (NULL == value) {
            return -1;
        }
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MISO);
        // Enable MOSI
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MOSI);
        // Load send buffer
        do {
            WRITE_PERI_REG((SPI_W0_REG(spiNum) + (idx << 2)), *value++);
        } while (++idx < ((pInData->tx_data_len / 4) + ((pInData->tx_data_len % 4) ? 1 : 0)));

        // Set data send buffer length.Max data length 64 bytes.
        SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(spiNum), SPI_USR_MOSI_DBITLEN, ((pInData->tx_data_len << 3) - 1), SPI_USR_MOSI_DBITLEN_S);

        SET_PERI_REG_BITS(SPI_MISO_DLEN_REG(spiNum), SPI_USR_MISO_DBITLEN, ((pInData->rx_data_len << 3) - 1), SPI_USR_MISO_DBITLEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MOSI);
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MISO);
        SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(spiNum), SPI_USR_MOSI_DBITLEN,
                          0, SPI_USR_MOSI_DBITLEN_S);

    }
    // Start send data
    SET_PERI_REG_MASK(SPI_CMD_REG(spiNum), SPI_USR);
    while (!(READ_PERI_REG(SPI_SLAVE_REG(spiNum))&SPI_TRANS_DONE));
    CLEAR_PERI_REG_MASK(SPI_SLAVE_REG(spiNum), SPI_TRANS_DONE);
    return 0;
}

/**
 * @brief Receive data from slave.
 *
 */
int  spi_master_recv_data(spi_num_t spiNum, spi_data_t* pData)
{
    char idx = 0;
    if ((spiNum > SPI_NUM_MAX)
        || (NULL == pData)) {
        return -1;
    }
    uint32_t *value = pData->rx_data;
    while (READ_PERI_REG(SPI_CMD_REG(spiNum))&SPI_USR);
    // Set command by user.
    if (pData->cmd_len != 0) {
        // Max command length 16 bits.
        SET_PERI_REG_BITS(SPI_USER2_REG(spiNum), SPI_USR_COMMAND_BITLEN,
                          ((pData->cmd_len << 3) - 1), SPI_USR_COMMAND_BITLEN_S);
        // Enable command
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_COMMAND);
        // Load command
        spi_master_cfg_cmd(spiNum, pData->cmd);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_COMMAND);
        SET_PERI_REG_BITS(SPI_USER2_REG(spiNum), SPI_USR_COMMAND_BITLEN,
                          0, SPI_USR_COMMAND_BITLEN_S);
    }
    // Set Address by user.
    if (pData->addr_len == 0) {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_ADDR);
        SET_PERI_REG_BITS(SPI_USER1_REG(spiNum), SPI_USR_ADDR_BITLEN,
                          0, SPI_USR_ADDR_BITLEN_S);
    } else {
        if (NULL == pData->addr) {
            return -1;
        }
        SET_PERI_REG_BITS(SPI_USER1_REG(spiNum), SPI_USR_ADDR_BITLEN,
                          ((pData->addr_len << 3) - 1), SPI_USR_ADDR_BITLEN_S);
        // Enable address
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_ADDR);
        // Load address
        spi_master_cfg_addr(spiNum, *pData->addr);
    }
    // Set data by user.
    if (pData->rx_data_len != 0) {
        if (NULL == value) {
            return -1;
        }
        // Clear MOSI enable
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MOSI);
        // Enable MOSI
        SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MISO);
        // Set data send buffer length.Max data length 64 bytes.
        SET_PERI_REG_BITS(SPI_MISO_DLEN_REG(spiNum), SPI_USR_MISO_DBITLEN, ((pData->rx_data_len << 3) - 1), SPI_USR_MISO_DBITLEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MOSI);
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MISO);
        SET_PERI_REG_BITS(SPI_MISO_DLEN_REG(spiNum), SPI_USR_MISO_DBITLEN, 0, SPI_USR_MISO_DBITLEN_S);
    }
    // Start send data
    SET_PERI_REG_MASK(SPI_CMD_REG(spiNum), SPI_USR);

    while (READ_PERI_REG(SPI_CMD_REG(spiNum))&SPI_USR);
    // Read data out
    do {
        *value++ =  READ_PERI_REG(SPI_W0_REG(spiNum) + (idx << 2));
    } while (++idx < ((pData->rx_data_len / 4) + ((pData->rx_data_len % 4) ? 1 : 0)));

    return 0;
}

/**
 * @brief Load data to send buffer by slave mode.

 *
 */
int  spi_slave_send_data(spi_num_t spiNum, uint32_t *pOutData, uint8_t outLen)
{
    if (NULL == pOutData) {
        return -1;
    }
    char i;
    uint32_t *value  = pOutData;
    for (i = 0; i < outLen; ++i) {
        WRITE_PERI_REG((SPI_W0_REG(spiNum) + (i << 2)), *value++);
    }
    return 0;
}

/**
 * @brief Configurate slave prepare for receive data.
 *
 */
int  spi_slave_recv_data(spi_num_t spiNum, void(*isrFunc)(void*))
{
    char idx = 0;
    if (spiNum > SPI_NUM_MAX) {
        return -1;
    }

    spi_int_enable(spiNum, SPI_INT_SRC_WR_STA_DONE
                   | SPI_INT_SRC_RD_STA_DONE | SPI_INT_SRC_WR_BUF_DONE | SPI_INT_SRC_RD_BUF_DONE);
    spi_int_disable(spiNum, SPI_INT_SRC_TRANS_DONE);

    spi_intr_func_set(isrFunc, spiNum);
    return 0;
}

/**
 * @brief Send data to slave(ESP32,RD_STATUS or WR_STATUS).
 *
 */
void  spi_master_send_status(spi_num_t spiNum, uint8_t data)
{
    if (spiNum > SPI_NUM_MAX) {
        return;
    }
    while (READ_PERI_REG(SPI_CMD_REG(spiNum))&SPI_USR);
    // enable MOSI
    SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MOSI);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MISO | SPI_USR_DUMMY | SPI_USR_ADDR);

    // 8bits cmd, 0x04 is eps32 slave write cmd value
    WRITE_PERI_REG(SPI_USER2_REG(spiNum),
                   ((7 & SPI_USR_COMMAND_BITLEN) << SPI_USR_COMMAND_BITLEN_S)
                   | MASTER_WRITE_STATUS_TO_SLAVE_CMD);
    // Set data send buffer length.
    SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(spiNum), SPI_USR_MOSI_DBITLEN,
                      ((sizeof(data) << 3) - 1), SPI_USR_MOSI_DBITLEN_S);

    WRITE_PERI_REG(SPI_W0_REG(spiNum), (uint32_t)(data));
    // start SPI
    SET_PERI_REG_MASK(SPI_CMD_REG(spiNum), SPI_USR);
}

/**
 * @brief Receive data from slave(ESP32).
 *
 */
int  spi_master_recv_status(spi_num_t spiNum)
{
    if (spiNum > SPI_NUM_MAX) {
        return -1;
    }

    while (READ_PERI_REG(SPI_CMD_REG(spiNum))&SPI_USR);
    // enable MISO
    SET_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MISO);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spiNum), SPI_USR_MOSI | SPI_USR_DUMMY | SPI_USR_ADDR);

    // 8bits cmd, 0x06 is eps32 slave read cmd value
    WRITE_PERI_REG(SPI_USER2_REG(spiNum),
                   ((7 & SPI_USR_COMMAND_BITLEN) << SPI_USR_COMMAND_BITLEN_S)
                   | MASTER_READ_STATUS_FROM_SLAVE_CMD);
    // Set revcive buffer length.
    SET_PERI_REG_BITS(SPI_MISO_DLEN_REG(spiNum), SPI_USR_MISO_DBITLEN,
                      7, SPI_USR_MISO_DBITLEN_S);

    // start spi module.
    SET_PERI_REG_MASK(SPI_CMD_REG(spiNum), SPI_USR);

    while (READ_PERI_REG(SPI_CMD_REG(spiNum))&SPI_USR);

    uint8_t data = (uint8_t)(READ_PERI_REG(SPI_W0_REG(spiNum)) & 0xff);
    return (uint8_t)(READ_PERI_REG(SPI_W0_REG(spiNum)) & 0xff);
}


/**
 * @brief Enable SPI interrupt source.
 *
 */
void  spi_int_enable(spi_num_t spiNum, spi_int_src_t intSrc)
{
    if (spiNum > SPI_NUM_MAX) {
        return;
    }
    SET_PERI_REG_MASK(SPI_SLAVE_REG(spiNum), intSrc << 4);
}

/**
 * @brief Disable SPI interrupt source.
 *
 */
void  spi_int_disable(spi_num_t spiNum, spi_int_src_t intSrc)
{
    if (spiNum > SPI_NUM_MAX) {
        return;
    }
    CLEAR_PERI_REG_MASK(SPI_SLAVE_REG(spiNum), intSrc);
}

/**
 * @brief Clear all of SPI interrupt source.
 *
 */
void  spi_int_clear(spi_num_t spiNum)
{
    if (spiNum > SPI_NUM_MAX) {
        return;
    }
    CLEAR_PERI_REG_MASK(SPI_SLAVE_REG(spiNum), SPI_INT_SRC_TRANS_DONE
                        | SPI_INT_SRC_WR_STA_DONE
                        | SPI_INT_SRC_RD_STA_DONE
                        | SPI_INT_SRC_WR_BUF_DONE
                        | SPI_INT_SRC_RD_BUF_DONE);
}

/**
 * @brief Get the SPI interrupt status.
 *
 */
int32_t spi_int_status_get(spi_num_t i2sNum)
{
    if (i2sNum > SPI_NUM_MAX) {
        return -1;
    }
    return READ_PERI_REG(SPI_SLAVE_REG(i2sNum));
}

#ifdef __cplusplus
}
#endif
