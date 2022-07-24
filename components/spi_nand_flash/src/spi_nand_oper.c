// Copyright 2015-2022 Espressif Systems (Shanghai) PTE LTD
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

#include "spi_nand_oper.h"

#include "driver/spi_master.h"

esp_err_t spi_nand_execute_transaction(spi_device_handle_t device, spi_nand_transaction_t *transaction) {
  spi_transaction_ext_t e = {
      .base = {
          .flags = SPI_TRANS_VARIABLE_ADDR |  SPI_TRANS_VARIABLE_CMD |  SPI_TRANS_VARIABLE_DUMMY,
          .rxlength = transaction->miso_len * 8,
          .rx_buffer = transaction->miso_data,
          .length = transaction->mosi_len * 8,
          .tx_buffer = transaction->mosi_data,
          .addr = transaction->address,
          .cmd = transaction->command
      },
      .address_bits = transaction->address_bytes * 8,
      .command_bits = 8,
      .dummy_bits = transaction->dummy_bits
  };

  return spi_device_transmit(device, (spi_transaction_t *) &e);
}

esp_err_t spi_nand_read_register(spi_device_handle_t device, uint8_t reg, uint8_t *val) {
  spi_nand_transaction_t t = {
      .command = CMD_READ_REGISTER,
      .address_bytes = 1,
      .address = reg,
      .miso_len = 1,
      .miso_data = val
  };

  return spi_nand_execute_transaction(device, &t);
}

esp_err_t spi_nand_write_register(spi_device_handle_t device, uint8_t reg, uint8_t val) {
  spi_nand_transaction_t  t = {
      .command = CMD_SET_REGISTER,
      .address_bytes = 1,
      .address = reg,
      .mosi_len = 1,
      .mosi_data = &val
  };

  return spi_nand_execute_transaction(device, &t);
}

esp_err_t spi_nand_write_enable(spi_device_handle_t device) {
  spi_nand_transaction_t  t = {
      .command = CMD_WRITE_ENABLE
  };

  return spi_nand_execute_transaction(device, &t);
}

esp_err_t spi_nand_read_page(spi_device_handle_t device, uint32_t page) {
  spi_nand_transaction_t  t = {
      .command = CMD_PAGE_READ,
      .address_bytes = 3,
      .address = page
  };

  return spi_nand_execute_transaction(device, &t);
}

esp_err_t spi_nand_read(spi_device_handle_t device, uint8_t *data, uint16_t column, uint16_t length) {
  spi_nand_transaction_t  t = {
      .command = CMD_READ_FAST,
      .address_bytes = 2,
      .address = column,
      .miso_len = length,
      .miso_data = data,
      .dummy_bits = 8
  };

  return spi_nand_execute_transaction(device, &t);
}

esp_err_t spi_nand_program_execute(spi_device_handle_t device, uint32_t page) {
  spi_nand_transaction_t  t = {
      .command = CMD_PROGRAM_EXECUTE,
      .address_bytes = 3,
      .address = page
  };

  return spi_nand_execute_transaction(device, &t);
}

esp_err_t spi_nand_program_load(spi_device_handle_t device, const uint8_t *data, uint16_t column, uint16_t length) {
  spi_nand_transaction_t  t = {
      .command = CMD_PROGRAM_LOAD,
      .address_bytes = 2,
      .address = column,
      .mosi_len = length,
      .mosi_data = data
  };

  return spi_nand_execute_transaction(device, &t);
}

esp_err_t spi_nand_erase_block(spi_device_handle_t device, uint16_t page) {
  spi_nand_transaction_t  t = {
      .command = CMD_ERASE_BLOCK,
      .address_bytes = 3,
      .address = page
  };

  return spi_nand_execute_transaction(device, &t);
}


