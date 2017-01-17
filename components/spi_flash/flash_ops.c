// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/param.h>  // For MIN/MAX(a, b)

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <rom/spi_flash.h>
#include <rom/cache.h>
#include <soc/soc.h>
#include <soc/dport_reg.h>
#include "sdkconfig.h"
#include "esp_ipc.h"
#include "esp_attr.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "cache_utils.h"

/* bytes erased by SPIEraseBlock() ROM function */
#define BLOCK_ERASE_SIZE 65536

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
static const char* TAG = "spi_flash";
static spi_flash_counters_t s_flash_stats;

#define COUNTER_START()     uint32_t ts_begin = xthal_get_ccount()
#define COUNTER_STOP(counter)  \
    do{ \
        s_flash_stats.counter.count++; \
        s_flash_stats.counter.time += (xthal_get_ccount() - ts_begin) / (XT_CLOCK_FREQ / 1000000); \
    } while(0)

#define COUNTER_ADD_BYTES(counter, size) \
    do { \
        s_flash_stats.counter.bytes += size; \
    } while (0)

#else
#define COUNTER_START()
#define COUNTER_STOP(counter)
#define COUNTER_ADD_BYTES(counter, size)

#endif //CONFIG_SPI_FLASH_ENABLE_COUNTERS

static esp_err_t spi_flash_translate_rc(SpiFlashOpResult rc);

const DRAM_ATTR spi_flash_guard_funcs_t g_flash_guard_default_ops = {
        .start  = spi_flash_disable_interrupts_caches_and_other_cpu,
        .end    = spi_flash_enable_interrupts_caches_and_other_cpu
};

const DRAM_ATTR spi_flash_guard_funcs_t g_flash_guard_no_os_ops = {
        .start  = spi_flash_disable_interrupts_caches_and_other_cpu_no_os,
        .end    = spi_flash_enable_interrupts_caches_no_os
};

static const spi_flash_guard_funcs_t *s_flash_guard_ops;

void spi_flash_init()
{
    spi_flash_init_lock();
#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
    spi_flash_reset_counters();
#endif
}

void spi_flash_guard_set(const spi_flash_guard_funcs_t* funcs)
{
    s_flash_guard_ops = funcs;
}

size_t spi_flash_get_chip_size()
{
    return g_rom_flashchip.chip_size;
}

SpiFlashOpResult IRAM_ATTR spi_flash_unlock()
{
    static bool unlocked = false;
    if (!unlocked) {
        SpiFlashOpResult rc = SPIUnlock();
        if (rc != SPI_FLASH_RESULT_OK) {
            return rc;
        }
        unlocked = true;
    }
    return SPI_FLASH_RESULT_OK;
}

static inline void IRAM_ATTR spi_flash_guard_start()
{
    if (s_flash_guard_ops) {
        s_flash_guard_ops->start();
    }
}

static inline void IRAM_ATTR spi_flash_guard_end()
{
    if (s_flash_guard_ops) {
        s_flash_guard_ops->end();
    }
}

esp_err_t IRAM_ATTR spi_flash_erase_sector(size_t sec)
{
    return spi_flash_erase_range(sec * SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE);
}

