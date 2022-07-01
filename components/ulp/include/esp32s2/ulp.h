/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "esp_err.h"
#include "soc/soc.h"
#include "ulp_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ulp_registers ULP coprocessor registers
 * @{
 */


#define R0 0    /*!< general purpose register 0 */
#define R1 1    /*!< general purpose register 1 */
#define R2 2    /*!< general purpose register 2 */
#define R3 3    /*!< general purpose register 3 */
/**@}*/

/** @defgroup ulp_opcodes ULP coprocessor opcodes, sub opcodes, and various modifiers/flags
 *
 * These definitions are not intended to be used directly.
 * They are used in definitions of instructions later on.
 *
 * @{
 */

#define OPCODE_WR_REG 1         /*!< Instruction: write peripheral register (RTC_CNTL/RTC_IO/SARADC) (not implemented yet) */

#define OPCODE_RD_REG 2         /*!< Instruction: read peripheral register (RTC_CNTL/RTC_IO/SARADC) (not implemented yet) */

#define RD_REG_PERIPH_RTC_CNTL 0    /*!< Identifier of RTC_CNTL peripheral for RD_REG and WR_REG instructions */
#define RD_REG_PERIPH_RTC_IO   1    /*!< Identifier of RTC_IO peripheral for RD_REG and WR_REG instructions */
#define RD_REG_PERIPH_SENS     2    /*!< Identifier of SARADC peripheral for RD_REG and WR_REG instructions */
#define RD_REG_PERIPH_RTC_I2C  3    /*!< Identifier of RTC_I2C peripheral for RD_REG and WR_REG instructions */

#define OPCODE_I2C 3            /*!< Instruction: read/write I2C (not implemented yet) */

#define OPCODE_DELAY 4          /*!< Instruction: delay (nop) for a given number of cycles */

#define OPCODE_ADC 5            /*!< Instruction: SAR ADC measurement (not implemented yet) */

#define OPCODE_ST 6             /*!< Instruction: store indirect to RTC memory */
#define SUB_OPCODE_ST_AUTO 1    /*!< Automatic Storage Mode - Access continuous addresses. Use SUB_OPCODE_ST_OFFSET to configure the initial address before using this instruction. */
#define SUB_OPCODE_ST_OFFSET 3  /*!< Automatic Storage Mode - Configure the initial address. */
#define SUB_OPCODE_ST 4         /*!< Manual Storage Mode. Store 32 bits, 16 MSBs contain PC, 16 LSBs contain value from source register */

#define OPCODE_ALU 7            /*!< Arithmetic instructions */
#define SUB_OPCODE_ALU_REG 0    /*!< Arithmetic instruction, both source values are in register */
#define SUB_OPCODE_ALU_IMM 1    /*!< Arithmetic instruction, one source value is an immediate */
#define SUB_OPCODE_ALU_CNT 2    /*!< Arithmetic instruction between counter register and an immediate (not implemented yet)*/
#define ALU_SEL_ADD 0           /*!< Addition */
#define ALU_SEL_SUB 1           /*!< Subtraction */
#define ALU_SEL_AND 2           /*!< Logical AND */
#define ALU_SEL_OR  3           /*!< Logical OR */
#define ALU_SEL_MOV 4           /*!< Copy value (immediate to destination register or source register to destination register */
#define ALU_SEL_LSH 5           /*!< Shift left by given number of bits */
#define ALU_SEL_RSH 6           /*!< Shift right by given number of bits */
#define ALU_SEL_STAGE_INC 0     /*!< Increment stage count register */
#define ALU_SEL_STAGE_DEC 1     /*!< Decrement stage count register */
#define ALU_SEL_STAGE_RST 2     /*!< Reset stage count register */

#define OPCODE_BRANCH 8         /*!< Branch instructions */
#define SUB_OPCODE_B  0         /*!< Branch to a relative offset */
#define SUB_OPCODE_BX  1        /*!< Branch to absolute PC (immediate or in register) */
#define SUB_OPCODE_BS  2        /*!< Branch to a relative offset by comparing the stage_cnt register */
#define BX_JUMP_TYPE_DIRECT 0   /*!< Unconditional jump */
#define BX_JUMP_TYPE_ZERO 1     /*!< Branch if last ALU result is zero */
#define BX_JUMP_TYPE_OVF 2      /*!< Branch if last ALU operation caused and overflow */
#define B_CMP_L 0               /*!< Branch if R0 is less than an immediate */
#define B_CMP_G 1               /*!< Branch if R0 is greater than an immediate */
#define B_CMP_E 2               /*!< Branch if R0 is equal to an immediate */
#define BS_CMP_L 0              /*!< Branch if stage_cnt is less than an immediate */
#define BS_CMP_GE 1             /*!< Branch if stage_cnt is greater than or equal to an immediate */
#define BS_CMP_LE 2             /*!< Branch if stage_cnt is less than or equal to an immediate */

#define OPCODE_END 9            /*!< Stop executing the program */
#define SUB_OPCODE_END 0        /*!< Stop executing the program and optionally wake up the chip */
#define SUB_OPCODE_SLEEP 1      /*!< Stop executing the program and run it again after selected interval */

#define OPCODE_TSENS 10         /*!< Instruction: temperature sensor measurement (not implemented yet) */

#define OPCODE_HALT 11          /*!< Halt the coprocessor */

#define OPCODE_LD 13            /*!< Indirect load lower 16 bits from RTC memory */

#define OPCODE_MACRO 15         /*!< Not a real opcode. Used to identify labels and branches in the program */
#define SUB_OPCODE_MACRO_LABEL 0    /*!< Label macro */
#define SUB_OPCODE_MACRO_BRANCH 1   /*!< Branch macro */
#define SUB_OPCODE_MACRO_LABELPC 2  /*!< Label pointer macro */
/**@}*/

/**
 * @brief Instruction format structure
 *
 * All ULP instructions are 32 bit long.
 * This union contains field layouts used by all of the supported instructions.
 * This union also includes a special "macro" instruction layout.
 * This is not a real instruction which can be executed by the CPU. It acts
 * as a token which is removed from the program by the
 * ulp_process_macros_and_load function.
 *
 * These structures are not intended to be used directly.
 * Preprocessor definitions provided below fill the fields of these structure with
 * the right arguments.
 */
union ulp_insn {

    struct {
        uint32_t cycles : 16;       /*!< Number of cycles to sleep */
        uint32_t unused : 12;       /*!< Unused */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_DELAY) */
    } delay;                        /*!< Format of DELAY instruction */

    struct {
        uint32_t dreg : 2;          /*!< Register which contains data to store */
        uint32_t sreg : 2;          /*!< Register which contains address in RTC memory (expressed in words) */
        uint32_t label: 2;          /*!< Data label, 2-bit user defined unsigned value */
        uint32_t upper: 1;          /*!< 0: write the low half-word; 1: write the high half-word */
        uint32_t wr_way: 2;         /*!< 0: write the full-word; 1: with the label; 3: without the label */
        uint32_t unused1 : 1;       /*!< Unused */
        uint32_t offset : 11;       /*!< Offset to add to sreg */
        uint32_t unused2 : 4;       /*!< Unused */
        uint32_t sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_ST) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_ST) */
    } st;                           /*!< Format of ST instruction */

    struct {
        uint32_t dreg : 2;          /*!< Register where the data should be loaded to */
        uint32_t sreg : 2;          /*!< Register which contains address in RTC memory (expressed in words) */
        uint32_t unused1 : 6;       /*!< Unused */
        uint32_t offset : 11;       /*!< Offset to add to sreg */
        uint32_t unused2 : 6;       /*!< Unused */
        uint32_t rd_upper: 1;       /*!< 0: read the high half-word; 1: read the low half-word*/
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_LD) */
    } ld;                           /*!< Format of LD instruction */

    struct {
        uint32_t unused : 28;       /*!< Unused */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_HALT) */
    } halt;                         /*!< Format of HALT instruction */

    struct {
        uint32_t dreg : 2;          /*!< Register which contains target PC, expressed in words (used if .reg == 1) */
        uint32_t addr : 11;         /*!< Target PC, expressed in words (used if .reg == 0) */
        uint32_t unused1 : 8;       /*!< Unused */
        uint32_t reg : 1;           /*!< Target PC in register (1) or immediate (0) */
        uint32_t type : 3;          /*!< Jump condition (BX_JUMP_TYPE_xxx) */
        uint32_t unused2 : 1;       /*!< Unused */
        uint32_t sub_opcode : 2;    /*!< Sub opcode (SUB_OPCODE_BX) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_BRANCH) */
    } bx;                           /*!< Format of BRANCH instruction (absolute address) */

    struct {
        uint32_t imm : 16;          /*!< Immediate value to compare against */
        uint32_t cmp : 2;           /*!< Comparison to perform: B_CMP_L or B_CMP_GE */
        uint32_t offset : 7;        /*!< Absolute value of target PC offset w.r.t. current PC, expressed in words */
        uint32_t sign : 1;          /*!< Sign of target PC offset: 0: positive, 1: negative */
        uint32_t sub_opcode : 2;    /*!< Sub opcode (SUB_OPCODE_B) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_BRANCH) */
    } b;                            /*!< Format of BRANCH instruction (relative address) */

    struct {
        uint32_t dreg : 2;          /*!< Destination register */
        uint32_t sreg : 2;          /*!< Register with operand A */
        uint32_t treg : 2;          /*!< Register with operand B */
        uint32_t unused1 : 15;      /*!< Unused */
        uint32_t sel : 4;           /*!< Operation to perform, one of ALU_SEL_xxx */
        uint32_t unused2 : 1;       /*!< Unused */
        uint32_t sub_opcode : 2;    /*!< Sub opcode (SUB_OPCODE_ALU_REG) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_ALU) */
    } alu_reg;                      /*!< Format of ALU instruction (both sources are registers) */

    struct {
        uint32_t dreg : 2;          /*!< Destination register */
        uint32_t sreg : 2;          /*!< Register with operand A */
        uint32_t imm : 16;          /*!< Immediate value of operand B */
        uint32_t unused1: 1;        /*!< Unused */
        uint32_t sel : 4;           /*!< Operation to perform, one of ALU_SEL_xxx */
        uint32_t unused2 : 1;       /*!< Unused */
        uint32_t sub_opcode : 2;    /*!< Sub opcode (SUB_OPCODE_ALU_IMM) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_ALU) */
    } alu_imm;                      /*!< Format of ALU instruction (one source is an immediate) */

    struct {
        uint32_t unused1: 4;        /*!< Unused */
        uint32_t imm : 8;           /*!< Immediate value */
        uint32_t unused2: 9;        /*!< Unused */
        uint32_t sel : 4;           /*!< Operation to perform, one of ALU_SEL_xxx */
        uint32_t unused3 : 1;       /*!< Unused */
        uint32_t sub_opcode : 2;    /*!< Sub opcode (SUB_OPCODE_ALU_CNT) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_ALU) */
    } alu_cnt;                      /*!< Format of ALU instruction with stage count register and an immediate */

    struct {
        uint32_t addr : 8;          /*!< Address within either RTC_CNTL, RTC_IO, or SARADC */
        uint32_t periph_sel : 2;    /*!< Select peripheral: RTC_CNTL (0), RTC_IO(1), SARADC(2) */
        uint32_t data : 8;          /*!< 8 bits of data to write */
        uint32_t low : 5;           /*!< Low bit */
        uint32_t high : 5;          /*!< High bit */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_WR_REG) */
    } wr_reg;                       /*!< Format of WR_REG instruction */

    struct {
        uint32_t addr : 8;          /*!< Address within either RTC_CNTL, RTC_IO, or SARADC */
        uint32_t periph_sel : 2;    /*!< Select peripheral: RTC_CNTL (0), RTC_IO(1), SARADC(2) */
        uint32_t unused : 8;        /*!< Unused */
        uint32_t low : 5;           /*!< Low bit */
        uint32_t high : 5;          /*!< High bit */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_RD_REG) */
    } rd_reg;                       /*!< Format of RD_REG instruction */

    struct {
        uint32_t dreg : 2;          /*!< Register where to store ADC result */
        uint32_t mux : 4;           /*!< Select SARADC pad (mux + 1) */
        uint32_t sar_sel : 1;       /*!< Select SARADC0 (0) or SARADC1 (1) */
        uint32_t unused1 : 1;       /*!< Unused */
        uint32_t cycles : 16;       /*!< TBD, cycles used for measurement */
        uint32_t unused2 : 4;       /*!< Unused */
        uint32_t opcode: 4;         /*!< Opcode (OPCODE_ADC) */
    } adc;                          /*!< Format of ADC instruction */

    struct {
        uint32_t dreg : 2;          /*!< Register where to store temperature measurement result */
        uint32_t wait_delay: 14;    /*!< Cycles to wait after measurement is done */
        uint32_t reserved: 12;      /*!< Reserved, set to 0 */
        uint32_t opcode: 4;         /*!< Opcode (OPCODE_TSENS) */
    } tsens;                        /*!< Format of TSENS instruction */

    struct {
        uint32_t i2c_addr : 8;      /*!< I2C slave address */
        uint32_t data : 8;          /*!< Data to read or write */
        uint32_t low_bits : 3;      /*!< TBD */
        uint32_t high_bits : 3;     /*!< TBD */
        uint32_t i2c_sel : 4;       /*!< TBD, select reg_i2c_slave_address[7:0] */
        uint32_t unused : 1;        /*!< Unused */
        uint32_t rw : 1;            /*!< Write (1) or read (0) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_I2C) */
    } i2c;                          /*!< Format of I2C instruction */

    struct {
        uint32_t wakeup : 1;        /*!< Set to 1 to wake up chip */
        uint32_t unused : 25;       /*!< Unused */
        uint32_t sub_opcode : 2;    /*!< Sub opcode (SUB_OPCODE_WAKEUP) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_END) */
    } end;                          /*!< Format of END instruction with wakeup */

    struct {
        uint32_t label : 16;        /*!< Label number */
        uint32_t unused : 8;        /*!< Unused */
        uint32_t sub_opcode : 4;    /*!< SUB_OPCODE_MACRO_LABEL or SUB_OPCODE_MACRO_BRANCH */
        uint32_t opcode: 4;         /*!< Opcode (OPCODE_MACRO) */
    } macro;                        /*!< Format of tokens used by LABEL and BRANCH macros */

};

