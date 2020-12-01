/**
 *                    ESP32-C3 Linker Script Memory Layout
 * This file describes the memory layout (memory blocks) by virtual memory addresses.
 * This linker script is passed through the C preprocessor to include configuration options.
 * Please use preprocessor features sparingly!
 * Restrict to simple macros with numeric values, and/or #if/#endif blocks.
 */

#include "sdkconfig.h"

#define SRAM_IRAM_START     0x4037C000
#define SRAM_DRAM_START     0x3FC7C000
#define ICACHE_SIZE         0x4000 /* ICache size is fixed to 16KB on ESP32-C3 */
#define I_D_SRAM_OFFSET     (SRAM_IRAM_START - SRAM_DRAM_START)
#define SRAM_DRAM_END       0x403D0000 - I_D_SRAM_OFFSET  /* 2nd stage bootloader iram_loader_seg start address */

#define SRAM_IRAM_ORG       (SRAM_IRAM_START + ICACHE_SIZE)
#define SRAM_DRAM_ORG       (SRAM_DRAM_START + ICACHE_SIZE)

#define I_D_SRAM_SIZE       SRAM_DRAM_END - SRAM_DRAM_ORG

#if CONFIG_ESP32C3_USE_FIXED_STATIC_RAM_SIZE
ASSERT((CONFIG_ESP32C3_FIXED_STATIC_RAM_SIZE <= I_D_SRAM_SIZE), "Fixed static ram data does not fit.")
#define DRAM0_0_SEG_LEN CONFIG_ESP3C3_FIXED_STATIC_RAM_SIZE
#else
#define DRAM0_0_SEG_LEN I_D_SRAM_SIZE
#endif // CONFIG_ESP32C3_USE_FIXED_STATIC_RAM_SIZE
MEMORY
{
  /**
   *  All these values assume the flash cache is on, and have the blocks this uses subtracted from the length
   *  of the various regions. The 'data access port' dram/drom regions map to the same iram/irom regions but
   *  are connected to the data port of the CPU and eg allow byte-wise access.
   */

  /* IRAM for PRO CPU. */
  iram0_0_seg (RX) :                 org = SRAM_IRAM_ORG, len = I_D_SRAM_SIZE

#if CONFIG_APP_BUILD_USE_FLASH_SECTIONS
  /* Flash mapped instruction data */
  iram0_2_seg (RX) :                 org = 0x42000020, len = 0x8000000-0x20

  /**
   * (0x20 offset above is a convenience for the app binary image generation.
   * Flash cache has 64KB pages. The .bin file which is flashed to the chip
   * has a 0x18 byte file header, and each segment has a 0x08 byte segment
   * header. Setting this offset makes it simple to meet the flash cache MMU's
   * constraint that (paddr % 64KB == vaddr % 64KB).)
   */
#endif // CONFIG_APP_BUILD_USE_FLASH_SECTIONS

  /**
   * Shared data RAM, excluding memory reserved for ROM bss/data/stack.
   * Enabling Bluetooth & Trace Memory features in menuconfig will decrease the amount of RAM available.
   */
  dram0_0_seg (RW) :                 org = SRAM_DRAM_ORG, len = DRAM0_0_SEG_LEN

#if CONFIG_APP_BUILD_USE_FLASH_SECTIONS
  /* Flash mapped constant data */
  drom0_0_seg (R) :                  org = 0x3C000020, len = 0x8000000-0x20

  /* (See iram0_2_seg for meaning of 0x20 offset in the above.) */
#endif // CONFIG_APP_BUILD_USE_FLASH_SECTIONS

  /**
   * RTC fast memory (executable). Persists over deep sleep.
   */
  rtc_iram_seg(RWX) :                org = 0x50000000, len = 0x2000
}

#if CONFIG_ESP32C3_USE_FIXED_STATIC_RAM_SIZE
/* static data ends at defined address */
_static_data_end = 0x3FCA0000 + DRAM0_0_SEG_LEN;
#else
_static_data_end = _bss_end;
#endif // CONFIG_ESP32C3_USE_FIXED_STATIC_RAM_SIZE

/* Heap ends at top of dram0_0_seg */
_heap_end = 0x40000000;

_data_seg_org = ORIGIN(rtc_data_seg);

/**
 *  The lines below define location alias for .rtc.data section
 *  As C3 only has RTC fast memory, this is not configurable like on other targets
 */
REGION_ALIAS("rtc_data_seg", rtc_iram_seg );
REGION_ALIAS("rtc_slow_seg", rtc_iram_seg );
REGION_ALIAS("rtc_data_location", rtc_iram_seg );

#if CONFIG_APP_BUILD_USE_FLASH_SECTIONS
  REGION_ALIAS("default_code_seg", iram0_2_seg);
#else
  REGION_ALIAS("default_code_seg", iram0_0_seg);
#endif // CONFIG_APP_BUILD_USE_FLASH_SECTIONS

#if CONFIG_APP_BUILD_USE_FLASH_SECTIONS
  REGION_ALIAS("default_rodata_seg", drom0_0_seg);
#else
  REGION_ALIAS("default_rodata_seg", dram0_0_seg);
#endif // CONFIG_APP_BUILD_USE_FLASH_SECTIONS
