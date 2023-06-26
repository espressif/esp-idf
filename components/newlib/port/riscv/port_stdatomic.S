/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#if __riscv_atomic == 1

.macro ALIGNED_PTR_2 ptr, offset
    andi    \ptr, a0, -4         // aligned ptr
    sub     \offset, a0, \ptr
    slli    \offset, \offset, 3  // offset (in bits) between ptr and aligned ptr
    li      t6, 24
    bne     \offset, t6, 1f      // do atomic operation in case var is not splited between 2 words
    lr.w    t2, (a0)             // invokes 'Load access fault!'
1:
.endm

    .global __atomic_load_2
    .type   __atomic_load_2, @function
__atomic_load_2:
    ALIGNED_PTR_2 t0, t1
    lr.w    t2, (t0)     // t2 - load atomic
    srl     t4, t2, t1
    slli    a0, t4, 0x10
    srli    a0, a0, 0x10
    ret
    .size   __atomic_load_2, . - __atomic_load_2


    .global __atomic_store_2
    .type   __atomic_store_2, @function
__atomic_store_2:
    ALIGNED_PTR_2 t0, t1
    li      t6, 0xffff
    sll     t6, t6, t1
    not     t6, t6       // t6 - bitwise mask
    sll     t5, a1, t1   // t5 - shifted new value to easy place into aligned memory
1:                       // do not change registers (t0, t1, t5, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    and     t3, t2, t6   // t3 - masked aliged memory. Atomic variable part is zeroed here
    or      t4, t5, t3   // t4 - combine desire half-word with half-word from origin aligned memory
    sc.w    t3, t4, (t0) // t3 - atomic write result (0 - success)
    bnez    t3, 1b
    ret
    .size   __atomic_store_2, . - __atomic_store_2


    .global __atomic_exchange_2
    .type   __atomic_exchange_2, @function
__atomic_exchange_2:
    ALIGNED_PTR_2 t0, t1
    li      t6, 0xffff
    sll     t6, t6, t1
    not     t6, t6       // t6 - bitwise mask
    sll     t5, a1, t1   // t5 - shifted new value to easy place into aligned memory
1:                       // do not change registers (t0, t1, t5, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    and     t3, t2, t6   // t3 - masked aliged memory. Atomic variable part is zeroed here
    or      t4, t5, t3   // t4 - combine desire half-word with half-word from origin aligned memory
    sc.w    t3, t4, (t0) // t3 - atomic write result (0 - success)
    bnez    t3, 1b
    srl     t4, t2, t1
    slli    a0, t4, 0x10
    srli    a0, a0, 0x10
    ret
    .size   __atomic_exchange_2, . - __atomic_exchange_2


    .global __atomic_compare_exchange_2
    .type   __atomic_compare_exchange_2, @function
__atomic_compare_exchange_2:
    ALIGNED_PTR_2 t0, t1
    li      t6, 0xffff0000
    srl     t6, t6, t1   // t6 - bitwise mask (0xffff0000/0x0000ffff)
    lhu     t5, (a1)
    sll     t5, t5, t1   // t5 - shifted expect value to easy compare with aligned memory
    sll     t4, a2, t1   // t4 - shifted desired value to easy place into aligned memory
1:                       // do not change registers (t0, t1, t4, t5) after this label
    not     t6, t6
    lr.w    t2, (t0)     // t2 - load atomic
    and     t3, t2, t6   // t3 - prepare half-word from aligned memory to compare with expected (t5)
    bne     t3, t5, 2f
    not     t6, t6
    and     t2, t2, t6
    or      t3, t4, t2   // t3 - combine desire half-word with half-word from origin aligned memory
    sc.w    t2, t3, (t0) // t2 - atomic write result (0 - success)
    bnez    t2, 1b
    li      a0, 1
    ret
