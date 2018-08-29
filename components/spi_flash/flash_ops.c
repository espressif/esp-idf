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
#include "esp_clk.h"
#include "esp_flash_partitions.h"
#include "esp_ota_ops.h"
#include "cache_utils.h"

/* bytes erased by SPIEraseBlock() ROM function */
#define BLOCK_ERASE_SIZE 65536

/* Limit number of bytes written/read in a single SPI operation,
   as these operations disable all higher priority tasks from running.
*/
#define MAX_WRITE_CHUNK 8192
#define MAX_READ_CHUNK 16384

static const char *TAG __attribute__((unused)) = "spi_flash";

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
static spi_flash_counters_t s_flash_stats;

#define COUNTER_START()     uint32_t ts_begin = xthal_get_ccount()
#define COUNTER_STOP(counter)  \
    do{ \
        s_flash_stats.counter.count++; \
        s_flash_stats.counter.time += (xthal_get_ccount() - ts_begin) / (esp_clk_cpu_freq() / 1000000); \
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

static esp_err_t spi_flash_translate_rc(esp_rom_spiflash_result_t rc);

const DRAM_ATTR spi_flash_guard_funcs_t g_flash_guard_default_ops = {
    .start     = spi_flash_disable_interrupts_caches_and_other_cpu,
    .end       = spi_flash_enable_interrupts_caches_and_other_cpu,
    .op_lock   = spi_flash_op_lock,
    .op_unlock = spi_flash_op_unlock
};

const DRAM_ATTR spi_flash_guard_funcs_t g_flash_guard_no_os_ops = {
    .start      = spi_flash_disable_interrupts_caches_and_other_cpu_no_os,
    .end        = spi_flash_enable_interrupts_caches_no_os,
    .op_lock    = 0,
    .op_unlock  = 0
};

static const spi_flash_guard_funcs_t *s_flash_guard_ops;

#ifdef CONFIG_SPI_FLASH_WRITING_DANGEROUS_REGIONS_ABORTS
#define UNSAFE_WRITE_ADDRESS abort()
#else
#define UNSAFE_WRITE_ADDRESS return false
#endif


/* CHECK_WRITE_ADDRESS macro to fail writes which land in the
   bootloader, partition table, or running application region.
*/
#if CONFIG_SPI_FLASH_WRITING_DANGEROUS_REGIONS_ALLOWED
#define CHECK_WRITE_ADDRESS(ADDR, SIZE)
#else /* FAILS or ABORTS */
#define CHECK_WRITE_ADDRESS(ADDR, SIZE) do {                            \
        if (!is_safe_write_address(ADDR, SIZE)) {                       \
            return ESP_ERR_INVALID_ARG;                                 \
        }                                                               \
    } while(0)
#endif // CONFIG_SPI_FLASH_WRITING_DANGEROUS_REGIONS_ALLOWED

static __attribute__((unused)) bool is_safe_write_address(size_t addr, size_t size)
{
    bool result = true;
    if (addr <= ESP_PARTITION_TABLE_OFFSET + ESP_PARTITION_TABLE_MAX_LEN) {
        UNSAFE_WRITE_ADDRESS;
    }

    const esp_partition_t *p = esp_ota_get_running_partition();
    if (addr >= p->address && addr < p->address + p->size) {
        UNSAFE_WRITE_ADDRESS;
    }
    if (addr < p->address && addr + size > p->address) {
        UNSAFE_WRITE_ADDRESS;
    }

    return result;
}


void spi_flash_init()
{
    spi_flash_init_lock();
#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
    spi_flash_reset_counters();
#endif
}

void IRAM_ATTR spi_flash_guard_set(const spi_flash_guard_funcs_t *funcs)
{
    s_flash_guard_ops = funcs;
}

const spi_flash_guard_funcs_t *IRAM_ATTR spi_flash_guard_get()
{
    return s_flash_guard_ops;
}

size_t IRAM_ATTR spi_flash_get_chip_size()
{
    return g_rom_flashchip.chip_size;
}

static inline void IRAM_ATTR spi_flash_guard_start()
{
    if (s_flash_guard_ops && s_flash_guard_ops->start) {
        s_flash_guard_ops->start();
    }
}

