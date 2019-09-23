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

#include "nvs_encr.hpp"
#include "nvs_types.hpp"
#include <string.h>

namespace nvs
{

    bool EncrMgr::isActive = false;
    EncrMgr* EncrMgr::instance = nullptr;


    EncrMgr* EncrMgr::getInstance()
    {
        if(!isActive)
        {
            instance = new EncrMgr();
            isActive = true;
        }
        return instance;
    }

    void EncrMgr::resetInstance()
    {
        if(isActive) {
            delete instance;
            instance = nullptr;
            isActive = false;
        }
    }

    bool EncrMgr::isEncrActive() {
        return isActive;
    }

    XtsCtxt* EncrMgr::findXtsCtxtFromAddr(uint32_t addr) {

        auto it = find_if(std::begin(xtsCtxtList), std::end(xtsCtxtList), [=](XtsCtxt& ctx) -> bool
                { return (ctx.baseSector * SPI_FLASH_SEC_SIZE  <= addr)
                && (addr < (ctx.baseSector + ctx.sectorCount) * SPI_FLASH_SEC_SIZE); });

        if (it == std::end(xtsCtxtList)) {
            return nullptr;
        }
        return it;
    }

    esp_err_t EncrMgr::setSecurityContext(uint32_t baseSector, uint32_t sectorCount, nvs_sec_cfg_t* cfg) {

        uint8_t* eky = reinterpret_cast<uint8_t*>(cfg);

        auto ctxt = new XtsCtxt();

        ctxt->baseSector = baseSector;
        ctxt->sectorCount = sectorCount;

        mbedtls_aes_xts_init(ctxt->ectxt);
        mbedtls_aes_xts_init(ctxt->dctxt);

        if(mbedtls_aes_xts_setkey_enc(ctxt->ectxt, eky, 2 * NVS_KEY_SIZE * 8)) {
            return ESP_ERR_NVS_XTS_CFG_FAILED;
        }

        if(mbedtls_aes_xts_setkey_dec(ctxt->dctxt, eky, 2 * NVS_KEY_SIZE * 8)) {
            return ESP_ERR_NVS_XTS_CFG_FAILED;
        }

        xtsCtxtList.push_back(ctxt);

        return ESP_OK;
    }

    esp_err_t EncrMgr::removeSecurityContext(uint32_t baseSector) {
        auto xtsCtxt = findXtsCtxtFromAddr(baseSector * SPI_FLASH_SEC_SIZE);
        if(!xtsCtxt) {
            return ESP_ERR_NVS_XTS_CFG_NOT_FOUND;
        }
        xtsCtxtList.erase(xtsCtxt);
        delete xtsCtxt;

        if(!xtsCtxtList.size()) {
            resetInstance();
        }
        return ESP_OK;
    }


    esp_err_t EncrMgr::encryptNvsData(uint8_t* ptxt, uint32_t addr, uint32_t ptxtLen, XtsCtxt* xtsCtxt) {

        uint8_t entrySize = sizeof(Item);

        //sector num required as an arr by mbedtls. Should have been just uint64/32.
        uint8_t data_unit[16];

        assert(ptxtLen % entrySize == 0);

        /* Use relative address instead of absolute address (relocatable), so that host-generated
         * encrypted nvs images can be used*/
        uint32_t relAddr = addr - (xtsCtxt->baseSector * SPI_FLASH_SEC_SIZE);

        memset(data_unit, 0, sizeof(data_unit));

        for(uint8_t entry = 0; entry < (ptxtLen/entrySize); entry++)
        {
            uint32_t offset = entry * entrySize;
            uint32_t *addr_loc = (uint32_t*) &data_unit[0];

            *addr_loc = relAddr + offset;
            if(mbedtls_aes_crypt_xts(xtsCtxt->ectxt, MBEDTLS_AES_ENCRYPT, entrySize, data_unit, ptxt + offset, ptxt + offset))  {
                return ESP_ERR_NVS_XTS_ENCR_FAILED;
            }
        }
        return ESP_OK;
    }

    esp_err_t EncrMgr::decryptNvsData(uint8_t* ctxt, uint32_t addr, uint32_t ctxtLen, XtsCtxt* xtsCtxt) {

        //sector num required as an arr by mbedtls. Should have been just uint64/32.
        uint8_t data_unit[16];


        /** Currently upper layer of NVS reads entries one by one even for variable size
        * multi-entry data types. So length should always be equal to size of an entry.*/
        assert(ctxtLen == sizeof(Item));

        uint32_t relAddr = addr - (xtsCtxt->baseSector * SPI_FLASH_SEC_SIZE);

        memset(data_unit, 0, sizeof(data_unit));

        memcpy(data_unit, &relAddr, sizeof(relAddr));

        if(mbedtls_aes_crypt_xts(xtsCtxt->dctxt, MBEDTLS_AES_DECRYPT, ctxtLen, data_unit, ctxt, ctxt))  {
            return ESP_ERR_NVS_XTS_DECR_FAILED;
        }
        return ESP_OK;
    }

} // namespace nvs
