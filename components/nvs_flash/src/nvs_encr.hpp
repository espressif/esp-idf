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
#ifndef nvs_encr_hpp
#define nvs_encr_hpp

#include "esp_err.h"
#include "mbedtls/aes.h"
#include "intrusive_list.h"
#include "nvs_flash.h"

namespace nvs
{

struct XtsCtxt : public intrusive_list_node<XtsCtxt> {
    public:
        mbedtls_aes_xts_context ectxt[1];
        mbedtls_aes_xts_context dctxt[1];
        uint32_t baseSector;
        uint32_t sectorCount;
};


/* A singleton class for managing nvs encryption*/
class EncrMgr
{
    public:
        static EncrMgr* getInstance();
        static void resetInstance();
        static bool isEncrActive();
        esp_err_t setSecurityContext(uint32_t baseSector, uint32_t sectorCount, nvs_sec_cfg_t* cfg);
        esp_err_t removeSecurityContext(uint32_t baseSector);
        esp_err_t encryptNvsData(uint8_t* ptxt, uint32_t addr, uint32_t ptxtLen, XtsCtxt* xtsCtxt);
        esp_err_t decryptNvsData(uint8_t* ctxt, uint32_t addr, uint32_t ctxtLen, XtsCtxt* xtsCtxt);
        XtsCtxt* findXtsCtxtFromAddr(uint32_t addr);
        ~EncrMgr() {}

    protected:
        static bool isActive;
        static EncrMgr* instance;
        intrusive_list<XtsCtxt> xtsCtxtList;
        EncrMgr() {}

}; // class EncrMgr

esp_err_t nvs_flash_write(size_t destAddr, const void *srcAddr, size_t size);
esp_err_t nvs_flash_read(size_t srcAddr, void *destAddr, size_t size);


} // namespace nvs


#endif /* nvs_encr_hpp */
