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
#include <stdint.h>
#include <limits.h>

#include "esp_attr.h"

#include "rom/cache.h"
#include "rom/ets_sys.h"
#include "rom/spi_flash.h"
#include "rom/crc.h"

#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/efuse_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/timers_reg.h"

#include "sdkconfig.h"

#include "bootloader_log.h"
#include "bootloader_config.h"

extern int _bss_start;
extern int _bss_end;
/*
We arrive here after the bootloader finished loading the program from flash. The hardware is mostly uninitialized,
flash cache is down and the app CPU is in reset. We do have a stack, so we can do the initialization in C.
*/

// TODO: make a nice header file for ROM functions instead of adding externs all over the place
extern void Cache_Flush(int);

void bootloader_main();
void unpack_load_app(const partition_pos_t *app_node);
void print_flash_info(struct flash_hdr* pfhdr);
void IRAM_ATTR set_cache_and_start_app(uint32_t drom_addr,
    uint32_t drom_load_addr,
    uint32_t drom_size,
    uint32_t irom_addr,
    uint32_t irom_load_addr,
    uint32_t irom_size,
    uint32_t entry_addr);


void IRAM_ATTR call_start_cpu0()
{
    //Make page 0 access raise an exception
    //Also some other unused pages so we can catch weirdness
    //ToDo: this but nicer.
    asm volatile (\
                  "movi a4,0x00000000\n" \
                  "movi a5,0xf\n" \
                  "wdtlb a5,a4\n" \
                  "witlb a5,a4\n" \
                  "movi a4,0x80000000\n" \
                  "wdtlb a5,a4\n" \
                  "witlb a5,a4\n" \
                  "movi a4,0xa0000000\n" \
                  "wdtlb a5,a4\n" \
                  "witlb a5,a4\n" \
                  "movi a4,0xc0000000\n" \
                  "wdtlb a5,a4\n" \
                  "witlb a5,a4\n" \
                  "movi a4,0xe0000000\n" \
                  "wdtlb a5,a4\n" \
                  "witlb a5,a4\n" \
                  "movi a4,0x20000000\n" \
                  "movi a5,0x0\n" \
                  "wdtlb a5,a4\n" \
                  "witlb a5,a4\n" \
                  "movi a4,0x40000000\n" \
                  "movi a5,0x2\n" \
                  "wdtlb a5,a4\n" \
                  "witlb a5,a4\n" \
                  "isync\n" \
                  :::"a4","a5");

    //Clear bss
    memset(&_bss_start, 0, (&_bss_end - &_bss_start) * sizeof(_bss_start));

	/* completely reset MMU for both CPUs
	   (in case serial bootloader was running) */
	Cache_Read_Disable(0);
	Cache_Read_Disable(1);
	Cache_Flush(0);
	Cache_Flush(1);
	mmu_init(0);
	mmu_init(1);
	/* (above steps probably unnecessary for most serial bootloader
	   usage, all that's absolutely needed is that we unmask DROM0
	   cache on the following two lines - normal ROM boot exits with
	   DROM0 cache unmasked, but serial bootloader exits with it
	   masked. However can't hurt to be thorough and reset
	   everything.)
	*/
	REG_CLR_BIT(PRO_CACHE_CTRL1_REG, DPORT_PRO_CACHE_MASK_DROM0);
	REG_CLR_BIT(APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MASK_DROM0);

    bootloader_main();
}

/**
 *  @function :     get_bin_len
 *  @description:   get bin's length 
 *
 *  @inputs:        pos     bin locate address in flash
 *  @return:        uint32  length of bin,if bin MAGIC error return 0
 */

uint32_t get_bin_len(uint32_t pos)
{   
    uint32_t len = 8 + 16;
    uint8_t i;
    log_debug("pos %d %x\n",pos,*(uint8_t *)pos);
    if(0xE9 != *(uint8_t *)pos) {
        return 0;
    }
    for (i = 0; i < *(uint8_t *)(pos + 1); i++) {
        len += *(uint32_t *)(pos + len + 4) + 8;
    }
    if (len % 16 != 0) {
        len = (len / 16 + 1) * 16;
    } else {
        len += 16;
    }
    log_debug("bin length = %d\n", len);
    return len;
}

