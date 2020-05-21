// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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


#include <string.h>

#include "esp_log.h"
#include "driver/uart.h"

#include "lwip/opt.h"
#include "lwip/sio.h"

static const char *TAG = "esp-netif_lwip-slip_sio";

/***
 * @brief slip IO (SIO) uart driver
 */
typedef struct lwip_slip_sio {
    uart_port_t uart_dev;
    bool blocking;
} lwip_slip_sio_t;

/***
 * @brief Open a serial device for communication
 */
sio_fd_t sio_open(uint8_t devnum)
{
    ESP_LOGD(TAG, "Opening device: %d\r\n", devnum);

    // Create SIO object
    lwip_slip_sio_t *slip_sio = malloc(sizeof(lwip_slip_sio_t));

    // Store device num etc.
    slip_sio->uart_dev = devnum;
    slip_sio->blocking = false;

    // Return SIO handle
    return slip_sio;
}

/***
 * @brief Send a single character to the serial device (blocking)
 */
void sio_send(uint8_t c, sio_fd_t fd)
{
    lwip_slip_sio_t *slip_sio = ( lwip_slip_sio_t *) fd;

    ESP_LOGD(TAG, "%s", __func__);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, &c, 1, ESP_LOG_DEBUG);

    int res = uart_write_bytes(slip_sio->uart_dev, (const char *)&c, 1);
    if (res < 0) {
        // Handle errors
        ESP_LOGD(TAG, "%s: uart_write_bytes error %i", __func__, res);
    }
}

/***
 * @brief Write to the serial port (blocking)
 */
uint32_t sio_write(sio_fd_t fd, uint8_t *data, uint32_t len)
{
    lwip_slip_sio_t *slip_sio = ( lwip_slip_sio_t *) fd;

    ESP_LOGD(TAG, "%s", __func__);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, data, len, ESP_LOG_DEBUG);

    int32_t res = uart_write_bytes(slip_sio->uart_dev, (char *)data, len);
    if (res < 0) {
        // Handle errors
        ESP_LOGD(TAG, "%s: uart_write_bytes error %i", __func__, res);
        return 0;
    }

    return (uint32_t) res;
}

/***
 * @brief Receive a single character from the serial device (blocking)
 */
uint8_t sio_recv(sio_fd_t fd)
{
    lwip_slip_sio_t *slip_sio = ( lwip_slip_sio_t *) fd;
    uint8_t b;

    slip_sio->blocking = true;

    while (slip_sio->blocking == true) {
        int res = uart_read_bytes(slip_sio->uart_dev, &b, 1, portTICK_RATE_MS * 1);
        if (res < 0) {
            // Handle errors
            ESP_LOGD(TAG, "%s: uart_read_bytes error %i", __func__, res);
            return 0;
        } else if (res == 1) {
            break;
        }
    }

    return b;
}

/***
 * @brief Read from the serial port (blocking, abort with `sio_read_abort`)
 */
uint32_t sio_read(sio_fd_t fd, uint8_t *data, uint32_t len)
{
    lwip_slip_sio_t *slip_sio = ( lwip_slip_sio_t *) fd;
    int res = 0;

    slip_sio->blocking = true;

    while (slip_sio->blocking == true) {
        res = uart_read_bytes(slip_sio->uart_dev, data, len, portTICK_RATE_MS * 1);
        if (res < 0) {
            // Handle errors
            ESP_LOGD(TAG, "%s: uart_read_bytes error %i", __func__, res);
            return 0;
        } else if (res > 0) {
            break;
        }
    }

    return (uint32_t) res;
}

/***
 * @brief Read from the serial port (non-blocking)
 */
uint32_t sio_tryread(sio_fd_t fd, uint8_t *data, uint32_t len)
{
    lwip_slip_sio_t *slip_sio = ( lwip_slip_sio_t *) fd;

    int res = uart_read_bytes(slip_sio->uart_dev, data, len, portTICK_RATE_MS * 1);
    if (res < 0) {
        ESP_LOGD(TAG, "%s: uart_read_bytes error %i", __func__, res);
        return 0;
    }

    return (uint32_t)res;
}


/***
 * @brief Abort a pending sio_read call
 */
void sio_read_abort(sio_fd_t fd)
{
    lwip_slip_sio_t *slip_sio = ( lwip_slip_sio_t *) fd;

    slip_sio->blocking = false;
}

