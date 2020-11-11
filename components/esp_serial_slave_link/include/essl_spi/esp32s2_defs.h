// Copyright 2010-2020 Espressif Systems (Shanghai) PTE LTD
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

// NOTE: From the view of master
#define CMD_HD_WRBUF_REG    0x01
#define CMD_HD_RDBUF_REG    0x02
#define CMD_HD_WRDMA_REG    0x03
#define CMD_HD_RDDMA_REG    0x04

#define CMD_HD_ONEBIT_MODE  0x00
#define CMD_HD_DOUT_MODE    0x10
#define CMD_HD_QOUT_MODE    0x20
#define CMD_HD_DIO_MODE     0x50
#define CMD_HD_QIO_MODE     0xA0

#define CMD_HD_SEG_END_REG  0x05
#define CMD_HD_EN_QPI_REG   0x06
#define CMD_HD_WR_END_REG   0x07
#define CMD_HD_INT0_REG     0x08
#define CMD_HD_INT1_REG     0x09
#define CMD_HD_INT2_REG     0x0A
#define CMD_HD_EX_QPI_REG   0xDD

#define SPI_SLAVE_HD_BUFFER_SIZE 72
