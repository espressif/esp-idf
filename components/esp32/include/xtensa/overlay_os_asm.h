// overlay_os_asm.h -- Overlay manager assembly macros for OS use.
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


#ifndef OVERLAY_OS_ASM_H
#define OVERLAY_OS_ASM_H

// The macros in here are intended to be used by RTOS task switch code
// to check overlay status. Such code is usually in assembly and cannot
// call C code without penalty. For C code usage, it is best to use the
// corresponding C functions from the library.


// Inline assembly version of xt_overlay_get_state(). The arguments are
// three AR registers (a0-a15):
//
// "pcreg" - should contain the outgoing task's PC, i.e. the point at
//           which the task got interrupted. The return value is also
//           returned in this register.
// "sr1/2" - Scratch registers. These must be distinct from "pcreg".
//
// The return value is a 32-bit result that should be saved with the
// task context and passed as-is to xt_overlay_check_map.

        .macro _xt_overlay_get_state    pcreg  sr1  sr2

        movi    \sr1, _mapping_id
        movi    \sr2, _ovly_id
        l16si   \sr1, \sr1, 0
        l16ui   \sr2, \sr2, 0
        slli    \sr1, \sr1, 16
        or      \pcreg, \sr1, \sr2

        .endm


// Inline assembly version of xt_overlay_check_map(). It requires 5 AR
// registers (a0-a15) as arguments.
//
// "pcreg" - should contain the interrupted task's PC, i.e. the point
//           at which the task got interrupted. This will be adjusted
//           if required.
// "psreg" - should contain the interrupted task's PS. This will be
//           adjusted if required.
// "ovreg" - should contain the overlay state on entry. Contents may
//           be clobbered.
// "spreg" - should contain the tasks stack pointer on entry.
// "sr1"   - Scratch register. Must be distinct from any of the above.
//
// The return values are "pcreg" and "psreg" and these must be used
// to update the task's PC and PS.
// Note that this macro may store data below the "spreg" pointer. If
// it does, then it will also disable interrupts via the PS, so that
// the task resumes with all interrupts disabled (to avoid corrupting
// this data).
//
// (SP - 24)    Overlay ID to restore
// (SP - 28)    Task PC
// (SP - 32)    Task PS

        .macro _xt_overlay_check_map    pcreg  psreg ovreg  spreg  sr1

// There are four cases to deal with:
//
// _ovly_id = -1, _mapping_id = -1
// No overlay is mapped or mapping, nothing to do.
//
// _ovly_id >= 0, _mapping_id = -1
// An overlay was mapped, check PC to see if we need a restore.
//
// _ovly_id = -1, _mapping_id >= 0
// An overlay is being mapped. Either it belongs to this task, which
// implies that the PC is in the mapping function, or it does not 
// belong to this task. Either way there is nothing to do.
//
// _ovly_id >= 0, _mapping_id >= 0
// Illegal, cannot happen by design. Don't need to handle this.
//
// So, the logic is to check _ovly_id first. If this is >= 0, then
// we check the task PC. If the PC is in the regions of interest then
// we'll patch the return PC to invoke xt_overlay_restore.

.L1:
        extui   \sr1, \ovreg, 0, 16             // Extract _ovly_id
        bbsi.l  \sr1, 15, .Lno                  // If -1 then we're done
        mov     \ovreg, \sr1                    // Restore this one

// Next check the PC to see if it falls within the ranges of interest.

.L2:
        movi    \sr1, _overlay_vma              // Is PC < VMA range ?
        bltu    \pcreg, \sr1, .L3
        movi    \sr1, _overlay_vma_end          // Is PC > VMA range ?
        bgeu    \pcreg, \sr1, .L3
        j       .L4                             // PC is in VMA range
.L3:
        movi    \sr1, _overlay_call_stubs_start // Is PC < call stubs range ?
        bltu    \pcreg, \sr1, .Lno
        movi    \sr1, _overlay_call_stubs_end   // Is PC > call stubs range ?
        bgeu    \pcreg, \sr1, .Lno

// If we get here then a restore is needed. Save the overlay ID, PC and PS.
// Return modified PC and PS so that xt_overlay_restore() will execute in
// the context of the task when resumed. Note that the OS resumption code
// may expect PS.EXCM to be set so we leave it as is in the return value.

.L4:
        s32e    \ovreg, \spreg, -24             // Save overlay ID
        s32e    \pcreg, \spreg, -28             // Save task PC
        s32e    \psreg, \spreg, -32             // Save task PS
        movi    \pcreg, xt_overlay_restore      // Adjust resumption PC
        movi    \sr1, 15
        or      \psreg, \psreg, \sr1            // Set intlevel to highest
.Lno:

        .endm

#endif // OVERLAY_OS_ASM_H

