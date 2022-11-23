// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_spi_flash.h"
#include "nvs_ops.hpp"
#include <cstddef>
#ifdef CONFIG_NVS_ENCRYPTION
#include "nvs_encr.hpp"
#include <string.h>
#include <cstdlib>
#endif

namespace nvs
{
#ifdef CONFIG_NVS_ENCRYPTION
esp_err_t nvs_flash_write(size_t destAddr, const void *srcAddr, size_t size) {

    if(EncrMgr::isEncrActive()) {
        auto encrMgr = EncrMgr::getInstance();

        if (!encrMgr) return ESP_ERR_NO_MEM;

        auto xtsCtxt = encrMgr->findXtsCtxtFromAddr(destAddr);

        if(xtsCtxt) {
            uint8_t* buf = static_cast<uint8_t*>(malloc(size));
            memcpy(buf, srcAddr, size);
            auto err = encrMgr->encryptNvsData(buf, destAddr, size, xtsCtxt);
            if( err != ESP_OK) {
                return err;
            }
            err = spi_flash_write(destAddr, buf, size);
            free(buf);
            return err;
        }
    }
    return spi_flash_write(destAddr, srcAddr, size);
}

esp_err_t nvs_flash_read(size_t srcAddr, void *destAddr, size_t size) {

    auto err = spi_flash_read(srcAddr, destAddr, size);

    if(err != ESP_OK) {
        return err;
    }

    if(EncrMgr::isEncrActive()) {
        auto encrMgr = EncrMgr::getInstance();

        if (!encrMgr) return ESP_ERR_NO_MEM;

        auto xtsCtxt = encrMgr->findXtsCtxtFromAddr(srcAddr);
        if(xtsCtxt) {
            return encrMgr->decryptNvsData(static_cast<uint8_t*>(destAddr),
                    srcAddr, size, xtsCtxt);
        }
    }
    return ESP_OK;
}
#else
esp_err_t nvs_flash_write(size_t destAddr, const void *srcAddr, size_t size) {
    return spi_flash_write(destAddr, srcAddr, size);
}

esp_err_t nvs_flash_read(size_t srcAddr, void *destAddr, size_t size) {
    return spi_flash_read(srcAddr, destAddr, size);
}
#endif
}
