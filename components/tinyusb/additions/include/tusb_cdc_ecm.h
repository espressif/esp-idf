/*
Copyright © 2021 Ci4Rail GmbH
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
 http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#ifndef _TUSB_ETHERNET_OVER_USB_H_
#define _TUSB_ETHERNET_OVER_USB_H_


typedef struct {
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gateway;
    uint8_t mac_address[6];
} tinyusb_config_ethernet_over_usb_t;

esp_err_t tusb_ethernet_over_usb_init(tinyusb_config_ethernet_over_usb_t cfg);

#endif