2:
    srl     t3, t3, t1
    sh      t3, (a1)     // store atomic value into expect variable
    li      a0, 0
    ret
    .size   __atomic_compare_exchange_2, . - __atomic_compare_exchange_2


    .global __atomic_fetch_or_2
    .type   __atomic_fetch_or_2, @function
__atomic_fetch_or_2:
    ALIGNED_PTR_2 t0, t1
    sll     t2, a1, t1   // t2 - shifted value half-word.
    amoor.w t0, t2, (t0) // t0 - shifted value before atomic operation performed
    srl     t0, t0, t1
    slli    a0, t0, 0x10
    srli    a0, a0, 0x10
    ret
    .size   __atomic_fetch_or_2, . - __atomic_fetch_or_2


    .global __atomic_or_fetch_2
    .type   __atomic_or_fetch_2, @function
__atomic_or_fetch_2:
    ALIGNED_PTR_2 t0, t1
    sll     t2, a1, t1   // t2 - shifted value half-word.
    amoor.w t0, t2, (t0) // t0 - shifted value before atomic operation performed
    srl     t2, t2, t1
    slli    a0, t2, 0x10
    srli    a0, a0, 0x10
    ret
    .size   __atomic_or_fetch_2, . - __atomic_or_fetch_2


    .global __atomic_fetch_xor_2
    .type   __atomic_fetch_xor_2, @function
__atomic_fetch_xor_2:
    ALIGNED_PTR_2 t0, t1
    sll     t2, a1, t1    // t2 - shifted value half-word.
    amoxor.w t0, t2, (t0) // t0 - shifted value before atomic operation performed
    srl     t0, t0, t1
    slli    a0, t0, 0x10
    srli    a0, a0, 0x10
    ret
    .size   __atomic_fetch_xor_2, . - __atomic_fetch_xor_2


    .global __atomic_xor_fetch_2
    .type   __atomic_xor_fetch_2, @function
__atomic_xor_fetch_2:
    ALIGNED_PTR_2 t0, t1
    sll     t2, a1, t1    // t2 - shifted value half-word.
    amoxor.w t0, t2, (t0) // t0 - shifted value before atomic operation performed
    srl     t2, t2, t1
    slli    a0, t2, 0x10
    srli    a0, a0, 0x10
    ret
    .size   __atomic_xor_fetch_2, . - __atomic_xor_fetch_2


    .global __atomic_fetch_and_2
    .type   __atomic_fetch_and_2, @function
__atomic_fetch_and_2:
    ALIGNED_PTR_2 t0, t1
    li      t6, 0xffff0000  // t6 - bitwise mask
    srl     t6, t6, t1      // t6 - using to fill non-atomic bytes with 0xff in aligned memory
    sll     t2, a1, t1      // t2 - shifted value half-word.
    or      t2, t2, t6      // t2 - 0xXXXXffff or 0xffffXXXX where is value halfword
    amoand.w   t0, t2, (t0) // t0 - shifted value before atomic operation performed
    srl     t0, t0, t1
    slli    a0, t0, 0x10
    srli    a0, a0, 0x10
    ret
    .size   __atomic_fetch_and_2, . - __atomic_fetch_and_2


    .global __atomic_and_fetch_2
    .type   __atomic_and_fetch_2, @function
__atomic_and_fetch_2:
    ALIGNED_PTR_2 t0, t1
    li      t6, 0xffff0000  // t6 - bitwise mask
    srl     t6, t6, t1      // t6 - using to fill non-atomic bytes with 0xff in aligned memory
    sll     t2, a1, t1      // t2 - shifted value half-word.
    or      t2, t2, t6      // t2 - 0xXXXXffff or 0xffffXXXX where XXXX is value halfword
    amoand.w   t0, t2, (t0) // t0 - shifted value before atomic operation performed
    srl     t2, t2, t1
    slli    a0, t2, 0x10
    srli    a0, a0, 0x10
    ret
    .size   __atomic_and_fetch_2, . - __atomic_and_fetch_2


    .global __atomic_fetch_nand_2
    .type   __atomic_fetch_nand_2, @function
