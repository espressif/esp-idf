/* This section is only included if CONFIG_SPIRAM_ALLOW_NOINIT_EXTERNAL_MEMORY
   is set, to link some NOINIT sections in PSRAM */

SECTIONS
{
   /* external memory bss, from any global variable with EXT_RAM_NOINIT_ATTR attribute*/
  .ext_ram.noinit (NOLOAD) :
  {
    _ext_ram_noinit_start = ABSOLUTE(.);
    *(.ext_ram.noinit*)
    . = ALIGN(4);
    _ext_ram_noinit_end = ABSOLUTE(.);
  } > extern_ram_seg
}
