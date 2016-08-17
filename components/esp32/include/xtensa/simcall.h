/*
 *  simcall.h  -  Simulator call numbers
 *
 *  Software that runs on a simulated Xtensa processor using
 *  the instruction set simulator (ISS) can invoke simulator
 *  services using the SIMCALL instruction.  The a2 register
 *  is set prior to executing SIMCALL to a "simcall number",
 *  indicating which service to invoke.  This file defines the
 *  simcall numbers defined and/or supported by the Xtensa ISS.
 *
 *  IMPORTANT NOTE:  These numbers are highly subject to change!
 *
 *  Copyright (c) 2002-2007 by Tensilica Inc.  ALL RIGHTS RESERVED.
 *  These coded instructions, statements, and computer programs are the
 *  copyrighted works and confidential proprietary information of Tensilica Inc.
 *  They may not be modified, copied, reproduced, distributed, or disclosed to
 *  third parties in any manner, medium, or form, in whole or in part, without
 *  the prior written consent of Tensilica Inc.
 */

#ifndef SIMCALL_INCLUDED
#define SIMCALL_INCLUDED

/*
 *  System call like services offered by the simulator host.
 *  These are modeled after the Linux 2.4 kernel system calls
 *  for Xtensa processors.  However not all system calls and
 *  not all functionality of a given system call are implemented,
 *  or necessarily have well defined or equivalent semantics in
 *  the context of a simulation (as opposed to a Unix kernel).
 *
 *  These services behave largely as if they had been invoked
 *  as a task in the simulator host's operating system
 *  (eg. files accessed are those of the simulator host).
 *  However, these SIMCALLs model a virtual operating system
 *  so that various definitions, bit assignments etc
 *  (eg. open mode bits, errno values, etc) are independent
 *  of the host operating system used to run the simulation.
 *  Rather these definitions are specific to the Xtensa ISS.
 *  This way Xtensa ISA code written to use these SIMCALLs
 *  can (in principle) be simulated on any host.
 *
 *  Up to 6 parameters are passed in registers a3 to a8
 *  (note the 6th parameter isn't passed on the stack,
 *   unlike windowed function calling conventions).
 *  The return value is in a2.  A negative value in the
 *  range -4096 to -1 indicates a negated error code to be
 *  reported in errno with a return value of -1, otherwise
 *  the value in a2 is returned as is.
 */

/* These #defines need to match what's in Xtensa/OS/vxworks/xtiss/simcalls.c */

#define SYS_nop		0	/* n/a - setup; used to flush register windows */
#define SYS_exit	1	/*x*/
#define SYS_fork	2
#define SYS_read	3	/*x*/
#define SYS_write	4	/*x*/
#define SYS_open	5	/*x*/
#define SYS_close	6	/*x*/
#define SYS_rename	7	/*x 38 - waitpid */
#define SYS_creat	8	/*x*/
#define SYS_link	9	/*x (not implemented on WIN32) */
#define SYS_unlink	10	/*x*/
#define SYS_execv	11	/* n/a - execve */
#define SYS_execve	12	/* 11 - chdir */
#define SYS_pipe	13	/* 42 - time */
#define SYS_stat	14	/* 106 - mknod */
#define SYS_chmod	15
#define SYS_chown	16	/* 202 - lchown */
#define SYS_utime	17	/* 30 - break */
#define SYS_wait	18	/* n/a - oldstat */
#define SYS_lseek	19	/*x*/
#define SYS_getpid	20
#define SYS_isatty	21	/* n/a - mount */
#define SYS_fstat	22	/* 108 - oldumount */
#define SYS_time	23	/* 13 - setuid */
#define SYS_gettimeofday 24	/*x 78 - getuid (not implemented on WIN32) */
#define SYS_times	25	/*X 43 - stime (Xtensa-specific implementation) */
#define SYS_socket      26
#define SYS_sendto      27
#define SYS_recvfrom    28
#define SYS_select_one  29      /* not compitible select, one file descriptor at the time */
#define SYS_bind        30
#define SYS_ioctl       31

