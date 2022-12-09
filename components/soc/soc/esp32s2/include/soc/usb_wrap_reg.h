/** Copyright 2020 Espressif Systems (Shanghai) PTE LTD
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** USB_WRAP_OTG_CONF_REG register
 *  PAD/DFIFO/PHY configuration register.
 */
#define USB_WRAP_OTG_CONF_REG (DR_REG_USB_WRAP_BASE + 0x0)
/** USB_WRAP_SRP_SESSEND_OVERRIDE : R/W; bitpos: [0]; default: 0;
 *  This bit is used to enable the software override of srp session end signal.1'b0:
 *  the signal is controlled by the chip input.1'b1: the signal is controlled by the
 *  software.
 */
#define USB_WRAP_SRP_SESSEND_OVERRIDE    (BIT(0))
#define USB_WRAP_SRP_SESSEND_OVERRIDE_M  (USB_WRAP_SRP_SESSEND_OVERRIDE_V << USB_WRAP_SRP_SESSEND_OVERRIDE_S)
#define USB_WRAP_SRP_SESSEND_OVERRIDE_V  0x00000001
#define USB_WRAP_SRP_SESSEND_OVERRIDE_S  0
/** USB_WRAP_SRP_SESSEND_VALUE : R/W; bitpos: [1]; default: 0;
 *  Software override value of srp session end signal.
 */
#define USB_WRAP_SRP_SESSEND_VALUE    (BIT(1))
#define USB_WRAP_SRP_SESSEND_VALUE_M  (USB_WRAP_SRP_SESSEND_VALUE_V << USB_WRAP_SRP_SESSEND_VALUE_S)
#define USB_WRAP_SRP_SESSEND_VALUE_V  0x00000001
#define USB_WRAP_SRP_SESSEND_VALUE_S  1
/** USB_WRAP_PHY_SEL : R/W; bitpos: [2]; default: 0;
 *  Select internal or external PHY.1'b0: Select internal PHY.1'b1: Select external PHY
 */
#define USB_WRAP_PHY_SEL    (BIT(2))
#define USB_WRAP_PHY_SEL_M  (USB_WRAP_PHY_SEL_V << USB_WRAP_PHY_SEL_S)
#define USB_WRAP_PHY_SEL_V  0x00000001
#define USB_WRAP_PHY_SEL_S  2
/** USB_WRAP_DFIFO_FORCE_PD : R/W; bitpos: [3]; default: 0;
 *  Force the dfifo to go into low power mode. The data in dfifo will not lost.
 */
#define USB_WRAP_DFIFO_FORCE_PD    (BIT(3))
#define USB_WRAP_DFIFO_FORCE_PD_M  (USB_WRAP_DFIFO_FORCE_PD_V << USB_WRAP_DFIFO_FORCE_PD_S)
#define USB_WRAP_DFIFO_FORCE_PD_V  0x00000001
#define USB_WRAP_DFIFO_FORCE_PD_S  3
/** USB_WRAP_DBNCE_FLTR_BYPASS : R/W; bitpos: [4]; default: 0;
 *  Bypass Debounce filters for avalid.
 */
#define USB_WRAP_DBNCE_FLTR_BYPASS    (BIT(4))
#define USB_WRAP_DBNCE_FLTR_BYPASS_M  (USB_WRAP_DBNCE_FLTR_BYPASS_V << USB_WRAP_DBNCE_FLTR_BYPASS_S)
#define USB_WRAP_DBNCE_FLTR_BYPASS_V  0x00000001
#define USB_WRAP_DBNCE_FLTR_BYPASS_S  4
/** USB_WRAP_EXCHG_PINS_OVERRIDE : R/W; bitpos: [5]; default: 0;
 *  Enable software to control USB D+ D- exchange
 */
#define USB_WRAP_EXCHG_PINS_OVERRIDE    (BIT(5))
#define USB_WRAP_EXCHG_PINS_OVERRIDE_M  (USB_WRAP_EXCHG_PINS_OVERRIDE_V << USB_WRAP_EXCHG_PINS_OVERRIDE_S)
#define USB_WRAP_EXCHG_PINS_OVERRIDE_V  0x00000001
#define USB_WRAP_EXCHG_PINS_OVERRIDE_S  5
/** USB_WRAP_EXCHG_PINS : R/W; bitpos: [6]; default: 0;
 *  USB D+/D- exchange.1'b0: don't change.1'b1: exchange D+ D-.
 */
