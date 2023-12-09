/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file DWARF Exception Frames parser
 *
 * This file performs parsing and execution of DWARF except frames described in
 * section `.eh_frame` and `.eh_frame_hdr`. This is currently used on RISC-V
 * boards to implement a complete backtracing when a panic occurs.
 *
 * More information about the sections structure and DWARF instructions can be
 * found in the official documentation:
 * http://dwarfstd.org/Download.php
 */
#include "sdkconfig.h"
#include <string.h>

#if CONFIG_ESP_SYSTEM_USE_EH_FRAME

#include "libunwind.h"
#include "esp_private/panic_internal.h"
#include "esp_private/eh_frame_parser.h"

#if UNW_UNKNOWN_TARGET
    #error "Unsupported architecture for unwinding"
#endif

/**
 * @brief Dimension of an array (number of elements)
 */
#ifndef DIM
#define DIM(array) (sizeof(array)/sizeof(*array))
#endif

/**
 * @brief DWARF Exception Header Encoding
 * This is used to know how the data in .eh_frame and .eh_frame_hdr sections
 * are encoded.
 */
/* DWARF Exception Exception Header value format. */
#define DW_EH_PE_omit	 0xff /*!< No value is present */
#define DW_EH_PE_uleb128 0x01 /*!< Unsigned value encoded in LEB128 (Little Endian Base 128). */
#define DW_EH_PE_udata2	 0x02 /*!< Unsigned 16-bit value. */
#define DW_EH_PE_udata4	 0x03 /*!< Unsigned 32-bit value. */
#define DW_EH_PE_udata8	 0x04 /*!< Unsigned 64-bit value. */
#define DW_EH_PE_sleb128 0x09 /*!< Signed value encoded in LEB128 (Little Endian Base 128). */
#define DW_EH_PE_sdata2	 0x0A /*!< Signed 16-bit value. */
#define DW_EH_PE_sdata4	 0x0B /*!< Signed 32-bit value. */
#define DW_EH_PE_sdata8	 0x0C /*!< Signed 64-bit value. */

/* DWARF Exception Exception Header value application.
 * These values are in fact represented in the high nibble of a given data.
 * For example:
 * 0x3A describes the values as signed 16-bit offsets relative to .eh_frame_hdr section.
 * 0x11 describes the values as unsigned value encoded in LEB128, relative to their location ion memory. */
#define DW_EH_PE_absptr  0x00 /*!< The value itself is a pointer, it is not an offset. */
#define DW_EH_PE_pcrel   0x01 /*!< The value is an offset, relative to its location in memory.  */
#define DW_EH_PE_datarel 0x03 /*!< The value is an offset, relative to .eh_frame_hdr section. */

/* Macros simplifying testing relative offset data encoding. */
#define ESP_ENCODING_PC_REL(ENCODING)        (((ENCODING >> 4) & 0xf) == DW_EH_PE_pcrel)
#define ESP_ENCODING_FRAME_HDR_REL(ENCODING) (((ENCODING >> 4) & 0xf) == DW_EH_PE_datarel)

/**
 * @brief Call Frame Information (CIE) fields information.
 * As the size of CIE is variable, the simplest way to described it is to
 * have a pointer at the beginning of CIE structure and access the fields
 * thanks to the index macros defined here.
 */
#define ESP_CIE_VARIABLE_FIELDS_IDX (9)  /*!< Offset, in bytes, where variable length fields start. */

/**
 * @brief Frame Description Entry (FDE) fields index.
 * For the same reasons as above, we prefer defining these macros rather than
 * having a structure.
 */
#define ESP_FDE_LENGTH_IDX          (0)  /*!< Length, in bytes, of the FDE excluding this field. 4 bytes field. */
#define ESP_FDE_CIE_IDX             (1)  /*!< Nearest preceding Common Information Entry (CIE) offset. 4 bytes field. */
#define ESP_FDE_INITLOC_IDX         (2)  /*!< Initial location (of the function) the FDE describes. Variable size (encoding in CIE). */
#define ESP_FDE_RANGELEN_IDX        (3)  /*!< Size, in bytes, of the function described by this FDE location the FDE describes. Variable size (encoding in CIE). */
#define ESP_FDE_AUGMENTATION_IDX    (4)  /*!< Augmentation data length. Unsigned LEB128. */

/**
 * @brief Pointers to both .eh_frame_hdr and .eh_frame sections.
 */
#define EH_FRAME_HDR_ADDR   (&__eh_frame_hdr)
#define EH_FRAME_ADDR       (&__eh_frame)

/**
 * @brief Structure of .eh_frame_hdr section header.
 */
typedef struct {
    uint8_t version; /*!< Structure version, must be 1.*/
    uint8_t eh_frame_ptr_enc; /*!< eh_frame_ptr entry encoding. */
    uint8_t fde_count_enc;  /*!< fde_count entry encoding. */
    uint8_t table_enc;  /*!< table entries encoding. */
    /* The rest of the structure has variable length. Thus, we cannot define
     * them here. Here are their names:
     * - eh_frame_ptr : encoded pointer to the .eh_frame section.
     * - fde_Count : number of entries in the array of table_entry.
     * - table_entry array : sorted array of table_entry. */
} __attribute__((packed)) fde_header;

/**
 * @brief .eh_frame_hdr table's entry format.
 * Each entry of the table contains 2 32-bit encoded addresses.
 * Encoding is defined in the previous structure. Check table_enc field.
 */