_Static_assert(sizeof(ulp_insn_t) == 4, "ULP coprocessor instruction size should be 4 bytes");

/**
 * Delay (nop) for a given number of cycles
 */
#define I_DELAY(cycles_) { .delay = {\
    .cycles = cycles_, \
    .unused = 0, \
    .opcode = OPCODE_DELAY } }

/**
 * Halt the coprocessor.
 *
 * This instruction halts the coprocessor, but keeps ULP timer active.
 * As such, ULP program will be restarted again by timer.
 * To stop the program and prevent the timer from restarting the program,
 * use I_END(0) instruction.
 */
#define I_HALT() { .halt = {\
    .unused = 0, \
    .opcode = OPCODE_HALT } }

/**
 * Map SoC peripheral register to periph_sel field of RD_REG and WR_REG
 * instructions.
 *
 * @param reg peripheral register in RTC_CNTL_, RTC_IO_, SENS_, RTC_I2C peripherals.
 * @return periph_sel value for the peripheral to which this register belongs.
 */
static inline uint32_t SOC_REG_TO_ULP_PERIPH_SEL(uint32_t reg)
{
    uint32_t ret = 3;
    if (reg < DR_REG_RTCCNTL_BASE) {
        assert(0 && "invalid register base");
    } else if (reg < DR_REG_RTCIO_BASE) {
        ret = RD_REG_PERIPH_RTC_CNTL;
    } else if (reg < DR_REG_SENS_BASE) {
        ret = RD_REG_PERIPH_RTC_IO;
    } else if (reg < DR_REG_RTC_I2C_BASE) {
        ret = RD_REG_PERIPH_SENS;
    } else if (reg < DR_REG_IO_MUX_BASE) {
        ret = RD_REG_PERIPH_RTC_I2C;
    } else {
        assert(0 && "invalid register base");
    }
    return ret;
}

