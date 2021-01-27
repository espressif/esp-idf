// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "riscv/rvruntime-frames.h"

/* The riscv panic handler in components/riscv/vectors.S doesn't allow the panic
   handler function to return.

   However, for the purposes of this test we want to allow the panic handler to return.

   There is functionality in vectors.S restore the CPU state, but it only
   restores when CONTEXT_SIZE registers are
   pushed onto the stack not RV_STK_FRMSZ registers

   Instead of messing with that, implement a full "restore from RvExcFrame"
   function here which restores the CPU and then
   returns from exception.

   Called as return_from_panic_handler(RvExcFrame *frame)
*/
.global return_from_panic_handler
return_from_panic_handler:
    or t0, a0, a0 /* use t0 as the working register */

    /* save general registers */
    lw ra,  RV_STK_RA(t0)
    lw sp,  RV_STK_SP(t0)
    lw gp,  RV_STK_GP(t0)
    lw tp,  RV_STK_TP(t0)
    lw s0,  RV_STK_S0(t0)
    lw s1,  RV_STK_S1(t0)
    lw a0,  RV_STK_A0(t0)
    lw a1,  RV_STK_A1(t0)
    lw a2,  RV_STK_A2(t0)
    lw a3,  RV_STK_A3(t0)
    lw a4,  RV_STK_A4(t0)
    lw a5,  RV_STK_A5(t0)
    lw a6,  RV_STK_A6(t0)
    lw a7,  RV_STK_A7(t0)
    lw s2,  RV_STK_S2(t0)
    lw s3,  RV_STK_S3(t0)
    lw s4,  RV_STK_S4(t0)
    lw s5,  RV_STK_S5(t0)
    lw s6,  RV_STK_S6(t0)
    lw s7,  RV_STK_S7(t0)
    lw s8,  RV_STK_S8(t0)
    lw s9,  RV_STK_S9(t0)
    lw s10, RV_STK_S10(t0)
    lw s11, RV_STK_S11(t0)
    lw t3,  RV_STK_T3(t0)
    lw t4,  RV_STK_T4(t0)
    lw t5,  RV_STK_T5(t0)
    lw t6,  RV_STK_T6(t0)

    lw t2,  RV_STK_MEPC(t0)
    csrw mepc, t2

    lw t1,  RV_STK_T1(t0)
    lw t2,  RV_STK_T2(t0)
    lw t0,  RV_STK_T0(t0)
    mret
