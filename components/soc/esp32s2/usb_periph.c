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

#include "soc/soc_caps.h"
#include "soc/usb_periph.h"

const usb_iopin_dsc_t usb_periph_iopins[] = {
    {USBPHY_VP_NUM, USB_EXTPHY_VP_IDX, 0, 1},
    {USBPHY_VM_NUM, USB_EXTPHY_VM_IDX, 0, 1},
    {USBPHY_RCV_NUM, USB_EXTPHY_RCV_IDX, 0, 1},
    {USBPHY_OEN_NUM, USB_EXTPHY_OEN_IDX, 1, 1},
    {USBPHY_VPO_NUM, USB_EXTPHY_VPO_IDX, 1, 1},
    {USBPHY_VMO_NUM, USB_EXTPHY_VMO_IDX, 1, 1},
    {GPIO_MATRIX_CONST_ONE_INPUT, USB_OTG_IDDIG_IN_IDX, 0, 0},     //connected connector is mini-B
    //connected connector is mini-B
    {GPIO_MATRIX_CONST_ONE_INPUT, USB_SRP_BVALID_IN_IDX, 0, 0},    //HIGH to force USB device mode
    {GPIO_MATRIX_CONST_ONE_INPUT, USB_OTG_VBUSVALID_IN_IDX, 0, 0}, //receiving a valid Vbus from host
    {GPIO_MATRIX_CONST_ZERO_INPUT, USB_OTG_AVALID_IN_IDX, 0, 0},
    {-1, -1, 0, 0}
};