__atomic_fetch_nand_2:
    ALIGNED_PTR_2 t0, t1
    li      t5, 0xffff
    sll     t5, t5, t1   // t5 - bitwise mask
    not     t6, t5       // t6 - bitwise mask
1:                       // do not change registers (t0, t1, t5, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    srl     t3, t2, t1
    and     t3, t3, a1
    not     t3, t3       // t3 - atomic value to write
    sll     t3, t3, t1
    and     t4, t2, t6   // t4 - masked aliged memory. Atomic variable part is zeroed here
    or      t4, t4, t3   // t4 - combine desire byte-word with origin aligned memory
    sc.w    t4, t4, (t0) // t3 - atomic write result (0 - success)
    bnez    t4, 1b
    srl     t4, t2, t1
    slli    a0, t4, 0x10
    srli    a0, a0, 0x10
    ret
    .size   __atomic_fetch_nand_2, . - __atomic_fetch_nand_2


    .global __atomic_nand_fetch_2
    .type   __atomic_nand_fetch_2, @function
__atomic_nand_fetch_2:
    ALIGNED_PTR_2 t0, t1
    li      t5, 0xffff
    sll     t5, t5, t1   // t5 - bitwise mask
    not     t6, t5       // t6 - bitwise mask
1:                       // do not change registers (t0, t1, t5, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    srl     t3, t2, t1
    and     t3, t3, a1
    not     t3, t3       // t3 - atomic value to write
    sll     t3, t3, t1
    and     t4, t2, t6   // t4 - masked aliged memory. Atomic variable part is zeroed here
    or      t4, t4, t3   // t4 - combine desire byte-word with origin aligned memory
    sc.w    t4, t4, (t0) // t3 - atomic write result (0 - success)
    bnez    t4, 1b
    srl     t4, t2, t1
    slli    a0, t3, 0x10
    srli    a0, a0, 0x10
    ret
    .size   __atomic_nand_fetch_2, . - __atomic_nand_fetch_2


    .global __atomic_fetch_sub_2
    .type   __atomic_fetch_sub_2, @function
__atomic_fetch_sub_2:
    ALIGNED_PTR_2 t0, t1
    li      t5, 0xffff   // t5 - bitwise mask
    not     t6, t5
    srl     t6, t6, t1   // t6 - bitwise mask
1:                       // do not change registers (t0, t1, t5, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    srl     a0, t2, t1
    and     a0, a0, t5   // a0 - value in atomic before performing operation
    sub     t3, a0, a1
    and     t3, t3, t5   // t3 - value to be written to atomic
    sll     t3, t3, t1
    and     t2, t2, t6
    or      t3, t3, t2   // t3 - value to be written into aligned memory
    sc.w    t2, t3, (t0) // t2 - atomic write result (0 - success)
    bnez    t2, 1b
    ret
    .size   __atomic_fetch_sub_2, . - __atomic_fetch_sub_2


    .global __atomic_sub_fetch_2
    .type   __atomic_sub_fetch_2, @function
__atomic_sub_fetch_2:
    ALIGNED_PTR_2 t0, t1
    li      t5, 0xffff   // t5 - bitwise mask
    not     t6, t5
    srl     t6, t6, t1   // t6 - bitwise mask
1:                       // do not change registers (t0, t1, t5, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    srl     t4, t2, t1
    and     t4, t4, t5
    sub     t4, t4, a1
    and     t4, t4, t5   // t4 - value to be written to atomic
    sll     t4, t4, t1
    and     t2, t2, t6
    or      t4, t4, t2   // t4 - value to be written into aligned memory
    sc.w    t2, t4, (t0) // t2 - atomic write result (0 - success)
    bnez    t2, 1b
    srl     t4, t4, t1
    slli    a0, t4, 0x10
    srli    a0, a0, 0x10
    ret
    .size   __atomic_sub_fetch_2, . - __atomic_sub_fetch_2


    .global __atomic_fetch_add_2
    .type   __atomic_fetch_add_2, @function
