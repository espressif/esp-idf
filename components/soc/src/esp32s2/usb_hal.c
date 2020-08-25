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


#include "hal/usb_ll.h"
#include "hal/usb_hal.h"

void usb_hal_init(usb_hal_context_t *usb)
{
    if (usb->use_external_phy) {
        usb_ll_ext_phy_enable();
    } else {
        usb_ll_int_phy_enable();
    }
}