typedef struct {
    uint32_t fun_addr; /*!< Address of the function described. */
    uint32_t fde_addr; /*!< Address of the FDE for the function.*/
} table_entry;

/**
 * @brief DWARF state constant macros.
 */
#define ESP_EH_FRAME_STACK_SIZE (2) /*!< DWARF virtual machine can save the push the current on a virtual
                                         stack. we mimic the stack with an array. While testing, a stack
                                         size of 2 was enough. */

/**
 * @brief
 * Structure representing the state of the DWARF virtual machine.
 */
typedef struct {
    /* Stack for DWARF state registers.
     * For caller saved registers, save their CFA address (value in previous call frame).
     * As these registers will be used to define offset in the CFA, they will always be
     * multiple of CPU word (4-bytes in our case). Thus, it will save the offset in word-size, not
     * in bytes. Plus, the highest bit will be used to mark that this register is NOY
     * ESP_EH_FRAME_REG_SAME. (0x80000000 is a valid value then, meaning that the register value
     * is CFA + 0 offset) */
    uint32_t regs_offset[ESP_EH_FRAME_STACK_SIZE][EXECUTION_FRAME_MAX_REGS];
    /* reg_offset represents the state of registers when PC reaches the following location. */
    uint32_t location;
    /* Index of the registers offset to use (1 for saved offset, 0 else). */
    uint8_t offset_idx;
} dwarf_regs;

/**
 * @brief DWARF's register state.
 * When a DWARF register is set to ESP_EH_FRAME_REG_SAME, the CPU register corresponding to this
 * virtual register will be unchanged after executing DWARF instructions.
 * Please see esp_eh_frame_restore_caller_state() for more details.
 */
#define ESP_EH_FRAME_REG_SAME (0)

/**
 * @brief Set a register's offset (relative to CFA).
 * The highest bit is set to 1 to mark that this register needs to be retrived because it has been
 * altered.
 */
#define ESP_EH_FRAME_SET_REG_OFFSET(offset)     (0x80000000 | offset)

/**
 * @brief Get a register's offset (relative to CFA).
 */
#define ESP_EH_FRAME_GET_REG_OFFSET(offset)     (0x7fffffff & offset)

/**
 * @brief Get a register's CFA offset.
 */
#define ESP_EH_FRAME_IS_CFA_RELATIVE(reg)       ((reg >> 31) == 1)

/**
 * @brief Test whether an offset is small enough to be stored
 * in our 32-bit register.
 * Note: the highest bit is used.
 */
#define ESP_EH_FRAME_CFA_OFFSET_VALID(offset)   (offset < 0x80000000)

/**
 * @brief Index of Call Frame Address (CFA) in DWARF registers array.
 */
#define ESP_ESH_FRAME_CFA_IDX                   (EXECUTION_FRAME_SP_REG)

/**
 * @brief Macros to get and set CFA's relative register and offset.
 * Indeed, CFA is defined by two values: register and offset. CFA is then
 * calculated by adding the offset to the register value.
 * `register` will be stored in the lowest 8 bits.
 * `offset` will be stored in the highest 24 bits.
 *
 * NOTE: with this implementation, CFA will be affected by
 * DW_CFA_REMEMBER_STATE and DW_CFA_RESTORE_STATE instructions.
 */
#if EXECUTION_FRAME_MAX_REGS > 255
    #error "Too many registers defined for the target ExecutionFrame"
#endif
#define ESP_EH_FRAME_CFA_REG_VALID(reg)         (reg < EXECUTION_FRAME_MAX_REGS)
#define ESP_EH_FRAME_CFA_OFF_VALID(off)         (((off) >> 24) == 0)
#define ESP_EH_FRAME_CFA(state)                 ((state)->regs_offset[(state)->offset_idx][ESP_ESH_FRAME_CFA_IDX])

#define ESP_EH_FRAME_NEW_CFA(reg, off)          (((off) << 8) | ((reg) & 0xff))
#define ESP_EH_FRAME_SET_CFA_REG(value, reg)    (((value) & ~0xff) | ((reg) & 0xff))
#define ESP_EH_FRAME_SET_CFA_OFF(value, off)    (((value) & 0xff) | ((off) << 8))
#define ESP_EH_FRAME_GET_CFA_REG(value)         ((value) & 0xff)
#define ESP_EH_FRAME_GET_CFA_OFF(value)         ((value) >> 8)


/**
 * @brief Unsupported opcode value to return when exeucting 0-opcode type instructions.
 */
#define ESP_EH_FRAME_UNSUPPORTED_OPCODE ((uint32_t) -1)

/**
 * @brief Macros defining the DWARF instructions code.
 */
#define DW_GET_OPCODE(OP)       ((OP) >> 6)
#define DW_GET_PARAM(OP)        ((OP) & 0b111111)
#define DW_CFA_ADVANCE_LOC      (1)
#define DW_CFA_OFFSET           (2)
#define DW_CFA_RESTORE          (3)
/**
 * @brief Constant for DWARF instructions code when high 2 bits are 0.
 */
