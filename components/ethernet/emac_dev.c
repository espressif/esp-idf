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

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"
#include "rom/gpio.h"

#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/emac_reg_v2.h"
#include "soc/emac_ex_reg.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "emac_common.h"

void emac_enable_flowctrl(void)
{
    REG_SET_BIT(EMAC_GMACFC_REG, EMAC_TFCE);
    REG_SET_BIT(EMAC_GMACFC_REG, EMAC_RFCE);
    REG_CLR_BIT(EMAC_GMACFC_REG, EMAC_DZPQ);
    REG_SET_FIELD(EMAC_GMACFC_REG, EMAC_PAUSE_TIME, 0x1648);
    REG_SET_FIELD(EMAC_GMACFC_REG, EMAC_PLT, 0x1);
}

void emac_disable_flowctrl(void)
{
    REG_CLR_BIT(EMAC_GMACFC_REG, EMAC_TFCE);
    REG_CLR_BIT(EMAC_GMACFC_REG, EMAC_RFCE);
    REG_CLR_BIT(EMAC_GMACFC_REG, EMAC_DZPQ);
    REG_SET_FIELD(EMAC_GMACFC_REG, EMAC_PAUSE_TIME, 0);
    REG_SET_FIELD(EMAC_GMACFC_REG, EMAC_PLT, 0);
}

void emac_enable_dma_tx(void)
{
    REG_SET_BIT(EMAC_DMAOPERATION_MODE_REG, EMAC_START_STOP_TRANSMISSION_COMMAND);
}

void emac_enable_dma_rx(void)
{
    REG_SET_BIT(EMAC_DMAOPERATION_MODE_REG, EMAC_START_STOP_RX);
}

void emac_disable_dma_tx(void)
{
    REG_CLR_BIT(EMAC_DMAOPERATION_MODE_REG, EMAC_START_STOP_TRANSMISSION_COMMAND);
}

void emac_disable_dma_rx(void)
{
    REG_CLR_BIT(EMAC_DMAOPERATION_MODE_REG, EMAC_START_STOP_RX);
}

void emac_enable_clk(bool enable)
{
    if (enable == true) {
        DPORT_REG_SET_BIT(EMAC_CLK_EN_REG, EMAC_CLK_EN);
    } else {
        DPORT_REG_CLR_BIT(EMAC_CLK_EN_REG, EMAC_CLK_EN);
    }
}

void emac_dma_init(void)
{
    REG_SET_BIT(EMAC_DMAOPERATION_MODE_REG, EMAC_FWD_UNDER_GF);
    REG_SET_BIT(EMAC_DMAOPERATION_MODE_REG, EMAC_OPT_SECOND_FRAME);
    REG_SET_FIELD(EMAC_DMABUSMODE_REG, EMAC_PROG_BURST_LEN, 4);
}

void emac_mac_enable_txrx(void)
{
    REG_SET_BIT(EMAC_GMACCONFIG_REG, EMAC_EMACRX);
    REG_SET_BIT(EMAC_GMACCONFIG_REG, EMAC_EMACTX);
}

void emac_mac_init(void)
{
    REG_SET_BIT(EMAC_GMACCONFIG_REG, EMAC_EMACDUPLEX);
    REG_SET_BIT(EMAC_GMACCONFIG_REG, EMAC_EMACMII);
    REG_CLR_BIT(EMAC_GMACCONFIG_REG, EMAC_EMACFESPEED);
}