/*
 *  Other...
 */
#define SYS_iss_argc         1000 /* returns value of argc */
#define SYS_iss_argv_size    1001 /* bytes needed for argv & arg strings */
#define SYS_iss_set_argv     1002 /* saves argv & arg strings at given addr */

#define SYS_memset           1004 /* fill a range of memory (fast) */

/*
 * SIMCALLs for the ferret memory debugger. All are invoked by
 * libferret.a ...  ( Xtensa/Target-Libs/ferret )
 */
#define SYS_ferret           1010
#define SYS_malloc           1011
#define SYS_free             1012
#define SYS_more_heap        1013
#define SYS_no_heap          1014
#define SYS_enter_ferret     1015
#define SYS_leave_ferret     1016

/*
 * SIMCALLs for ISS client commands
 */
#define SYS_profile_enable   1020
#define SYS_profile_disable  1021
#define SYS_trace_level      1022
#define SYS_client_command   1023

/*
 * SIMCALL for simulation mode switching
 */
#define SYS_sim_mode_switch  1030

/*
 * SIMCALLs for XTMP/XTSC event notify and core stall
 */
#define SYS_event_fire       1040
#define SYS_event_stall      1041

/*
 *  SIMCALLs for callbacks registered in XTMP/XTSC
 */
#define SYS_callback_first      100
#define SYS_callback_last       999

/*
 * User defined simcall 
 */
#define SYS_user_simcall        100

#define SYS_xmpa_errinfo        200
#define SYS_xmpa_proc_status    201
#define SYS_xmpa_proc_start     202
#define SYS_xmpa_proc_stop      203
#define SYS_xmpa_proc_mem_read  204
#define SYS_xmpa_proc_mem_write 205
#define SYS_xmpa_proc_mem_fill  206
#define SYS_xmpa_proc_reg_read  207
#define SYS_xmpa_proc_reg_write 208


/*
 *  Extra SIMCALLs for GDB:
 */
#define SYS_gdb_break         -1	/* invoked by XTOS on user exceptions if EPC points
					   to a break.n/break, regardless of cause! */
#define SYS_xmon_out          -2	/* invoked by XMON: ... */
#define SYS_xmon_in           -3	/* invoked by XMON: ... */
#define SYS_xmon_flush        -4	/* invoked by XMON: ... */
#define SYS_gdb_abort         -5	/* invoked by XTOS in _xtos_panic() */
#define SYS_gdb_illegal_inst  -6	/* invoked by XTOS for illegal instructions (too deeply) */
#define SYS_xmon_init         -7	/* invoked by XMON: ... */
#define SYS_gdb_enter_sktloop -8	/* invoked by XTOS on debug exceptions */
#define SYS_unhandled_kernel_exc  -9	/* invoked by XTOS for unhandled kernel exceptions */
#define SYS_unhandled_user_exc   -10	/* invoked by XTOS for unhandled user exceptions */
#define SYS_unhandled_double_exc -11	/* invoked by XTOS for unhandled double exceptions */
#define SYS_unhandled_highpri_interrupt -12	/* invoked by XTOS for unhandled high-priority interrupts */
#define SYS_xmon_close        -13	/* invoked by XMON: ... */

/*
 *  SIMCALLs for vxWorks xtiss BSP:
 */
#define SYS_setup_ppp_pipes   -83
#define SYS_log_msg           -84

/*
 * SYS_select_one specifiers 
 */
#define  XTISS_SELECT_ONE_READ    1
#define  XTISS_SELECT_ONE_WRITE   2
#define  XTISS_SELECT_ONE_EXCEPT  3

/*
 * SIMCALL for client calling arbitrary code in a client plug in.
 * see clients/xcc_instr to see how this works.
 */

#define SYS_client           0xC0DECAFE



#endif /* !SIMCALL_INCLUDED */
