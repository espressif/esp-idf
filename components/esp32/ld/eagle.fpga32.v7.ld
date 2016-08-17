/* THESE ARE THE VIRTUAL RUNTIME ADDRESSES  */
/* The load addresses are defined later using the AT statements. */
MEMORY
{
  /* All these values assume the flash cache is on, and have the blocks this uses subtracted from the length
  of the various regions. The 'data access port' dram/drom regions map to the same iram/irom regions but
  are connected to the data port of the CPU and eg allow bytewise access. */
  iram0_0_seg (RX) :                 org = 0x40080000, len = 0x18000   /* IRAM for PRO cpu. Not sure if happy with this, this is MMU area... */
  iram0_2_seg (RX) :                 org = 0x400D0018, len = 0x330000  /* Even though the segment name is iram, it is actually mapped to flash */
  dram0_0_seg (RW) :                 org = 0x3FFC0000, len = 0x20000   /* Shared RAM, minus rom bss/data/stack.*/
  drom0_0_seg (R) :                  org = 0x3F400010, len = 0x800000
}

_heap_end = 0x3fffe000;

/*  Default entry point:  */
ENTRY(call_user_start_cpu0);

SECTIONS
{
  /* Send .iram0 code to iram */
  .iram0.vectors : 
  {
    /* Vectors go to IRAM */
    _init_start = ABSOLUTE(.);
    /* Vectors according to builds/RF-2015.2-win32/esp108_v1_2_s5_512int_2/config.html */
    . = 0x0;
    KEEP(*(.WindowVectors.text));
    . = 0x180;
    KEEP(*(.Level2InterruptVector.text));
    . = 0x1c0;
    KEEP(*(.Level3InterruptVector.text));
    . = 0x200;
    KEEP(*(.Level4InterruptVector.text));
    . = 0x240;
    KEEP(*(.Level5InterruptVector.text));
    . = 0x280;
    KEEP(*(.DebugExceptionVector.text));
    . = 0x2c0;
    KEEP(*(.NMIExceptionVector.text));
    . = 0x300;
    KEEP(*(.KernelExceptionVector.text));
    . = 0x340;
    KEEP(*(.UserExceptionVector.text));
    . = 0x3C0;
    KEEP(*(.DoubleExceptionVector.text));
    . = 0x400;
    *(.*Vector.literal)

    *(.UserEnter.literal);
    *(.UserEnter.text);
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)
    _init_end = ABSOLUTE(.);
  } > iram0_0_seg

  .iram0.text :
  {
    /* Code marked as runnning out of IRAM */
    _iram_text_start = ABSOLUTE(.);
    *(.iram1 .iram1.*)
    *libfreertos.a:(.literal .text .literal.* .text.*)
    *libphy.a:(.literal .text .literal.* .text.*)
    *librtc.a:(.literal .text .literal.* .text.*)
    *libpp.a:(.literal .text .literal.* .text.*)
    *libhal.a:(.literal .text .literal.* .text.*)
    _iram_text_end = ABSOLUTE(.);
  } > iram0_0_seg

  /* Shared RAM */
  .dram0.bss (NOLOAD) :
  {
    . = ALIGN (8);
    _bss_start = ABSOLUTE(.);
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    KEEP(*(.bss))
    *(.bss.*)
    *(.share.mem)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
  } >dram0_0_seg

  .dram0.data :
  {
    _data_start = ABSOLUTE(.);
    KEEP(*(.data))
    KEEP(*(.data.*))
    KEEP(*(.gnu.linkonce.d.*))
    KEEP(*(.data1))
    KEEP(*(.sdata))
    KEEP(*(.sdata.*))
    KEEP(*(.gnu.linkonce.s.*))
    KEEP(*(.sdata2))
    KEEP(*(.sdata2.*))
    KEEP(*(.gnu.linkonce.s2.*))
    KEEP(*(.jcr))
    _data_end = ABSOLUTE(.);
    . = ALIGN(4);
    _heap_start = ABSOLUTE(.);
  } >dram0_0_seg

  .flash.rodata :
  {
    _rodata_start = ABSOLUTE(.);
    *(.rodata)
    *(.rodata.*)
	*(.irom1.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    *(.eh_frame)
    . = (. + 3) & ~ 3;
    /*  C++ constructor and destructor tables, properly ordered:  */
    __init_array_start = ABSOLUTE(.);
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    __init_array_end = ABSOLUTE(.);
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    /*  C++ exception handlers table:  */
    __XT_EXCEPTION_DESCS_ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    _rodata_end = ABSOLUTE(.);
	/* Literals are also RO data. */
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
    . = ALIGN(4);
  } >drom0_0_seg

  .flash.text :
  {
    _stext = .;
    _text_start = ABSOLUTE(.);
    *(.literal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.irom0.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)
    _text_end = ABSOLUTE(.);
    _etext = .;
  } >iram0_2_seg
}
