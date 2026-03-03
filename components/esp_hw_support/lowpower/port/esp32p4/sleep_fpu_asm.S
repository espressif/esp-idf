/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "rvsleep-frames.h"
#include "freertos/FreeRTOSConfig.h"
#include "sdkconfig.h"

    .section    .tcm.text,"ax"
    .global     rv_core_fpu_save
    .type       rv_core_fpu_save,@function
    .align      4

/* Bit to set in mstatus to enable the FPU */
#define CSR_MSTATUS_FPU_ENABLE      (1 << 13)
/* Bit to clear in mstatus to disable the FPU */
#define CSR_MSTATUS_FPU_DISABLE     (3 << 13)

.macro fpu_enable reg
    li     \reg, CSR_MSTATUS_FPU_ENABLE
    csrs   mstatus, \reg
.endm

rv_core_fpu_save:
    fpu_enable t0
    mv      t0, a0                       /* t0 = frame (arg0) */
    fsw     ft0,  RV_SLP_CTX_FPU_FT0(t0)
    fsw     ft1,  RV_SLP_CTX_FPU_FT1(t0)
    fsw     ft2,  RV_SLP_CTX_FPU_FT2(t0)
    fsw     ft3,  RV_SLP_CTX_FPU_FT3(t0)
    fsw     ft4,  RV_SLP_CTX_FPU_FT4(t0)
    fsw     ft5,  RV_SLP_CTX_FPU_FT5(t0)
    fsw     ft6,  RV_SLP_CTX_FPU_FT6(t0)
    fsw     ft7,  RV_SLP_CTX_FPU_FT7(t0)
    fsw     fs0,  RV_SLP_CTX_FPU_FS0(t0)
    fsw     fs1,  RV_SLP_CTX_FPU_FS1(t0)
    fsw     fa0,  RV_SLP_CTX_FPU_FA0(t0)
    fsw     fa1,  RV_SLP_CTX_FPU_FA1(t0)
    fsw     fa2,  RV_SLP_CTX_FPU_FA2(t0)
    fsw     fa3,  RV_SLP_CTX_FPU_FA3(t0)
    fsw     fa4,  RV_SLP_CTX_FPU_FA4(t0)
    fsw     fa5,  RV_SLP_CTX_FPU_FA5(t0)
    fsw     fa6,  RV_SLP_CTX_FPU_FA6(t0)
    fsw     fa7,  RV_SLP_CTX_FPU_FA7(t0)
    fsw     fs2,  RV_SLP_CTX_FPU_FS2(t0)
    fsw     fs3,  RV_SLP_CTX_FPU_FS3(t0)
    fsw     fs4,  RV_SLP_CTX_FPU_FS4(t0)
    fsw     fs5,  RV_SLP_CTX_FPU_FS5(t0)
    fsw     fs6,  RV_SLP_CTX_FPU_FS6(t0)
    fsw     fs7,  RV_SLP_CTX_FPU_FS7(t0)
    fsw     fs8,  RV_SLP_CTX_FPU_FS8(t0)
    fsw     fs9,  RV_SLP_CTX_FPU_FS9(t0)
    fsw     fs10, RV_SLP_CTX_FPU_FS10(t0)
    fsw     fs11, RV_SLP_CTX_FPU_FS11(t0)
    fsw     ft8,  RV_SLP_CTX_FPU_FT8(t0)
    fsw     ft9,  RV_SLP_CTX_FPU_FT9(t0)
    fsw     ft10, RV_SLP_CTX_FPU_FT10(t0)
    fsw     ft11, RV_SLP_CTX_FPU_FT11(t0)
    csrr    t1, fcsr
    sw      t1, RV_SLP_CTX_FPU_FCSR(t0)
    /* Chip will go to sleep and FPU will be powered down, not necessary to disable FPU here. */
    ret

    .size   rv_core_fpu_save, . - rv_core_fpu_save

/*
--------------------------------------------------------------------------------
     FPU restore: a0 = RvCoreCriticalSleepFrame *. Restore all FP registers from the frame.
--------------------------------------------------------------------------------
*/
    .section    .iram1,"ax"
    .global     rv_core_fpu_restore
    .type       rv_core_fpu_restore,@function
    .align      4

rv_core_fpu_restore:
    fpu_enable t0
    mv      t0, a0                       /* t0 = frame (arg0) */
    flw     ft0,  RV_SLP_CTX_FPU_FT0(t0)
    flw     ft1,  RV_SLP_CTX_FPU_FT1(t0)
    flw     ft2,  RV_SLP_CTX_FPU_FT2(t0)
    flw     ft3,  RV_SLP_CTX_FPU_FT3(t0)
    flw     ft4,  RV_SLP_CTX_FPU_FT4(t0)
    flw     ft5,  RV_SLP_CTX_FPU_FT5(t0)
    flw     ft6,  RV_SLP_CTX_FPU_FT6(t0)
    flw     ft7,  RV_SLP_CTX_FPU_FT7(t0)
    flw     fs0,  RV_SLP_CTX_FPU_FS0(t0)
    flw     fs1,  RV_SLP_CTX_FPU_FS1(t0)
    flw     fa0,  RV_SLP_CTX_FPU_FA0(t0)
    flw     fa1,  RV_SLP_CTX_FPU_FA1(t0)
    flw     fa2,  RV_SLP_CTX_FPU_FA2(t0)
    flw     fa3,  RV_SLP_CTX_FPU_FA3(t0)
    flw     fa4,  RV_SLP_CTX_FPU_FA4(t0)
    flw     fa5,  RV_SLP_CTX_FPU_FA5(t0)
    flw     fa6,  RV_SLP_CTX_FPU_FA6(t0)
    flw     fa7,  RV_SLP_CTX_FPU_FA7(t0)
    flw     fs2,  RV_SLP_CTX_FPU_FS2(t0)
    flw     fs3,  RV_SLP_CTX_FPU_FS3(t0)
    flw     fs4,  RV_SLP_CTX_FPU_FS4(t0)
    flw     fs5,  RV_SLP_CTX_FPU_FS5(t0)
    flw     fs6,  RV_SLP_CTX_FPU_FS6(t0)
    flw     fs7,  RV_SLP_CTX_FPU_FS7(t0)
    flw     fs8,  RV_SLP_CTX_FPU_FS8(t0)
    flw     fs9,  RV_SLP_CTX_FPU_FS9(t0)
    flw     fs10, RV_SLP_CTX_FPU_FS10(t0)
    flw     fs11, RV_SLP_CTX_FPU_FS11(t0)
    flw     ft8,  RV_SLP_CTX_FPU_FT8(t0)
    flw     ft9,  RV_SLP_CTX_FPU_FT9(t0)
    flw     ft10, RV_SLP_CTX_FPU_FT10(t0)
    flw     ft11, RV_SLP_CTX_FPU_FT11(t0)
    lw      t1, RV_SLP_CTX_FPU_FCSR(t0)
    csrw    fcsr, t1
    /* Caller restores mstatus (and thus FPU state); not necessary to disable FPU here. */
    ret

    .size   rv_core_fpu_restore, . - rv_core_fpu_restore
