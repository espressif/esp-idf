/* This is only included if CONFIG_COMPILER_CXX_RTTI is not set, to
 * move RTTI sections of libstdc++ to an unused non-loadable memory region.
 */

SECTIONS
{
  .rodata.discard-rtti (NOLOAD):
  {
    *libstdc++.a:(.rodata._ZTI* .rodata._ZTS*)
  } > discard_seg
}