esp_err_t IRAM_ATTR spi_flash_erase_range(uint32_t start_addr, uint32_t size)
{
    if (start_addr % SPI_FLASH_SEC_SIZE != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (size % SPI_FLASH_SEC_SIZE != 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (size + start_addr > spi_flash_get_chip_size()) {
        return ESP_ERR_INVALID_SIZE;
    }
    size_t start = start_addr / SPI_FLASH_SEC_SIZE;
    size_t end = start + size / SPI_FLASH_SEC_SIZE;
    const size_t sectors_per_block = BLOCK_ERASE_SIZE / SPI_FLASH_SEC_SIZE;
    COUNTER_START();
    spi_flash_guard_start();
    SpiFlashOpResult rc;
    rc = spi_flash_unlock();
    if (rc == SPI_FLASH_RESULT_OK) {
        for (size_t sector = start; sector != end && rc == SPI_FLASH_RESULT_OK; ) {
            if (sector % sectors_per_block == 0 && end - sector > sectors_per_block) {
                rc = SPIEraseBlock(sector / sectors_per_block);
                sector += sectors_per_block;
                COUNTER_ADD_BYTES(erase, sectors_per_block * SPI_FLASH_SEC_SIZE);
            } else {
                rc = SPIEraseSector(sector);
                ++sector;
                COUNTER_ADD_BYTES(erase, SPI_FLASH_SEC_SIZE);
            }
        }
    }
    spi_flash_guard_end();
    COUNTER_STOP(erase);
    return spi_flash_translate_rc(rc);
}

esp_err_t IRAM_ATTR spi_flash_write(size_t dst, const void *srcv, size_t size)
{
    // Out of bound writes are checked in ROM code, but we can give better
    // error code here
    if (dst + size > g_rom_flashchip.chip_size) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (size == 0) {
        return ESP_OK;
    }

    SpiFlashOpResult rc = SPI_FLASH_RESULT_OK;
    COUNTER_START();
    const char *srcc = (const char *) srcv;
    /*
     * Large operations are split into (up to) 3 parts:
     * - Left padding: 4 bytes up to the first 4-byte aligned destination offset.
     * - Middle part
     * - Right padding: 4 bytes from the last 4-byte aligned offset covered.
     */
    size_t left_off = dst & ~3U;
    size_t left_size = MIN(((dst + 3) & ~3U) - dst, size);
    size_t mid_off = left_size;
    size_t mid_size = (size - left_size) & ~3U;
    size_t right_off = left_size + mid_size;
    size_t right_size = size - mid_size - left_size;
    rc = spi_flash_unlock();
    if (rc != SPI_FLASH_RESULT_OK) {
        goto out;
    }
    if (left_size > 0) {
        uint32_t t = 0xffffffff;
        memcpy(((uint8_t *) &t) + (dst - left_off), srcc, left_size);
        spi_flash_guard_start();
        rc = SPIWrite(left_off, &t, 4);
        spi_flash_guard_end();
        if (rc != SPI_FLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(write, 4);
    }
    if (mid_size > 0) {
        /* If src buffer is 4-byte aligned as well and is not in a region that
         * requires cache access to be enabled, we can write it all at once. */
#ifdef ESP_PLATFORM
        bool in_dram = ((uintptr_t) srcc >= 0x3FFAE000 &&
                        (uintptr_t) srcc < 0x40000000);
#else
        bool in_dram = true;
#endif
        if (in_dram && (((uintptr_t) srcc) + mid_off) % 4 == 0) {
            spi_flash_guard_start();
            rc = SPIWrite(dst + mid_off, (const uint32_t *) (srcc + mid_off), mid_size);
            spi_flash_guard_end();
            if (rc != SPI_FLASH_RESULT_OK) {
                goto out;
            }
            COUNTER_ADD_BYTES(write, mid_size);
        } else {
            /*
             * Otherwise, unlike for read, we cannot manipulate data in the
             * user-provided buffer, so we write in 32 byte blocks.
             */
            while (mid_size > 0) {
                uint32_t t[8];
                uint32_t write_size = MIN(mid_size, sizeof(t));
                memcpy(t, srcc + mid_off, write_size);
                spi_flash_guard_start();
                rc = SPIWrite(dst + mid_off, t, write_size);
                spi_flash_guard_end();
                if (rc != SPI_FLASH_RESULT_OK) {
                    goto out;
                }
                COUNTER_ADD_BYTES(write, write_size);
                mid_size -= write_size;
                mid_off += write_size;
            }
        }
    }
    if (right_size > 0) {
        uint32_t t = 0xffffffff;
        memcpy(&t, srcc + right_off, right_size);
        spi_flash_guard_start();
        rc = SPIWrite(dst + right_off, &t, 4);
        spi_flash_guard_end();
        if (rc != SPI_FLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(write, 4);
    }
out:
    COUNTER_STOP(write);
    return spi_flash_translate_rc(rc);
}

esp_err_t IRAM_ATTR spi_flash_write_encrypted(size_t dest_addr, const void *src, size_t size)
{
    if ((dest_addr % 32) != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((size % 32) != 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    if ((uint32_t) src < 0x3ff00000) {
        // if source address is in DROM, we won't be able to read it
        // from within SPIWrite
        // TODO: consider buffering source data using heap and writing it anyway?
        return ESP_ERR_INVALID_ARG;
    }
    COUNTER_START();
    spi_flash_disable_interrupts_caches_and_other_cpu();
    SpiFlashOpResult rc;
    rc = spi_flash_unlock();
    if (rc == SPI_FLASH_RESULT_OK) {
        /* SPI_Encrypt_Write encrypts data in RAM as it writes,
           so copy to a temporary buffer - 32 bytes at a time.
        */
        uint32_t encrypt_buf[32/sizeof(uint32_t)];
        for (size_t i = 0; i < size; i += 32) {
            memcpy(encrypt_buf, ((const uint8_t *)src) + i, 32);
            rc = SPI_Encrypt_Write((uint32_t) dest_addr + i, encrypt_buf, 32);
            if (rc != SPI_FLASH_RESULT_OK) {
                break;
            }
        }
        bzero(encrypt_buf, sizeof(encrypt_buf));
    }
    COUNTER_ADD_BYTES(write, size);
    return spi_flash_translate_rc(rc);
}

esp_err_t IRAM_ATTR spi_flash_read(size_t src, void *dstv, size_t size)
{
    // Out of bound reads are checked in ROM code, but we can give better
    // error code here
    if (src + size > g_rom_flashchip.chip_size) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (size == 0) {
        return ESP_OK;
    }

    SpiFlashOpResult rc = SPI_FLASH_RESULT_OK;
    COUNTER_START();
    spi_flash_guard_start();
    /* To simplify boundary checks below, we handle small reads separately. */
    if (size < 16) {
        uint32_t t[6]; /* Enough for 16 bytes + 4 on either side for padding. */
        uint32_t read_src = src & ~3U;
        uint32_t left_off = src & 3U;
        uint32_t read_size = (left_off + size + 3) & ~3U;
        rc = SPIRead(read_src, t, read_size);
        if (rc != SPI_FLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(read, read_size);
        memcpy(dstv, ((char *) t) + left_off, size);
        goto out;
    }
    char *dstc = (char *) dstv;
    intptr_t dsti = (intptr_t) dstc;
    /*
     * Large operations are split into (up to) 3 parts:
     * - The middle part: from the first 4-aligned position in src to the first
     *   4-aligned position in dst.
     */
    size_t src_mid_off = (src % 4 == 0 ? 0 : 4 - (src % 4));
    size_t dst_mid_off = (dsti % 4 == 0 ? 0 : 4 - (dsti % 4));
    size_t mid_size = (size - MAX(src_mid_off, dst_mid_off)) & ~3U;
    /*
     * - Once the middle part is in place, src_mid_off bytes from the preceding
     *   4-aligned source location are added on the left.
     */
    size_t pad_left_src = src & ~3U;
    size_t pad_left_size = src_mid_off;
    /*
     * - Finally, the right part is added: from the end of the middle part to
     *   the end. Depending on the alignment of source and destination, this may
     *   be a 4 or 8 byte read from pad_right_src.
     */
    size_t pad_right_src = (src + pad_left_size + mid_size) & ~3U;
    size_t pad_right_off = (pad_right_src - src);
    size_t pad_right_size = (size - pad_right_off);
    if (mid_size > 0) {
        rc = SPIRead(src + src_mid_off, (uint32_t *) (dstc + dst_mid_off), mid_size);
        if (rc != SPI_FLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(read, mid_size);
        /*
         * If offsets in src and dst are different, perform an in-place shift
         * to put destination data into its final position.
         * Note that the shift can be left (src_mid_off < dst_mid_off) or right.
         */
        if (src_mid_off != dst_mid_off) {
            memmove(dstc + src_mid_off, dstc + dst_mid_off, mid_size);
        }
    }
    if (pad_left_size > 0) {
        uint32_t t;
        rc = SPIRead(pad_left_src, &t, 4);
        if (rc != SPI_FLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(read, 4);
        memcpy(dstc, ((uint8_t *) &t) + (4 - pad_left_size), pad_left_size);
    }
    if (pad_right_size > 0) {
        uint32_t t[2];
        int32_t read_size = (pad_right_size <= 4 ? 4 : 8);
        rc = SPIRead(pad_right_src, t, read_size);
        if (rc != SPI_FLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(read, read_size);
        memcpy(dstc + pad_right_off, t, pad_right_size);
    }
out:
    spi_flash_guard_end();
    COUNTER_STOP(read);
    return spi_flash_translate_rc(rc);
}

static esp_err_t spi_flash_translate_rc(SpiFlashOpResult rc)
{
    switch (rc) {
    case SPI_FLASH_RESULT_OK:
        return ESP_OK;
    case SPI_FLASH_RESULT_TIMEOUT:
        return ESP_ERR_FLASH_OP_TIMEOUT;
    case SPI_FLASH_RESULT_ERR:
    default:
        return ESP_ERR_FLASH_OP_FAIL;
    }
}

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS

static inline void dump_counter(spi_flash_counter_t* counter, const char* name)
{
    ESP_LOGI(TAG, "%s  count=%8d  time=%8dms  bytes=%8d\n", name,
             counter->count, counter->time, counter->bytes);
}

const spi_flash_counters_t* spi_flash_get_counters()
{
    return &s_flash_stats;
}

void spi_flash_reset_counters()
{
    memset(&s_flash_stats, 0, sizeof(s_flash_stats));
}

void spi_flash_dump_counters()
{
    dump_counter(&s_flash_stats.read,  "read ");
    dump_counter(&s_flash_stats.write, "write");
    dump_counter(&s_flash_stats.erase, "erase");
}

#endif //CONFIG_SPI_FLASH_ENABLE_COUNTERS
