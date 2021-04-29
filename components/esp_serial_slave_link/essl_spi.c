// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include <sys/param.h>
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/periph_ctrl.h"
#include "essl_spi/esp32s2_defs.h"
#include "essl_spi.h"


static uint16_t get_hd_command(uint16_t cmd_i, uint32_t flags)
{
    //have no prefixes
    if (cmd_i == CMD_HD_EN_QPI_REG) return cmd_i;
    //doesn't support 4-line commands
    if(flags & SPI_TRANS_MODE_QIO && flags & SPI_TRANS_MODE_DIOQIO_ADDR &&
       (cmd_i == CMD_HD_WR_END_REG || cmd_i == CMD_HD_INT0_REG ||
       cmd_i == CMD_HD_INT1_REG || cmd_i == CMD_HD_INT2_REG)) {
           //the transaction will be sent in corresponding 1/2/4 bit mode, without address and data.
           //the CMD will have no 0xA- prefix
           return cmd_i;
    }

    if (flags & SPI_TRANS_MODE_DIO) {
        if (flags & SPI_TRANS_MODE_DIOQIO_ADDR) {
            return cmd_i | CMD_HD_DIO_MODE;
        } else {
            return cmd_i | CMD_HD_DOUT_MODE;
        }
    } else if (flags & SPI_TRANS_MODE_QIO) {
        if (flags & SPI_TRANS_MODE_DIOQIO_ADDR) {
            return cmd_i | CMD_HD_QIO_MODE;
        } else {
            return cmd_i | CMD_HD_QOUT_MODE;
        }
    }
    return cmd_i | CMD_HD_ONEBIT_MODE;
}

static int get_hd_dummy_bits(uint32_t flags)
{
    //dummy is always 4 cycles when dual or quad mode is enabled. Otherwise 8 cycles in normal mode.
    if (flags & (SPI_TRANS_MODE_DIO | SPI_TRANS_MODE_QIO)) {
        return 4;
    } else {
        return 8;
    }
}

esp_err_t essl_spi_rdbuf(spi_device_handle_t spi, uint8_t *out_data, int addr, int len, uint32_t flags)
{
    spi_transaction_ext_t t = {
        .base = {
            .cmd = get_hd_command(CMD_HD_RDBUF_REG, flags),
            .addr = addr % 72,
            .rxlength = len * 8,
            .rx_buffer = out_data,
            .flags = flags | SPI_TRANS_VARIABLE_DUMMY,
        },
        .dummy_bits = get_hd_dummy_bits(flags),
    };

    return spi_device_transmit(spi, (spi_transaction_t*)&t);
}

esp_err_t essl_spi_rdbuf_polling(spi_device_handle_t spi, uint8_t *out_data, int addr, int len, uint32_t flags)
{
    spi_transaction_ext_t t = {
        .base = {
            .cmd = get_hd_command(CMD_HD_RDBUF_REG, flags),
            .addr = addr % 72,
            .rxlength = len * 8,
            .rx_buffer = out_data,
            .flags = flags | SPI_TRANS_VARIABLE_DUMMY,
        },
        .dummy_bits = get_hd_dummy_bits(flags),
    };

    return spi_device_polling_transmit(spi, (spi_transaction_t*)&t);
}

esp_err_t essl_spi_wrbuf(spi_device_handle_t spi, const uint8_t *data, int addr, int len, uint32_t flags)
{
    spi_transaction_ext_t t = {
        .base = {
            .cmd = get_hd_command(CMD_HD_WRBUF_REG, flags),
            .addr = addr % 72,
            .length = len * 8,
            .tx_buffer = data,
            .flags = flags | SPI_TRANS_VARIABLE_DUMMY,
        },
        .dummy_bits = get_hd_dummy_bits(flags),
    };
    return spi_device_transmit(spi, (spi_transaction_t*)&t);
}