/** 
 *  @function :     boot_cache_redirect
 *  @description:   Configure several pages in flash map so that `size` bytes 
 *                  starting at `pos` are mapped to 0x3f400000.
 *                  This sets up mapping only for PRO CPU.
 *
 *  @inputs:        pos     address in flash
 *                  size    size of the area to map, in bytes
 */
void boot_cache_redirect( uint32_t pos, size_t size )
{
    uint32_t pos_aligned = pos & 0xffff0000;
    uint32_t count = (size + 0xffff) / 0x10000;
    Cache_Read_Disable( 0 );
    Cache_Flush( 0 );
    log_debug( "mmu set paddr=%08x count=%d", pos_aligned, count );
    cache_flash_mmu_set( 0, 0, 0x3f400000, pos_aligned, 64, count );
    Cache_Read_Enable( 0 );
}

/**
 *  @function :     load_partition_table
 *  @description:   Parse partition table, get useful data such as location of 
 *                  OTA info sector, factory app sector, and test app sector.
 *
 *  @inputs:        bs     bootloader state structure used to save the data
 *                  addr   address of partition table in flash
 *  @return:        return true, if the partition table is loaded (and MD5 checksum is valid)
 *
 */
bool load_partition_table(bootloader_state_t* bs, uint32_t addr)
{
    partition_info_t partition;
    uint32_t end = addr + 0x1000;
    int index = 0;
    char *partition_usage;

    log_info("Partition Table:");
    log_info("## Label            Usage          Type ST Offset   Length");

    while (addr < end) {
        log_debug("load partition table entry from %x(%08x)", addr, MEM_CACHE(addr));
        memcpy(&partition, MEM_CACHE(addr), sizeof(partition));
        log_debug("type=%x subtype=%x", partition.type, partition.subtype);
        partition_usage = "unknown";

        if (partition.magic == PARTITION_MAGIC) { /* valid partition definition */
            switch(partition.type) {
            case PART_TYPE_APP: /* app partition */
                switch(partition.subtype) {
                case PART_SUBTYPE_FACTORY: /* factory binary */
                    bs->factory = partition.pos;
                    partition_usage = "factory app";
                    break;
                case PART_SUBTYPE_TEST: /* test binary */
                    bs->test = partition.pos;
                    partition_usage = "test app";
                    break;
                default:
                    /* OTA binary */
                    if ((partition.subtype & ~PART_SUBTYPE_OTA_MASK) == PART_SUBTYPE_OTA_FLAG) {
                        bs->ota[partition.subtype & PART_SUBTYPE_OTA_MASK] = partition.pos;
                        ++bs->app_count;
                        partition_usage = "OTA app";
                    }
                    else {
                        partition_usage = "Unknown app";
                    }
                    break;
                }
                break; /* PART_TYPE_APP */
            case PART_TYPE_DATA: /* data partition */
                switch(partition.subtype) {
                case PART_SUBTYPE_DATA_OTA: /* ota data */
                    bs->ota_info = partition.pos;
                    partition_usage = "OTA data";
                    break;
                case PART_SUBTYPE_DATA_RF:
                    partition_usage = "RF data";
                    break;
                case PART_SUBTYPE_DATA_WIFI:
                    partition_usage = "WiFi data";
                    break;
                default:
                    partition_usage = "Unknown data";
                    break;
                }
                break; /* PARTITION_USAGE_DATA */
            default: /* other partition type */
                break;
            }
        }
        /* invalid partition magic number */
        else {
            break; /* todo: validate md5 */
        }

        /* print partition type info */
        log_info("%2d %-16s %-16s %02x %02x %08x %08x", index, partition.label, partition_usage,
                 partition.type, partition.subtype,
                 partition.pos.offset, partition.pos.size);
        index++;
        addr += sizeof(partition);
    }

    log_info("End of partition table");
    return true;
}

static uint32_t ota_select_crc(const ota_select *s)
{
  return crc32_le(UINT32_MAX, (uint8_t*)&s->ota_seq, 4);
}

static bool ota_select_valid(const ota_select *s)
{
  return s->ota_seq != UINT32_MAX && s->crc == ota_select_crc(s);
}

/**
 *  @function :     bootloader_main
 *  @description:   entry function of 2nd bootloader
 *
 *  @inputs:        void
 */

