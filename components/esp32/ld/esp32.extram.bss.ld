/* This section is only included if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
   is set, to link some sections to BSS in PSRAM */

SECTIONS
{
   /* external memory bss, from any global variable with EXT_RAM_ATTR attribute*/
  .ext_ram.bss (NOLOAD) :
  {
    _ext_ram_bss_start = ABSOLUTE(.);
    *(.ext_ram.bss*)
    *libnet80211.a:(.dynsbss .sbss .sbss.* .gnu.linkonce.sb.* .scommon .sbss2.* .gnu.linkonce.sb2.* .dynbss .bss .bss.* .share.mem .gnu.linkonce.b.* COMMON)
    *libpp.a:(.dynsbss .sbss .sbss.* .gnu.linkonce.sb.* .scommon .sbss2.* .gnu.linkonce.sb2.* .dynbss .bss .bss.* .share.mem .gnu.linkonce.b.* COMMON)
    *liblwip.a:(.dynsbss .sbss .sbss.* .gnu.linkonce.sb.* .scommon .sbss2.* .gnu.linkonce.sb2.* .dynbss .bss .bss.* .share.mem .gnu.linkonce.b.* COMMON)
    *libbt.a:(EXCLUDE_FILE (libbtdm_app.a) .dynsbss .sbss .sbss.* .gnu.linkonce.sb.* .scommon .sbss2.* .gnu.linkonce.sb2.* .dynbss .bss .bss.* .share.mem .gnu.linkonce.b.* COMMON)
    . = ALIGN(4);
    _ext_ram_bss_end = ABSOLUTE(.);
  } > extern_ram_seg
}