#define USB_WRAP_EXCHG_PINS    (BIT(6))
#define USB_WRAP_EXCHG_PINS_M  (USB_WRAP_EXCHG_PINS_V << USB_WRAP_EXCHG_PINS_S)
#define USB_WRAP_EXCHG_PINS_V  0x00000001
#define USB_WRAP_EXCHG_PINS_S  6
/** USB_WRAP_VREFH : R/W; bitpos: [8:7]; default: 0;
 *  Control single-end input high threshold.
 */
#define USB_WRAP_VREFH    0x00000003
#define USB_WRAP_VREFH_M  (USB_WRAP_VREFH_V << USB_WRAP_VREFH_S)
#define USB_WRAP_VREFH_V  0x00000003
#define USB_WRAP_VREFH_S  7
/** USB_WRAP_VREFL : R/W; bitpos: [10:9]; default: 0;
 *  Control single-end input low threshold.
 */
#define USB_WRAP_VREFL    0x00000003
#define USB_WRAP_VREFL_M  (USB_WRAP_VREFL_V << USB_WRAP_VREFL_S)
#define USB_WRAP_VREFL_V  0x00000003
#define USB_WRAP_VREFL_S  9
/** USB_WRAP_VREF_OVERRIDE : R/W; bitpos: [11]; default: 0;
 *  Enable software to control input  threshold.
 */
#define USB_WRAP_VREF_OVERRIDE    (BIT(11))
#define USB_WRAP_VREF_OVERRIDE_M  (USB_WRAP_VREF_OVERRIDE_V << USB_WRAP_VREF_OVERRIDE_S)
#define USB_WRAP_VREF_OVERRIDE_V  0x00000001
#define USB_WRAP_VREF_OVERRIDE_S  11
/** USB_WRAP_PAD_PULL_OVERRIDE : R/W; bitpos: [12]; default: 0;
 *  Enable software to control USB pad in pullup or pulldown mode.
 */
#define USB_WRAP_PAD_PULL_OVERRIDE    (BIT(12))
#define USB_WRAP_PAD_PULL_OVERRIDE_M  (USB_WRAP_PAD_PULL_OVERRIDE_V << USB_WRAP_PAD_PULL_OVERRIDE_S)
#define USB_WRAP_PAD_PULL_OVERRIDE_V  0x00000001
#define USB_WRAP_PAD_PULL_OVERRIDE_S  12
/** USB_WRAP_DP_PULLUP : R/W; bitpos: [13]; default: 0;
 *  Control USB D+ pullup.
 */
#define USB_WRAP_DP_PULLUP    (BIT(13))
#define USB_WRAP_DP_PULLUP_M  (USB_WRAP_DP_PULLUP_V << USB_WRAP_DP_PULLUP_S)
#define USB_WRAP_DP_PULLUP_V  0x00000001
#define USB_WRAP_DP_PULLUP_S  13
/** USB_WRAP_DP_PULLDOWN : R/W; bitpos: [14]; default: 0;
 *  Control USB D+ pulldown.
 */
#define USB_WRAP_DP_PULLDOWN    (BIT(14))
#define USB_WRAP_DP_PULLDOWN_M  (USB_WRAP_DP_PULLDOWN_V << USB_WRAP_DP_PULLDOWN_S)
#define USB_WRAP_DP_PULLDOWN_V  0x00000001
#define USB_WRAP_DP_PULLDOWN_S  14
/** USB_WRAP_DM_PULLUP : R/W; bitpos: [15]; default: 0;
 *  Control USB D+ pullup.
 */
#define USB_WRAP_DM_PULLUP    (BIT(15))
#define USB_WRAP_DM_PULLUP_M  (USB_WRAP_DM_PULLUP_V << USB_WRAP_DM_PULLUP_S)
#define USB_WRAP_DM_PULLUP_V  0x00000001
#define USB_WRAP_DM_PULLUP_S  15
/** USB_WRAP_DM_PULLDOWN : R/W; bitpos: [16]; default: 0;
 *  Control USB D+ pulldown.
 */
