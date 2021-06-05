// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include "soc/soc.h"
#include "soc/system_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/usb_periph.h"

static inline void usb_ll_int_phy_enable(void)
{
    USB_WRAP.otg_conf.pad_enable = 1;
    USB_WRAP.otg_conf.phy_sel = 0;
}

static inline void usb_ll_ext_phy_enable(void)
{
    USB_WRAP.otg_conf.pad_enable = 1;
    USB_WRAP.otg_conf.phy_sel = 1;
}

static inline void usb_ll_int_phy_pullup_conf(bool dp_pu, bool dp_pd, bool dm_pu, bool dm_pd)
{
    usb_wrap_otg_conf_reg_t conf = USB_WRAP.otg_conf;
    conf.pad_pull_override = 1;
    conf.dp_pullup = dp_pu;
    conf.dp_pulldown = dp_pd;
    conf.dm_pullup = dm_pu;
    conf.dm_pulldown = dm_pd;
    USB_WRAP.otg_conf = conf;
}
