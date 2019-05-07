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

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_types.h"
#include "soc/emac_reg_v2.h"

#define EMAC_INTR_ENABLE_BIT (EMAC_DMAIN_TIE | EMAC_DMAIN_RIE | EMAC_DMAIN_RBUE | EMAC_DMAIN_NISE)

struct dma_desc {
    uint32_t desc0;
    uint32_t desc1;
    uint32_t desc2;
    uint32_t desc3;
};

typedef struct dma_extended_desc {
    struct dma_desc basic;
    uint32_t desc4;
    uint32_t desc5;
    uint32_t desc6;
    uint32_t desc7;
} dma_extended_desc_t;

void emac_enable_clk(bool enable);
esp_err_t emac_reset(void);
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
void emac_enable_promiscuous(void);
void emac_disable_promiscuous(void);

static inline uint32_t emac_read_tx_cur_reg(void)
{
    return REG_READ(EMAC_DMATXCURRDESC_REG);
}

static inline uint32_t emac_read_rx_cur_reg(void)
{
    return REG_READ(EMAC_DMARXCURRDESC_REG);
}

static inline void emac_poll_tx_cmd(void)
{
    //write any to wake up dma
    REG_WRITE(EMAC_DMATXPOLLDEMAND_REG, 1);
}

static inline void emac_poll_rx_cmd(void)
{
    //write any to wake up dma
    REG_WRITE(EMAC_DMARXPOLLDEMAND_REG, 1);
}

static inline void emac_disable_rx_intr(void)
{
    REG_CLR_BIT(EMAC_DMAIN_EN_REG, EMAC_DMAIN_RIE);
}

static inline void emac_enable_rx_intr(void)
{
    REG_SET_BIT(EMAC_DMAIN_EN_REG, EMAC_DMAIN_RIE);
}

static inline void emac_disable_rx_unavail_intr(void)
{
    REG_CLR_BIT(EMAC_DMAIN_EN_REG, EMAC_DMAIN_RBUE);
}

static inline void emac_enable_rx_unavail_intr(void)
{
    REG_SET_BIT(EMAC_DMAIN_EN_REG, EMAC_DMAIN_RBUE);
}

static inline void IRAM_ATTR emac_send_pause_frame_enable(void)
{
    REG_SET_BIT(EMAC_EX_PHYINF_CONF_REG, EMAC_EX_SBD_FLOWCTRL);
}

static inline void emac_send_pause_zero_frame_enable(void)
{
    REG_CLR_BIT(EMAC_EX_PHYINF_CONF_REG, EMAC_EX_SBD_FLOWCTRL);
}

#ifdef __cplusplus
}
#endif

#endif