esp_err_t essl_spi_wrbuf_polling(spi_device_handle_t spi, const uint8_t *data, int addr, int len, uint32_t flags)
{
    spi_transaction_ext_t t = {
        .base = {
            .cmd = get_hd_command(CMD_HD_WRBUF_REG, flags),
            .addr = addr % 72,
            .length = len * 8,
            .tx_buffer = data,
            .flags = flags | SPI_TRANS_VARIABLE_DUMMY,
        },
        .dummy_bits = get_hd_dummy_bits(flags),
    };
    return spi_device_polling_transmit(spi, (spi_transaction_t*)&t);
}

esp_err_t essl_spi_rddma_seg(spi_device_handle_t spi, uint8_t *out_data, int seg_len, uint32_t flags)
{
    spi_transaction_ext_t t = {
        .base = {
            .cmd = get_hd_command(CMD_HD_RDDMA_REG, flags),
            .rxlength = seg_len * 8,
            .rx_buffer = out_data,
            .flags = flags | SPI_TRANS_VARIABLE_DUMMY,
        },
        .dummy_bits = get_hd_dummy_bits(flags),
    };
    return spi_device_transmit(spi, (spi_transaction_t*)&t);
}

esp_err_t essl_spi_rddma_done(spi_device_handle_t spi, uint32_t flags)
{
    spi_transaction_t end_t = {
        .cmd = get_hd_command(CMD_HD_INT0_REG, flags),
        .flags = flags,
    };
    return spi_device_transmit(spi, &end_t);
}

esp_err_t essl_spi_rddma(spi_device_handle_t spi, uint8_t *out_data, int len, int seg_len, uint32_t flags)
{
    if (!esp_ptr_dma_capable(out_data) || ((intptr_t)out_data % 4) != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    seg_len = (seg_len > 0)? seg_len : len;

    uint8_t* read_ptr = out_data;
    esp_err_t err = ESP_OK;
    while (len > 0) {
        int send_len = MIN(seg_len, len);

        err = essl_spi_rddma_seg(spi, read_ptr, send_len, flags);
        if (err != ESP_OK) return err;

        len -= send_len;
        read_ptr += send_len;
    }
    return essl_spi_rddma_done(spi, flags);
}

esp_err_t essl_spi_wrdma_seg(spi_device_handle_t spi, const uint8_t *data, int seg_len, uint32_t flags)
{
    spi_transaction_ext_t t = {
        .base = {
            .cmd = get_hd_command(CMD_HD_WRDMA_REG, flags),
            .length = seg_len * 8,
            .tx_buffer = data,
            .flags = flags | SPI_TRANS_VARIABLE_DUMMY,
        },
        .dummy_bits = get_hd_dummy_bits(flags),
    };
    return spi_device_transmit(spi, (spi_transaction_t*)&t);
}

esp_err_t essl_spi_wrdma_done(spi_device_handle_t spi, uint32_t flags)
{
    spi_transaction_t end_t = {
        .cmd = get_hd_command(CMD_HD_WR_END_REG, flags),
        .flags = flags,
    };
    return spi_device_transmit(spi, &end_t);
}

esp_err_t essl_spi_wrdma(spi_device_handle_t spi, const uint8_t *data, int len, int seg_len, uint32_t flags)
{
    if (!esp_ptr_dma_capable(data)) {
        return ESP_ERR_INVALID_ARG;
    }
    seg_len = (seg_len > 0)? seg_len : len;

    while (len > 0) {
        int send_len = MIN(seg_len, len);

        esp_err_t ret = essl_spi_wrdma_seg(spi, data, send_len, flags);
        if (ret != ESP_OK) return ret;

        len -= send_len;
        data += send_len;
    }

    return essl_spi_wrdma_done(spi, flags);
}

esp_err_t essl_spi_int(spi_device_handle_t spi, int int_n, uint32_t flags)
{
    spi_transaction_t end_t = {
        .cmd = get_hd_command(CMD_HD_INT0_REG + int_n, flags),
        .flags = flags,
    };
    return spi_device_transmit(spi, &end_t);
}