static inline void IRAM_ATTR spi_flash_guard_end()
{
    if (s_flash_guard_ops && s_flash_guard_ops->end) {
        s_flash_guard_ops->end();
    }
}

static inline void IRAM_ATTR spi_flash_guard_op_lock()
{
    if (s_flash_guard_ops && s_flash_guard_ops->op_lock) {
        s_flash_guard_ops->op_lock();
    }
}

static inline void IRAM_ATTR spi_flash_guard_op_unlock()
{
    if (s_flash_guard_ops && s_flash_guard_ops->op_unlock) {
        s_flash_guard_ops->op_unlock();
    }
}

static esp_rom_spiflash_result_t IRAM_ATTR spi_flash_unlock()
{
    static bool unlocked = false;
    if (!unlocked) {
        spi_flash_guard_start();
        esp_rom_spiflash_result_t rc = esp_rom_spiflash_unlock();
        spi_flash_guard_end();
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            return rc;
        }
        unlocked = true;
    }
    return ESP_ROM_SPIFLASH_RESULT_OK;
}

esp_err_t IRAM_ATTR spi_flash_erase_sector(size_t sec)
{
    CHECK_WRITE_ADDRESS(sec * SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE);
    return spi_flash_erase_range(sec * SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE);
}

esp_err_t IRAM_ATTR spi_flash_erase_range(uint32_t start_addr, uint32_t size)
{
    CHECK_WRITE_ADDRESS(start_addr, size);
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
    esp_rom_spiflash_result_t rc;
    rc = spi_flash_unlock();
    if (rc == ESP_ROM_SPIFLASH_RESULT_OK) {
        for (size_t sector = start; sector != end && rc == ESP_ROM_SPIFLASH_RESULT_OK; ) {
            spi_flash_guard_start();
            if (sector % sectors_per_block == 0 && end - sector >= sectors_per_block) {
                rc = esp_rom_spiflash_erase_block(sector / sectors_per_block);
                sector += sectors_per_block;
                COUNTER_ADD_BYTES(erase, sectors_per_block * SPI_FLASH_SEC_SIZE);
            } else {
                rc = esp_rom_spiflash_erase_sector(sector);
                ++sector;
                COUNTER_ADD_BYTES(erase, SPI_FLASH_SEC_SIZE);
            }
            spi_flash_guard_end();
        }
    }
    COUNTER_STOP(erase);
    return spi_flash_translate_rc(rc);
}

