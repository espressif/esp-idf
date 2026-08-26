/*
 * SPDX-FileCopyrightText: 2017 SiFive Inc.
 *
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD AND Apache-2.0
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */
/* Copyright (c) 2017  SiFive Inc. All rights reserved.

   This copyrighted material is made available to anyone wishing to use,
   modify, copy, or redistribute it subject to the terms and conditions
   of the FreeBSD License.   This program is distributed in the hope that
   it will be useful, but WITHOUT ANY WARRANTY expressed or implied,
   including the implied warranties of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  A copy of this license is available at
   http://www.opensource.org/licenses.
*/

#include <sys/asm.h>

.text
.globl strcmp
.type  strcmp, @function
strcmp:
  li    t2, -1

#if !(__riscv_misaligned_slow || __riscv_misaligned_fast)
  or    a4, a0, a1
  and   a4, a4, SZREG-1
  bnez  a4, .Lmisaligned
#endif

#if SZREG == 4
  li a5, 0x7f7f7f7f
#else
  ld a5, mask
#endif

  .macro check_one_word i n
    REG_L a2, \i*SZREG(a0)
    REG_L a3, \i*SZREG(a1)

    and   t0, a2, a5
    or    t1, a2, a5
    add   t0, t0, a5
    or    t0, t0, t1

    bne   t0, t2, .Lnull\i
    .if \i+1-\n
      bne   a2, a3, .Lmismatch
    .else
      add   a0, a0, \n*SZREG
      add   a1, a1, \n*SZREG
      beq   a2, a3, .Lloop
      # fall through to .Lmismatch
    .endif
  .endm

  .macro foundnull i n
    .ifne \i
      .Lnull\i:
      add   a0, a0, \i*SZREG
      add   a1, a1, \i*SZREG
      .ifeq \i-1
        .Lnull0:
      .endif
      bne   a2, a3, .Lmisaligned
      li    a0, 0
      ret
    .endif
  .endm

.Lloop:
  # examine full words at a time, favoring strings of a couple dozen chars
#if __riscv_xlen == 32
  check_one_word 0 5
  check_one_word 1 5
  check_one_word 2 5
  check_one_word 3 5
  check_one_word 4 5
#else
  check_one_word 0 3
  check_one_word 1 3
  check_one_word 2 3
#endif
  # backwards branch to .Lloop contained above

.Lmismatch:
  # words don't match, but a2 has no null byte.

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

#if __riscv_xlen == 64
  sll   a4, a2, 48
  sll   a5, a3, 48
  bne   a4, a5, .Lmismatch_upper
  sll   a4, a2, 32
  sll   a5, a3, 32
  bne   a4, a5, .Lmismatch_upper
#endif
  sll   a4, a2, 16
  sll   a5, a3, 16
  bne   a4, a5, .Lmismatch_upper

  srl   a4, a2, 8*SZREG-16
  srl   a5, a3, 8*SZREG-16
  sub   a0, a4, a5
  and   a1, a0, 0xff
  bnez  a1, 1f
  ret

.Lmismatch_upper:
  srl   a4, a4, 8*SZREG-16
  srl   a5, a5, 8*SZREG-16
  sub   a0, a4, a5
  and   a1, a0, 0xff
  bnez  a1, 1f
  ret

1:and   a4, a4, 0xff
  and   a5, a5, 0xff
  sub   a0, a4, a5
  ret

#else

#if __riscv_xlen == 64
  srl   a4, a2, 48
  srl   a5, a3, 48
  bne   a4, a5, .Lmismatch_lower
  srl   a4, a2, 32
  srl   a5, a3, 32
  bne   a4, a5, .Lmismatch_lower
#endif
  srl   a4, a2, 16
  srl   a5, a3, 16
  bne   a4, a5, .Lmismatch_lower

  srl	a4, a2, 8
  srl   a5, a3, 8
  bne   a4, a5, 1f
  and   a4, a2, 0xff
  and   a5, a3, 0xff
1:sub	a0, a4, a5
  ret

.Lmismatch_lower:
  srl	a2, a4, 8
  srl   a3, a5, 8
  bne   a2, a3, 1f
  and   a2, a4, 0xff
  and   a3, a5, 0xff
1:sub	a0, a2, a3
  ret

#endif

.Lmisaligned:
  # misaligned
  lbu   a2, 0(a0)
  lbu   a3, 0(a1)
  add   a0, a0, 1
  add   a1, a1, 1
  bne   a2, a3, 1f
  bnez  a2, .Lmisaligned

1:
  sub   a0, a2, a3
  ret

  # cases in which a null byte was detected
#if __riscv_xlen == 32
  foundnull 0 5
  foundnull 1 5
  foundnull 2 5
  foundnull 3 5
  foundnull 4 5
#else
  foundnull 0 3
  foundnull 1 3
  foundnull 2 3
#endif
.size	strcmp, .-strcmp

#if SZREG == 8
.section .srodata.cst8,"aM",@progbits,8
.align 3
mask:
.dword 0x7f7f7f7f7f7f7f7f
#endif

.text
.globl esp_libc_include_strcmp_impl
.type  esp_libc_include_strcmp_impl, @function
esp_libc_include_strcmp_impl:
