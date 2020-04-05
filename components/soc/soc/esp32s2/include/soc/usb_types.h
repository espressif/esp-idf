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

#pragma once
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/* USB IN EP index */
typedef enum {
    USB_IN_EP_0 = 0,
    USB_IN_EP_1,
    USB_IN_EP_2,
    USB_IN_EP_3,
    USB_IN_EP_4,
    USB_IN_EP_5,
    USB_IN_EP_6,
    USB_IN_EP_NUM
} usb_in_ep_idx_t;

/* USB OUT EP index */
typedef enum {
    USB_OUT_EP_0 = 0,
    USB_OUT_EP_1,
    USB_OUT_EP_2,
    USB_OUT_EP_3,
    USB_OUT_EP_4,
    USB_OUT_EP_5,
    USB_OUT_EP_6,
    USB_OUT_EP_NUM
} usb_out_ep_idx_t;

/* USB IN EP Register block type */
typedef struct usb_in_ep_reg {
    volatile uint32_t diepctl;
    uint32_t reserved;
    volatile uint32_t diepint;
    uint32_t reserved1;
    volatile uint32_t dieptsiz;
    volatile uint32_t diepdma;
    volatile uint32_t dtxfsts;
    uint32_t reserved2;
} usb_in_endpoint_t;

/* USB OUT EP Register block type */
typedef struct usb_out_ep_reg {
    volatile uint32_t doepctl;
    uint32_t reserved;
    volatile uint32_t doepint;
    uint32_t reserved1;
    volatile uint32_t doeptsiz;
    volatile uint32_t doepdma;
    uint32_t reserved2;
    uint32_t reserved3;
} usb_out_endpoint_t;

#ifdef __cplusplus
}
#endif
