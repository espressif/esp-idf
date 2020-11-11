// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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
extern "C" {
#endif

#include <stdint.h>
typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef uint32_t DWORD;

#define FF_DRV_NOT_USED 0xFF

#include "diskio.h"
#include "esp_err.h"

/**
 * Structure of pointers to disk IO driver functions.
 *
 * See FatFs documentation for details about these functions
 */
typedef struct {
    DSTATUS (*init) (unsigned char pdrv);    /*!< disk initialization function */
    DSTATUS (*status) (unsigned char pdrv);  /*!< disk status check function */
    DRESULT (*read) (unsigned char pdrv, unsigned char* buff, uint32_t sector, unsigned count);  /*!< sector read function */
    DRESULT (*write) (unsigned char pdrv, const unsigned char* buff, uint32_t sector, unsigned count);   /*!< sector write function */
    DRESULT (*ioctl) (unsigned char pdrv, unsigned char cmd, void* buff); /*!< function to get info about disk and do some misc operations */
} ff_diskio_impl_t;

/**
 * Register or unregister diskio driver for given drive number.
 *
 * When FATFS library calls one of disk_xxx functions for driver number pdrv,
 * corresponding function in discio_impl for given pdrv will be called.
 *
 * @param pdrv drive number
 * @param discio_impl   pointer to ff_diskio_impl_t structure with diskio functions
 *                      or NULL to unregister and free previously registered drive
 */
void ff_diskio_register(BYTE pdrv, const ff_diskio_impl_t* discio_impl);

#define ff_diskio_unregister(pdrv_) ff_diskio_register(pdrv_, NULL)


/**
 * Get next available drive number
 *
 * @param   out_pdrv            pointer to the byte to set if successful
 *
 * @return  ESP_OK              on success
 *          ESP_ERR_NOT_FOUND   if all drives are attached
 */
esp_err_t ff_diskio_get_drive(BYTE* out_pdrv);


#ifdef __cplusplus
}
#endif
