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
#ifndef __BOOT_CONFIG_H__
#define __BOOT_CONFIG_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
#define BOOT_VERSION "V0.1"
#define SPI_SEC_SIZE 0x1000
#define MEM_CACHE(offset)   (uint8_t *)(0x3f400000 + (offset))
#define CACHE_READ_32(offset)   ((uint32_t *)(0x3f400000 + (offset)))
#define PARTITION_ADD 0x4000
#define PARTITION_MAGIC 0x50AA
#define IROM_LOW    0x400D0000
#define IROM_HIGH   0x40400000
#define DROM_LOW    0x3F400000
#define DROM_HIGH   0x3F800000

/*spi mode,saved in third byte in flash */
enum {
    SPI_MODE_QIO,
    SPI_MODE_QOUT,
    SPI_MODE_DIO,
    SPI_MODE_DOUT,
    SPI_MODE_FAST_READ,
    SPI_MODE_SLOW_READ
};
/* spi speed*/
enum {
    SPI_SPEED_40M,
    SPI_SPEED_26M,
    SPI_SPEED_20M,
    SPI_SPEED_80M = 0xF
};
/*suppport flash size in esp32 */
enum {
    SPI_SIZE_1MB = 0,
    SPI_SIZE_2MB,
    SPI_SIZE_4MB,
    SPI_SIZE_8MB,
    SPI_SIZE_16MB,
    SPI_SIZE_MAX
};


struct flash_hdr {
    char magic;
    char blocks;
    char spi_mode;      /* flag of flash read mode in unpackage and usage in future */
    char spi_speed: 4;  /* low bit */
    char spi_size: 4;
    unsigned int entry_addr;
    uint8_t encrypt_flag;    /* encrypt flag */
    uint8_t secury_boot_flag; /* secury boot flag */
    char extra_header[14]; /* ESP32 additional header, unused by second bootloader */
};

/* each header of flash bin block */
struct block_hdr {
    unsigned int load_addr;
    unsigned int data_len;
};

/* OTA selection structure (two copies in the OTA data partition.)

   Size of 32 bytes is friendly to flash encryption */
typedef struct {
    uint32_t ota_seq;
    uint8_t  seq_label[24];
    uint32_t crc; /* CRC32 of ota_seq field only */
} ota_select;

typedef struct {
    uint32_t offset;
    uint32_t size;
} partition_pos_t;

typedef struct {
	uint16_t magic;
	uint8_t  type;        /* partition Type */
    uint8_t  subtype;     /* part_subtype */
    partition_pos_t pos;
	uint8_t  label[16];    /* label for the partition */
    uint8_t  reserved[4];     /* reserved */
} partition_info_t;

#define PART_TYPE_APP 0x00
#define PART_SUBTYPE_FACTORY  0x00
#define PART_SUBTYPE_OTA_FLAG 0x10
#define PART_SUBTYPE_OTA_MASK 0x0f
#define PART_SUBTYPE_TEST     0x20

#define PART_TYPE_DATA 0x01
#define PART_SUBTYPE_DATA_OTA 0x00
#define PART_SUBTYPE_DATA_RF  0x01
#define PART_SUBTYPE_DATA_WIFI 0x02

#define PART_TYPE_END 0xff
#define PART_SUBTYPE_END 0xff

#define SPI_ERROR_LOG "spi flash error"

typedef struct {
    partition_pos_t ota_info;
    partition_pos_t factory;
    partition_pos_t test;
    partition_pos_t ota[16];
    uint32_t app_count;
    uint32_t selected_subtype;
} bootloader_state_t;

void boot_cache_redirect( uint32_t pos, size_t size );
uint32_t get_bin_len(uint32_t pos);

bool flash_encrypt(bootloader_state_t *bs);
bool secure_boot(void);


#ifdef __cplusplus
}
#endif

#endif /* __BOOT_CONFIG_H__ */