/**
 * Write literal value to a peripheral register
 *
 * reg[high_bit : low_bit] = val
 * This instruction can access RTC_CNTL_, RTC_IO_, SENS_, and RTC_I2C peripheral registers.
 */
#define I_WR_REG(reg, low_bit, high_bit, val) {.wr_reg = {\
    .addr = (reg & 0xff) / sizeof(uint32_t), \
    .periph_sel = SOC_REG_TO_ULP_PERIPH_SEL(reg), \
    .data = val, \
    .low = low_bit, \
    .high = high_bit, \
    .opcode = OPCODE_WR_REG } }

/**
 * Read from peripheral register into R0
 *
 * R0 = reg[high_bit : low_bit]
 * This instruction can access RTC_CNTL_, RTC_IO_, SENS_, and RTC_I2C peripheral registers.
 */
#define I_RD_REG(reg, low_bit, high_bit) {.rd_reg = {\
    .addr = (reg & 0xff) / sizeof(uint32_t), \
    .periph_sel = SOC_REG_TO_ULP_PERIPH_SEL(reg), \
    .unused = 0, \
    .low = low_bit, \
    .high = high_bit, \
    .opcode = OPCODE_RD_REG } }

/**
 * Set or clear a bit in the peripheral register.
 *
 * Sets bit (1 << shift) of register reg to value val.
 * This instruction can access RTC_CNTL_, RTC_IO_, SENS_, and RTC_I2C peripheral registers.
 */