void bootloader_main()
{
    //Run start routine.
    /*ESP32 2ND bootload start here*/


    log_info( "\n" );
    log_info( "**************************************" );
    log_info( "*       hello espressif ESP32!       *" );
    log_info( "*        2nd boot is running!        *" );
    log_info( "*            version (%s)          *", BOOT_VERSION);
    log_info( "**************************************");

    struct flash_hdr    fhdr;
    bootloader_state_t bs;
    SpiFlashOpResult spiRet1,spiRet2;    
    ota_select sa,sb;
    memset(&bs, 0, sizeof(bs));

    log_notice( "compile time %s\n", __TIME__ );
    /* close watch dog here */
    REG_CLR_BIT( RTC_WDTCONFIG0, RTC_CNTL_WDT_FLASHBOOT_MOD_EN );
    REG_CLR_BIT( WDTCONFIG0, TIMERS_WDT_FLASHBOOT_MOD_EN );
    SPIUnlock();
    /*register first sector in drom0 page 0 */
    boot_cache_redirect( 0, 0x5000 );

    memcpy((unsigned int *) &fhdr, MEM_CACHE(0x1000), sizeof(struct flash_hdr) );

    print_flash_info(&fhdr);

    if (!load_partition_table(&bs, PARTITION_ADD)) {
        log_error("load partition table error!");
        return;
    }

    partition_pos_t load_part_pos;

    if (bs.ota_info.offset != 0) {              // check if partition table has OTA info partition
        //log_error("OTA info sector handling is not implemented");
        boot_cache_redirect(bs.ota_info.offset, bs.ota_info.size );
        memcpy(&sa,MEM_CACHE(bs.ota_info.offset & 0x0000ffff),sizeof(sa));
        memcpy(&sb,MEM_CACHE((bs.ota_info.offset + 0x1000)&0x0000ffff) ,sizeof(sb));
        if(sa.ota_seq == 0xFFFFFFFF && sb.ota_seq == 0xFFFFFFFF) {
            // init status flash
            load_part_pos = bs.ota[0];
            sa.ota_seq = 0x01;
            sa.crc = ota_select_crc(&sa);
            sb.ota_seq = 0x00;
            sb.crc = ota_select_crc(&sb);

            Cache_Read_Disable(0);  
            spiRet1 = SPIEraseSector(bs.ota_info.offset/0x1000);       
            spiRet2 = SPIEraseSector(bs.ota_info.offset/0x1000+1);       
            if (spiRet1 != SPI_FLASH_RESULT_OK || spiRet2 != SPI_FLASH_RESULT_OK ) {  
                log_error(SPI_ERROR_LOG);
                return;
            } 
            spiRet1 = SPIWrite(bs.ota_info.offset,(uint32_t *)&sa,sizeof(ota_select));
            spiRet2 = SPIWrite(bs.ota_info.offset + 0x1000,(uint32_t *)&sb,sizeof(ota_select));
            if (spiRet1 != SPI_FLASH_RESULT_OK || spiRet2 != SPI_FLASH_RESULT_OK ) {  
                log_error(SPI_ERROR_LOG);
                return;
            } 
            Cache_Read_Enable(0);  
            //TODO:write data in ota info
        } else  {
            if(ota_select_valid(&sa) && ota_select_valid(&sb)) {
                load_part_pos = bs.ota[(((sa.ota_seq > sb.ota_seq)?sa.ota_seq:sb.ota_seq) - 1)%bs.app_count];
            }else if(ota_select_valid(&sa)) {
                load_part_pos = bs.ota[(sa.ota_seq - 1) % bs.app_count];
            }else if(ota_select_valid(&sb)) {
                load_part_pos = bs.ota[(sb.ota_seq - 1) % bs.app_count];
            }else {
                log_error("ota data partition info error");
                return;
            }
        }
    } else if (bs.factory.offset != 0) {        // otherwise, look for factory app partition
        load_part_pos = bs.factory;
    } else if (bs.test.offset != 0) {           // otherwise, look for test app parition
        load_part_pos = bs.test;
    } else {                                    // nothing to load, bail out
        log_error("nothing to load");
        return;
    }

    log_info("Loading app partition at offset %08x", load_part_pos);
    if(fhdr.secury_boot_flag == 0x01) {
        /* protect the 2nd_boot  */    
        if(false == secure_boot()){
            log_error("secure boot failed");
            return;
        }
    }

    if(fhdr.encrypt_flag == 0x01) {
        /* encrypt flash */            
        if (false == flash_encrypt(&bs)) {
           log_error("flash encrypt failed");
           return;
        }
    }

    // copy sections to RAM, set up caches, and start application
    unpack_load_app(&load_part_pos);
}