#define DW_CFA_0_OPCODE             (0)
#define DW_CFA_NOP                  (0x0)
#define DW_CFA_SET_LOC              (0x1)
#define DW_CFA_ADVANCE_LOC1         (0x2)
#define DW_CFA_ADVANCE_LOC2         (0x3)
#define DW_CFA_ADVANCE_LOC4         (0x4)
#define DW_CFA_OFFSET_EXTENDED      (0x5)
#define DW_CFA_RESTORE_EXTENDED     (0x6)
#define DW_CFA_UNDEFINED            (0x7)
#define DW_CFA_SAME_VALUE           (0x8)
#define DW_CFA_REGISTER             (0x9)
#define DW_CFA_REMEMBER_STATE       (0xA)
#define DW_CFA_RESTORE_STATE        (0xB)
#define DW_CFA_DEF_CFA              (0xC)
#define DW_CFA_DEF_CFA_REGISTER     (0xD)
#define DW_CFA_DEF_CFA_OFFSET       (0xE)
#define DW_CFA_DEF_CFA_EXPRESSION   (0xF)
#define DW_CFA_EXPRESSION           (0x10)
#define DW_CFA_OFFSET_EXTENDED_SF   (0x11)
#define DW_CFA_DEF_CFA_SF           (0x12)
#define DW_CFA_DEF_CFA_OFFSET_SF    (0x13)
#define DW_CFA_VAL_OFFSET           (0x14)
#define DW_CFA_VAL_OFFSET_SF        (0x15)
#define DW_CFA_VAL_EXPRESSION       (0x16)
#define DW_CFA_LO_USER              (0x1C)

/**
 * @brief Constants used for decoding (U)LEB128 integers.
 */
#define DW_LEB128_HIGHEST_BIT(byte) (((byte) >> 7) & 1)
#define DW_LEB128_SIGN_BIT(byte)    (((byte) >> 6) & 1)
#define DW_LEB128_MAX_SHIFT         (31)


/**
 * @brief Symbols defined by the linker.
 * Retrieve the addresses of both .eh_frame_hdr and .eh_frame sections.
 */
extern void *__eh_frame_hdr;
extern void *__eh_frame;

/**
 * @brief Decode multiple bytes encoded in LEB128.
 *
 * @param bytes bytes encoded in LEB128. They will not be modified.
 * @param is_signed true if bytes represent a signed value, false else.
 * @param size Size in bytes of the encoded value.
 *
 * @return Decoded bytes.
 */
static uint32_t decode_leb128(const uint8_t* bytes, bool is_signed, uint32_t* lebsize)
{
    uint32_t res = 0;
    uint32_t shf = 0;
    uint32_t size = 0;
    uint8_t byte = 0;

    while(1) {
        byte = bytes[size++];
        res |= (byte & 0x7f) << shf;
        shf += 7;
        if (DW_LEB128_HIGHEST_BIT(byte) == 0)
            break;
    }

    if (is_signed && shf <= DW_LEB128_MAX_SHIFT && DW_LEB128_SIGN_BIT(byte)) {
        res |= ((uint32_t) ~0 << shf);
    }

    if (lebsize) {
        *lebsize = size;
    }

    return res;
}

/**
 * @brief Get the value of data encoded.
 *
 * @param data Pointer to the encoded data.
 * @param encoding Encoding for the data to read.
 * @param psize Reference to be filled with data size, in bytes.
 *
 * @return Decoded data read from the pointer.
 */
static uint32_t esp_eh_frame_get_encoded(void* data, uint8_t encoding, uint32_t* psize)
{
    int32_t  svalue = 0;
    uint32_t uvalue = 0;
    uint32_t fvalue = 0;
    uint32_t size = 0;
    const uint32_t high = encoding >> 4;
    const uint32_t low  = encoding & 0xf;

    assert(psize != NULL);

    if (encoding == DW_EH_PE_omit) {
        *psize = size;
        return uvalue;
    }

    switch (low) {
        case DW_EH_PE_udata2:
            size = 2;
            uvalue = *((uint16_t*) data);
            break;
        case DW_EH_PE_udata4:
            size = 4;
            uvalue = *((uint32_t*) data);
            break;
        case DW_EH_PE_sdata2:
            size = 2;
            svalue = *((int16_t*) data);
            break;
        case DW_EH_PE_sdata4:
            size = 4;
            svalue = *((int32_t*) data);
            break;
        default:
            /* Unsupported yet. */
            assert(false);
            break;
    }

    switch (high) {
        case DW_EH_PE_absptr:
            /* Do not change the values, as one of them will be 0, fvalue will
             * contain the data no matter whether it is signed or unsigned. */
            fvalue = svalue + uvalue;
            break;
        case DW_EH_PE_pcrel:
            /* Relative to the address of the data.
             * svalue has been casted to an 32-bit value, so even if it was a
             * 2-byte signed value, fvalue will be calculated correctly here. */
            fvalue = (uint32_t) data + svalue + uvalue;
            break;
        case DW_EH_PE_datarel:
            fvalue = (uint32_t) EH_FRAME_HDR_ADDR + svalue + uvalue;
            break;
    }

    *psize = size;
    return fvalue;
}

/**
 * @brief Find entry in the table for the given return_address.
 *
 * @param sorted_table Pointer to the sorted table of entries.
 * @param length Number of entries in the table.
 * @param encoding Encoding for the addresses in the table
 *                 (Check DWARF documentation for more info about encoding).
 * @param return_address The address to find in the table. This address can be
 * part of one in the function listed.
 *
 * @note The table is structured like this (after decoding the addresses):
 *  Function address    FDE address   Index
 * +-------------------------------+
 * |0x403805a4           0x4038d014|    0
 * +-------------------------------+
 * |0x403805be           0x4038d034|    1
 * +-------------------------------+
 * |0x403805d8           0x4038d070|    2
 * +-------------------------------+
 * |..........           ..........|   ...
 * +-------------------------------+
 * |0x42020c48           0x4038ddb4| length-3
 * +-------------------------------+
 * |0x42020dca           0x4038dde4| length-2
  *+-------------------------------+
 * |0x42020f92           0x4038debc| length-1
 * +-------------------------------+
 *
 * For example, if return_address passed is 0x403805b4, this function will
 * return a pointer to the entry (0x403805a4, 0x4038d014).
 *
 * @return Pointer to the entry found, NULL if not found.
 */
static const table_entry* esp_eh_frame_find_entry(const table_entry* sorted_table,
                                                  const uint32_t length,
                                                  const uint32_t encoding,
                                                  const uint32_t return_address)
{
    int32_t ra = 0;

    /* Used for decoding addresses in the table. */
    uint32_t is_signed = (encoding & 0xf) >= 0x9;
    uint32_t pc_relative = true;

    /* The following local variables are used for dichotomic search. */
    uint32_t found = false;
    uint32_t begin = 0;
    uint32_t end = length;
    uint32_t middle = (end + begin) / 2;

    /* If the addresses in the table are offsets relative to the eh_frame section,
    * instead of decoding each of them, we can simply encode the return_address
    * we have to find. If addresses are offsets relative to the programe counter,
    * then we have no other choice than decoding each of them to compare them
    * with return_address. */
    if (ESP_ENCODING_FRAME_HDR_REL(encoding)) {
        ra = return_address - (uint32_t) EH_FRAME_HDR_ADDR;
        pc_relative = false;
    }

    /* Perform dichotomic search. */
    while (end != 0 && middle != (length - 1) && !found) {
        const uint32_t fun_addr = sorted_table[middle].fun_addr;
        const uint32_t nxt_addr = sorted_table[middle + 1].fun_addr;

        if (pc_relative) {
            ra = return_address - (uint32_t) (sorted_table + middle);
        }

        if (is_signed) {
            /* Signed comparisons. */
            const int32_t sfun_addr = (int32_t) fun_addr;
            const int32_t snxt_addr = (int32_t) nxt_addr;
            if (sfun_addr <= ra && snxt_addr > ra)
                found = true;
            else if (snxt_addr <= ra)
                begin = middle + 1;
            else
                end = middle;

        } else {
            /* Unsigned comparisons. */
            const uint32_t ura = (uint32_t) ra;
            if (fun_addr <= ura && nxt_addr > ura)
                found = true;
            else if (nxt_addr <= ura)
                begin = middle + 1;
            else
                end = middle;
        }

        middle = (end + begin) / 2;
    }

    /* If 'end' reached the beginning of the array, it means the return_address
     * passed was below the first address of the array, thus, it was wrong.
     * Else, return the address found. */
    return (end == 0) ? 0 : sorted_table + middle;
}

/**
 * @brief Decode an address according to the encoding passed.
 *
 * @param addr Pointer to the address to decode.
 *             This pointer's value MUST be an address in .eh_frame_hdr section.
 * @param encoding DWARF encoding byte.
 *
 * @return address dedoded (e.g. absolute address)
 */
static inline uint32_t* esp_eh_frame_decode_address(const uint32_t* addr,
                                                    const uint32_t encoding)
{
    uint32_t* decoded = 0;

    if (ESP_ENCODING_FRAME_HDR_REL(encoding))
        decoded = (uint32_t*) (*addr + (uint32_t) EH_FRAME_HDR_ADDR);
    else if (ESP_ENCODING_PC_REL(encoding))
        decoded = (uint32_t*) (*addr + (uint32_t) addr);
    else
        decoded = (uint32_t*) (*addr);

    return decoded;
}

/**
 * @brief Execute the DWARF instruction which high 2 bits are 0.
 *
 * @param opcode low 6 bits of the instruction code.
 * @param operands pointer to the possible operands.
 * @param state state of the DWARF machine. Its registers may be modified.
 *
 * @return Number of operands used for executing the instruction.
 */
