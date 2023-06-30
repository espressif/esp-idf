/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** USB_WRAP_OTG_CONF_REG register
 *  USB wrapper configuration registers.
 */
#define USB_WRAP_OTG_CONF_REG (DR_REG_USB_WRAP_BASE + 0x0)
/** USB_WRAP_SRP_SESSEND_OVERRIDE : R/W; bitpos: [0]; default: 0;
 *  This bit is used to enable the software over-ride of srp session end signal. 1'b0:
 *  the signal is controlled by the chip input, 1'b1: the signal is controlled by the
 *  software.
 */
#define USB_WRAP_SRP_SESSEND_OVERRIDE    (BIT(0))
#define USB_WRAP_SRP_SESSEND_OVERRIDE_M  (USB_WRAP_SRP_SESSEND_OVERRIDE_V << USB_WRAP_SRP_SESSEND_OVERRIDE_S)
#define USB_WRAP_SRP_SESSEND_OVERRIDE_V  0x00000001U
#define USB_WRAP_SRP_SESSEND_OVERRIDE_S  0
/** USB_WRAP_SRP_SESSEND_VALUE : R/W; bitpos: [1]; default: 0;
 *  Software over-ride value of srp session end signal.
 */
#define USB_WRAP_SRP_SESSEND_VALUE    (BIT(1))
#define USB_WRAP_SRP_SESSEND_VALUE_M  (USB_WRAP_SRP_SESSEND_VALUE_V << USB_WRAP_SRP_SESSEND_VALUE_S)
#define USB_WRAP_SRP_SESSEND_VALUE_V  0x00000001U
#define USB_WRAP_SRP_SESSEND_VALUE_S  1
/** USB_WRAP_PHY_SEL : R/W; bitpos: [2]; default: 0;
 *  Select internal external PHY. 1'b0: Select internal PHY, 1'b1: Select external PHY.
 */
#define USB_WRAP_PHY_SEL    (BIT(2))
#define USB_WRAP_PHY_SEL_M  (USB_WRAP_PHY_SEL_V << USB_WRAP_PHY_SEL_S)
#define USB_WRAP_PHY_SEL_V  0x00000001U
#define USB_WRAP_PHY_SEL_S  2
/** USB_WRAP_DFIFO_FORCE_PD : R/W; bitpos: [3]; default: 0;
 *  Force the dfifo to go into low power mode. The data in dfifo will not lost.
 */
#define USB_WRAP_DFIFO_FORCE_PD    (BIT(3))
#define USB_WRAP_DFIFO_FORCE_PD_M  (USB_WRAP_DFIFO_FORCE_PD_V << USB_WRAP_DFIFO_FORCE_PD_S)
#define USB_WRAP_DFIFO_FORCE_PD_V  0x00000001U
#define USB_WRAP_DFIFO_FORCE_PD_S  3
/** USB_WRAP_DBNCE_FLTR_BYPASS : R/W; bitpos: [4]; default: 0;
 *  Bypass Debounce filters for avalid,bvalid,vbusvalid,session end, id signals
 */
#define USB_WRAP_DBNCE_FLTR_BYPASS    (BIT(4))
#define USB_WRAP_DBNCE_FLTR_BYPASS_M  (USB_WRAP_DBNCE_FLTR_BYPASS_V << USB_WRAP_DBNCE_FLTR_BYPASS_S)
#define USB_WRAP_DBNCE_FLTR_BYPASS_V  0x00000001U
#define USB_WRAP_DBNCE_FLTR_BYPASS_S  4
/** USB_WRAP_EXCHG_PINS_OVERRIDE : R/W; bitpos: [5]; default: 0;
 *  Enable software controlle USB D+ D- exchange
 */
#define USB_WRAP_EXCHG_PINS_OVERRIDE    (BIT(5))
#define USB_WRAP_EXCHG_PINS_OVERRIDE_M  (USB_WRAP_EXCHG_PINS_OVERRIDE_V << USB_WRAP_EXCHG_PINS_OVERRIDE_S)
#define USB_WRAP_EXCHG_PINS_OVERRIDE_V  0x00000001U
#define USB_WRAP_EXCHG_PINS_OVERRIDE_S  5
/** USB_WRAP_EXCHG_PINS : R/W; bitpos: [6]; default: 0;
 *  USB D+ D- exchange. 1'b0: don't change, 1'b1: exchange D+ D-.
 */
#define USB_WRAP_EXCHG_PINS    (BIT(6))
#define USB_WRAP_EXCHG_PINS_M  (USB_WRAP_EXCHG_PINS_V << USB_WRAP_EXCHG_PINS_S)
#define USB_WRAP_EXCHG_PINS_V  0x00000001U
#define USB_WRAP_EXCHG_PINS_S  6
/** USB_WRAP_VREFH : R/W; bitpos: [8:7]; default: 0;
 *  Control single-end input high threshold,1.76V to 2V, step 80mV.
 */
#define USB_WRAP_VREFH    0x00000003U
#define USB_WRAP_VREFH_M  (USB_WRAP_VREFH_V << USB_WRAP_VREFH_S)
#define USB_WRAP_VREFH_V  0x00000003U
#define USB_WRAP_VREFH_S  7
/** USB_WRAP_VREFL : R/W; bitpos: [10:9]; default: 0;
 *  Control single-end input low threshold,0.8V to 1.04V, step 80mV.
 */
#define USB_WRAP_VREFL    0x00000003U
#define USB_WRAP_VREFL_M  (USB_WRAP_VREFL_V << USB_WRAP_VREFL_S)
#define USB_WRAP_VREFL_V  0x00000003U
#define USB_WRAP_VREFL_S  9
/** USB_WRAP_VREF_OVERRIDE : R/W; bitpos: [11]; default: 0;
 *  Enable software controlle input  threshold.
 */
#define USB_WRAP_VREF_OVERRIDE    (BIT(11))
#define USB_WRAP_VREF_OVERRIDE_M  (USB_WRAP_VREF_OVERRIDE_V << USB_WRAP_VREF_OVERRIDE_S)
#define USB_WRAP_VREF_OVERRIDE_V  0x00000001U
#define USB_WRAP_VREF_OVERRIDE_S  11
/** USB_WRAP_PAD_PULL_OVERRIDE : R/W; bitpos: [12]; default: 0;
 *  Enable software controlle USB D+ D- pullup pulldown.
 */
#define USB_WRAP_PAD_PULL_OVERRIDE    (BIT(12))
#define USB_WRAP_PAD_PULL_OVERRIDE_M  (USB_WRAP_PAD_PULL_OVERRIDE_V << USB_WRAP_PAD_PULL_OVERRIDE_S)
#define USB_WRAP_PAD_PULL_OVERRIDE_V  0x00000001U
#define USB_WRAP_PAD_PULL_OVERRIDE_S  12
/** USB_WRAP_DP_PULLUP : R/W; bitpos: [13]; default: 0;
 *  Controlle USB D+ pullup.
 */
#define USB_WRAP_DP_PULLUP    (BIT(13))
#define USB_WRAP_DP_PULLUP_M  (USB_WRAP_DP_PULLUP_V << USB_WRAP_DP_PULLUP_S)
#define USB_WRAP_DP_PULLUP_V  0x00000001U
#define USB_WRAP_DP_PULLUP_S  13
/** USB_WRAP_DP_PULLDOWN : R/W; bitpos: [14]; default: 0;
 *  Controlle USB D+ pulldown.
 */
#define USB_WRAP_DP_PULLDOWN    (BIT(14))
#define USB_WRAP_DP_PULLDOWN_M  (USB_WRAP_DP_PULLDOWN_V << USB_WRAP_DP_PULLDOWN_S)
#define USB_WRAP_DP_PULLDOWN_V  0x00000001U
#define USB_WRAP_DP_PULLDOWN_S  14
/** USB_WRAP_DM_PULLUP : R/W; bitpos: [15]; default: 0;
 *  Controlle USB D+ pullup.
 */
#define USB_WRAP_DM_PULLUP    (BIT(15))
#define USB_WRAP_DM_PULLUP_M  (USB_WRAP_DM_PULLUP_V << USB_WRAP_DM_PULLUP_S)
#define USB_WRAP_DM_PULLUP_V  0x00000001U
#define USB_WRAP_DM_PULLUP_S  15
/** USB_WRAP_DM_PULLDOWN : R/W; bitpos: [16]; default: 0;
 *  Controlle USB D+ pulldown.
 */
