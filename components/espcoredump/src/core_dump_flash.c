// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include <string.h>
#include "esp_ota_ops.h"
#include "rom/crc.h"
#include "esp_partition.h"
#include "esp_core_dump_priv.h"

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_flash";

#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH
#define DUER_COREDUMP_VALID_NUM     3
#define DUER_COREDUMP_MAX_LEN       64*1024
#define DUER_CRAB_ELF_SHA256_LEN    64
#define DUER_CRAB_ELF_APPNAME_LEN   32

typedef struct _duer_crab_head_t
{
    uint32_t    magic_num;
    long        crash_time;
    char        app_name[DUER_CRAB_ELF_APPNAME_LEN];
    char        elf_sha256[DUER_CRAB_ELF_SHA256_LEN + 1];
} __attribute__((__packed__)) duer_crab_head_t;

typedef struct _core_dump_write_flash_data_t
{
    uint32_t        off; // current offset in partition
    core_dump_crc_t crc; // CRC of dumped data
} core_dump_write_flash_data_t;

typedef struct _core_dump_partition_t
{
    // core dump partition start
    uint32_t start;
    // core dump partition size
    uint32_t size;
} core_dump_partition_t;

typedef struct _core_dump_flash_config_t
{
    // core dump partition config
    core_dump_partition_t partition;
    // CRC of core dump partition config
    core_dump_crc_t       partition_config_crc;
    /*add by zhangjiajie for multiple coredump*/
    uint8_t               exist_coredump_num;
    /*add end*/
} core_dump_flash_config_t;

// core dump flash data
static core_dump_flash_config_t s_core_flash_config;
duer_crab_head_t duer_crab_head;

static inline core_dump_crc_t esp_core_dump_calc_flash_config_crc(void)
{
    return crc32_le(0, (uint8_t const *)&s_core_flash_config.partition, sizeof(s_core_flash_config.partition));
}

static uint32_t esp_core_dump_read_coredump_num(char* valid_num) {
    esp_err_t  err;
    uint32_t   data32 = 0;
    uint8_t    coredump_num = 0;

    *valid_num = 0;
    do {
        err = spi_flash_read(s_core_flash_config.partition.start + coredump_num*DUER_COREDUMP_MAX_LEN, &data32, sizeof(uint32_t));
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to read coredump head (%d)!", err);
            break;
        }
        ESP_COREDUMP_LOGI("crab read head data=%x", data32);
        if ((data32 & 0xffffff00) != 0xa5a5a500) {
            break;
        }
        if ((data32 & 0xff) > DUER_COREDUMP_VALID_NUM) {
            ESP_COREDUMP_LOGE("Coredump num err[%x]", valid_num);
            break;
        }
        coredump_num ++;
    } while (coredump_num < DUER_COREDUMP_VALID_NUM);

    *valid_num = coredump_num;

    return ESP_OK;
}

void esp_core_dump_flash_init() 
{
    char valid_num = 0;
    const esp_partition_t *core_part;

    ESP_COREDUMP_LOGI("Init core dump to flash");
    core_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, NULL);
    if (!core_part) {
        ESP_COREDUMP_LOGE("No core dump partition found!");
        return;
    }
    ESP_COREDUMP_LOGI("Found partition '%s' @ %x %d bytes", core_part->label, core_part->address, core_part->size);
    s_core_flash_config.partition.start      = core_part->address;
    s_core_flash_config.partition.size       = core_part->size;
    s_core_flash_config.partition_config_crc = esp_core_dump_calc_flash_config_crc();

    /*add by zhangjiajie for multiple coredump*/
    memset(&duer_crab_head, 0, sizeof(duer_crab_head));

    /*若只有1个coredump，coredump_num=1 */
    s_core_flash_config.exist_coredump_num = 0;
    esp_core_dump_read_coredump_num(&valid_num);
    s_core_flash_config.exist_coredump_num = valid_num;

    /*save current sha256,avoid access cache when crash*/
    esp_ota_get_app_elf_sha256(duer_crab_head.elf_sha256, sizeof(duer_crab_head.elf_sha256));
    ESP_COREDUMP_LOGI("crab sha256:%s", duer_crab_head.elf_sha256);
    /*save current app name,avoid access cache when crash*/
    esp_crab_get_app_name(duer_crab_head.app_name, sizeof(duer_crab_head.app_name));
    ESP_COREDUMP_LOGI("crab appname:%s", duer_crab_head.app_name);
    /*add end*/
}

