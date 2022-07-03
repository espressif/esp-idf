/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#pragma once

enum {
    // FatFS only allows to format disks with number of blocks greater than 128
    DISK_BLOCK_NUM  = 128 + 1,
    DISK_BLOCK_SIZE = 512
};

#define README_CONTENTS \
"This is tinyusb's MassStorage Class demo.\r\n\r\n\
If you find any bugs or get any questions, feel free to file an\r\n\
issue at github.com/hathach/tinyusb"

void device_app(void);