#define USB_WRAP_DM_PULLDOWN    (BIT(16))
#define USB_WRAP_DM_PULLDOWN_M  (USB_WRAP_DM_PULLDOWN_V << USB_WRAP_DM_PULLDOWN_S)
#define USB_WRAP_DM_PULLDOWN_V  0x00000001
#define USB_WRAP_DM_PULLDOWN_S  16
/** USB_WRAP_PULLUP_VALUE : R/W; bitpos: [17]; default: 0;
 *  Control pullup value.1'b0: typical value is 2.4K.1'b1: typical value is 1.2K.
 */
#define USB_WRAP_PULLUP_VALUE    (BIT(17))
#define USB_WRAP_PULLUP_VALUE_M  (USB_WRAP_PULLUP_VALUE_V << USB_WRAP_PULLUP_VALUE_S)
#define USB_WRAP_PULLUP_VALUE_V  0x00000001
#define USB_WRAP_PULLUP_VALUE_S  17
/** USB_WRAP_PAD_ENABLE : R/W; bitpos: [18]; default: 0;
 *  Enable USB pad function.
 */
#define USB_WRAP_PAD_ENABLE    (BIT(18))
#define USB_WRAP_PAD_ENABLE_M  (USB_WRAP_PAD_ENABLE_V << USB_WRAP_PAD_ENABLE_S)
#define USB_WRAP_PAD_ENABLE_V  0x00000001
#define USB_WRAP_PAD_ENABLE_S  18
/** USB_WRAP_AHB_CLK_FORCE_ON : R/W; bitpos: [19]; default: 1;
 *  Force AHB clock always on.
 */
#define USB_WRAP_AHB_CLK_FORCE_ON    (BIT(19))
#define USB_WRAP_AHB_CLK_FORCE_ON_M  (USB_WRAP_AHB_CLK_FORCE_ON_V << USB_WRAP_AHB_CLK_FORCE_ON_S)
#define USB_WRAP_AHB_CLK_FORCE_ON_V  0x00000001
#define USB_WRAP_AHB_CLK_FORCE_ON_S  19
/** USB_WRAP_PHY_CLK_FORCE_ON : R/W; bitpos: [20]; default: 1;
 *  Force PHY clock always on.
 */
#define USB_WRAP_PHY_CLK_FORCE_ON    (BIT(20))
#define USB_WRAP_PHY_CLK_FORCE_ON_M  (USB_WRAP_PHY_CLK_FORCE_ON_V << USB_WRAP_PHY_CLK_FORCE_ON_S)
#define USB_WRAP_PHY_CLK_FORCE_ON_V  0x00000001
#define USB_WRAP_PHY_CLK_FORCE_ON_S  20
/** USB_WRAP_PHY_TX_EDGE_SEL : R/W; bitpos: [21]; default: 0;
 *  Select PHY tx signal output clock edge.1'b0: negedge;1'b1: posedge.
 */
#define USB_WRAP_PHY_TX_EDGE_SEL    (BIT(21))
#define USB_WRAP_PHY_TX_EDGE_SEL_M  (USB_WRAP_PHY_TX_EDGE_SEL_V << USB_WRAP_PHY_TX_EDGE_SEL_S)
#define USB_WRAP_PHY_TX_EDGE_SEL_V  0x00000001
#define USB_WRAP_PHY_TX_EDGE_SEL_S  21
/** USB_WRAP_DFIFO_FORCE_PU : R/W; bitpos: [22]; default: 0;
 *  Disable the dfifo to go into low power mode. The data in dfifo will not lost.
 */
#define USB_WRAP_DFIFO_FORCE_PU    (BIT(22))
#define USB_WRAP_DFIFO_FORCE_PU_M  (USB_WRAP_DFIFO_FORCE_PU_V << USB_WRAP_DFIFO_FORCE_PU_S)
#define USB_WRAP_DFIFO_FORCE_PU_V  0x00000001
#define USB_WRAP_DFIFO_FORCE_PU_S  22
/** USB_WRAP_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  Disable auto clock gating of CSR registers.
 */
#define USB_WRAP_CLK_EN    (BIT(31))
#define USB_WRAP_CLK_EN_M  (USB_WRAP_CLK_EN_V << USB_WRAP_CLK_EN_S)
#define USB_WRAP_CLK_EN_V  0x00000001
#define USB_WRAP_CLK_EN_S  31

/** USB_WRAP_TEST_CONF_REG register
 *  TEST relative configuration registers.
 */