static uint32_t esp_core_dump_write_flash_padded(size_t off, uint8_t *data, uint32_t data_size)
{
    esp_err_t err;
    uint32_t data_len = 0, k, len;
    union
    {
        uint8_t    data8[4];
        uint32_t   data32;
    } rom_data;

    data_len = (data_size / sizeof(uint32_t)) * sizeof(uint32_t);

    assert(off >= s_core_flash_config.partition.start);
    assert((off + data_len + (data_size % sizeof(uint32_t) ? sizeof(uint32_t) : 0)) <=
        s_core_flash_config.partition.start + s_core_flash_config.partition.size);

    err = spi_flash_write(off, data, data_len);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write data to flash (%d)!", err);
        return 0;
    }

    len = data_size % sizeof(uint32_t);
    if (len) {
        // write last bytes with padding, actual TCB len can be retrieved by esptool from core dump header
        rom_data.data32 = 0;
        for (k = 0; k < len; k++) {
            rom_data.data8[k] = *(data + data_len + k);
        }
        err = spi_flash_write(off + data_len, &rom_data, sizeof(uint32_t));
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to finish write data to flash (%d)!", err);
            return 0;
        }
        data_len += sizeof(uint32_t);
    }

    return data_len;
}

static esp_err_t esp_core_dump_flash_write_prepare(void *priv, uint32_t *data_len)
{
    esp_err_t err;
    uint32_t sec_num;
    int total_len = 0;
    core_dump_write_flash_data_t *wr_data = (core_dump_write_flash_data_t *)priv;

    // check for available space in partition
    if ((*data_len + sizeof(uint32_t) + sizeof(uint32_t)) > DUER_COREDUMP_MAX_LEN) {
        ESP_COREDUMP_LOGE("[%d]Not enough space to save core dump!", *data_len);
        return ESP_ERR_NO_MEM;
    }
    // add space for CRC
    *data_len += sizeof(core_dump_crc_t);
    total_len = *data_len + sizeof(uint32_t);
    ESP_COREDUMP_LOGE("total_len: (0x%x)!", total_len);

//    memset(wr_data, 0, sizeof(*wr_data));
    /*offset add duer coredump head and sha256*/
    wr_data->off = DUER_COREDUMP_MAX_LEN * s_core_flash_config.exist_coredump_num + sizeof(duer_crab_head);
    wr_data->crc = 0;

    /*add coredump head len*/
    sec_num = total_len / SPI_FLASH_SEC_SIZE;
    if (total_len % SPI_FLASH_SEC_SIZE) {
        sec_num++;
    }
    assert(sec_num * SPI_FLASH_SEC_SIZE <= s_core_flash_config.partition.size);
    err = spi_flash_erase_range(s_core_flash_config.partition.start + DUER_COREDUMP_MAX_LEN * s_core_flash_config.exist_coredump_num, DUER_COREDUMP_MAX_LEN);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to erase flash (%d)!", err);
        return err;
    }
    return err;
}

static esp_err_t esp_core_dump_flash_write_word(core_dump_write_flash_data_t *wr_data, uint32_t word)
{
    esp_err_t err = ESP_OK;
    uint32_t  data32 = word;

    assert(wr_data->off + sizeof(uint32_t) <= s_core_flash_config.partition.size);
    err = spi_flash_write(s_core_flash_config.partition.start + wr_data->off, &data32, sizeof(uint32_t));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write to flash (%d)!", err);
        return err;
    }
    wr_data->off += sizeof(uint32_t);

    return err;
}

static esp_err_t esp_core_dump_flash_write_start(void *priv)
{
    ets_printf(DRAM_STR("================= CORE DUMP START =================\r\n"));
    return ESP_OK;
}

static esp_err_t esp_core_dump_flash_write_end(void *priv)
{
    ets_printf(DRAM_STR("================= CORE DUMP END =================\r\n"));
    core_dump_write_flash_data_t *wr_data = (core_dump_write_flash_data_t *)priv;
#if LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG
    union
    {
        uint8_t    data8[20];
        uint32_t   data32[5];
    } rom_data;

    esp_err_t err = spi_flash_read(s_core_flash_config.partition.start + s_core_flash_config.exist_coredump_num*DUER_COREDUMP_MAX_LEN, &rom_data, sizeof(rom_data));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to read flash (%d)!", err);
        return err;
    } else {
        ESP_COREDUMP_LOG_PROCESS("Data from flash:");
        for (uint32_t i = 0; i < sizeof(rom_data)/sizeof(rom_data.data32[0]); i++) {
            ESP_COREDUMP_LOG_PROCESS("%x", rom_data.data32[i]);
        }
    }
#endif
    // write core dump CRC
    ESP_COREDUMP_LOG_PROCESS("Dump data CRC = 0x%x", wr_data->crc);
    esp_err_t ret = esp_core_dump_flash_write_word(wr_data, wr_data->crc);
    if (ESP_OK == ret) {

        duer_crab_head.magic_num = 0xa5a5a500 | (s_core_flash_config.exist_coredump_num + 1);

        ESP_COREDUMP_LOGI("crab magic num:0x%x", duer_crab_head.magic_num);
        ESP_COREDUMP_LOGI("crab crash time:%d", duer_crab_head.crash_time);
        ESP_COREDUMP_LOGI("crab app name:%s", duer_crab_head.app_name);
        ESP_COREDUMP_LOGI("crab sha256:%s", duer_crab_head.elf_sha256);

        ret = spi_flash_write(s_core_flash_config.partition.start + s_core_flash_config.exist_coredump_num*DUER_COREDUMP_MAX_LEN, &duer_crab_head, sizeof(duer_crab_head));
        if (ESP_OK != ret) {
            ESP_COREDUMP_LOGE("Failed to write coredump head to flash (%d)!", ret);
            return ret;
        }

        s_core_flash_config.exist_coredump_num++;
    }

    return ret;
}