/* Wrapper around esp_rom_spiflash_write() that verifies data as written if CONFIG_SPI_FLASH_VERIFY_WRITE is set.

   If CONFIG_SPI_FLASH_VERIFY_WRITE is not set, this is esp_rom_spiflash_write().
*/
static IRAM_ATTR esp_rom_spiflash_result_t spi_flash_write_inner(uint32_t target, const uint32_t *src_addr, int32_t len)
{
#ifndef CONFIG_SPI_FLASH_VERIFY_WRITE
    return esp_rom_spiflash_write(target, src_addr, len);
#else // CONFIG_SPI_FLASH_VERIFY_WRITE
    esp_rom_spiflash_result_t res = ESP_ROM_SPIFLASH_RESULT_OK;
    assert(len % sizeof(uint32_t) == 0);

    uint32_t before_buf[ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM / sizeof(uint32_t)];
    uint32_t after_buf[ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM / sizeof(uint32_t)];
    int32_t remaining = len;
    for(int i = 0; i < len; i += sizeof(before_buf)) {
        int i_w = i / sizeof(uint32_t); // index in words (i is an index in bytes)

        int32_t read_len = MIN(sizeof(before_buf), remaining);

        // Read "before" contents from flash
        res = esp_rom_spiflash_read(target + i, before_buf, read_len);
        if (res != ESP_ROM_SPIFLASH_RESULT_OK) {
            break;
        }

#ifdef CONFIG_SPI_FLASH_WARN_SETTING_ZERO_TO_ONE
        for (int r = 0; r < read_len; r += sizeof(uint32_t)) {
            int r_w = r / sizeof(uint32_t); // index in words (r is index in bytes)

            uint32_t write = src_addr[i_w + r_w];
            uint32_t before = before_buf[r_w];
            if ((before & write) != write) {
                spi_flash_guard_end();
                ESP_LOGW(TAG, "Write at offset 0x%x requests 0x%08x but will write 0x%08x -> 0x%08x",
                         target + i + r, write, before, before & write);
                spi_flash_guard_start();
            }
        }
#endif

        res = esp_rom_spiflash_write(target + i, &src_addr[i_w], read_len);
        if (res != ESP_ROM_SPIFLASH_RESULT_OK) {
            break;
        }

        res = esp_rom_spiflash_read(target + i, after_buf, read_len);
        if (res != ESP_ROM_SPIFLASH_RESULT_OK) {
            break;
        }

        for (int r = 0; r < read_len; r += sizeof(uint32_t)) {
            int r_w = r / sizeof(uint32_t); // index in words (r is index in bytes)

            uint32_t expected = src_addr[i_w + r_w] & before_buf[r_w];
            uint32_t actual = after_buf[r_w];
            if (expected != actual) {
#ifdef CONFIG_SPI_FLASH_LOG_FAILED_WRITE
                spi_flash_guard_end();
                ESP_LOGE(TAG, "Bad write at offset 0x%x expected 0x%08x readback 0x%08x", target + i + r, expected, actual);
                spi_flash_guard_start();
#endif
                res = ESP_ROM_SPIFLASH_RESULT_ERR;
            }
        }
        if (res != ESP_ROM_SPIFLASH_RESULT_OK) {
            break;
        }
        remaining -= read_len;
    }
    return res;
#endif // CONFIG_SPI_FLASH_VERIFY_WRITE
}


esp_err_t IRAM_ATTR spi_flash_write(size_t dst, const void *srcv, size_t size)
{
    CHECK_WRITE_ADDRESS(dst, size);
    // Out of bound writes are checked in ROM code, but we can give better
    // error code here
    if (dst + size > g_rom_flashchip.chip_size) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (size == 0) {
        return ESP_OK;
    }

    esp_rom_spiflash_result_t rc = ESP_ROM_SPIFLASH_RESULT_OK;
    COUNTER_START();
    const uint8_t *srcc = (const uint8_t *) srcv;
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
    if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
        goto out;
    }
    if (left_size > 0) {
        uint32_t t = 0xffffffff;
        memcpy(((uint8_t *) &t) + (dst - left_off), srcc, left_size);
        spi_flash_guard_start();
        rc = spi_flash_write_inner(left_off, &t, 4);
        spi_flash_guard_end();
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(write, 4);
    }
    if (mid_size > 0) {
        /* If src buffer is 4-byte aligned as well and is not in a region that requires cache access to be enabled, we
         * can write directly without buffering in RAM. */
#ifdef ESP_PLATFORM
        bool direct_write = esp_ptr_internal(srcc)
                && esp_ptr_byte_accessible(srcc)
                && ((uintptr_t) srcc + mid_off) % 4 == 0;
#else
        bool direct_write = true;
#endif
        while(mid_size > 0 && rc == ESP_ROM_SPIFLASH_RESULT_OK) {
            uint32_t write_buf[8];
            uint32_t write_size = MIN(mid_size, MAX_WRITE_CHUNK);
            const uint8_t *write_src = srcc + mid_off;
            if (!direct_write) {
                write_size = MIN(write_size, sizeof(write_buf));
                memcpy(write_buf, write_src, write_size);
                write_src = (const uint8_t *)write_buf;
            }
            spi_flash_guard_start();
            rc = spi_flash_write_inner(dst + mid_off, (const uint32_t *) write_src, write_size);
            spi_flash_guard_end();
            COUNTER_ADD_BYTES(write, write_size);
            mid_size -= write_size;
            mid_off += write_size;
        }
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            goto out;
        }
    }

    if (right_size > 0) {
        uint32_t t = 0xffffffff;
        memcpy(&t, srcc + right_off, right_size);
        spi_flash_guard_start();
        rc = spi_flash_write_inner(dst + right_off, &t, 4);
        spi_flash_guard_end();
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(write, 4);
    }