__atomic_fetch_add_2:
    ALIGNED_PTR_2 t0, t1
    li      t5, 0xffff   // t5 - bitwise mask
    not     t6, t5
    srl     t6, t6, t1   // t6 - bitwise mask
1:                       // do not change registers (t0, t1, t5, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    srl     t4, t2, t1
    and     t4, t4, t5   // t4 - half-word value in atomic before performing operation
    add     t3, t4, a1
    and     t4, t4, t5   // t3 - half-word value to be written to atomic
    sll     t3, t3, t1
    and     t2, t2, t6
    or      t3, t3, t2   // t3 - value to be written into aligned memory
    sc.w    t2, t3, (t0) // t2 - atomic write result (0 - success)
    bnez    t2, 1b
    slli    a0, t4, 0x10
    srli    a0, a0, 0x10
    ret
    .size   __atomic_fetch_add_2, . - __atomic_fetch_add_2


    .global __atomic_add_fetch_2
    .type   __atomic_add_fetch_2, @function
__atomic_add_fetch_2:
    ALIGNED_PTR_2 t0, t1
    li      t5, 0xffff   // t5 - bitwise mask
    not     t6, t5
    srl     t6, t6, t1   // t6 - bitwise mask
1:                       // do not change registers (t0, t1, t5, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    srl     t4, t2, t1
    and     t4, t4, t5
    add     t4, t4, a1
    and     t4, t4, t5   // t4 - value to be written to atomic
    sll     t4, t4, t1
    and     t2, t2, t6
    or      t4, t4, t2   // t4 - value to be written into aligned memory
    sc.w    t2, t4, (t0) // t2 - atomic write result (0 - success)
    bnez    t2, 1b
    srl     t4, t4, t1
    slli    a0, t4, 0x10
    srli    a0, a0, 0x10
    ret
    .size   __atomic_add_fetch_2, . - __atomic_add_fetch_2


    .global __atomic_load_1
    .type   __atomic_load_1, @function
__atomic_load_1:
    andi    t0, a0, -4   // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3    // t1 - offset (in bits) between ptr and aligned ptr
    li      t6, 0xff
    sll     t6, t6, t1
    not     t6, t6       // t6 - bitwise mask
    lr.w    t2, (t0)     // t2 - load atomic
    srl     t4, t2, t1
    andi    a0, t4, 0xff
    ret
    .size   __atomic_load_1, . - __atomic_load_1


    .global __atomic_store_1
    .type   __atomic_store_1, @function
__atomic_store_1:
    andi    t0, a0, -4   // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3    // t1 - offset (in bits) between ptr and aligned ptr
    li      t6, 0xff
    sll     t6, t6, t1
    not     t6, t6       // t6 - bitwise mask
    sll     t5, a1, t1   // t5 - shifted new value to easy place into aligned memory
1:                       // do not change registers (t0, t1, t5, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    and     t3, t2, t6   // t3 - masked aliged memory. Atomic variable part is zeroed here
    or      t4, t5, t3   // t4 - combine desire byte-word with origin aligned memory
    sc.w    t3, t4, (t0) // t3 - atomic write result (0 - success)
    bnez    t3, 1b
    ret
    .size   __atomic_store_1, . - __atomic_store_1


    .global __atomic_exchange_1
    .type   __atomic_exchange_1, @function
__atomic_exchange_1:
    andi    t0, a0, -4   // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3    // t1 - offset (in bits) between ptr and aligned ptr
    li      t6, 0xff
    sll     t6, t6, t1
    not     t6, t6       // t6 - bitwise mask
    sll     t5, a1, t1   // t5 - shifted new value to easy place into aligned memory