#define USB_WRAP_DM_PULLDOWN    (BIT(16))
#define USB_WRAP_DM_PULLDOWN_M  (USB_WRAP_DM_PULLDOWN_V << USB_WRAP_DM_PULLDOWN_S)
#define USB_WRAP_DM_PULLDOWN_V  0x00000001U
#define USB_WRAP_DM_PULLDOWN_S  16
/** USB_WRAP_PULLUP_VALUE : R/W; bitpos: [17]; default: 0;
 *  Controlle pullup value. 1'b0: typical value is 2.4K, 1'b1: typical value is 1.2K.
 */
#define USB_WRAP_PULLUP_VALUE    (BIT(17))
#define USB_WRAP_PULLUP_VALUE_M  (USB_WRAP_PULLUP_VALUE_V << USB_WRAP_PULLUP_VALUE_S)
#define USB_WRAP_PULLUP_VALUE_V  0x00000001U
#define USB_WRAP_PULLUP_VALUE_S  17
/** USB_WRAP_USB_PAD_ENABLE : R/W; bitpos: [18]; default: 0;
 *  Enable USB pad function.
 */
#define USB_WRAP_USB_PAD_ENABLE    (BIT(18))
#define USB_WRAP_USB_PAD_ENABLE_M  (USB_WRAP_USB_PAD_ENABLE_V << USB_WRAP_USB_PAD_ENABLE_S)
#define USB_WRAP_USB_PAD_ENABLE_V  0x00000001U
#define USB_WRAP_USB_PAD_ENABLE_S  18
/** USB_WRAP_AHB_CLK_FORCE_ON : R/W; bitpos: [19]; default: 0;
 *  Force ahb clock always on.
 */
#define USB_WRAP_AHB_CLK_FORCE_ON    (BIT(19))
#define USB_WRAP_AHB_CLK_FORCE_ON_M  (USB_WRAP_AHB_CLK_FORCE_ON_V << USB_WRAP_AHB_CLK_FORCE_ON_S)
#define USB_WRAP_AHB_CLK_FORCE_ON_V  0x00000001U
#define USB_WRAP_AHB_CLK_FORCE_ON_S  19
/** USB_WRAP_PHY_CLK_FORCE_ON : R/W; bitpos: [20]; default: 1;
 *  Force phy clock always on.
 */
#define USB_WRAP_PHY_CLK_FORCE_ON    (BIT(20))
#define USB_WRAP_PHY_CLK_FORCE_ON_M  (USB_WRAP_PHY_CLK_FORCE_ON_V << USB_WRAP_PHY_CLK_FORCE_ON_S)
#define USB_WRAP_PHY_CLK_FORCE_ON_V  0x00000001U
#define USB_WRAP_PHY_CLK_FORCE_ON_S  20
/** USB_WRAP_DFIFO_FORCE_PU : R/W; bitpos: [22]; default: 0;
 *  Disable the dfifo to go into low power mode. The data in dfifo will not lost.
 */
#define USB_WRAP_DFIFO_FORCE_PU    (BIT(22))
#define USB_WRAP_DFIFO_FORCE_PU_M  (USB_WRAP_DFIFO_FORCE_PU_V << USB_WRAP_DFIFO_FORCE_PU_S)
#define USB_WRAP_DFIFO_FORCE_PU_V  0x00000001U
#define USB_WRAP_DFIFO_FORCE_PU_S  22
/** USB_WRAP_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  Disable auto clock gating of CSR registers.
 */
#define USB_WRAP_CLK_EN    (BIT(31))
#define USB_WRAP_CLK_EN_M  (USB_WRAP_CLK_EN_V << USB_WRAP_CLK_EN_S)
#define USB_WRAP_CLK_EN_V  0x00000001U
#define USB_WRAP_CLK_EN_S  31

/** USB_WRAP_DATE_REG register
 *  Date register.
 */
#define USB_WRAP_DATE_REG (DR_REG_USB_WRAP_BASE + 0x3fc)
/** USB_WRAP_USB_WRAP_DATE : HRO; bitpos: [31:0]; default: 587400452;
 *  Date register.
 */
#define USB_WRAP_USB_WRAP_DATE    0xFFFFFFFFU
#define USB_WRAP_USB_WRAP_DATE_M  (USB_WRAP_USB_WRAP_DATE_V << USB_WRAP_USB_WRAP_DATE_S)
#define USB_WRAP_USB_WRAP_DATE_V  0xFFFFFFFFU
#define USB_WRAP_USB_WRAP_DATE_S  0

#ifdef __cplusplus
}
#endif