out:
    COUNTER_STOP(write);

    spi_flash_guard_op_lock();
    spi_flash_mark_modified_region(dst, size);
    spi_flash_guard_op_unlock();

    return spi_flash_translate_rc(rc);
}

esp_err_t IRAM_ATTR spi_flash_write_encrypted(size_t dest_addr, const void *src, size_t size)
{
    CHECK_WRITE_ADDRESS(dest_addr, size);
    const uint8_t *ssrc = (const uint8_t *)src;
    if ((dest_addr % 16) != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((size % 16) != 0) {
        return ESP_ERR_INVALID_SIZE;
    }

    COUNTER_START();
    esp_rom_spiflash_result_t rc;
    rc = spi_flash_unlock();
    if (rc == ESP_ROM_SPIFLASH_RESULT_OK) {
        /* esp_rom_spiflash_write_encrypted encrypts data in RAM as it writes,
           so copy to a temporary buffer - 32 bytes at a time.

           Each call to esp_rom_spiflash_write_encrypted takes a 32 byte "row" of
           data to encrypt, and each row is two 16 byte AES blocks
           that share a key (as derived from flash address).
        */
        uint8_t encrypt_buf[32] __attribute__((aligned(4)));
        uint32_t row_size;
        for (size_t i = 0; i < size; i += row_size) {
            uint32_t row_addr = dest_addr + i;
            if (i == 0 && (row_addr % 32) != 0) {
                /* writing to second block of a 32 byte row */
                row_size = 16;
                row_addr -= 16;
                /* copy to second block in buffer */
                memcpy(encrypt_buf + 16, ssrc + i, 16);
                /* decrypt the first block from flash, will reencrypt to same bytes */
                spi_flash_read_encrypted(row_addr, encrypt_buf, 16);
            } else if (size - i == 16) {
                /* 16 bytes left, is first block of a 32 byte row */
                row_size = 16;
                /* copy to first block in buffer */
                memcpy(encrypt_buf, ssrc + i, 16);
                /* decrypt the second block from flash, will reencrypt to same bytes */
                spi_flash_read_encrypted(row_addr + 16, encrypt_buf + 16, 16);
            } else {
                /* Writing a full 32 byte row (2 blocks) */
                row_size = 32;
                memcpy(encrypt_buf, ssrc + i, 32);
            }

            spi_flash_guard_start();
            rc = esp_rom_spiflash_write_encrypted(row_addr, (uint32_t *)encrypt_buf, 32);
            spi_flash_guard_end();
            if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
                break;
            }
        }
        bzero(encrypt_buf, sizeof(encrypt_buf));
    }
    COUNTER_ADD_BYTES(write, size);
    COUNTER_STOP(write);

    spi_flash_guard_op_lock();
    spi_flash_mark_modified_region(dest_addr, size);
    spi_flash_guard_op_unlock();

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

    esp_rom_spiflash_result_t rc = ESP_ROM_SPIFLASH_RESULT_OK;
    COUNTER_START();
    spi_flash_guard_start();
    /* To simplify boundary checks below, we handle small reads separately. */
    if (size < 16) {
        uint32_t t[6]; /* Enough for 16 bytes + 4 on either side for padding. */
        uint32_t read_src = src & ~3U;
        uint32_t left_off = src & 3U;
        uint32_t read_size = (left_off + size + 3) & ~3U;
        rc = esp_rom_spiflash_read(read_src, t, read_size);
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(read, read_size);
        memcpy(dstv, ((uint8_t *) t) + left_off, size);
        goto out;
    }
    uint8_t *dstc = (uint8_t *) dstv;
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

#ifdef ESP_PLATFORM
    bool direct_read = esp_ptr_internal(dstc)
            && esp_ptr_byte_accessible(dstc)
            && ((uintptr_t) dstc + dst_mid_off) % 4 == 0;
#else
    bool direct_read = true;
#endif
    if (mid_size > 0) {
        uint32_t mid_remaining = mid_size;
        uint32_t mid_read = 0;
        while (mid_remaining > 0) {
            uint32_t read_size = MIN(mid_remaining, MAX_READ_CHUNK);
            uint32_t read_buf[8];
            uint8_t *read_dst_final = dstc + dst_mid_off + mid_read;
            uint8_t *read_dst = read_dst_final;
            if (!direct_read) {
                read_size = MIN(read_size, sizeof(read_buf));
                read_dst = (uint8_t *) read_buf;
            }
            rc = esp_rom_spiflash_read(src + src_mid_off + mid_read,
                    (uint32_t *) read_dst, read_size);
            if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
                goto out;
            }
            mid_remaining -= read_size;
            mid_read += read_size;
            if (!direct_read) {
                spi_flash_guard_end();
                memcpy(read_dst_final, read_buf, read_size);
                spi_flash_guard_start();
            } else if (mid_remaining > 0) {
                /* Drop guard momentarily, allows other tasks to preempt */
                spi_flash_guard_end();
                spi_flash_guard_start();
            }
        }
        COUNTER_ADD_BYTES(read, mid_size);
        /*
         * If offsets in src and dst are different, perform an in-place shift
         * to put destination data into its final position.
         * Note that the shift can be left (src_mid_off < dst_mid_off) or right.
         */
        if (src_mid_off != dst_mid_off) {
            if (!direct_read) {
                spi_flash_guard_end();
            }
            memmove(dstc + src_mid_off, dstc + dst_mid_off, mid_size);
            if (!direct_read) {
                spi_flash_guard_start();
            }
        }
    }
    if (pad_left_size > 0) {
        uint32_t t;
        rc = esp_rom_spiflash_read(pad_left_src, &t, 4);
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(read, 4);
        if (!direct_read) {
            spi_flash_guard_end();
        }
        memcpy(dstc, ((uint8_t *) &t) + (4 - pad_left_size), pad_left_size);
        if (!direct_read) {
            spi_flash_guard_start();
        }
    }
    if (pad_right_size > 0) {
        uint32_t t[2];
        int32_t read_size = (pad_right_size <= 4 ? 4 : 8);
        rc = esp_rom_spiflash_read(pad_right_src, t, read_size);
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            goto out;
        }
        COUNTER_ADD_BYTES(read, read_size);
        if (!direct_read) {
            spi_flash_guard_end();
        }
        memcpy(dstc + pad_right_off, t, pad_right_size);
        if (!direct_read) {
            spi_flash_guard_start();
        }
    }