1:                       // do not change registers (t0, t1, t5, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    and     t3, t2, t6   // t3 - masked aliged memory. Atomic variable part is zeroed here
    or      t4, t5, t3   // t4 - combine desire byte-word with origin aligned memory
    sc.w    t3, t4, (t0) // t3 - atomic write result (0 - success)
    bnez    t3, 1b
    srl     t4, t2, t1
    andi    a0, t4, 0xff
    ret
    .size   __atomic_exchange_1, . - __atomic_exchange_1


    .global __atomic_compare_exchange_1
    .type   __atomic_compare_exchange_1, @function
__atomic_compare_exchange_1:
    andi    t0, a0, -4   // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3    // t1 - offset (in bits) between ptr and aligned ptr
    li      t6, 0xff
    sll     t6, t6, t1
    not     t6, t6       // t6 - bitwise mask
    lbu     t5, (a1)
    sll     t5, t5, t1   // t5 - shifted expect value to easy compare with aligned memory
    sll     t4, a2, t1   // t4 - shifted desired value to easy place into aligned memory
1:                       // do not change registers (t0, t1, t4, t5) after this label
    not     t6, t6
    lr.w    t2, (t0)     // t2 - load atomic
    and     t3, t2, t6   // t3 - prepare half-word from aligned memory to compare with expected (t5)
    bne     t3, t5, 2f   // goto fail
    not     t6, t6
    and     t2, t2, t6
    or      t3, t4, t2   // t3 - combine desire half-word with half-word from origin aligned memory
    sc.w    t2, t3, (t0) // t2 - atomic write result (0 - success)
    bnez    t2, 1b       // retry
    li      a0, 1
    ret
2:
    srl     t3, t3, t1
    sb      t3, (a1)     // store atomic value into expect variable
    li      a0, 0
    ret
    .size   __atomic_compare_exchange_1, . - __atomic_compare_exchange_1


    .global __atomic_fetch_or_1
    .type   __atomic_fetch_or_1, @function
__atomic_fetch_or_1:
    andi    t0, a0, -4   // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3    // t1 - offset (in bits) between ptr and aligned ptr
    sll     t2, a1, t1   // t2 - shifted value half-word.
    amoor.w t0, t2, (t0) // t0 - shifted value before atomic operation performed
    srl     t0, t0, t1
    andi    a0, t0, 0xff
    ret
    .size   __atomic_fetch_or_1, . - __atomic_fetch_or_1


    .global __atomic_or_fetch_1
    .type   __atomic_or_fetch_1, @function
__atomic_or_fetch_1:
    andi    t0, a0, -4   // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3    // t1 - offset (in bits) between ptr and aligned ptr
    sll     t2, a1, t1   // t2 - shifted byte-word value.
    amoor.w t0, t2, (t0) // t0 - shifted value before atomic operation performed
    srl     t2, t2, t1
    andi    a0, t2, 0xff
    ret
    .size   __atomic_or_fetch_1, . - __atomic_or_fetch_1


    .global __atomic_fetch_xor_1
    .type   __atomic_fetch_xor_1, @function
__atomic_fetch_xor_1:
    andi    t0, a0, -4    // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3     // t1 - offset (in bits) between ptr and aligned ptr
    sll     t2, a1, t1    // t2 - shifted value byte-word.
    amoxor.w t0, t2, (t0) // t0 - shifted value before atomic operation performed
    srl     t0, t0, t1
    andi    a0, t0, 0xff
    ret
    .size   __atomic_fetch_xor_1, . - __atomic_fetch_xor_1


    .global __atomic_xor_fetch_1
    .type   __atomic_xor_fetch_1, @function
__atomic_xor_fetch_1:
    andi    t0, a0, -4     // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3      // t1 - offset (in bits) between ptr and aligned ptr
    sll     t2, a1, t1     // t2 - shifted value byte-word.
    amoxor.w t0, t2, (t0)  // t0 - shifted value before atomic operation performed
    srl     t2, t2, t1
    andi    a0, t2, 0xff
    ret
    .size   __atomic_xor_fetch_1, . - __atomic_xor_fetch_1


    .global __atomic_fetch_and_1
    .type   __atomic_fetch_and_1, @function
