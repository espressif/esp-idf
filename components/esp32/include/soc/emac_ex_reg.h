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
#ifndef _EMAC_EX_H_
#define _EMAC_EX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "soc.h"
#define REG_EMAC_EX_BASE (DR_REG_EMAC_BASE + 0x800)

#define EMAC_EX_CLKOUT_CONF_REG (REG_EMAC_EX_BASE + 0x0000)
#define EMAC_EX_CLK_OUT_DLY_NUM 0x00000003
#define EMAC_EX_CLK_OUT_DLY_NUM_S 8
#define EMAC_EX_CLK_OUT_H_DIV_NUM 0x0000000F
#define EMAC_EX_CLK_OUT_H_DIV_NUM_S 4
#define EMAC_EX_CLK_OUT_DIV_NUM 0x0000000F
#define EMAC_EX_CLK_OUT_DIV_NUM_S 0

#define EMAC_EX_OSCCLK_CONF_REG (REG_EMAC_EX_BASE + 0x0004)
#define EMAC_EX_OSC_CLK_SEL (BIT(24))
#define EMAC_EX_OSC_CLK_SEL_S 24
#define EMAC_EX_OSC_H_DIV_NUM_100M 0x0000003F
#define EMAC_EX_OSC_H_DIV_NUM_100M_S 18
#define EMAC_EX_OSC_DIV_NUM_100M 0x0000003F
#define EMAC_EX_OSC_DIV_NUM_100M_S 12
#define EMAC_EX_OSC_H_DIV_NUM_10M 0x0000003F
#define EMAC_EX_OSC_H_DIV_NUM_10M_S 6
#define EMAC_EX_OSC_DIV_NUM_10M 0x0000003F
#define EMAC_EX_OSC_DIV_NUM_10M_S 0

#define EMAC_EX_CLK_CTRL_REG (REG_EMAC_EX_BASE + 0x0008)
#define EMAC_EX_CLK_EN (BIT(5))
#define EMAC_EX_CLK_EN_S 5
#define EMAC_EX_MII_CLK_RX_EN (BIT(4))
#define EMAC_EX_MII_CLK_RX_EN_S 4
#define EMAC_EX_MII_CLK_TX_EN (BIT(3))
#define EMAC_EX_MII_CLK_TX_EN_S 3
#define EMAC_EX_RX_125_CLK_EN (BIT(2))
#define EMAC_EX_RX_125_CLK_EN_S 2
#define EMAC_EX_INT_OSC_EN (BIT(1))
#define EMAC_EX_INT_OSC_EN_S 1
#define EMAC_EX_EXT_OSC_EN (BIT(0))
#define EMAC_EX_EXT_OSC_EN_S 0

#define EMAC_EX_PHYINF_CONF_REG (REG_EMAC_EX_BASE + 0x000c)
#define EMAC_EX_TX_ERR_OUT_EN (BIT(20))
#define EMAC_EX_TX_ERR_OUT_EN_S 20
#define EMAC_EX_SCR_SMI_DLY_RX_SYNC (BIT(19))
#define EMAC_EX_SCR_SMI_DLY_RX_SYNC_S 19
#define EMAC_EX_PMT_CTRL_EN (BIT(18))
#define EMAC_EX_PMT_CTRL_EN_S 18
#define EMAC_EX_SBD_CLK_GATING_EN (BIT(17))
#define EMAC_EX_SBD_CLK_GATING_EN_S 17
#define EMAC_EX_SS_MODE (BIT(16))
#define EMAC_EX_SS_MODE_S 16
#define EMAC_EX_PHY_INTF_SEL 0x00000007
#define EMAC_EX_PHY_INTF_SEL_S 13
#define EMAC_EX_REVMII_PHY_ADDR 0x0000001F
#define EMAC_EX_REVMII_PHY_ADDR_S 8
#define EMAC_EX_CORE_PHY_ADDR 0x0000001F
#define EMAC_EX_CORE_PHY_ADDR_S 3
#define EMAC_EX_SBD_FLOWCTRL (BIT(2))
#define EMAC_EX_SBD_FLOWCTRL_S 2
#define EMAC_EX_EXT_REVMII_RX_CLK_SEL (BIT(1))
#define EMAC_EX_EXT_REVMII_RX_CLK_SEL_S 1
#define EMAC_EX_INT_REVMII_RX_CLK_SEL (BIT(0))
#define EMAC_EX_INT_REVMII_RX_CLK_SEL_S 0

#define EMAC_EX_PHY_INTF_RMII  4

#define EMAC_EX_EMAC_PD_SEL_REG (REG_EMAC_EX_BASE + 0x0010)
#define EMAC_EX_RAM_PD_EN 0x00000003
#define EMAC_EX_RAM_PD_EN_S 0

#define EMAC_EX_DATE_REG (REG_EMAC_EX_BASE + 0x00fc)
#define EMAC_EX_DATE 0xFFFFFFFF
#define EMAC_EX_DATE_S 0
#define EMAC_EX_DATE_VERSION 0x16042200

#define EMAC_CLK_EN_REG 0x3ff000cc
#define EMAC_CLK_EN (BIT(14))

#ifdef __cplusplus
}
#endif

#endif