static inline uint32_t esp_eh_frame_execute_opcode_0(const uint32_t opcode, const uint8_t* operands,
                                                     dwarf_regs* state)
{
    uint32_t operand1 = 0;
    uint32_t used_operands = 0;
    uint32_t operand2 = 0;
    uint32_t used_operands2 = 0;

    switch(opcode) {
        case DW_CFA_NOP:
            break;
        case DW_CFA_ADVANCE_LOC1:
            /* Advance location with a 1-byte delta. */
            used_operands = 1;
            state->location += *operands;
            break;
        case DW_CFA_ADVANCE_LOC2:
            /* Advance location with a 2-byte delta. */
            used_operands = 2;
            state->location += *((const uint16_t*) operands);
            break;
        case DW_CFA_ADVANCE_LOC4:
            /* Advance location with a 4-byte delta. */
            used_operands = 4;
            state->location += *((const uint32_t*) operands);
            break;
        case DW_CFA_REMEMBER_STATE:
            assert(state->offset_idx == 0);
            memcpy(state->regs_offset[1], state->regs_offset[0],
                   EXECUTION_FRAME_MAX_REGS * sizeof(uint32_t));
            state->offset_idx++;
            break;
        case DW_CFA_RESTORE_STATE:
            assert(state->offset_idx == 1);
            /* Drop the saved state. */
            state->offset_idx--;
            break;
        case DW_CFA_DEF_CFA:
            /* CFA changes according to a register and an offset.
             * This instruction appears when the assembly code saves the
             * SP in the middle of a routine, before modifying it.
             * For example (on RISC-V):
             * addi s0, sp, 80
             * addi sp, sp, -10
             * ... */
            /* Operand1 is the register containing the CFA value. */
            operand1 = decode_leb128(operands, false, &used_operands);
            /* Offset for the register's value. */
            operand2 = decode_leb128(operands + used_operands, false, &used_operands2);
            /* Calculate the number of bytes  */
            used_operands += used_operands2;
            /* Assert that the register and the offset are valid. */
            assert(ESP_EH_FRAME_CFA_REG_VALID(operand1));
            assert(ESP_EH_FRAME_CFA_OFF_VALID(operand2));
            ESP_EH_FRAME_CFA(state) = ESP_EH_FRAME_NEW_CFA(operand1, operand2);
            break;
        case DW_CFA_DEF_CFA_REGISTER:
            /* Define the register of the current frame address (CFA).
             * Its operand is in the next bytes, its type is ULEB128. */
            operand1 = decode_leb128(operands, false, &used_operands);
            /* Check whether the value is valid or not. */
            assert(ESP_EH_FRAME_CFA_OFF_VALID(operand1));
            /* Offset will be unchanged, only register changes. */
            ESP_EH_FRAME_CFA(state) = ESP_EH_FRAME_SET_CFA_REG(ESP_EH_FRAME_CFA(state), operand1);
            break;
        case DW_CFA_DEF_CFA_OFFSET:
            /* Same as above but for the offset. The register of CFA remains unchanged. */
            operand1 = decode_leb128(operands, false, &used_operands);
            assert(ESP_EH_FRAME_CFA_OFF_VALID(operand1));
            ESP_EH_FRAME_CFA(state) = ESP_EH_FRAME_SET_CFA_OFF(ESP_EH_FRAME_CFA(state), operand1);
            break;
        default:
            panic_print_str("\r\nUnsupported DWARF opcode 0: 0x");
            panic_print_hex(opcode);
            panic_print_str("\r\n");
            used_operands = ESP_EH_FRAME_UNSUPPORTED_OPCODE;
            break;
    }

    return used_operands;
}


/**
 * @brief Execute DWARF instructions.
 *
 * @param instructions Array of instructions to execute.
 * @param instructions_length Length of the array of instructions.
 * @param frame Execution frame of the crashed task. This will only be used to
 *              get the PC where the task crashed.
 * @param state DWARF machine state. The registers contained in the state will
 *              modified accordingly to the instructions.
 *
 * @return true if the execution went fine, false if an unsupported instruction was met.
 */
static bool esp_eh_frame_execute(const uint8_t* instructions, const uint32_t instructions_length,
                                 const ExecutionFrame* frame, dwarf_regs* state)
{
    for (uint32_t i = 0; i < instructions_length; i++) {
        const uint8_t instr = instructions[i];
        const uint8_t param = DW_GET_PARAM(instr);
        uint32_t operand1 = 0;
        uint32_t size = 0;
        uint32_t used_operands = 0;

        /* Decode the instructions. According to DWARF documentation, there are three
         * types of Call Frame Instructions. The upper 2 bits defines the type. */
        switch (DW_GET_OPCODE(instr)) {
            case DW_CFA_0_OPCODE:
                used_operands = esp_eh_frame_execute_opcode_0(param, &instructions[i + 1], state);
                /* Exit the function if an unsupported opcode was met. */
                if (used_operands == ESP_EH_FRAME_UNSUPPORTED_OPCODE) {
                    return false;
                }
                i += used_operands;
                break;
            case DW_CFA_ADVANCE_LOC:
                /* Move the location forward. This instruction will mark when to stop:
                 * once we reach the instruction where the PC left, we can break out of the loop
                 * The delta is part of the lowest 6 bits.
                 */
                state->location += param;
                break;
            case DW_CFA_OFFSET:
                operand1 = decode_leb128(&instructions[i + 1], false, &size);
                assert(ESP_EH_FRAME_CFA_OFFSET_VALID(operand1));
                state->regs_offset[state->offset_idx][param] = ESP_EH_FRAME_SET_REG_OFFSET(operand1);
                i += size;
                break;

            case DW_CFA_RESTORE:
                state->regs_offset[state->offset_idx][param] = ESP_EH_FRAME_REG_SAME;
                break;
            default:
                /* Illegal opcode */
                assert(false);
                break;
        }

        /* As the state->location can also be modified by 0-opcode instructions (in the function)
         * and also because we need to break the loop (and not only the switch), let's put this
         * check here, after the execution of the instruction, outside of the switch block. */
        if (state->location >= EXECUTION_FRAME_PC(*frame))
            break;
    }

    /* Everything went fine, no unsupported opcode was met, return true. */
    return true;
}

/**
 * @brief Initialize the DWARF registers state by parsing and executing CIE instructions.
 *
 * @param cie Pointer to the CIE data.
 * @param frame Pointer to the execution frame.
 * @param state DWARF machine state (DWARF registers).
 *
 * @return index of the DWARF register containing the return address.
 */