static void baidu_esp_core_dump_b64_encode(const uint8_t *src, uint32_t src_len, uint8_t *dst) {
    const static char b64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int i = 0, j = 0, a = 0, b = 0, c = 0;

    for (i = j = 0; i < src_len; i += 3) {
        a = src[i];
        b = i + 1 >= src_len ? 0 : src[i + 1];
        c = i + 2 >= src_len ? 0 : src[i + 2];

        dst[j++] = b64[a >> 2];
        dst[j++] = b64[((a & 3) << 4) | (b >> 4)];
        if (i + 1 < src_len) {
            dst[j++] = b64[(b & 0x0F) << 2 | (c >> 6)];
        }
        if (i + 2 < src_len) {
            dst[j++] = b64[c & 0x3F];
        }
    }
    while (j % 4 != 0) {
        dst[j++] = '=';
    }
    dst[j++] = '\0';
}
static esp_err_t baidu_esp_core_dump_uart_write_data(void * data, uint32_t data_len)
{
    esp_err_t err = ESP_OK;
    char buf[64 + 4], *addr = data;
    char *end = addr + data_len;

    while (addr < end) {
        size_t len = end - addr;
        if (len > 48) len = 48;
        /* Copy to stack to avoid alignment restrictions. */
        char *tmp = buf + (sizeof(buf) - len);
        memcpy(tmp, addr, len);
        baidu_esp_core_dump_b64_encode((const uint8_t *)tmp, len, (uint8_t *)buf);
        addr += len;
        ets_printf(DRAM_STR("%s\r\n"), buf);
    }

    return err;
}
static esp_err_t esp_core_dump_flash_write_data(void *priv, void * data, uint32_t data_len)
{
    esp_err_t err = ESP_OK;
    core_dump_write_flash_data_t *wr_data = (core_dump_write_flash_data_t *)priv;

    uint32_t len = esp_core_dump_write_flash_padded(s_core_flash_config.partition.start + wr_data->off, data, data_len);
    if (len != data_len) {
        return ESP_FAIL;
    }

    wr_data->off += len;
    wr_data->crc = crc32_le(wr_data->crc, data, data_len);

    baidu_esp_core_dump_uart_write_data(data, data_len);

    return err;
}

#ifdef CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
#define esp_core_dump_to_flash esp_core_dump_to_flash_inner
#endif // CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY

void esp_core_dump_to_flash(XtExcFrame *frame)
{
    ESP_COREDUMP_LOGI("%s called with sp: %p frame: %p", __func__, get_sp(), frame);
    core_dump_write_config_t wr_cfg;
    core_dump_write_flash_data_t wr_data;

    core_dump_crc_t crc = esp_core_dump_calc_flash_config_crc();
    if (s_core_flash_config.partition_config_crc != crc) {
        ESP_COREDUMP_LOGE("Core dump flash config is corrupted! CRC=0x%x instead of 0x%x", crc, s_core_flash_config.partition_config_crc);
        return;
    }
    // check that partition can hold at least core dump data length
    if (s_core_flash_config.partition.start == 0 || s_core_flash_config.partition.size < sizeof(uint32_t)) {
        ESP_COREDUMP_LOGE("Invalid flash partition config!");
        return;
    }

    /* init non-OS flash access critical section */
    spi_flash_guard_set(&g_flash_guard_no_os_ops);

    memset(&wr_cfg, 0, sizeof(wr_cfg));
    wr_cfg.prepare = esp_core_dump_flash_write_prepare;
    wr_cfg.start = esp_core_dump_flash_write_start;
    wr_cfg.end = esp_core_dump_flash_write_end;
    wr_cfg.write = esp_core_dump_flash_write_data;
    wr_cfg.priv = &wr_data;

    duer_crab_head.crash_time = 0;

    ESP_COREDUMP_LOGI("Exist Coredump num [%d]...", s_core_flash_config.exist_coredump_num);
    if (DUER_COREDUMP_VALID_NUM < s_core_flash_config.exist_coredump_num) {
        ESP_COREDUMP_LOGE("Coredump is full,exist_coredump_num=[%d]...", s_core_flash_config.exist_coredump_num);
        return;
    }
    /*check that partition can hold next coredump*/
    if (s_core_flash_config.partition.size < (DUER_COREDUMP_MAX_LEN*(1 + s_core_flash_config.exist_coredump_num))) {
        ESP_COREDUMP_LOGI("partition can not hold next coredump,num=[%d],flash size=[%d]...", s_core_flash_config.exist_coredump_num, s_core_flash_config.partition.size);
        return;
    }

    ESP_COREDUMP_LOGI("Save core dump to flash...");
    esp_core_dump_write((void*)frame, &wr_cfg);
    ESP_COREDUMP_LOGI("Core dump has been saved to flash.");
}

#endif // CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH
