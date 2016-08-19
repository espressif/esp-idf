/* THESE ARE THE VIRTUAL RUNTIME ADDRESSES  */
/* The load addresses are defined later using the AT statements. */
MEMORY
{
  /* All these values assume the flash cache is on, and have the blocks this uses subtracted from the length
  of the various regions. The 'data access port' dram/drom regions map to the same iram/irom regions but
  are connected to the data port of the CPU and eg allow bytewise access. */
  iram0_0_seg (RX) :                 org = 0x40080000, len = 0x20000   /* IRAM for PRO cpu. Not sure if happy with this, this is MMU area... */
  iram0_2_seg (RX) :                 org = 0x400D0018, len = 0x330000  /* Even though the segment name is iram, it is actually mapped to flash */
  dram0_0_seg (RW) :                 org = 0x3FFC0000, len = 0x38000   /* Shared RAM, minus rom bss/data/stack.*/
  drom0_0_seg (R) :                  org = 0x3F400010, len = 0x800000
}

_heap_end = 0x3FFF8000;
