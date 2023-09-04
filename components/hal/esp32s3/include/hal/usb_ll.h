/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "soc/soc.h"
#include "soc/system_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/usb_periph.h"
#include "soc/rtc_cntl_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void usb_ll_int_phy_enable(void)
{
    USB_WRAP.otg_conf.pad_enable = 1;
    // USB_OTG use internal PHY
    USB_WRAP.otg_conf.phy_sel = 0;
    // phy_sel is controlled by the following register value
    RTCCNTL.usb_conf.sw_hw_usb_phy_sel = 1;
    // phy_sel=sw_usb_phy_sel=1, USB_OTG is connected with internal PHY
    RTCCNTL.usb_conf.sw_usb_phy_sel = 1;
}

static inline void usb_ll_ext_phy_enable(void)
{
    USB_WRAP.otg_conf.pad_enable = 1;
    // USB_OTG use external PHY
    USB_WRAP.otg_conf.phy_sel = 1;
    // phy_sel is controlled by the following register value
    RTCCNTL.usb_conf.sw_hw_usb_phy_sel = 1;
    // phy_sel=sw_usb_phy_sel=0, USB_OTG is connected with external PHY through GPIO Matrix
    RTCCNTL.usb_conf.sw_usb_phy_sel = 0;
}

static inline void usb_ll_int_phy_pullup_conf(bool dp_pu, bool dp_pd, bool dm_pu, bool dm_pd)
{
    usb_wrap_otg_conf_reg_t conf;
    conf.val = USB_WRAP.otg_conf.val;

    conf.pad_pull_override = 1;
    conf.dp_pullup = dp_pu;
    conf.dp_pulldown = dp_pd;
    conf.dm_pullup = dm_pu;
    conf.dm_pulldown = dm_pd;
    USB_WRAP.otg_conf.val = conf.val;
}

#ifdef __cplusplus
}
#endif
