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

#ifndef __BT_H__
#define __BT_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief  Initialize BT controller
 *
 * This function should be called only once, before any other BT functions are called.
 */
void esp_bt_controller_init(void);

/** @brief esp_vhci_host_callback
 *  used for vhci call host function to notify what host need to do
 */
typedef struct esp_vhci_host_callback {
    void (*notify_host_send_available)(void);               /*!< callback used to notify that the host can send packet to controller */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   /*!< callback used to notify that the controller has a packet to send to the host*/
} esp_vhci_host_callback_t;

/** @brief esp_vhci_host_check_send_available
 *  used for check actively if the host can send packet to controller or not.
 *  @return true for ready to send, false means cannot send packet
 */
bool esp_vhci_host_check_send_available(void);

/** @brief esp_vhci_host_send_packet
 * host send packet to controller
 * @param data the packet point
 *,@param len the packet length
 */
void esp_vhci_host_send_packet(uint8_t *data, uint16_t len);

/** @brief esp_vhci_host_register_callback
 * register the vhci referece callback, the call back
 * struct defined by vhci_host_callback structure.
 * @param callback esp_vhci_host_callback type variable
 */
void esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback);

#ifdef __cplusplus
}
#endif

#endif /* __BT_H__ */