static uint32_t esp_eh_frame_initialize_state(const uint8_t* cie, ExecutionFrame* frame, dwarf_regs* state)
{
    char c = 0;
    uint32_t size = 0;

    /* The first word in the CIE structure is the length of the structure,
     * excluding this field itself. */
    const uint32_t length = ((uint32_t*) cie)[0];

    /* ID of the CIE, should be 0 for .eh_frame (which is our case) */
    const uint32_t id = ((uint32_t*) cie)[1];
    assert(id == 0);

    /* Ignore CIE version (1 byte). */

    /* The following data in the structure have variable length as they are
     * encoded in (U)LEB128. Thus, let's use a byte pointer to parse them. */
    uint8_t* cie_data = (uint8_t*) cie + ESP_CIE_VARIABLE_FIELDS_IDX;

    /* Next field is a null-terminated UTF-8 string. Ignore it, look for the end. */
    while((c = *cie_data++) != 0);

    /* Field alignment factor shall be 1. It is encoded in ULEB128. */
    const uint32_t code_align = decode_leb128(cie_data, false, &size);
    assert(code_align == 1);
    /* Jump to the next field */
    cie_data += size;

    /* Same goes for data alignment factor. Shall be equal to -4. */
    const int32_t data_align = decode_leb128(cie_data, true, &size);
    cie_data += size;
    assert(data_align == -4);

    /* Field describing the index of the DWARF register which will contain
     * the return address. */
    const uint32_t ra_reg = decode_leb128(cie_data, false, &size);
    cie_data += size;

    /* Augmentation data length is encoded in ULEB128. It represents the,
     * length of the augmentation data. Jump after it to retrieve the
     * instructions to execute. */
    const uint32_t augmentation_len = decode_leb128(cie_data, false, &size);
    cie_data += size + augmentation_len;

    /* Calculate the instructions length in order to prevent any out of bounds
     * bug. Subtract the offset of this field (minus sizeof(uint32_t) because
     * `length` field is not part of the structure length) to the total length
     * of the structure. */
    const uint32_t instructions_length = length - (cie_data - sizeof(uint32_t) - cie);

    /* Execute the instructions contained in CIE structure. Their goal is to
     * initialize the DWARF registers. Usually it binds the CFA (virtual stack
     * pointer), to its hardware equivalent. It will also bind a hardware
     * register to the virtual return address register. For example, x86
     * doesn't have a return address register, the address to return to
     * it stored on the stack when `call` instruction is used. DWARF will
     * use `eip` (instruction pointer, a.k.a. program counter) as a
     * register containing the return address register. */
    esp_eh_frame_execute(cie_data, instructions_length, frame, state);

    return ra_reg;
}

/**
 * @brief Modify the execution frame and DWARF VM state for restoring caller's context.
 *
 * @param fde Pointer to the Frame Description Entry for the current program counter (defined by frame's MEPC register)
 * @param frame Snapshot of the CPU registers when the CPU stopped its normal execution.
 * @param state DWARF VM registers.
 *
 * @return Return Address of the current context. Frame has been restored to the previous context
 * (before calling the function program counter is currently going throught).
 */
static uint32_t esp_eh_frame_restore_caller_state(const uint32_t* fde,
                                                  ExecutionFrame* frame,
                                                  dwarf_regs* state)
{
    /* Length of the whole Frame Description Entry (FDE), excluding this field. */
    const uint32_t length = fde[ESP_FDE_LENGTH_IDX];

    /* The addresses in FDE are relative to the location of each field.
     * Thus, to get the absolute address of the function it is pointing to,
     * we have to compute:
     * fun_addr = &fde[IDX] +/- fde[IDX]
     */
    const uint8_t* cie = (uint8_t*) ((uint32_t) &fde[ESP_FDE_CIE_IDX] - fde[ESP_FDE_CIE_IDX]);
    const uint32_t initial_location = ((uint32_t) &fde[ESP_FDE_INITLOC_IDX] + fde[ESP_FDE_INITLOC_IDX]);
    const uint32_t range_length = fde[ESP_FDE_RANGELEN_IDX];
    const uint8_t augmentation = *((uint8_t*) (fde + ESP_FDE_AUGMENTATION_IDX));

    /* The length, in byte, of the instructions is the size of the FDE header minus
     * the above fields' length. */
    const uint32_t instructions_length = length - 3 * sizeof(uint32_t) - sizeof(uint8_t);
    const uint8_t* instructions = ((uint8_t*) (fde + ESP_FDE_AUGMENTATION_IDX)) + 1;

    /* Make sure this FDE is the correct one for the PC given. */
    assert(initial_location <= EXECUTION_FRAME_PC(*frame) &&
           EXECUTION_FRAME_PC(*frame) < initial_location + range_length);

    /* Augmentation not supported. */
    assert(augmentation == 0);

    /* Initialize the DWARF state by executing the CIE's instructions. */
    const uint32_t ra_reg = esp_eh_frame_initialize_state(cie, frame, state);
    state->location = initial_location;

    /**
     * Execute the DWARf instructions is order to create rules that will be executed later to retrieve
     * the registers former value.
     */
    bool success = esp_eh_frame_execute(instructions, instructions_length, frame, state);
    if (!success) {
        /* An error occured (unsupported opcode), return PC as the return address.
         * This will be tested by the caller, and the backtrace will be finished. */
        return EXECUTION_FRAME_PC(*frame);
    }

    /* Execute the rules calculated previously. Start with the CFA. */
    const uint32_t cfa_val = ESP_EH_FRAME_CFA(state);
    const uint32_t cfa_reg = ESP_EH_FRAME_GET_CFA_REG(cfa_val);
    const uint32_t cfa_off = ESP_EH_FRAME_GET_CFA_OFF(cfa_val);
    const uint32_t cfa_addr = EXECUTION_FRAME_REG(frame, cfa_reg) + cfa_off;

    /* Restore the registers that need to be restored. */
    for (uint32_t i = 0; i < DIM(state->regs_offset[0]); i++) {
        uint32_t value_addr = state->regs_offset[state->offset_idx][i];
        /* Check that the value changed and that we are not treating the CFA register (if it is part of the array). */
        if (i != ESP_ESH_FRAME_CFA_IDX && value_addr != ESP_EH_FRAME_REG_SAME) {
            /* value_addr contains a description of how to find its address:
             * it has an offset relative to the CFA, which will point to the actual former value.
             * In fact, the register's previous value (in the context of the caller) is on the stack,
             * this is what value_addr will point to. */
            value_addr = cfa_addr - ESP_EH_FRAME_GET_REG_OFFSET(value_addr) * sizeof(uint32_t);
            EXECUTION_FRAME_REG(frame, i) = *((uint32_t*) value_addr);
        }
    }

    /* Restore the stack pointer according to DWARF CFA register. */
    EXECUTION_FRAME_SP(*frame) = cfa_addr;

    /* If the frame was not available, it would be possible to retrieve the return address
     * register thanks to CIE structure.
     * The return address points to the address the PC needs to jump to. It
     * does NOT point to the instruction where the routine call occured.
     * This can cause problems with functions without epilogue (i.e. function
     * which last instruction is a function call). This happens when compiler
     * optimization are ON or when a function is marked as "noreturn".
     *
     * Thus, in order to point to the call/jal instruction, we need to
     * subtract at least 1 byte but not more than an instruction size.
     */
    return EXECUTION_FRAME_REG(frame, ra_reg) - 2;
}

/**
 * @brief Test whether the DWARF information for the given PC are missing or not.
 *
 * @param fde FDE associated to this PC. This FDE is the one found thanks to
 *            `esp_eh_frame_find_entry()`.
 * @param pc PC to get information from.
 *
 * @return true is DWARF information are missing, false else.
 */
static bool esp_eh_frame_missing_info(const uint32_t* fde, uint32_t pc) {
    if (fde == NULL) {
        return true;
    }

    /* Get the range of this FDE entry. It is possible that there are some
     * gaps between DWARF entries, in that case, the FDE entry found has
     * indeed an initial_location very close to PC but doesn't reach it.
     * For example, if FDE initial_location is 0x40300000 and its length is
     * 0x100, but PC value is 0x40300200, then some DWARF information
     * are missing as there is a gap.
     * End the backtrace. */
    const uint32_t initial_location = ((uint32_t) &fde[ESP_FDE_INITLOC_IDX] + fde[ESP_FDE_INITLOC_IDX]);
    const uint32_t range_length = fde[ESP_FDE_RANGELEN_IDX];

    return (initial_location + range_length) <= pc;
}

/**
 * @brief When one step of the backtrace is generated, output it to the serial.
 * This function can be overriden as it is defined as weak.
 *
 * @param pc Program counter of the backtrace step.
 * @param sp Stack pointer of the backtrace step.
 */
void __attribute__((weak)) esp_eh_frame_generated_step(uint32_t pc, uint32_t sp)
{
    panic_print_str(" 0x");
    panic_print_hex(pc);
    panic_print_str(":0x");
    panic_print_hex(sp);
}

/**
 * @brief Print backtrace for the given execution frame.
 *
 * @param frame_or Snapshot of the CPU registers when the CPU stopped its normal execution.
 */
void esp_eh_frame_print_backtrace(const void *frame_or)
{
    assert(frame_or != NULL);

    static dwarf_regs state = { 0 };
    ExecutionFrame frame = *((ExecutionFrame*) frame_or);
    uint32_t size = 0;
    uint8_t* enc_values = NULL;
    bool end_of_backtrace = false;

    /* Start parsing the .eh_frame_hdr section. */
    fde_header* header = (fde_header*) EH_FRAME_HDR_ADDR;
    assert(header->version == 1);

    /* Make enc_values point to the end of the structure, where the encoded
     * values start. */
    enc_values = (uint8_t*) (header + 1);

    /* Retrieve the encoded value eh_frame_ptr. Get the size of the data also. */
    const uint32_t eh_frame_ptr = esp_eh_frame_get_encoded(enc_values, header->eh_frame_ptr_enc, &size);
    assert(eh_frame_ptr == (uint32_t) EH_FRAME_ADDR);
    enc_values += size;

    /* Same for the number of entries in the sorted table. */
    const uint32_t fde_count = esp_eh_frame_get_encoded(enc_values, header->fde_count_enc, &size);
    enc_values += size;

    /* enc_values points now at the beginning of the sorted table. */
    /* Only support 4-byte entries. */
    const uint32_t table_enc = header->table_enc;
    assert(((table_enc >> 4) == 0x3) || ((table_enc >> 4) == 0xB));

    const table_entry* sorted_table = (const table_entry*) enc_values;

    panic_print_str("Backtrace:");
    while (!end_of_backtrace) {

        /* Output one step of the backtrace. */
        esp_eh_frame_generated_step(EXECUTION_FRAME_PC(frame), EXECUTION_FRAME_SP(frame));

        const table_entry* from_fun = esp_eh_frame_find_entry(sorted_table, fde_count,
                                                              table_enc, EXECUTION_FRAME_PC(frame));

        /* Get absolute address of FDE entry describing the function where PC left of. */
        uint32_t* fde = NULL;

        if (from_fun != NULL) {
            fde = esp_eh_frame_decode_address(&from_fun->fde_addr, table_enc);
        }

        if (esp_eh_frame_missing_info(fde, EXECUTION_FRAME_PC(frame))) {
            /* Address was not found in the list. */
            panic_print_str("\r\nBacktrace ended abruptly: cannot find DWARF information for"
                            " instruction at address 0x");
            panic_print_hex(EXECUTION_FRAME_PC(frame));
            panic_print_str("\r\n");
            break;
        }

        /* Clean and set the DWARF register structure. */
        memset(&state, 0, sizeof(dwarf_regs));

        const uint32_t prev_sp = EXECUTION_FRAME_SP(frame);

        /* Retrieve the return address of the frame. The frame's registers will be modified.
         * The frame we get then is the caller's one. */
        uint32_t ra = esp_eh_frame_restore_caller_state(fde, &frame, &state);

        /* End of backtrace is reached if the stack and the PC don't change anymore. */
        end_of_backtrace = (EXECUTION_FRAME_SP(frame) == prev_sp) && (EXECUTION_FRAME_PC(frame) == ra);

        /* Go back to the caller: update stack pointer and program counter. */
        EXECUTION_FRAME_PC(frame) = ra;
    }

    panic_print_str("\r\n");
}