#define I_WR_REG_BIT(reg, shift, val) I_WR_REG(reg, shift, shift, val)

/**
 * Wake the SoC from deep sleep.
 *
 * This instruction initiates wake up from deep sleep.
 * Use esp_deep_sleep_enable_ulp_wakeup to enable deep sleep wakeup
 * triggered by the ULP before going into deep sleep.
 * Note that ULP program will still keep running until the I_HALT
 * instruction, and it will still be restarted by timer at regular
 * intervals, even when the SoC is woken up.
 *
 * To stop the ULP program, use I_HALT instruction.
 *
 * To disable the timer which start ULP program, use I_END()
 * instruction. I_END instruction clears the
 * RTC_CNTL_ULP_CP_SLP_TIMER_EN_S bit of RTC_CNTL_ULP_CP_TIMER_REG
 * register, which controls the ULP timer.
 */
#define I_WAKE() { .end = { \
        .wakeup = 1, \
        .unused = 0, \
        .sub_opcode = SUB_OPCODE_END, \
        .opcode = OPCODE_END } }

/**
 * Stop ULP program timer.
 *
 * This is a convenience macro which disables the ULP program timer.
 * Once this instruction is used, ULP program will not be restarted
 * anymore until ulp_run function is called.
 *
 * ULP program will continue running after this instruction. To stop
 * the currently running program, use I_HALT().
 */
#define I_END() \
    I_WR_REG_BIT(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN_S, 0)

/**
 * Perform temperature sensor measurement and store it into reg_dest.
 *
 * Delay can be set between 1 and ((1 << 14) - 1). Higher values give
 * higher measurement resolution.
 */
#define I_TSENS(reg_dest, delay) { .tsens = { \
        .dreg = reg_dest, \
        .wait_delay = delay, \
        .reserved = 0, \
        .opcode = OPCODE_TSENS } }

/**
 * Perform ADC measurement and store result in reg_dest.
 *
 * adc_idx selects ADC (0 or 1).
 * pad_idx selects ADC pad (0 - 7).
 */
#define I_ADC(reg_dest, adc_idx, pad_idx) { .adc = {\
        .dreg = reg_dest, \
        .mux = pad_idx + 1, \
        .sar_sel = adc_idx, \
        .unused1 = 0, \
        .cycles = 0, \
        .unused2 = 0, \
        .opcode = OPCODE_ADC } }

/**
 * Store lower half-word, upper half-word or full-word data from register reg_val into RTC memory address.
 *
 * This instruction can be used to write data to discontinuous addresses in the RTC_SLOW_MEM.
 * The value is written to an offset calculated by adding the value of
 * reg_addr register and offset_ field (this offset is expressed in 32-bit words).
 * The storage method is dictated by the wr_way and upper field settings as summarized in the following table:
 *
 * @verbatim
 * |--------|-------|----------------------------------------------------------------------------------------|----------------------------|
 * | wr_way | upper |                                        data                                            |          operation         |
 * |--------|-------|----------------------------------------------------------------------------------------|----------------------------|
 * |        |       |                                                                                        | Write full-word, including |
 * |   0    |   X   | RTC_SLOW_MEM[addr + offset_]{31:0} = {insn_PC[10:0], 3’b0, label_[1:0], reg_val[15:0]} | the PC and the data        |
 * |--------|-------|----------------------------------------------------------------------------------------|----------------------------|
 * |        |       |                                                                                        | Store the data with label  |
 * |   1    |   0   | RTC_SLOW_MEM[addr + offset_]{15:0} = {label_[1:0], reg_val[13:0]}                      | in the low half-word       |
 * |--------|-------|----------------------------------------------------------------------------------------|----------------------------|
 * |        |       |                                                                                        | Store the data with label  |
 * |   1    |   1   | RTC_SLOW_MEM[addr + offset_]{31:16} = {label_[1:0], reg_val[13:0]}                     | in the high half-word      |
 * |--------|-------|----------------------------------------------------------------------------------------|----------------------------|
 * |        |       |                                                                                        | Store the data without     |
 * |   3    |   0   | RTC_SLOW_MEM[addr + offset_]{15:0} = reg_val[15:0]                                     | label in the low half-word |
 * |--------|-------|----------------------------------------------------------------------------------------|----------------------------|
 * |        |       |                                                                                        | Store the data without     |
 * |   3    |   1   | RTC_SLOW_MEM[addr + offset_]{31:16} = reg_val[15:0]                                    | label in the high half-word|
 * |--------|-------|----------------------------------------------------------------------------------------|----------------------------|
 * @endverbatim
 *
 * SUB_OPCODE_ST = manual_en:1, offset_set:0, wr_auto:0
 */
#define I_ST_MANUAL(reg_val, reg_addr, offset_, label_, upper_, wr_way_) { .st = { \
    .dreg = reg_val, \
    .sreg = reg_addr, \
    .label = label_, \
    .upper = upper_, \
    .wr_way = wr_way_, \
    .unused1 = 0, \
    .offset = offset_, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ST, \
    .opcode = OPCODE_ST } }

/**
 * Store value from register reg_val into RTC memory.
 *
 * I_ST() instruction provides backward compatibility for code written for esp32 to be run on esp32s2.
 * This instruction is equivalent to calling I_ST_MANUAL() instruction with label = 0, upper = 0 and wr_way = 3.
 */
#define I_ST(reg_val, reg_addr, offset_)  I_ST_MANUAL(reg_val, reg_addr, offset_, 0, 0, 3)

/**
 * Store value from register reg_val to lower 16 bits of the RTC memory address.
 *
 * This instruction is equivalent to calling I_ST_MANUAL() instruction with label = 0, upper = 0 and wr_way = 3.
 */
#define I_STL(reg_val, reg_addr, offset_)  I_ST_MANUAL(reg_val, reg_addr, offset_, 0, 0, 3)

