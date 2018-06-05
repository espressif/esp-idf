// overlay.h -- Overlay manager header file
// $Id$

// Copyright (c) 2013 Tensilica Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#ifndef OVERLAY_H
#define OVERLAY_H


#include <xtensa/xtruntime.h>


#ifdef __cplusplus
extern "C" {
#endif

// Define this to turn off overlay support
#ifdef XT_DISABLE_OVERLAYS

#define OVERLAY(n)
#define DECLARE_OVERLAY(n)

#define xt_overlay_map(ov_id)
#define xt_overlay_map_async(ov_id)                 0
#define xt_overlay_map_in_progress()                0
#define xt_overlay_get_id()                         0
#define xt_overlay_get_state(pc)                    0
#define xt_overlay_check_map(pc,ps,ovstate,sp)      0

#else

// Shorthand for convenience and portability.
#define OVERLAY(n)  __attribute__((overlay(n)))

// Structure of the overlay table required by gdb and the overlay
// manager. Should not be accessed by user code unless overriding
// the load process.
struct ovly_table {
    void *        vma;    // The overlay's mapped address.
    unsigned int  size;   // The size of the overlay, in bytes.
    void *        lma;    // The overlay's load address.
    unsigned int  mapped; // Non-zero if overlay is currently mapped; zero otherwise.
};

// Constructed by the linker. Required for gdb and for the overlay
// manager. Should not be accessed by user code unless overriding
// the load process.
extern struct ovly_table _ovly_table[];

// Functions.
void xt_overlay_map(int ov_id);
int  xt_overlay_map_async(int ov_id);
int  xt_overlay_map_in_progress(void);
unsigned int xt_overlay_get_state(unsigned int pc);
unsigned int xt_overlay_check_map(unsigned int * pc, unsigned int * ps,
                                  unsigned int ovstate, unsigned int sp);
int  xt_overlay_start_map(void * dst, void * src, unsigned int len, int ov_id);
int  xt_overlay_is_mapping(int ov_id);
void xt_overlay_fatal_error(int ov_id);


// Returns the current overlay ID. If no overlay is mapped or an overlay
// is in the middle of being mapped, returns -1. Inlined to avoid calling
// out of overlay (wastes cycles, can end up reading wrong ID on interrupt
// activity).
//
static inline int xt_overlay_get_id(void)
{
#pragma always_inline
extern short _mapping_id;
extern short _ovly_id;

    int ret;
    unsigned int flags = XTOS_SET_INTLEVEL(15);

    if (_mapping_id >= 0) {
        ret = -1;
    }
    else {
        ret = _ovly_id;
    }

    XTOS_RESTORE_INTLEVEL(flags);
    return ret;
}


// The following macros are used to declare numbered overlays and generate
// the corresponding call stubs. Use as follows:
//
//    DECLARE_OVERLAY(n)
//
// See documentation for more details.

//#include <xtensa/config/core-isa.h>

// At this time overlays are not supported without windowing.
#if defined(__XTENSA_WINDOWED_ABI__)

#define xstr(x)   str(x)
#define str(x)    #x

// At entry, register a8 holds the return address and a9 holds the target
// function address. This stub saves a8 on the stack at (SP - 20) which
// is the only location that is safe for us to use. Then it allocates 32
// bytes on the stack for working storage, loads the overlay number into
// a8, and jumps to the common handler. The common handler will make sure
// that the called function is loaded into memory before calling it.
// NOTE: we are using the stack area normally reserved for nested functions.
// This means nested functions cannot be used when overlays are in use.

#define CALL_IN(num) \
    asm(".section .gnu.linkonce.t.overlay.call." xstr(num) ".text, \"ax\"\n" \
        ".global  _overlay_call_in_" xstr(num)  "_\n" \
        ".align   4\n" \
        "_overlay_call_in_" xstr(num) "_:\n" \
        "s32e    a8, a1, -20\n" \
        "addi    a8, a1, -32\n" \
        "movsp   a1, a8\n" \
        "movi    a8, " xstr(num) "\n" \
        "j       _overlay_call_in_common\n" \
        ".size   _overlay_call_in_" xstr(num) "_, . - _overlay_call_in_" xstr(num) "_\n");

// The call-out stub first calls the target function, then loads the overlay
// number into register a14 and jumps to the common handler. The handler will
// make sure that the caller function is present in memory before returning.
// Note that registers a10-a13 may contain return values so must be preserved.
//
// Because we came here via a call4, the return address is in a4, and the top
// 2 bits are set to the window increment. We'll restore the top 2 bits of
// the return address from the called function's address, assuming that both
// are in the same 1 GB segment. For now this is always true.

#define CALL_OUT(num) \
    asm(".section .gnu.linkonce.t.overlay.call." xstr(num) ".text, \"ax\"\n" \
        ".global  _overlay_call_out_" xstr(num) "_\n" \
        ".align   4\n" \
        "_overlay_call_out_" xstr(num) "_:\n" \
        "slli    a4, a4, 2\n" \
        "srli    a4, a4, 2\n" \
        "extui   a8, a9, 30, 2\n" \
        "slli    a8, a8, 30\n" \
        "or      a4, a4, a8\n" \
        "callx8  a9\n" \
        "movi    a14, " xstr(num) "\n" \
        "j       _overlay_call_out_common\n" \
        ".size   _overlay_call_out_" xstr(num) "_, . - _overlay_call_out_" xstr(num) "_\n");

// Generate a call-in and a call-out stub for each overlay.

#define DECLARE_OVERLAY(num) \
    CALL_IN(num) \
    CALL_OUT(num)

#endif // defined(__XTENSA_WINDOWED_ABI__)

#endif // XT_DISABLE_OVERLAYS

#ifdef __cplusplus
}
#endif

#endif // OVERLAY_H

