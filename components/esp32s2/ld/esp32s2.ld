/* ESP32S2 Linker Script Memory Layout

   This file describes the memory layout (memory blocks) by virtual memory addresses.

   This linker script is passed through the C preprocessor to include configuration options.

   Please use preprocessor features sparingly!
   Restrict to simple macros with numeric values, and/or #if/#endif blocks.
*/
#include "sdkconfig.h"

#ifdef CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC
#define ESP_BOOTLOADER_RESERVE_RTC (CONFIG_BOOTLOADER_RESERVE_RTC_SIZE + CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC_SIZE)
#elif defined(CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP)
#define ESP_BOOTLOADER_RESERVE_RTC (CONFIG_BOOTLOADER_RESERVE_RTC_SIZE)
#else
#define ESP_BOOTLOADER_RESERVE_RTC 0
#endif

#ifdef CONFIG_ESP32S2_INSTRUCTION_CACHE_8KB
#define CONFIG_ESP32S2_INSTRUCTION_CACHE_SIZE 0x2000
#else
#define CONFIG_ESP32S2_INSTRUCTION_CACHE_SIZE 0x4000
#endif

#ifdef CONFIG_ESP32S2_DATA_CACHE_0KB
#define CONFIG_ESP32S2_DATA_CACHE_SIZE 0
#elif defined CONFIG_ESP32S2_DATA_CACHE_8KB
#define CONFIG_ESP32S2_DATA_CACHE_SIZE 0x2000
#else
#define CONFIG_ESP32S2_DATA_CACHE_SIZE 0x4000
#endif

#define RAM_IRAM_START    0x40020000
#define RAM_DRAM_START    0x3FFB0000

#define DATA_RAM_END      0x3FFE0000  /* 2nd stage bootloader iram_loader_seg starts at SRAM block 14 (reclaimed after app boots) */

#define IRAM_ORG    (RAM_IRAM_START + CONFIG_ESP32S2_INSTRUCTION_CACHE_SIZE \
                                    + CONFIG_ESP32S2_DATA_CACHE_SIZE)

#define DRAM_ORG    (RAM_DRAM_START + CONFIG_ESP32S2_INSTRUCTION_CACHE_SIZE \
                                    + CONFIG_ESP32S2_DATA_CACHE_SIZE)

#define I_D_RAM_SIZE   DATA_RAM_END - DRAM_ORG

#if defined(CONFIG_ESP32S2_USE_FIXED_STATIC_RAM_SIZE)

ASSERT((CONFIG_ESP32S2_FIXED_STATIC_RAM_SIZE <= I_D_RAM_SIZE),
          "Fixed static ram data does not fit.")

#define STATIC_RAM_SIZE CONFIG_ESP32S2_FIXED_STATIC_RAM_SIZE

#else
#define STATIC_RAM_SIZE 0
#endif

MEMORY
{
  /* All these values assume the flash cache is on, and have the blocks this uses subtracted from the length
  of the various regions. The 'data access port' dram/drom regions map to the same iram/irom regions but
  are connected to the data port of the CPU and eg allow bytewise access. */

  /* IRAM for CPU.*/
  iram0_0_seg (RX) :                 org = IRAM_ORG, len = I_D_RAM_SIZE

#ifdef CONFIG_APP_BUILD_USE_FLASH_SECTIONS
  /* Even though the segment name is iram, it is actually mapped to flash
  */
  iram0_2_seg (RX) :                 org = 0x40080020, len = 0x780000-0x20

  /*
    (0x20 offset above is a convenience for the app binary image generation.
    Flash cache has 64KB pages. The .bin file which is flashed to the chip
    has a 0x18 byte file header, and each segment has a 0x08 byte segment
    header. Setting this offset makes it simple to meet the flash cache MMU's
    constraint that (paddr % 64KB == vaddr % 64KB).)
  */
#endif // CONFIG_APP_BUILD_USE_FLASH_SECTIONS


  /* Shared data RAM, excluding memory reserved for bootloader and ROM bss/data/stack. */
  dram0_0_seg (RW) :                 org = DRAM_ORG, len = I_D_RAM_SIZE - STATIC_RAM_SIZE

#ifdef CONFIG_APP_BUILD_USE_FLASH_SECTIONS
  /* Flash mapped constant data */
  drom0_0_seg (R) :                  org = 0x3F000020, len = 0x3f0000-0x20

  /* (See iram0_2_seg for meaning of 0x20 offset in the above.) */
#endif // CONFIG_APP_BUILD_USE_FLASH_SECTIONS

  /* RTC fast memory (executable). Persists over deep sleep.
   */
  rtc_iram_seg(RWX) :                org = 0x40070000, len = 0x2000

  /* RTC slow memory (data accessible). Persists over deep sleep.

     Start of RTC slow memory is reserved for ULP co-processor code + data, if enabled.
  */
  rtc_slow_seg(RW)  :                org = 0x50000000 + CONFIG_ESP32S2_ULP_COPROC_RESERVE_MEM,
                                     len = 0x2000 - CONFIG_ESP32S2_ULP_COPROC_RESERVE_MEM

  /* RTC fast memory (same block as above), viewed from data bus */
  rtc_data_seg(RW) :                 org = 0x3ff9e000, len = 0x2000 - ESP_BOOTLOADER_RESERVE_RTC
}

#if defined(CONFIG_ESP32S2_USE_FIXED_STATIC_RAM_SIZE)
/* static data ends at defined address */
_static_data_end = DRAM_ORG + STATIC_RAM_SIZE;
#else
_static_data_end = _bss_end;
#endif

_heap_end = 0x40000000;

_data_seg_org = ORIGIN(rtc_data_seg);

/* The lines below define location alias for .rtc.data section based on Kconfig option.
   When the option is not defined then use slow memory segment
   else the data will be placed in fast memory segment
   TODO: check whether the rtc_data_location is correct for esp32s2 - IDF-761 */
#ifndef CONFIG_ESP32S2_RTCDATA_IN_FAST_MEM
REGION_ALIAS("rtc_data_location", rtc_slow_seg );
#else
REGION_ALIAS("rtc_data_location", rtc_data_seg );
#endif

#ifdef CONFIG_APP_BUILD_USE_FLASH_SECTIONS
  REGION_ALIAS("default_code_seg", iram0_2_seg);
#else
  REGION_ALIAS("default_code_seg", iram0_0_seg);
#endif // CONFIG_APP_BUILD_USE_FLASH_SECTIONS

#ifdef CONFIG_APP_BUILD_USE_FLASH_SECTIONS
  REGION_ALIAS("default_rodata_seg", drom0_0_seg);
#else
  REGION_ALIAS("default_rodata_seg", dram0_0_seg);
#endif // CONFIG_APP_BUILD_USE_FLASH_SECTIONS