/**
 * Store value from register reg_val to upper 16 bits of the RTC memory address.
 *
 * This instruction is equivalent to calling I_ST_MANUAL() instruction with label = 0, upper = 1 and wr_way = 3.
 */
#define I_STH(reg_val, reg_addr, offset_)  I_ST_MANUAL(reg_val, reg_addr, offset_, 0, 1, 3)

/**
 * Store value from register reg_val to full 32 bit word of the RTC memory address.
 *
 * This instruction is equivalent to calling I_ST_MANUAL() instruction with wr_way = 0.
 */
#define I_ST32(reg_val, reg_addr, offset_, label_)  I_ST_MANUAL(reg_val, reg_addr, offset_, label_, 0, 0)

/**
 * Store value from register reg_val with label to lower 16 bits of RTC memory address.
 *
 * This instruction is equivalent to calling I_ST_MANUAL() instruction with label = label_, upper = 0 and wr_way = 1.
 */
#define I_STL_LABEL(reg_val, reg_addr, offset_, label_)  I_ST_MANUAL(reg_val, reg_addr, offset_, label_, 0, 1)

/**
 * Store value from register reg_val with label to upper 16 bits of RTC memory address.
 *
 * This instruction is equivalent to calling I_ST_MANUAL() instruction with label = label_, upper = 1 and wr_way = 1.
 */
#define I_STH_LABEL(reg_val, reg_addr, offset_, label_)  I_ST_MANUAL(reg_val, reg_addr, offset_, label_, 1, 1)

/**
 * Store lower half-word, upper half-word or full-word data from register reg_val into RTC memory address with auto-increment of the offset value.
 *
 * This instruction can be used to write data to continuous addresses in the RTC_SLOW_MEM.
 * The initial address must be set using the SUB_OPCODE_ST_OFFSET instruction before the auto store instruction is called.
 * The data written to the RTC memory address could be written to the full 32 bit word or to the lower half-word or the
 * upper half-word. The storage method is dictated by the wr_way field and the number of times the SUB_OPCODE_ST_AUTO instruction is called.
 * write_cnt indicates the later. The following table summarizes the storage method:
 *
 * @verbatim
 * |--------|-----------|----------------------------------------------------------------------------------------|----------------------------|
 * | wr_way | write_cnt |                                        data                                            |          operation         |
 * |--------|-----------|----------------------------------------------------------------------------------------|----------------------------|
 * |        |           |                                                                                        | Write full-word, including |
 * |   0    |     X     | RTC_SLOW_MEM[addr + offset_]{31:0} = {insn_PC[10:0], 3’b0, label_[1:0], reg_val[15:0]} | the PC and the data        |
 * |--------|-----------|----------------------------------------------------------------------------------------|----------------------------|
 * |        |           |                                                                                        | Store the data with label  |
 * |   1    |    odd    | RTC_SLOW_MEM[addr + offset_]{15:0} = {label_[1:0], reg_val[13:0]}                      | in the low half-word       |
 * |--------|-----------|----------------------------------------------------------------------------------------|----------------------------|
 * |        |           |                                                                                        | Store the data with label  |
 * |   1    |    even   | RTC_SLOW_MEM[addr + offset_]{31:16} = {label_[1:0], reg_val[13:0]}                     | in the high half-word      |
 * |--------|-----------|----------------------------------------------------------------------------------------|----------------------------|
 * |        |           |                                                                                        | Store the data without     |
 * |   3    |    odd    | RTC_SLOW_MEM[addr + offset_]{15:0} = reg_val[15:0]                                     | label in the low half-word |
 * |--------|-----------|----------------------------------------------------------------------------------------|----------------------------|
 * |        |           |                                                                                        | Store the data without     |
 * |   3    |    even   | RTC_SLOW_MEM[addr + offset_]{31:16} = reg_val[15:0]                                    | label in the high half-word|
 * |--------|-----------|----------------------------------------------------------------------------------------|----------------------------|
 * @endverbatim
 *
 * The initial address offset is incremented after each store operation as follows:
 * - When a full-word is written, the offset is automatically incremented by 1 after each SUB_OPCODE_ST_AUTO operation.
 * - When a half-word is written (lower half-word first), the offset is automatically incremented by 1 after two
 *   SUB_OPCODE_ST_AUTO operations.
 *
 *   SUB_OPCODE_ST_AUTO =  manual_en:0, offset_set:0, wr_auto:1
 */
#define I_ST_AUTO(reg_val, reg_addr, label_, wr_way_) { .st = { \
    .dreg = reg_addr, \
    .sreg = reg_val, \
    .label = label_, \
    .upper = 0, \
    .wr_way = wr_way_, \
    .unused1 = 0, \
    .offset = 0, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ST_AUTO, \
    .opcode = OPCODE_ST } }

/**
 * Set the initial address offset for auto-store operation
 *
 * This instruction sets the initial address of the RTC_SLOW_MEM to be used by the auto-store operation.
 * The offset is incremented automatically.
 * Refer I_ST_AUTO() for detailed explaination.
 *
 * SUB_OPCODE_ST_OFFSET = manual_en:0, offset_set:1, wr_auto:1
 */
#define I_STO(offset_) { .st = { \
    .dreg = 0, \
    .sreg = 0, \
    .label = 0, \
    .upper = 0, \
    .wr_way = 0, \
    .unused1 = 0, \
    .offset = offset_, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ST_OFFSET, \
    .opcode = OPCODE_ST } }

/**
 * Store value from register reg_val to 32 bit word of the RTC memory address.
 *
 * This instruction is equivalent to calling I_ST_AUTO() instruction with label = 0 and wr_way = 3.
 * The data in reg_val will be either written to the lower half-word or the upper half-word of the RTC memory address
 * depending on the count of the number of times the I_STI() instruction is called.
 * The initial offset is automatically incremented with I_STI() is called twice.
 * Refer I_ST_AUTO() for detailed explaination.
 */
