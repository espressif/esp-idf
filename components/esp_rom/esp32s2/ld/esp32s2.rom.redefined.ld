/* 
   ROM Functions defined in this file are not used in ESP-IDF as is,
   and different definitions for functions with the same names are provided.
   This file is not used when linking ESP-IDF and is intended for reference only
*/

PROVIDE ( abort = 0x400151e0 );
PROVIDE ( ets_timer_arm = 0x4000d76c );
PROVIDE ( ets_timer_arm_us = 0x4000d7b0 );
PROVIDE ( ets_timer_disarm = 0x4000d7f0 );
PROVIDE ( ets_timer_done = 0x4000d82c );
PROVIDE ( ets_timer_handler_isr = 0x4000d858 );
PROVIDE ( ets_timer_init = 0x4000d8ec );
PROVIDE ( ets_timer_setfn = 0x4000d754 );

PROVIDE ( _KernelExceptionVector = 0x40000300 );
PROVIDE ( _KernelExceptionVector_text_end = 0x40000306 );
PROVIDE ( _KernelExceptionVector_text_start = 0x40000300 );
PROVIDE ( _NMIExceptionVector = 0x400002c0 );
PROVIDE ( _NMIExceptionVector_text_end = 0x400002c3 );
PROVIDE ( _NMIExceptionVector_text_start = 0x400002c0 );
PROVIDE ( _UserExceptionVector = 0x40000340 );
PROVIDE ( _UserExceptionVector_text_end = 0x40000357 );
PROVIDE ( _UserExceptionVector_text_start = 0x40000340 );
PROVIDE ( _DebugExceptionVector = 0x40000280 );
PROVIDE ( _DebugExceptionVector_text_end = 0x4000028b );
PROVIDE ( _DebugExceptionVector_text_start = 0x40000280 );
PROVIDE ( _DoubleExceptionVector = 0x400003c0 );
PROVIDE ( _DoubleExceptionVector_text_end = 0x400003c6 );
PROVIDE ( _DoubleExceptionVector_text_start = 0x400003c0 );
PROVIDE ( _Level2FromVector = 0x400078d4 );
PROVIDE ( _Level2HandlerLabel = 0x00000000 );
PROVIDE ( _Level2InterruptVector_text_end = 0x40000186 );
PROVIDE ( _Level2InterruptVector_text_start = 0x40000180 );
PROVIDE ( _Level2Vector = 0x40000180 );
PROVIDE ( _Level3FromVector = 0x40007970 );
PROVIDE ( _Level3HandlerLabel = 0x00000000 );
PROVIDE ( _Level3InterruptVector_text_end = 0x400001c6 );
PROVIDE ( _Level3InterruptVector_text_start = 0x400001c0 );
PROVIDE ( _Level3Vector = 0x400001c0 );
PROVIDE ( _Level4FromVector = 0x40007a08 );
PROVIDE ( _Level4HandlerLabel = 0x00000000 );
PROVIDE ( _Level4InterruptVector_text_end = 0x40000206 );
PROVIDE ( _Level4InterruptVector_text_start = 0x40000200 );
PROVIDE ( _Level4Vector = 0x40000200 );
PROVIDE ( _Level5FromVector = 0x40007b38 );
PROVIDE ( _Level5HandlerLabel = 0x00000000 );
PROVIDE ( _Level5InterruptVector_text_end = 0x40000246 );
PROVIDE ( _Level5InterruptVector_text_start = 0x40000240 );
PROVIDE ( _Level5Vector = 0x40000240 );
PROVIDE ( _LevelOneInterrupt = 0x400077e6 );
PROVIDE ( _ResetHandler = 0x4000044c );
PROVIDE ( _ResetVector = 0x40000400 );
PROVIDE ( _ResetVector_literal_end = 0x40000540 );
PROVIDE ( _ResetVector_literal_start = 0x40000540 );
PROVIDE ( _ResetVector_text_end = 0x4000053d );
PROVIDE ( _ResetVector_text_start = 0x40000400 );
PROVIDE ( _SyscallException = 0x40007706 );
PROVIDE ( _WindowOverflow12 = 0x40000100 );
PROVIDE ( _WindowOverflow4 = 0x40000000 );
PROVIDE ( _WindowOverflow8 = 0x40000080 );
PROVIDE ( _WindowUnderflow12 = 0x40000140 );
PROVIDE ( _WindowUnderflow4 = 0x40000040 );
PROVIDE ( _WindowUnderflow8 = 0x400000c0 );
PROVIDE ( _WindowVectors_text_end = 0x40000170 );
PROVIDE ( _WindowVectors_text_start = 0x40000000 );

PROVIDE ( hmac_md5 = 0x4000586c );
PROVIDE ( hmac_md5_vector = 0x4000577c );
PROVIDE ( MD5Final = 0x400056e8 );
PROVIDE ( MD5Init = 0x40005648 );
PROVIDE ( MD5Update = 0x40005668 );
PROVIDE ( md5_vector = 0x40005750 );

PROVIDE ( _xtos_alloca_handler = 0x40000010 );
PROVIDE ( xtos_cause3_handler = 0x4000774c );
PROVIDE ( xtos_c_handler_table = 0x3fffcea8 );
PROVIDE ( xtos_c_wrapper_handler = 0x4000775c );
PROVIDE ( _xtos_enabled = 0x3fffcfb0 );
PROVIDE ( xtos_exc_handler_table = 0x3fffcda8 );
PROVIDE ( xtos_interrupt_mask_table = 0x3fffd0b8 );
PROVIDE ( xtos_interrupt_table = 0x3fffcfb8 );
PROVIDE ( _xtos_ints_off = 0x4001560c );
PROVIDE ( _xtos_ints_on = 0x400155e8 );
PROVIDE ( _xtos_intstruct = 0x3fffcfb0 );
PROVIDE ( _xtos_l1int_handler = 0x400077c8 );
PROVIDE ( xtos_p_none = 0x40015acc );
PROVIDE ( _xtos_restore_intlevel = 0x400078a8 );
PROVIDE ( _xtos_return_from_exc = 0x40015ad4 );
PROVIDE ( _xtos_set_exception_handler = 0x40007690 );
PROVIDE ( _xtos_set_interrupt_handler = 0x400155ac );
PROVIDE ( _xtos_set_interrupt_handler_arg = 0x40015570 );
PROVIDE ( _xtos_set_intlevel = 0x40015aec );
PROVIDE ( _xtos_set_min_intlevel = 0x40015b08 );
PROVIDE ( _xtos_set_vpri = 0x400078b4 );
PROVIDE ( _xtos_syscall_handler = 0x400076d8 );
PROVIDE ( xtos_unhandled_exception = 0x40015b2c );
PROVIDE ( xtos_unhandled_interrupt = 0x40015b3c );
PROVIDE ( _xtos_vectors_ref_ = 0x00000000 );
PROVIDE ( _xtos_vpri_enabled = 0x3fffcfb4 );

PROVIDE ( setlocale = 0x40002020 );
PROVIDE ( _setlocale_r = 0x40001fb8 );