__atomic_fetch_and_1:
    andi    t0, a0, -4    // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3
    li      t6, 0xff      // t6 - bitwise mask
    sll     t6, t6, t1    // t6 - using to fill non-atomic bytes with 0xff in aligned memory
    not     t6, t6
    sll     t2, a1, t1    // t2 - shifted value byte-word.
    or      t2, t2, t6    // t2 - (0xXXffffff or 0xffXXffff ...) where XX - new value to write
    amoand.w t0, t2, (t0) // t0 - shifted value before atomic operation performed
    srl     t0, t0, t1
    andi    a0, t0, 0xff
    ret
    .size   __atomic_fetch_and_1, . - __atomic_fetch_and_1


    .global __atomic_and_fetch_1
    .type   __atomic_and_fetch_1, @function
__atomic_and_fetch_1:
    andi    t0, a0, -4    // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3
    li      t6, 0xff      // t6 - bitwise mask
    sll     t6, t6, t1    // t6 - using to fill non-atomic bytes with 0xff in aligned memory
    not     t6, t6
    sll     t2, a1, t1    // t2 - shifted value byte-word.
    or      t2, t2, t6    // t2 - (0xXXffffff or 0xffXXffff ...) where XX - new value to write
    amoand.w t0, t2, (t0) // t0 - shifted value before atomic operation performed
    srl     t2, t2, t1
    andi    a0, t2, 0xff
    ret
    .size   __atomic_and_fetch_1, . - __atomic_and_fetch_1


    .global __atomic_nand_fetch_1
    .type   __atomic_nand_fetch_1, @function
__atomic_nand_fetch_1:
    andi    t0, a0, -4   // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3    // t1 - offset (in bits) between ptr and aligned ptr
    li      t6, 0xff
    sll     t6, t6, t1
    not     t6, t6       // t6 - bitwise mask
1:                       // do not change registers (t0, t1, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    srl     t3, t2, t1
    and     t3, t3, a1
    not     t3, t3       // t3 - atomic value to write
    sll     t3, t3, t1
    and     t4, t2, t6   // t4 - masked aliged memory. Atomic variable part is zeroed here
    or      t4, t4, t3   // t4 - combine desire byte-word with origin aligned memory
    sc.w    t3, t4, (t0) // t3 - atomic write result (0 - success)
    bnez    t3, 1b
    srl     t4, t4, t1
    andi    a0, t4, 0xff
    ret
    .size   __atomic_nand_fetch_1, . - __atomic_nand_fetch_1


    .global __atomic_fetch_nand_1
    .type   __atomic_fetch_nand_1, @function
__atomic_fetch_nand_1:
    andi    t0, a0, -4   // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3    // t1 - offset (in bits) between ptr and aligned ptr
    li      t6, 0xff
    sll     t6, t6, t1
    not     t6, t6       // t6 - bitwise mask
1:                       // do not change registers (t0, t1, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    srl     t3, t2, t1
    and     t3, t3, a1
    not     t3, t3       // t3 - atomic value to write
    sll     t3, t3, t1
    and     t4, t2, t6   // t4 - masked aliged memory. Atomic variable part is zeroed here
    or      t4, t4, t3   // t4 - combine desire byte-word with origin aligned memory
    sc.w    t3, t4, (t0) // t3 - atomic write result (0 - success)
    bnez    t3, 1b
    srl     t4, t2, t1
    andi    a0, t4, 0xff
    ret
    .size   __atomic_fetch_nand_1, . - __atomic_fetch_nand_1


    .global __atomic_fetch_sub_1
    .type   __atomic_fetch_sub_1, @function
__atomic_fetch_sub_1:
    andi    t0, a0, -4   // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3    // t1 - offset (in bits) between ptr and aligned ptr
    li      t6, 0xff
    sll     t6, t6, t1
    not     t6, t6       // t6 - bitwise mask