#define I_STI(reg_val, reg_addr) I_ST_AUTO(reg_val, reg_addr, 0, 3)

/**
 * Store value from register reg_val with label to 32 bit word of the RTC memory address.
 *
 * This instruction is equivalent to calling I_ST_AUTO() instruction with label = label_ and wr_way = 1.
 * The data in reg_val will be either written to the lower half-word or the upper half-word of the RTC memory address
 * depending on the count of the number of times the I_STI_LABEL() instruction is called.
 * The initial offset is automatically incremented with I_STI_LABEL() is called twice.
 * Refer I_ST_AUTO() for detailed explaination.
 */
#define I_STI_LABEL(reg_val, reg_addr, label_) I_ST_AUTO(reg_val, reg_addr, label_, 1)

/**
 * Store value from register reg_val to full 32 bit word of the RTC memory address.
 *
 * This instruction is equivalent to calling I_ST_AUTO() instruction with label = label_ and wr_way = 0.
 * The data in reg_val will be written to the RTC memory address along with the label and the PC.
 * The initial offset is automatically incremented each time the I_STI32() instruction is called.
 * Refer I_ST_AUTO() for detailed explaination.
 */
#define I_STI32(reg_val, reg_addr, label_) I_ST_AUTO(reg_val, reg_addr, label_, 0)

/**
 * Load lower half-word, upper half-word or full-word data from RTC memory address into the register reg_dest.
 *
 * This instruction reads the lower half-word or upper half-word of the RTC memory address depending on the value
 * of rd_upper_. The following table summarizes the loading method:
 *
 * @verbatim
 * |----------|------------------------------------------------------|-------------------------|
 * | rd_upper |                       data                           |        operation        |
 * |----------|------------------------------------------------------|-------------------------|
 * |          |                                                      | Read lower half-word of |
 * |    0     | reg_dest{15:0} = RTC_SLOW_MEM[addr + offset_]{31:16} | the memory              |
 * |----------|------------------------------------------------------|-------------------------|
 * |          |                                                      | Read upper half-word of |
 * |    1     | reg_dest{15:0} = RTC_SLOW_MEM[addr + offset_]{15:0}  | the memory              |
 * |----------|------------------------------------------------------|-------------------------|
 * @endverbatim
 *
 */
#define I_LD_MANUAL(reg_dest, reg_addr, offset_, rd_upper_) { .ld = { \
    .dreg = reg_dest, \
    .sreg = reg_addr, \
    .unused1 = 0, \
    .offset = offset_, \
    .unused2 = 0, \
    .rd_upper = rd_upper_, \
    .opcode = OPCODE_LD } }

/**
 * Load lower 16 bits value from RTC memory into reg_dest register.
 *
 * Loads 16 LSBs (rd_upper = 1) from RTC memory word given by the sum of value in reg_addr and
 * value of offset_.
 * I_LD() instruction provides backward compatibility for code written for esp32 to be run on esp32s2.
 */
#define I_LD(reg_dest, reg_addr, offset_) I_LD_MANUAL(reg_dest, reg_addr, offset_, 0)

/**
 * Load lower 16 bits value from RTC memory into reg_dest register.
 *
 * I_LDL() instruction and I_LD() instruction can be used interchangably.
 */
#define I_LDL(reg_dest, reg_addr, offset_) I_LD(reg_dest, reg_addr, offset_)

/**
 * Load upper 16 bits value from RTC memory into reg_dest register.
 *
 * Loads 16 MSBs (rd_upper = 0) from RTC memory word given by the sum of value in reg_addr and
 * value of offset_.
 */
#define I_LDH(reg_dest, reg_addr, offset_) I_LD_MANUAL(reg_dest, reg_addr, offset_, 1)

/**
 *  Branch relative if R0 register less than the immediate value.
 *
 *  pc_offset is expressed in words, and can be from -127 to 127
 *  imm_value is a 16-bit value to compare R0 against
 */
#define I_BL(pc_offset, imm_value) { .b = { \
    .imm = imm_value, \
    .cmp = B_CMP_L, \
    .offset = abs(pc_offset), \
    .sign = (pc_offset >= 0) ? 0 : 1, \
    .sub_opcode = SUB_OPCODE_B, \
    .opcode = OPCODE_BRANCH } }

/**
 *  Branch relative if R0 register greater than the immediate value.
 *
 *  pc_offset is expressed in words, and can be from -127 to 127
 *  imm_value is a 16-bit value to compare R0 against
 */
#define I_BG(pc_offset, imm_value) { .b = { \
    .imm = imm_value, \
    .cmp = B_CMP_G, \
    .offset = abs(pc_offset), \
    .sign = (pc_offset >= 0) ? 0 : 1, \
    .sub_opcode = SUB_OPCODE_B, \
    .opcode = OPCODE_BRANCH } }

/**
 *  Branch relative if R0 register is equal to the immediate value.
 *
 *  pc_offset is expressed in words, and can be from -127 to 127
 *  imm_value is a 16-bit value to compare R0 against
 */
#define I_BE(pc_offset, imm_value) { .b = { \
    .imm = imm_value, \
    .cmp = B_CMP_E, \
    .offset = abs(pc_offset), \
    .sign = (pc_offset >= 0) ? 0 : 1, \
    .sub_opcode = SUB_OPCODE_B, \
    .opcode = OPCODE_BRANCH } }

/**
 * Unconditional branch to absolute PC, address in register.
 *
 * reg_pc is the register which contains address to jump to.
 * Address is expressed in 32-bit words.
 */
#define I_BXR(reg_pc) { .bx = { \
    .dreg = reg_pc, \
    .addr = 0, \
    .unused1 = 0, \
    .reg = 1, \
    .type = BX_JUMP_TYPE_DIRECT, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_BX, \
    .opcode = OPCODE_BRANCH } }

/**
 *  Unconditional branch to absolute PC, immediate address.
 *
 *  Address imm_pc is expressed in 32-bit words.
 */
