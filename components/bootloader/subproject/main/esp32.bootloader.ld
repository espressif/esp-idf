/*
Linker file used to link the bootloader.
*/


/* Simplified memory map for the bootloader

   The main purpose is to make sure the bootloader can load into main memory
   without overwriting itself.
*/
MEMORY
{
  /* I/O */
  dport0_seg (RW) :                 	org = 0x3FF00000, len = 0x10
  /* IRAM POOL1, used for APP CPU cache. We can abuse it in bootloader because APP CPU is still held in reset, the main app enables APP CPU cache */
  iram_seg (RWX) :           org = 0x40078000, len = 0x8000
  /* 64k at the end of DRAM, after ROM bootloader stack */
  dram_seg (RW) :                  	org = 0x3FFF0000, len = 0x10000
}

/*  Default entry point:  */
ENTRY(call_start_cpu0);


SECTIONS
{
  .iram1.text :
  {
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)
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
    *(.bss)
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
  } >dram_seg

  .dram0.data :
  {
    _data_start = ABSOLUTE(.);
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)
    _data_end = ABSOLUTE(.);
  } >dram_seg

  .dram0.rodata :
  {
    _rodata_start = ABSOLUTE(.);
    *(.rodata)
    *(.rodata.*)
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

  .iram.text :
  {
    _stext = .;
    _text_start = ABSOLUTE(.);
    *(.literal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.iram1 .iram1.*) /* catch stray IRAM_ATTR */
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)
    _text_end = ABSOLUTE(.);
    _etext = .;
  } > iram_seg

}