out:
    spi_flash_guard_end();
    COUNTER_STOP(read);
    return spi_flash_translate_rc(rc);
}

esp_err_t IRAM_ATTR spi_flash_read_encrypted(size_t src, void *dstv, size_t size)
{
    if (src + size > g_rom_flashchip.chip_size) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (size == 0) {
        return ESP_OK;
    }

    esp_err_t err;
    const uint8_t *map;
    spi_flash_mmap_handle_t map_handle;
    size_t map_src = src & ~(SPI_FLASH_MMU_PAGE_SIZE - 1);
    size_t map_size = size + (src - map_src);

    err = spi_flash_mmap(map_src, map_size, SPI_FLASH_MMAP_DATA, (const void **)&map, &map_handle);
    if (err != ESP_OK) {
        return err;
    }
    memcpy(dstv, map + (src - map_src), size);
    spi_flash_munmap(map_handle);
    return err;
}


static esp_err_t IRAM_ATTR spi_flash_translate_rc(esp_rom_spiflash_result_t rc)
{
    switch (rc) {
    case ESP_ROM_SPIFLASH_RESULT_OK:
        return ESP_OK;
    case ESP_ROM_SPIFLASH_RESULT_TIMEOUT:
        return ESP_ERR_FLASH_OP_TIMEOUT;
    case ESP_ROM_SPIFLASH_RESULT_ERR:
    default:
        return ESP_ERR_FLASH_OP_FAIL;
    }
}

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS

static inline void dump_counter(spi_flash_counter_t *counter, const char *name)
{
    ESP_LOGI(TAG, "%s  count=%8d  time=%8dus  bytes=%8d\n", name,
             counter->count, counter->time, counter->bytes);
}

const spi_flash_counters_t *spi_flash_get_counters()
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