#define I_BXI(imm_pc) { .bx = { \
    .dreg = 0, \
    .addr = imm_pc, \
    .unused1 = 0, \
    .reg = 0, \
    .type = BX_JUMP_TYPE_DIRECT, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_BX, \
    .opcode = OPCODE_BRANCH } }

/**
 * Branch to absolute PC if ALU result is zero, address in register.
 *
 * reg_pc is the register which contains address to jump to.
 * Address is expressed in 32-bit words.
 */
#define I_BXZR(reg_pc) { .bx = { \
    .dreg = reg_pc, \
    .addr = 0, \
    .unused1 = 0, \
    .reg = 1, \
    .type = BX_JUMP_TYPE_ZERO, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_BX, \
    .opcode = OPCODE_BRANCH } }

/**
 * Branch to absolute PC if ALU result is zero, immediate address.
 *
 * Address imm_pc is expressed in 32-bit words.
 */
#define I_BXZI(imm_pc) { .bx = { \
    .dreg = 0, \
    .addr = imm_pc, \
    .unused1 = 0, \
    .reg = 0, \
    .type = BX_JUMP_TYPE_ZERO, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_BX, \
    .opcode = OPCODE_BRANCH } }

/**
 * Branch to absolute PC if ALU overflow, address in register
 *
 * reg_pc is the register which contains address to jump to.
 * Address is expressed in 32-bit words.
 */
#define I_BXFR(reg_pc) { .bx = { \
    .dreg = reg_pc, \
    .addr = 0, \
    .unused1 = 0, \
    .reg = 1, \
    .type = BX_JUMP_TYPE_OVF, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_BX, \
    .opcode = OPCODE_BRANCH } }

/**
 * Branch to absolute PC if ALU overflow, immediate address
 *
 * Address imm_pc is expressed in 32-bit words.
 */
#define I_BXFI(imm_pc) { .bx = { \
    .dreg = 0, \
    .addr = imm_pc, \
    .unused1 = 0, \
    .reg = 0, \
    .type = BX_JUMP_TYPE_OVF, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_BX, \
    .opcode = OPCODE_BRANCH } }

/**
 *  Branch relative if stage_cnt is less than or equal to the immediate value.
 *
 *  pc_offset is expressed in words, and can be from -127 to 127
 *  imm_value is a 16-bit value to compare R0 against
 */
#define I_BSLE(pc_offset, imm_value) { .b = { \
    .imm = imm_value, \
    .cmp = BS_CMP_LE, \
    .offset = abs(pc_offset), \
    .sign = (pc_offset >= 0) ? 0 : 1, \
    .sub_opcode = SUB_OPCODE_BS, \
    .opcode = OPCODE_BRANCH } }

/**
 *  Branch relative if stage_cnt register is greater than or equal to the immediate value.
 *
 *  pc_offset is expressed in words, and can be from -127 to 127
 *  imm_value is a 16-bit value to compare R0 against
 */
#define I_BSGE(pc_offset, imm_value) { .b = { \
    .imm = imm_value, \
    .cmp = BS_CMP_GE, \
    .offset = abs(pc_offset), \
    .sign = (pc_offset >= 0) ? 0 : 1, \
    .sub_opcode = SUB_OPCODE_BS, \
    .opcode = OPCODE_BRANCH } }

/**
 *  Branch relative if stage_cnt register is less than the immediate value.
 *
 *  pc_offset is expressed in words, and can be from -127 to 127
 *  imm_value is a 16-bit value to compare R0 against
 */
#define I_BSL(pc_offset, imm_value) { .b = { \
    .imm = imm_value, \
    .cmp = BS_CMP_L, \
    .offset = abs(pc_offset), \
    .sign = (pc_offset >= 0) ? 0 : 1, \
    .sub_opcode = SUB_OPCODE_BS, \
    .opcode = OPCODE_BRANCH } }

/**
 * Addition: dest = src1 + src2
 */
#define I_ADDR(reg_dest, reg_src1, reg_src2) { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src1, \
    .treg = reg_src2, \
    .unused1 = 0, \
    .sel = ALU_SEL_ADD, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }

/**
 * Subtraction: dest = src1 - src2
 */
#define I_SUBR(reg_dest, reg_src1, reg_src2) { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src1, \
    .treg = reg_src2, \
    .unused1 = 0, \
    .sel = ALU_SEL_SUB, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }

/**
 * Logical AND: dest = src1 & src2
 */
#define I_ANDR(reg_dest, reg_src1, reg_src2) { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src1, \
    .treg = reg_src2, \
    .unused1 = 0, \
    .sel = ALU_SEL_AND, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }

/**
 * Logical OR: dest = src1 | src2
 */
#define I_ORR(reg_dest, reg_src1, reg_src2)  { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src1, \
    .treg = reg_src2, \
    .unused1 = 0, \
    .sel = ALU_SEL_OR, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }

/**
 * Copy: dest = src
 */
#define I_MOVR(reg_dest, reg_src) { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .treg = 0, \
    .unused1 = 0, \
    .sel = ALU_SEL_MOV, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }

/**
 * Logical shift left: dest = src << shift
 */
#define I_LSHR(reg_dest, reg_src, reg_shift)  { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .treg = reg_shift, \
    .unused1 = 0, \
    .sel = ALU_SEL_LSH, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }


/**
 * Logical shift right: dest = src >> shift
 */
#define I_RSHR(reg_dest, reg_src, reg_shift)  { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .treg = reg_shift, \
    .unused1 = 0, \
    .sel = ALU_SEL_RSH, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }

/**
 * Add register and an immediate value: dest = src1 + imm
 */
#define I_ADDI(reg_dest, reg_src, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .imm = imm_, \
    .unused1 = 0, \
    .sel = ALU_SEL_ADD, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }


/**
 *  Subtract register and an immediate value: dest = src - imm
 */
#define I_SUBI(reg_dest, reg_src, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .imm = imm_, \
    .unused1 = 0, \
    .sel = ALU_SEL_SUB, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }

