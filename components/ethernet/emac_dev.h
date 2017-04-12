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

#ifndef _EMAC_DEV_H_
#define _EMAC_DEV_H_

#include <stdint.h>
#include "soc/emac_reg_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EMAC_INTR_ENABLE_BIT (EMAC_TRANSMIT_INTERRUPT_ENABLE | EMAC_RECEIVE_INTERRUPT_ENABLE | EMAC_RECEIVE_BUFFER_UNAVAILABLE_ENABLE | EMAC_NORMAL_INTERRUPT_SUMMARY_ENABLE)

struct dma_desc {
    uint32_t desc0;
    uint32_t desc1;
    uint32_t desc2;
    uint32_t desc3;
};

struct dma_extended_desc {
    struct dma_desc basic;
    uint32_t desc4;
    uint32_t desc5;
    uint32_t desc6;
    uint32_t desc7;
};

void emac_enable_clk(bool enable);
void emac_set_clk_rmii(void);
void emac_set_clk_mii(void);
void emac_reset(void);
void emac_set_gpio_pin_rmii(void);
void emac_set_gpio_pin_mii(void);
uint32_t emac_read_mac_version(void);
void emac_dma_init(void);
void emac_mac_init(void);
void emac_enable_dma_tx(void);
void emac_enable_dma_rx(void);
void emac_disable_dma_tx(void);
void emac_disable_dma_rx(void);
void emac_enable_flowctrl(void);
void emac_disable_flowctrl(void);
void emac_mac_enable_txrx(void);

uint32_t inline emac_read_tx_cur_reg(void)
{
    return REG_READ(EMAC_DMATXCURRDESC_REG);
}

uint32_t inline emac_read_rx_cur_reg(void)
{
    return REG_READ(EMAC_DMARXCURRDESC_REG);
}

void inline emac_poll_tx_cmd(void)
{
    //write any to wake up dma
    REG_WRITE(EMAC_DMATXPOLLDEMAND_REG, 1);
}

void inline emac_poll_rx_cmd(void)
{
    //write any to wake up dma
    REG_WRITE(EMAC_DMARXPOLLDEMAND_REG, 1);
}

void inline emac_disable_rx_intr(void)
{
    REG_CLR_BIT(EMAC_DMAINTERRUPT_EN_REG, EMAC_RECEIVE_INTERRUPT_ENABLE);
}

void inline emac_enable_rx_intr(void)
{
    REG_SET_BIT(EMAC_DMAINTERRUPT_EN_REG, EMAC_RECEIVE_INTERRUPT_ENABLE);
}

void inline emac_disable_rx_unavail_intr(void)
{
    REG_CLR_BIT(EMAC_DMAINTERRUPT_EN_REG, EMAC_RECEIVE_BUFFER_UNAVAILABLE_ENABLE);
}

void inline emac_enable_rx_unavail_intr(void)
{
    REG_SET_BIT(EMAC_DMAINTERRUPT_EN_REG, EMAC_RECEIVE_BUFFER_UNAVAILABLE_ENABLE);
}

void IRAM_ATTR inline emac_send_pause_frame_enable(void)
{
    REG_SET_BIT(EMAC_EX_PHYINF_CONF_REG, EMAC_EX_SBD_FLOWCTRL);
}

void inline emac_send_pause_zero_frame_enable(void)
{
    REG_CLR_BIT(EMAC_EX_PHYINF_CONF_REG, EMAC_EX_SBD_FLOWCTRL);
}

#ifdef __cplusplus
}
#endif

#endif

