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

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/sio.h"

#include "driver/uart.h"

#ifndef SIO_DEBUG
#define SIO_DEBUG LWIP_DBG_OFF
#endif

sio_fd_t sio_open(u8_t devnum)
{
    uart_port_t uart_num = devnum;

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };

    if (uart_param_config(uart_num, &uart_config) != ESP_OK) {
        LWIP_DEBUGF(SIO_DEBUG, ("sio_open[%d]: uart_param_config failed\n",
            uart_num));
        return NULL;
    }

    if (uart_driver_install(uart_num, 1500, 1500, 10, NULL, 0) != ESP_OK) {
        LWIP_DEBUGF(SIO_DEBUG, ("sio_open[%d]: uart_driver_install failed\n",
            uart_num));
        return NULL;
    }

    return (sio_fd_t)uart_num;
}

void sio_send(u8_t c, sio_fd_t fd)
{
    uart_write_bytes((uart_port_t)fd, (const char *)&c, sizeof(u8_t));
}

u8_t sio_recv(sio_fd_t fd)
{
    u8_t c = 0;
    uart_read_bytes((uart_port_t)fd, (uint8_t *)&c, sizeof(u8_t), portMAX_DELAY);
    return c;
}

u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
    int ret = uart_read_bytes((uart_port_t)fd, data, len, portMAX_DELAY);
    if (ret < 0) {
        LWIP_DEBUGF(SIO_DEBUG, ("sio_read[%d]: uart_read_bytes failed\n",
            (uart_port_t)fd));
        return 0;
    }

    return ret;
}

u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len)
{
    // Wait a maximum of 0.25 ms, if no data available, return.
    int ret = uart_read_bytes((uart_port_t)fd, data, len, 0);
    if (ret < 0) {
        LWIP_DEBUGF(SIO_DEBUG, ("sio_tryread[%d]: uart_read_bytes failed\n",
            (uart_port_t)fd));
        return 0;
    }

    return ret;
}

u32_t sio_write(sio_fd_t fd, u8_t *data, u32_t len)
{
    int ret = uart_write_bytes((uart_port_t)fd, (const char *)data, len);
    if (ret < 0) {
        LWIP_DEBUGF(SIO_DEBUG, ("sio_write[%d]: uart_write_bytes failed\n",
            (uart_port_t)fd));
        return 0;
    }

    return ret;
}

void sio_read_abort(sio_fd_t fd)
{
    LWIP_DEBUGF(SIO_DEBUG, ("sio_read_abort[%d]: not yet implemented for esp32\n",
        (uart_port_t)fd));
}

// esp32 specific sio functions

void sio_set_baudrate(sio_fd_t fd, uint32_t baudrate)
{
    uart_set_baudrate((uart_port_t)fd, baudrate);
}

void sio_set_pin(sio_fd_t fd, int tx, int rx, int rts, int cts)
{
    uart_set_pin((uart_port_t)fd, tx, rx, rts, cts);
}
