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

static const char *TAG = "emac";

void emac_enable_flowctrl(void)
{
    REG_SET_BIT(EMAC_GMACFLOWCONTROL_REG, EMAC_TRANSMIT_FLOW_CONTROL_ENABLE);
    REG_SET_BIT(EMAC_GMACFLOWCONTROL_REG, EMAC_RECEIVE_FLOW_CONTROL_ENABLE);
    REG_CLR_BIT(EMAC_GMACFLOWCONTROL_REG, EMAC_DISABLE_ZERO_QUANTA_PAUSE);
    REG_SET_FIELD(EMAC_GMACFLOWCONTROL_REG, EMAC_PAUSE_TIME, 0x1648);
    REG_SET_FIELD(EMAC_GMACFLOWCONTROL_REG, EMAC_PAUSE_LOW_THRESHOLD, 0x1);
}

void emac_disable_flowctrl(void)
{
    REG_CLR_BIT(EMAC_GMACFLOWCONTROL_REG, EMAC_TRANSMIT_FLOW_CONTROL_ENABLE);
    REG_CLR_BIT(EMAC_GMACFLOWCONTROL_REG, EMAC_RECEIVE_FLOW_CONTROL_ENABLE);
    REG_CLR_BIT(EMAC_GMACFLOWCONTROL_REG, EMAC_DISABLE_ZERO_QUANTA_PAUSE);
    REG_SET_FIELD(EMAC_GMACFLOWCONTROL_REG, EMAC_PAUSE_TIME, 0);
    REG_SET_FIELD(EMAC_GMACFLOWCONTROL_REG, EMAC_PAUSE_LOW_THRESHOLD, 0);
}

void emac_enable_dma_tx(void)
{
    REG_SET_BIT(EMAC_DMAOPERATION_MODE_REG, EMAC_START_STOP_TRANSMISSION_COMMAND);
}

void emac_enable_dma_rx(void)
{
    REG_SET_BIT(EMAC_DMAOPERATION_MODE_REG, EMAC_START_STOP_RECEIVE);
}

void emac_disable_dma_tx(void)
{
    REG_CLR_BIT(EMAC_DMAOPERATION_MODE_REG, EMAC_OPERATE_SECOND_FRAME);
}

void emac_disable_dma_rx(void)
{
    REG_CLR_BIT(EMAC_DMAOPERATION_MODE_REG, EMAC_START_STOP_RECEIVE);
}


uint32_t emac_read_mac_version(void)
{
    uint32_t data = 0;
    data = REG_READ(EMAC_GMACVERSION_REG);
    return data;
}

void emac_reset(void)
{
    REG_SET_BIT(EMAC_DMABUSMODE_REG, EMAC_SW_RST);

    while (REG_GET_BIT(EMAC_DMABUSMODE_REG, EMAC_SW_RST) == 1) {
        //nothing to do ,if stop here,maybe emac have not clk input.
        ESP_LOGI(TAG, "emac resetting ....");
    }

    ESP_LOGI(TAG, "emac reset done");
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
    REG_SET_BIT(EMAC_DMAOPERATION_MODE_REG, EMAC_FORWARD_UNDERSIZED_GOOD_FRAMES);
    REG_SET_BIT(EMAC_DMAOPERATION_MODE_REG, EMAC_OPERATE_SECOND_FRAME);
    REG_SET_FIELD(EMAC_DMABUSMODE_REG, EMAC_PROG_BURST_LEN, 4);
    REG_SET_BIT(EMAC_DMAOPERATION_MODE_REG, EMAC_DMAOPERATION_MODE_REG);
}

void emac_mac_enable_txrx(void)
{
    REG_SET_BIT(EMAC_GMACCONFIG_REG, EMAC_GMACRX);
    REG_SET_BIT(EMAC_GMACCONFIG_REG, EMAC_GMACTX);
}

void emac_mac_init(void)
{
    REG_SET_BIT(EMAC_GMACCONFIG_REG, EMAC_GMACDUPLEX);
    REG_SET_BIT(EMAC_GMACCONFIG_REG, EMAC_GMACMIIGMII);
    REG_CLR_BIT(EMAC_GMACCONFIG_REG, EMAC_GMACFESPEED);
    REG_SET_BIT(EMAC_GMACFRAMEFILTER_REG, EMAC_PROMISCUOUS_MODE);
}
