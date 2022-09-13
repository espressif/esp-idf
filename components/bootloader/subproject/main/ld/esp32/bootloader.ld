/*
Linker file used to link the bootloader.
*/


/* Simplified memory map for the bootloader

   The main purpose is to make sure the bootloader can load into main memory
   without overwriting itself.
*/

MEMORY
{
  /* IRAM POOL1, used for APP CPU cache. Bootloader runs from here during the final stage of loading the app because APP CPU is still held in reset, the main app enables APP CPU cache */
  iram_loader_seg (RWX) :           org = 0x40078000, len = 0x8000  /* 32KB, APP CPU cache */
  /* 63kB, IRAM. We skip the first 1k to prevent the entry point being
     placed into the same range as exception vectors in the app.
     This leads to idf_monitor decoding ROM bootloader "entry 0x40080xxx"
     message as one of the exception vectors, which looks scary to users.
  */
  iram_seg (RWX) :                  org = 0x40080400, len = 0xfc00
  /* 64k at the end of DRAM, after ROM bootloader stack */
  dram_seg (RW) :                  	org = 0x3FFF0000, len = 0x10000
}

/*  Default entry point:  */
ENTRY(call_start_cpu0);


SECTIONS
{

  .iram_loader.text :
  {
    . = ALIGN (16);
    _loader_text_start = ABSOLUTE(.);
    *(.stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
     *(.iram1 .iram1.*) /* catch stray IRAM_ATTR */
    *liblog.a:(.literal .text .literal.* .text.*)
    *libgcc.a:(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_clock.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_common.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_flash.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_random.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_random*.*(.literal.bootloader_random_disable .text.bootloader_random_disable)
    *libbootloader_support.a:bootloader_efuse_esp32.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_utility.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_sha.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:esp_image_format.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:flash_encrypt.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:flash_partitions.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:secure_boot.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:secure_boot_signatures.*(.literal .text .literal.* .text.*)
    *libmicro-ecc.a:*.*(.literal .text .literal.* .text.*)
    *libspi_flash.a:*.*(.literal .text .literal.* .text.*)
    *libsoc.a:wdt_hal_iram.*(.literal .text .literal.* .text.*)
    *libsoc.a:rtc_clk.*(.literal .text .literal.* .text.*)
    *libefuse.a:*.*(.literal .text .literal.* .text.*)
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)
    _loader_text_end = ABSOLUTE(.);
  } > iram_loader_seg

  .iram.text :
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
    _dram_start = ABSOLUTE(.);
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
    KEEP (*crtbegin.*(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.*) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    __init_array_end = ABSOLUTE(.);
    KEEP (*crtbegin.*(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.*) .dtors))
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
    _dram_end = ABSOLUTE(.);
  } >dram_seg

  .iram.text :
  {
    _stext = .;
    _text_start = ABSOLUTE(.);
    *(.literal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.iram .iram.*) /* catch stray IRAM_ATTR */
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)

    /** CPU will try to prefetch up to 16 bytes of
      * of instructions. This means that any configuration (e.g. MMU, PMS) must allow
      * safe access to up to 16 bytes after the last real instruction, add
      * dummy bytes to ensure this
      */
    . += 16;

    _text_end = ABSOLUTE(.);
    _etext = .;
  } > iram_seg

  /** This section will be used by the debugger and disassembler to get more information
   * about raw data present in the code.
   * Indeed, it may be required to add some padding at some points in the code
   * in order to align a branch/jump destination on a particular bound.
   * Padding these instructions will generate null bytes that shall be
   * interpreted as data, and not code by the debugger or disassembler.
   * This section will only be present in the ELF file, not in the final binary
   * For more details, check GCC-212
   */
  .xt.prop 0 :
  {
    KEEP (*(.xt.prop .gnu.linkonce.prop.*))
  }

  .xt.lit 0 :
  {
    KEEP (*(.xt.lit .gnu.linkonce.p.*))
  }

}
