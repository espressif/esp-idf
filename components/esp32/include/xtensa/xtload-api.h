/* Customer ID=11656; Build=0x5f626; Copyright (c) 2003-2012 Tensilica Inc.  ALL RIGHTS RESERVED.
   These coded instructions, statements, and computer programs are the
   copyrighted works and confidential proprietary information of Tensilica Inc.
   They may not be modified, copied, reproduced, distributed, or disclosed to
   third parties in any manner, medium, or form, in whole or in part, without
   the prior written consent of Tensilica Inc.  */

#ifndef _XTLOAD_API_H
#define _XTLOAD_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XTENSA_BYTES_PER_WORD 4
#define XLOAD_ALL_CORES -1

typedef int core_number_t;
typedef uint32_t xtload_address_t;
typedef uint32_t xtload_word_count_t;
typedef uint32_t * xtload_word_ptr_t;

/* These functions correspond one-to-one with xt-load script
   commands. See the documentation for xt-load for their usage.

   There are however, several higher-level script commands--such as
   load-elf-file--which don't have direct analogues here.  These
   "missing" commands are essentially just macros that result in
   several of these commands below.  Note that you can execute several
   of these commands, then the results of a script, or vice-versa.
 */

void xtload_bootloader_wake (void);
void xtload_bootloader_sleep (void);
void xtload_bootloader_done (void);
void xtload_bootloader_not_done (void);
void xtload_reset_and_cont (core_number_t core);
void xtload_stall_and_reset (core_number_t core);
#define xtload_reset_and_stall xtload_stall_and_reset
void xtload_stall_and_target (core_number_t core);
void xtload_ignore_and_stall (core_number_t core);
void xtload_ignore_and_cont (core_number_t core);
#define xtload_ignore_and_continue xtload_ignore_and_cont
void xtload_read_words (xtload_address_t addr, xtload_word_count_t count);
void xtload_zero_words (xtload_address_t addr, xtload_word_count_t count);
void xtload_write_words (int swap, xtload_address_t addr,
			 xtload_word_ptr_t ptr, xtload_word_count_t count);
void xtload_setup_write_words (xtload_address_t addr, xtload_word_count_t count);
void xtload_read_register (core_number_t core);

/* *I M P O R T A N T*
   
   The bootloader API calls this function whenever it outputs a word
   to the bootloader hardware chain. 

   Because the API has no information about how the bootloader
   hardware is connected to the host hardware, the user must
   implement this function to write a word to the bootloader's register.

   A user's implementation might write the bytes to an Xtensa queue or
   to a memory-mapped register.

   For example, xt-load uses this API just like any other client. Its
   implementation of this function simply writes this word to an
   output file.
*/

void xtload_user_output_word (uint32_t word);

#ifdef __cplusplus
}
#endif

#endif /* _XTLOAD_API_H */