/**
 * The following functions are the implementation of libunwind API
 * Check the header libunwind.h for more information
 */

int unw_init_local(unw_cursor_t* c, unw_context_t* ctxt) {
    /* In our implementation, a context and a cursor is the same, so we simply need
     * to copy a structure inside another one */
    _Static_assert(sizeof(unw_cursor_t) >= sizeof(unw_context_t), "unw_cursor_t size must be greater or equal to unw_context_t's");
    int ret = -UNW_EUNSPEC;
    if (c != NULL && ctxt != NULL) {
        memcpy(c, ctxt, sizeof(unw_context_t));
        ret = UNW_ESUCCESS;
    }
    return ret;
}

int unw_step(unw_cursor_t* cp) {
    static dwarf_regs state = { 0 };
    ExecutionFrame* frame = (ExecutionFrame*) cp;
    uint32_t size = 0;
    uint8_t* enc_values = NULL;

    /* Start parsing the .eh_frame_hdr section. */
    fde_header* header = (fde_header*) EH_FRAME_HDR_ADDR;
    if (header->version != 1) {
        goto badversion;
    }

    /* Make enc_values point to the end of the structure, where the encoded
     * values start. */
    enc_values = (uint8_t*) (header + 1);

    /* Retrieve the encoded value eh_frame_ptr. Get the size of the data also. */
    const uint32_t eh_frame_ptr = esp_eh_frame_get_encoded(enc_values, header->eh_frame_ptr_enc, &size);
    assert(eh_frame_ptr == (uint32_t) EH_FRAME_ADDR);
    enc_values += size;

    /* Same for the number of entries in the sorted table. */
    const uint32_t fde_count = esp_eh_frame_get_encoded(enc_values, header->fde_count_enc, &size);
    enc_values += size;

    /* enc_values points now at the beginning of the sorted table. */
    /* Only support 4-byte entries. */
    const uint32_t table_enc = header->table_enc;
    if ( ((table_enc >> 4) != 0x3) && ((table_enc >> 4) != 0xB) ) {
        goto badversion;
    }

    const table_entry* sorted_table = (const table_entry*) enc_values;

    const table_entry* from_fun = esp_eh_frame_find_entry(sorted_table, fde_count,
                                                          table_enc, EXECUTION_FRAME_PC(*frame));

    /* Get absolute address of FDE entry describing the function where PC left of. */
    uint32_t* fde = NULL;
    if (from_fun != NULL) {
        fde = esp_eh_frame_decode_address(&from_fun->fde_addr, table_enc);
    }

    if (esp_eh_frame_missing_info(fde, EXECUTION_FRAME_PC(*frame))) {
        goto missinginfo;
    }

    const uint32_t prev_sp = EXECUTION_FRAME_SP(*frame);

    /* Retrieve the return address of the frame. The frame's registers will be modified.
     * The frame we get then is the caller's one. */
    uint32_t ra = esp_eh_frame_restore_caller_state(fde, frame, &state);

    /* End of backtrace is reached if the stack and the PC don't change anymore. */
    if ((EXECUTION_FRAME_SP(*frame) == prev_sp) && (EXECUTION_FRAME_PC(*frame) == ra)) {
        goto stopunwind;
    }

    /* Go back to the caller: update stack pointer and program counter. */
    EXECUTION_FRAME_PC(*frame) = ra;

    return 1;
badversion:
    return -UNW_EBADVERSION;
missinginfo:
    return -UNW_ENOINFO;
stopunwind:
    return 0;
}

int unw_get_reg(unw_cursor_t* cp, unw_regnum_t reg, unw_word_t* valp) {
    if (cp == NULL || valp == NULL) {
        goto invalid;
    }
    if (reg >= EXECUTION_FRAME_MAX_REGS) {
        goto badreg;
    }

    *valp = EXECUTION_FRAME_REG(cp, reg);
    return UNW_ESUCCESS;
invalid:
    return -UNW_EUNSPEC;
badreg:
    return -UNW_EBADREG;
}

int unw_set_reg(unw_cursor_t* cp, unw_regnum_t reg, unw_word_t val) {
    if (cp == NULL) {
        goto invalid;
    }
    if (reg >= EXECUTION_FRAME_MAX_REGS) {
        goto badreg;
    }

    EXECUTION_FRAME_REG(cp, reg) = val;
    return UNW_ESUCCESS;
invalid:
    return -UNW_EUNSPEC;
badreg:
    return -UNW_EBADREG;
}

#endif //ESP_SYSTEM_USE_EH_FRAME
