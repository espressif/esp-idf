/* ESP32 Linker Script Memory Layout

   This file describes the memory layout (memory blocks) as virtual
   memory addresses.

   esp32.common.ld contains output sections to link compiler output
   into these memory blocks.

   ***

   This linker script is passed through the C preprocessor to include
   configuration options.

   Please use preprocessor features sparingly! Restrict
   to simple macros with numeric values, and/or #if/#endif blocks.
*/
#include "sdkconfig.h"

MEMORY
{
  /* All these values assume the flash cache is on, and have the blocks this uses subtracted from the length
  of the various regions. The 'data access port' dram/drom regions map to the same iram/irom regions but
  are connected to the data port of the CPU and eg allow bytewise access. */

  /* IRAM for PRO cpu. Not sure if happy with this, this is MMU area... */
  iram0_0_seg (RX) :                 org = 0x40080000, len = 0x20000

  /* Even though the segment name is iram, it is actually mapped to flash */
  iram0_2_seg (RX) :                 org = 0x400D0018, len = 0x330000

  /* Shared data RAM, excluding memory reserved for ROM bss/data/stack.

     Enabling Bluetooth & Trace Memory features in menuconfig will decrease
     the amount of RAM available.
  */
  dram0_0_seg (RW) :                 org = 0x3FFB0000 + CONFIG_BT_RESERVE_DRAM,
                                     len = 0x50000 - CONFIG_TRACEMEM_RESERVE_DRAM - CONFIG_BT_RESERVE_DRAM

  /* Flash mapped constant data */
  drom0_0_seg (R) :                  org = 0x3F400010, len = 0x800000

  /* RTC fast memory (executable). Persists over deep sleep.
   */
  rtc_iram_seg(RWX) :                org = 0x400C0000, len = 0x2000

  /* RTC slow memory (data accessible). Persists over deep sleep.

     Start of RTC slow memory is reserved for ULP co-processor code + data, if enabled.
  */
  rtc_slow_seg(RW)  :                org = 0x50000000 + CONFIG_ULP_COPROC_RESERVE_MEM,
                                     len = 0x1000 - CONFIG_ULP_COPROC_RESERVE_MEM
}

/* Heap ends at top of dram0_0_seg */
_heap_end = 0x40000000 - CONFIG_TRACEMEM_RESERVE_DRAM;
