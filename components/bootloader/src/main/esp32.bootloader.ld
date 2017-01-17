/*
Linker file used to link the bootloader.

*WARNING* For now this linker dumps everything into IRAM/DRAM. ToDo: move
some/most stuff to DROM/IROM.

*/


/* THESE ARE THE VIRTUAL RUNTIME ADDRESSES  */
/* The load addresses are defined later using the AT statements. */
MEMORY
{
  /* All these values assume the flash cache is on, and have the blocks this uses subtracted from the length
  of the various regions. The 'data access port' dram/drom regions map to the same iram/irom regions but
  are connected to the data port of the CPU and eg allow bytewise access. */
  dport0_seg (RW) :                 	org = 0x3FF00000, len = 0x10		/* IO */
  iram_seg (RWX) :                 	org = 0x40080000, len = 0x400		/* 1k of IRAM used by bootloader functions which need to flush/enable APP CPU cache */ 
  iram_pool_1_seg (RWX) :           org = 0x40078000, len = 0x8000    /* IRAM POOL1, used for APP CPU cache. We can abuse it in bootloader because APP CPU is still held in reset, until we enable APP CPU cache */
  dram_seg (RW) :                  	org = 0x3FFF0000, len = 0x10000		/* 64k at the end of DRAM, after ROM bootloader stack */
}

/*  Default entry point:  */
ENTRY(call_start_cpu0);


SECTIONS
{
  .iram1.text :
  {
    _init_start = ABSOLUTE(.);
    *(.UserEnter.literal);
    *(.UserEnter.text);
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)
    _init_end = ABSOLUTE(.);

	  /* Code marked as runnning out of IRAM */
	  _iram_text_start = ABSOLUTE(.);
    *(.iram1 .iram1.*)
	  _iram_text_end = ABSOLUTE(.);
  } > iram_seg


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
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
  } >dram_seg


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
  } >dram_seg




  .dram0.rodata :
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
    _heap_start = ABSOLUTE(.);
  } >dram_seg

  .iram_pool_1.text :
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
  } >iram_pool_1_seg

}
