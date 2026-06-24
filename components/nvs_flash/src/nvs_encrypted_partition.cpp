/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cstring>
#include "nvs_encrypted_partition.hpp"
#include "nvs_types.hpp"
#include "nvs_constants.h"
#include "nvs_xts_aes.h"

namespace nvs {

#ifdef CONFIG_NVS_BDL_STACK
NVSEncryptedPartition::NVSEncryptedPartition(const char* label, const esp_blockdev_handle_t bdl, const bool managed_bdl)
    : NVSPartition(label, bdl, managed_bdl) { }
#else
NVSEncryptedPartition::NVSEncryptedPartition(const esp_partition_t *partition)
    : NVSPartition(partition) { }
#endif // CONFIG_NVS_BDL_STACK

esp_err_t NVSEncryptedPartition::init(nvs_sec_cfg_t* cfg)
{
    uint8_t* eky = reinterpret_cast<uint8_t*>(cfg);

    XTS_FUNC(xts_init)(&mEctxt);
    XTS_FUNC(xts_init)(&mDctxt);

    if (XTS_FUNC(xts_setkey_enc)(&mEctxt, eky, 2 * NVS_KEY_SIZE * 8) != 0) {
        return ESP_ERR_NVS_XTS_CFG_FAILED;
    }

    if (XTS_FUNC(xts_setkey_dec)(&mDctxt, eky, 2 * NVS_KEY_SIZE * 8) != 0) {
        return ESP_ERR_NVS_XTS_CFG_FAILED;
    }

    return ESP_OK;
}

esp_err_t NVSEncryptedPartition::read(size_t src_offset, void* dst, size_t size)
{
    // Currently upper layer of NVS reads entries one by one even for variable size
    // multi-entry data types. So length should always be equal to size of an entry.
    // here we make sure that the size is really compliant with the minimal encryption block size.
    if (size % NVS_ENCRYPT_BLOCK_SIZE != 0) return ESP_ERR_INVALID_SIZE;

    // read data
    esp_err_t read_result = NVSPartition::read(src_offset, dst, size);
    if (read_result != ESP_OK) {
        return read_result;
    }

    // decrypt data
    //sector num required as an arr by mbedtls. Should have been just uint64/32.
    uint8_t data_unit[NVS_ENCRYPT_BLOCK_SIZE];

    uint32_t relAddr = src_offset;

    memset(data_unit, 0, sizeof(data_unit));

    memcpy(data_unit, &relAddr, sizeof(relAddr));

    uint8_t *destination = reinterpret_cast<uint8_t*>(dst);

    if (XTS_FUNC(crypt_xts)(&mDctxt, XTS_MODE(DECRYPT), size, data_unit, destination, destination) != 0)  {
        return ESP_ERR_NVS_XTS_DECR_FAILED;
    }

    return ESP_OK;
}

esp_err_t NVSEncryptedPartition::write(size_t addr, const void* src, size_t size)
{
    if (size % NVS_ENCRYPT_BLOCK_SIZE != 0) return ESP_ERR_INVALID_SIZE;

    // copy data to buffer for encryption
    uint8_t* buf = new (std::nothrow) uint8_t [size];

    if (!buf) return ESP_ERR_NO_MEM;

    memcpy(buf, src, size);

    // encrypt data
    uint8_t entrySize = sizeof(Item);

    //sector num required as an arr by mbedtls. Should have been just uint64/32.
    uint8_t data_unit[NVS_ENCRYPT_BLOCK_SIZE];

    /* Use relative address instead of absolute address (relocatable), so that host-generated
     * encrypted nvs images can be used*/
    uint32_t relAddr = addr;

    memset(data_unit, 0, sizeof(data_unit));

    for(uint8_t entry = 0; entry < (size/entrySize); entry++)
    {
        uint32_t offset = entry * entrySize;
        uint32_t *addr_loc = (uint32_t*) &data_unit[0];

        *addr_loc = relAddr + offset;
        if (XTS_FUNC(crypt_xts)(&mEctxt,
                                  XTS_MODE(ENCRYPT),
                                  entrySize,
                                  data_unit,
                                  buf + offset,
                                  buf + offset) != 0)  {
            delete [] buf;
            return ESP_ERR_NVS_XTS_ENCR_FAILED;
        }
    }

    // write data
    esp_err_t result = NVSPartition::write(addr, buf, size);

    delete [] buf;

    return result;
}

} // nvs
