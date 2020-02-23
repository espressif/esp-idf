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
#include "esp32s2/rom/gpio.h"
#include "soc/gpio_sig_map.h"
#include "soc/usb_periph.h"

void gpio_pad_input_enable(uint32_t pin);

static inline void usb_ll_init(bool external_phy)
{
    REG_SET_BIT(DPORT_PERIP_CLK_EN0_REG, DPORT_USB_CLK_EN);
    REG_CLR_BIT(DPORT_PERIP_RST_EN0_REG, DPORT_USB_RST);

    if (external_phy) {
        REG_SET_BIT(DR_REG_USB_WRAP_BASE, BIT(18)); //set usb_pad_enable
        REG_SET_BIT(DR_REG_USB_WRAP_BASE, BIT(2)); // set reg_phy_sel (external phy)
        gpio_output_set_high(0x10, 0, 0x1E, 0xE);
    } else {
        REG_SET_BIT(DR_REG_USB_WRAP_BASE, BIT(18)); //set usb_pad_enable
        REG_CLR_BIT(DR_REG_USB_WRAP_BASE, BIT(2)); // clear reg_phy_sel (internal phy)
        //drive strength needs to be 3 for full speed
        REG_SET_FIELD(GPIO_PIN19_REG, GPIO_PIN19_PAD_DRIVER, 3);
        REG_SET_FIELD(GPIO_PIN20_REG, GPIO_PIN20_PAD_DRIVER, 3);
    }

    int i = 0;
    while (usb_periph_iopins[i].pin != -1) {
        if ((external_phy) || (usb_periph_iopins[i].ext_phy_only == 0)) {
            gpio_pad_select_gpio(usb_periph_iopins[i].pin);
            if (usb_periph_iopins[i].is_output) {
                gpio_matrix_out(usb_periph_iopins[i].pin, usb_periph_iopins[i].func, false, false);
            } else {
                gpio_matrix_in(usb_periph_iopins[i].pin, usb_periph_iopins[i].func, false);
                gpio_pad_input_enable(usb_periph_iopins[i].pin);
            }
            gpio_pad_unhold(usb_periph_iopins[i].pin);
        }
        i++;
    }
    REG_SET_BIT(DR_REG_USB_WRAP_BASE, BIT(12)); //pull override
    REG_SET_BIT(DR_REG_USB_WRAP_BASE, BIT(13)); //dp pullup
    REG_CLR_BIT(DR_REG_USB_WRAP_BASE, BIT(14)); //dp pulldwn
    REG_CLR_BIT(DR_REG_USB_WRAP_BASE, BIT(15)); //dm pullup
    REG_CLR_BIT(DR_REG_USB_WRAP_BASE, BIT(16)); //dm pulldown
}
