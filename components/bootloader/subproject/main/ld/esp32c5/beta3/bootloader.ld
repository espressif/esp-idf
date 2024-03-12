/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** Simplified memory map for the bootloader.
 *  Make sure the bootloader can load into main memory without overwriting itself.
 *
 *  TODO: [ESP32C5] IDF-9358 Check this file whether need update for MP ROM
 *  ESP32-C5 ROM static data usage is as follows:
 *  - 0x4086b2b8 - 0x4087cbc0: Shared buffers, used in UART/USB/SPI download mode only
 *  - 0x4087cbc0 - 0x4087ebc0: PRO CPU stack, can be reclaimed as heap after RTOS startup
 *  - 0x4087ebc0 - 0x40880000: ROM .bss and .data (not easily reclaimable)
 *
 *  The 2nd stage bootloader can take space up to the end of ROM shared
 *  buffers area (0x4087cbc0).
 */

/* We consider 0x4087cbc0 to be the last usable address for 2nd stage bootloader stack overhead, dram_seg,
 * and work out iram_seg and iram_loader_seg addresses from there, backwards.
 */

/* These lengths can be adjusted, if necessary: */
bootloader_usable_dram_end = 0x4087cbc0;
bootloader_stack_overhead = 0x2000; /* For safety margin between bootloader data section and startup stacks */
bootloader_dram_seg_len = 0x5000;
bootloader_iram_loader_seg_len = 0x7000;
bootloader_iram_seg_len = 0x2200;

/* Start of the lower region is determined by region size and the end of the higher region */
bootloader_dram_seg_end = bootloader_usable_dram_end - bootloader_stack_overhead;
bootloader_dram_seg_start = bootloader_dram_seg_end - bootloader_dram_seg_len;
bootloader_iram_loader_seg_start = bootloader_dram_seg_start - bootloader_iram_loader_seg_len;
bootloader_iram_seg_start = bootloader_iram_loader_seg_start - bootloader_iram_seg_len;

MEMORY
{
  iram_seg (RWX) :                  org = bootloader_iram_seg_start, len = bootloader_iram_seg_len
  iram_loader_seg (RWX) :           org = bootloader_iram_loader_seg_start, len = bootloader_iram_loader_seg_len
  dram_seg (RW) :                   org = bootloader_dram_seg_start, len = bootloader_dram_seg_len
}

/* The app may use RAM for static allocations up to the start of iram_loader_seg.
 * If you have changed something above and this assert fails:
 * 1. Check what the new value of bootloader_iram_loader_seg start is.
 * 2. Update the value in this assert.
 * 3. Update SRAM_DRAM_END in components/esp_system/ld/esp32c5/memory.ld.in to the same value.
 */
ASSERT(bootloader_iram_loader_seg_start == 0x4086EBC0, "bootloader_iram_loader_seg_start inconsistent with SRAM_DRAM_END");

/* Default entry point: */
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
    /* we use either libgcc or compiler-rt, so put similar entries for them here */
    *libgcc.a:(.literal .text .literal.* .text.*)
    *libclang_rt.builtins.a:(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_clock_loader.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_common_loader.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_flash.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_random.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_random*.*(.literal.bootloader_random_disable .text.bootloader_random_disable)
    *libbootloader_support.a:bootloader_random*.*(.literal.bootloader_random_enable .text.bootloader_random_enable)
    *libbootloader_support.a:bootloader_efuse.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_utility.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_sha.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_console_loader.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_panic.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_soc.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:esp_image_format.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:flash_encrypt.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:flash_encryption_secure_features.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:flash_partitions.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:secure_boot.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:secure_boot_secure_features.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:secure_boot_signatures_bootloader.*(.literal .text .literal.* .text.*)
    *libmicro-ecc.a:*.*(.literal .text .literal.* .text.*)
    *libspi_flash.a:*.*(.literal .text .literal.* .text.*)
    *libhal.a:wdt_hal_iram.*(.literal .text .literal.* .text.*)
    *libhal.a:mmu_hal.*(.literal .text .literal.* .text.*)
    *libhal.a:cache_hal.*(.literal .text .literal.* .text.*)
    *libhal.a:efuse_hal.*(.literal .text .literal.* .text.*)
    *libesp_hw_support.a:rtc_clk.*(.literal .text .literal.* .text.*)
    *libesp_hw_support.a:rtc_time.*(.literal .text .literal.* .text.*)
    *libesp_hw_support.a:regi2c_ctrl.*(.literal .text .literal.* .text.*)
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
  } > dram_seg

  .dram0.bootdesc : ALIGN(0x10)
  {
    _data_start = ABSOLUTE(.);
    *(.data_bootloader_desc .data_bootloader_desc.*)               /* Should be the first.  Bootloader version info.        DO NOT PUT ANYTHING BEFORE IT! */
  } > dram_seg

  .dram0.data :
  {
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)
    _data_end = ABSOLUTE(.);
  } > dram_seg

  .dram0.rodata :
  {
    _rodata_start = ABSOLUTE(.);
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    *(.sdata2 .sdata2.* .srodata .srodata.*)
    __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    *(.eh_frame)
    . = (. + 3) & ~ 3;
    /* C++ constructor and destructor tables, properly ordered: */
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
  } > dram_seg

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

}


/**
 *  Appendix: Memory Usage of ROM bootloader
 *
 * 0x4086b2b8 ------------------> _dram0_0_start
 *            |               |
 *            |               |
 *            |               |   1. Large buffers that are only used in certain boot modes, see shared_buffers.h
 *            |               |
 *            |               |
 * 0x4087cbc0 ------------------> __stack_sentry
 *            |               |
 *            |               |   2. Startup pro cpu stack (freed when IDF app is running)
 *            |               |
 * 0x4087ebc0 ------------------> __stack (pro cpu)
 *            |               |
 *            |               |
 *            |               |   3. Shared memory only used in startup code or nonos/early boot*
 *            |               |      (can be freed when IDF runs)
 *            |               |
 *            |               |
 * 0x4087fb14 ------------------> _dram0_rtos_reserved_start
 *            |               |
 *            |               |
 *            |               |   4. Shared memory used in startup code and when IDF runs
 *            |               |
 *            |               |
 * 0x4087fefc ------------------> _dram0_rtos_reserved_end
 *            |               |
 * 0x4087ffb8 ------------------> _data_start_interface
 *            |               |
 *            |               |   5. End of DRAM is the 'interface' data with constant addresses (ECO compatible)
 *            |               |
 * 0x40880000 ------------------> _data_end_interface
 */