1:                       // do not change registers (t0, t1, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    srl     t4, t2, t1
    andi    t4, t4, 0xff // t4 - value in atomic before performing operation
    sub     t3, t4, a1
    andi    t3, t3, 0xff // t3 - value to be written to atomic
    sll     t3, t3, t1
    and     t2, t2, t6
    or      t3, t3, t2   // t3 - value to be written into aligned memory
    sc.w    t2, t3, (t0) // t2 - atomic write result (0 - success)
    bnez    t2, 1b
    andi    a0, t4, 0xff
    ret
    .size   __atomic_fetch_sub_1, . - __atomic_fetch_sub_1


    .global __atomic_sub_fetch_1
    .type   __atomic_sub_fetch_1, @function
__atomic_sub_fetch_1:
    andi    t0, a0, -4   // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3    // t1 - offset (in bits) between ptr and aligned ptr
    li      t6, 0xff
    sll     t6, t6, t1
    not     t6, t6       // t6 - bitwise mask
1:                       // do not change registers (t0, t1, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    srl     t3, t2, t1
    andi    t3, t3, 0xff // t3 - value in atomic before performing operation
    sub     t3, t3, a1
    andi    t3, t3, 0xff // t3 - value to be written to atomic
    sll     t3, t3, t1
    and     t2, t2, t6
    or      t3, t3, t2   // t3 - value to be written into aligned memory
    sc.w    t2, t3, (t0) // t2 - atomic write result (0 - success)
    bnez    t2, 1b
    srl     t3, t3, t1
    andi    a0, t3, 0xff
    ret
    .size   __atomic_sub_fetch_1, . - __atomic_sub_fetch_1


    .global __atomic_fetch_add_1
    .type   __atomic_fetch_add_1, @function
__atomic_fetch_add_1:
    andi    t0, a0, -4   // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3    // t1 - offset (in bits) between ptr and aligned ptr
    li      t6, 0xff
    sll     t6, t6, t1
    not     t6, t6       // t6 - bitwise mask
1:                       // do not change registers (t0, t1, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    srl     t4, t2, t1
    andi    t4, t4, 0xff // t4 - value in atomic before performing operation
    add     t3, t4, a1
    andi    t3, t3, 0xff // t3 - value to be written to atomic
    sll     t3, t3, t1
    and     t2, t2, t6
    or      t3, t3, t2   // t3 - value to be written into aligned memory
    sc.w    t2, t3, (t0) // t2 - atomic write result (0 - success)
    bnez    t2, 1b
    andi    a0, t4, 0xff
    ret
    .size   __atomic_fetch_add_1, . - __atomic_fetch_add_1


    .global __atomic_add_fetch_1
    .type   __atomic_add_fetch_1, @function
__atomic_add_fetch_1:
    andi    t0, a0, -4   // t0 - aligned ptr
    sub     t1, a0, t0
    slli    t1, t1, 3    // t1 - offset (in bits) between ptr and aligned ptr
    li      t6, 0xff
    sll     t6, t6, t1
    not     t6, t6       // t6 - bitwise mask
1:                       // do not change registers (t0, t1, t6) after this label
    lr.w    t2, (t0)     // t2 - load atomic
    srl     t3, t2, t1
    andi    t3, t3, 0xff // t3 - value in atomic before performing operation
    add     t3, t3, a1
    andi    t3, t3, 0xff // t3 - value to be written to atomic
    sll     t3, t3, t1
    and     t2, t2, t6
    or      t3, t3, t2   // t3 - value to be written into aligned memory
    sc.w    t2, t3, (t0) // t2 - atomic write result (0 - success)
    bnez    t2, 1b
    srl     t3, t3, t1
    andi    a0, t3, 0xff
    ret
    .size   __atomic_add_fetch_1, . - __atomic_add_fetch_1

#endif // if __riscv_atomic == 1