/**
 * Logical AND register and an immediate value: dest = src & imm
 */
#define I_ANDI(reg_dest, reg_src, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .imm = imm_, \
    .unused1 = 0, \
    .sel = ALU_SEL_AND, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }

/**
 * Logical OR register and an immediate value: dest = src | imm
 */
#define I_ORI(reg_dest, reg_src, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .imm = imm_, \
    .unused1 = 0, \
    .sel = ALU_SEL_OR, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }

/**
 * Copy an immediate value into register: dest = imm
 */
#define I_MOVI(reg_dest, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = 0, \
    .imm = imm_, \
    .unused1 = 0, \
    .sel = ALU_SEL_MOV, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }

/**
 * Logical shift left register value by an immediate: dest = src << imm
 */
#define I_LSHI(reg_dest, reg_src, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .imm = imm_, \
    .unused1 = 0, \
    .sel = ALU_SEL_LSH, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }


/**
 * Logical shift right register value by an immediate: dest = val >> imm
 */
#define I_RSHI(reg_dest, reg_src, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .imm = imm_, \
    .unused1 = 0, \
    .sel = ALU_SEL_RSH, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }

/**
 * Increment stage_cnt register by an immediate: stage_cnt = stage_cnt + imm
 */
#define I_STAGE_INC(reg_dest, reg_src, imm_) { .alu_cnt = { \
    .unused1 = 0, \
    .imm = imm_, \
    .unused2 = 0, \
    .sel = ALU_SEL_STAGE_INC, \
    .unused3 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_CNT, \
    .opcode = OPCODE_ALU } }

/**
 * Decrement stage_cnt register by an immediate: stage_cnt = stage_cnt - imm
 */
#define I_STAGE_DEC(reg_dest, reg_src, imm_) { .alu_cnt = { \
    .unused1 = 0, \
    .imm = imm_, \
    .unused2 = 0, \
    .sel = ALU_SEL_STAGE_DEC, \
    .unused3 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_CNT, \
    .opcode = OPCODE_ALU } }

/**
 * Reset stage_cnt register by an immediate: stage_cnt = 0
 */
#define I_STAGE_RST(reg_dest, reg_src, imm_) { .alu_cnt = { \
    .unused1 = 0, \
    .imm = imm_, \
    .unused2 = 0, \
    .sel = ALU_SEL_STAGE_RST, \
    .unused3 = 0, \
    .sub_opcode = SUB_OPCODE_ALU_CNT, \
    .opcode = OPCODE_ALU } }

/**
 * Define a label with number label_num.
 *
 * This is a macro which doesn't generate a real instruction.
 * The token generated by this macro is removed by ulp_process_macros_and_load
 * function. Label defined using this macro can be used in branch macros defined
 * below.
 */
#define M_LABEL(label_num) { .macro = { \
    .label = label_num, \
    .unused = 0, \
    .sub_opcode = SUB_OPCODE_MACRO_LABEL, \
    .opcode = OPCODE_MACRO } }

/**
 * Token macro used by M_B and M_BX macros. Not to be used directly.
 */
#define M_BRANCH(label_num) { .macro = { \
    .label = label_num, \
    .unused = 0, \
    .sub_opcode = SUB_OPCODE_MACRO_BRANCH, \
    .opcode = OPCODE_MACRO } }

/**
 * Macro: branch to label label_num if R0 is less than immediate value.
 *
 * This macro generates two ulp_insn_t values separated by a comma, and should
 * be used when defining contents of ulp_insn_t arrays. First value is not a
 * real instruction; it is a token which is removed by ulp_process_macros_and_load
 * function.
 */
#define M_BL(label_num, imm_value) \
    M_BRANCH(label_num), \
    I_BL(0, imm_value)

/**
 * Macro: branch to label label_num if R0 is greater than immediate value
 *
 * This macro generates two ulp_insn_t values separated by a comma, and should
 * be used when defining contents of ulp_insn_t arrays. First value is not a
 * real instruction; it is a token which is removed by ulp_process_macros_and_load
 * function.
 */
#define M_BG(label_num, imm_value) \
    M_BRANCH(label_num), \
    I_BG(0, imm_value)

/**
 * Macro: branch to label label_num if R0 equal to the immediate value
 *
 * This macro generates two ulp_insn_t values separated by a comma, and should
 * be used when defining contents of ulp_insn_t arrays. First value is not a
 * real instruction; it is a token which is removed by ulp_process_macros_and_load
 * function.
 */
#define M_BE(label_num, imm_value) \
    M_BRANCH(label_num), \
    I_BE(0, imm_value)

/**
 * Macro: unconditional branch to label
 *
 * This macro generates two ulp_insn_t values separated by a comma, and should
 * be used when defining contents of ulp_insn_t arrays. First value is not a
 * real instruction; it is a token which is removed by ulp_process_macros_and_load
 * function.
 */
#define M_BX(label_num) \
    M_BRANCH(label_num), \
    I_BXI(0)

/**
 * Macro: branch to label if ALU result is zero
 *
 * This macro generates two ulp_insn_t values separated by a comma, and should
 * be used when defining contents of ulp_insn_t arrays. First value is not a
 * real instruction; it is a token which is removed by ulp_process_macros_and_load
 * function.
 */
#define M_BXZ(label_num) \
    M_BRANCH(label_num), \
    I_BXZI(0)

/**
 * Macro: branch to label if ALU overflow
 *
 * This macro generates two ulp_insn_t values separated by a comma, and should
 * be used when defining contents of ulp_insn_t arrays. First value is not a
 * real instruction; it is a token which is removed by ulp_process_macros_and_load
 * function.
 */
#define M_BXF(label_num) \
    M_BRANCH(label_num), \
    I_BXFI(0)


#define RTC_SLOW_MEM ((uint32_t*) 0x50000000)       /*!< RTC slow memory, 8k size */

#ifdef __cplusplus
}
#endif