void unpack_load_app(const partition_pos_t* partition)
{
    boot_cache_redirect(partition->offset, partition->size);

    uint32_t pos = 0;
    struct flash_hdr image_header;
    memcpy(&image_header, MEM_CACHE(pos), sizeof(image_header));
    pos += sizeof(image_header);

    uint32_t drom_addr = 0;
    uint32_t drom_load_addr = 0;
    uint32_t drom_size = 0;
    uint32_t irom_addr = 0;
    uint32_t irom_load_addr = 0;
    uint32_t irom_size = 0;

    log_debug("bin_header: %u %u %u %u %08x\n", image_header.magic,
              image_header.blocks,
              image_header.spi_mode,
              image_header.spi_size,
              (unsigned)image_header.entry_addr);

    for (uint32_t section_index = 0;
            section_index < image_header.blocks;
            ++section_index) {
        struct block_hdr section_header = {0};
        memcpy(&section_header, MEM_CACHE(pos), sizeof(section_header));
        pos += sizeof(section_header);

        const uint32_t address = section_header.load_addr;
        bool load = true;
        bool map = false;
        if (address == 0x00000000) {        // padding, ignore block
            load = false;
        }
        if (address == 0x00000004) {
            load = false;                   // md5 checksum block
            // TODO: actually check md5
        }

        if (address >= DROM_LOW && address < DROM_HIGH) {
            log_debug("found drom section, map from %08x to %08x\n", pos,
                      section_header.load_addr);
            drom_addr = partition->offset + pos - sizeof(section_header);
            drom_load_addr = section_header.load_addr;
            drom_size = section_header.data_len + sizeof(section_header);
            load = false;
            map = true;
        }

        if (address >= IROM_LOW && address < IROM_HIGH) {
            log_debug("found irom section, map from %08x to %08x\n", pos,
                      section_header.load_addr);
            irom_addr = partition->offset + pos - sizeof(section_header);
            irom_load_addr = section_header.load_addr;
            irom_size = section_header.data_len + sizeof(section_header);
            load = false;
            map = true;
        }

        log_notice("section %d: paddr=0x%08x vaddr=0x%08x size=0x%05x (%6d) %s", section_index, pos, section_header.load_addr, section_header.data_len, section_header.data_len, (load)?"load":(map)?"map":"");

        if (!load) {
            pos += section_header.data_len;
            continue;
        }

        memcpy((void*) section_header.load_addr, MEM_CACHE(pos), section_header.data_len);
        pos += section_header.data_len;
    }
    
    set_cache_and_start_app(drom_addr,
        drom_load_addr,
        drom_size,
        irom_addr,
        irom_load_addr,
        irom_size,
        image_header.entry_addr);
}

void IRAM_ATTR set_cache_and_start_app(
    uint32_t drom_addr,
    uint32_t drom_load_addr,
    uint32_t drom_size,
    uint32_t irom_addr,
    uint32_t irom_load_addr,
    uint32_t irom_size, 
    uint32_t entry_addr)
{
    log_debug("configure drom and irom and start\n");
    Cache_Read_Disable( 0 );
    Cache_Read_Disable( 1 );
    Cache_Flush( 0 );
    Cache_Flush( 1 );
    uint32_t drom_page_count = (drom_size + 64*1024 - 1) / (64*1024); // round up to 64k
    log_debug( "d mmu set paddr=%08x vaddr=%08x size=%d n=%d \n", drom_addr & 0xffff0000, drom_load_addr & 0xffff0000, drom_size, drom_page_count );
    int rc = cache_flash_mmu_set( 0, 0, drom_load_addr & 0xffff0000, drom_addr & 0xffff0000, 64, drom_page_count );
    log_debug( "rc=%d", rc );
    rc = cache_flash_mmu_set( 1, 0, drom_load_addr & 0xffff0000, drom_addr & 0xffff0000, 64, drom_page_count );
    log_debug( "rc=%d", rc );
    uint32_t irom_page_count = (irom_size + 64*1024 - 1) / (64*1024); // round up to 64k
    log_debug( "i mmu set paddr=%08x vaddr=%08x size=%d n=%d\n", irom_addr & 0xffff0000, irom_load_addr & 0xffff0000, irom_size, irom_page_count );
    rc = cache_flash_mmu_set( 0, 0, irom_load_addr & 0xffff0000, irom_addr & 0xffff0000, 64, irom_page_count );
    log_debug( "rc=%d", rc );
    rc = cache_flash_mmu_set( 1, 0, irom_load_addr & 0xffff0000, irom_addr & 0xffff0000, 64, irom_page_count );
    log_debug( "rc=%d", rc );
    REG_CLR_BIT( PRO_CACHE_CTRL1_REG, (DPORT_PRO_CACHE_MASK_IRAM0) | (DPORT_PRO_CACHE_MASK_IRAM1 & 0) | (DPORT_PRO_CACHE_MASK_IROM0 & 0) | DPORT_PRO_CACHE_MASK_DROM0 | DPORT_PRO_CACHE_MASK_DRAM1 );
    REG_CLR_BIT( APP_CACHE_CTRL1_REG, (DPORT_APP_CACHE_MASK_IRAM0) | (DPORT_APP_CACHE_MASK_IRAM1 & 0) | (DPORT_APP_CACHE_MASK_IROM0 & 0) | DPORT_APP_CACHE_MASK_DROM0 | DPORT_APP_CACHE_MASK_DRAM1 );
    Cache_Read_Enable( 0 );
    Cache_Read_Enable( 1 );

    log_notice("start: 0x%08x\n", entry_addr);
    typedef void (*entry_t)(void);
    entry_t entry = ((entry_t) entry_addr);

    // TODO: we have used quite a bit of stack at this point.
    // use "movsp" instruction to reset stack back to where ROM stack starts.
    (*entry)();
}


void print_flash_info(struct flash_hdr* pfhdr)
{
#if (BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_NOTICE)

    struct flash_hdr fhdr = *pfhdr;

    log_debug( "[D]: magic %02x\n", fhdr.magic );
    log_debug( "[D]: blocks %02x\n", fhdr.blocks );
    log_debug( "[D]: spi_mode %02x\n", fhdr.spi_mode );
    log_debug( "[D]: spi_speed %02x\n", fhdr.spi_speed );
    log_debug( "[D]: spi_size %02x\n", fhdr.spi_size );

    const char* str;
    switch ( fhdr.spi_speed ) {
    case SPI_SPEED_40M:
        str = "40MHz";
        break;

    case SPI_SPEED_26M:
        str = "26.7MHz";
        break;

    case SPI_SPEED_20M:
        str = "20MHz";
        break;

    case SPI_SPEED_80M:
        str = "80MHz";
        break;

    default:
        str = "20MHz";
        break;
    }
    log_notice( "  SPI Speed      : %s", str );

    

    switch ( fhdr.spi_mode ) {
    case SPI_MODE_QIO:
        str = "QIO";
        break;

    case SPI_MODE_QOUT:
        str = "QOUT";
        break;

    case SPI_MODE_DIO:
        str = "DIO";
        break;

    case SPI_MODE_DOUT:
        str = "DOUT";
        break;

    case SPI_MODE_FAST_READ:
        str = "FAST READ";
        break;

    case SPI_MODE_SLOW_READ:
        str = "SLOW READ";
        break;
    default:
        str = "DIO";
        break;
    }
    log_notice( "  SPI Mode       : %s", str );

    

    switch ( fhdr.spi_size ) {
    case SPI_SIZE_1MB:
        str = "1MB";
        break;

    case SPI_SIZE_2MB:
        str = "2MB";
        break;

    case SPI_SIZE_4MB:
        str = "4MB";
        break;

    case SPI_SIZE_8MB:
        str = "8MB";
        break;

    case SPI_SIZE_16MB:
        str = "16MB";
        break;

    default:
        str = "1MB";
        break;
    }
    log_notice( "  SPI Flash Size : %s", str );
#endif
}