#define USB_WRAP_TEST_CONF_REG (DR_REG_USB_WRAP_BASE + 0x4)
/** USB_WRAP_TEST_ENABLE : R/W; bitpos: [0]; default: 0;
 *  Enable to test the USB pad.
 */
#define USB_WRAP_TEST_ENABLE    (BIT(0))
#define USB_WRAP_TEST_ENABLE_M  (USB_WRAP_TEST_ENABLE_V << USB_WRAP_TEST_ENABLE_S)
#define USB_WRAP_TEST_ENABLE_V  0x00000001
#define USB_WRAP_TEST_ENABLE_S  0
/** USB_WRAP_TEST_USB_WRAP_OE : R/W; bitpos: [1]; default: 0;
 *  USB pad oen in test.
 */
#define USB_WRAP_TEST_USB_WRAP_OE    (BIT(1))
#define USB_WRAP_TEST_USB_WRAP_OE_M  (USB_WRAP_TEST_USB_WRAP_OE_V << USB_WRAP_TEST_USB_WRAP_OE_S)
#define USB_WRAP_TEST_USB_WRAP_OE_V  0x00000001
#define USB_WRAP_TEST_USB_WRAP_OE_S  1
/** USB_WRAP_TEST_TX_DP : R/W; bitpos: [2]; default: 0;
 *  USB D+ tx value in test.
 */
#define USB_WRAP_TEST_TX_DP    (BIT(2))
#define USB_WRAP_TEST_TX_DP_M  (USB_WRAP_TEST_TX_DP_V << USB_WRAP_TEST_TX_DP_S)
#define USB_WRAP_TEST_TX_DP_V  0x00000001
#define USB_WRAP_TEST_TX_DP_S  2
/** USB_WRAP_TEST_TX_DM : R/W; bitpos: [3]; default: 0;
 *  USB D- tx value in test.
 */
#define USB_WRAP_TEST_TX_DM    (BIT(3))
#define USB_WRAP_TEST_TX_DM_M  (USB_WRAP_TEST_TX_DM_V << USB_WRAP_TEST_TX_DM_S)
#define USB_WRAP_TEST_TX_DM_V  0x00000001
#define USB_WRAP_TEST_TX_DM_S  3
/** USB_WRAP_TEST_RX_RCV : RO; bitpos: [4]; default: 0;
 *  USB differential rx value in test.
 */
#define USB_WRAP_TEST_RX_RCV    (BIT(4))
#define USB_WRAP_TEST_RX_RCV_M  (USB_WRAP_TEST_RX_RCV_V << USB_WRAP_TEST_RX_RCV_S)
#define USB_WRAP_TEST_RX_RCV_V  0x00000001
#define USB_WRAP_TEST_RX_RCV_S  4
/** USB_WRAP_TEST_RX_DP : RO; bitpos: [5]; default: 0;
 *  USB D+ rx value in test.
 */
#define USB_WRAP_TEST_RX_DP    (BIT(5))
#define USB_WRAP_TEST_RX_DP_M  (USB_WRAP_TEST_RX_DP_V << USB_WRAP_TEST_RX_DP_S)
#define USB_WRAP_TEST_RX_DP_V  0x00000001
#define USB_WRAP_TEST_RX_DP_S  5
/** USB_WRAP_TEST_RX_DM : RO; bitpos: [6]; default: 0;
 *  USB D- rx value in test.
 */
#define USB_WRAP_TEST_RX_DM    (BIT(6))
#define USB_WRAP_TEST_RX_DM_M  (USB_WRAP_TEST_RX_DM_V << USB_WRAP_TEST_RX_DM_S)
#define USB_WRAP_TEST_RX_DM_V  0x00000001
#define USB_WRAP_TEST_RX_DM_S  6

/** USB_WRAP_DATE_REG register
 *  Version register.
 */
#define USB_WRAP_DATE_REG (DR_REG_USB_WRAP_BASE + 0x3fc)
/** USB_WRAP_DATE : R/W; bitpos: [31:0]; default: 419631616;
 *  data register.
 */
#define USB_WRAP_DATE    0xFFFFFFFF
#define USB_WRAP_DATE_M  (USB_WRAP_DATE_V << USB_WRAP_DATE_S)
#define USB_WRAP_DATE_V  0xFFFFFFFF
#define USB_WRAP_DATE_S  0

#ifdef __cplusplus
}
#endif


