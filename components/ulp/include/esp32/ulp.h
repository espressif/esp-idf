// Copyright 2016-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

#define ULP_FSM_PREPARE_SLEEP_CYCLES 2    /*!< Cycles spent by FSM preparing ULP for sleep */
#define ULP_FSM_WAKEUP_SLEEP_CYCLES  2    /*!< Cycles spent by FSM waking up ULP from sleep */

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

#define OPCODE_WR_REG 1         /*!< Instruction: write peripheral register (RTC_CNTL/RTC_IO/SARADC) */

#define OPCODE_RD_REG 2         /*!< Instruction: read peripheral register (RTC_CNTL/RTC_IO/SARADC) */

#define RD_REG_PERIPH_RTC_CNTL 0    /*!< Identifier of RTC_CNTL peripheral for RD_REG and WR_REG instructions */
#define RD_REG_PERIPH_RTC_IO   1    /*!< Identifier of RTC_IO peripheral for RD_REG and WR_REG instructions */
#define RD_REG_PERIPH_SENS     2    /*!< Identifier of SARADC peripheral for RD_REG and WR_REG instructions */
#define RD_REG_PERIPH_RTC_I2C  3    /*!< Identifier of RTC_I2C peripheral for RD_REG and WR_REG instructions */

#define OPCODE_I2C 3            /*!< Instruction: read/write I2C */
#define SUB_OPCODE_I2C_RD 0     /*!< I2C read */
#define SUB_OPCODE_I2C_WR 1     /*!< I2C write */

#define OPCODE_DELAY 4          /*!< Instruction: delay (nop) for a given number of cycles */

#define OPCODE_ADC 5            /*!< Instruction: SAR ADC measurement */

#define OPCODE_ST 6             /*!< Instruction: store indirect to RTC memory */
#define SUB_OPCODE_ST 4         /*!< Store 32 bits, 16 MSBs contain PC, 16 LSBs contain value from source register */

#define OPCODE_ALU 7            /*!< Arithmetic instructions */
#define SUB_OPCODE_ALU_REG 0    /*!< Arithmetic instruction, both source values are in register */
#define SUB_OPCODE_ALU_IMM 1    /*!< Arithmetic instruction, one source value is an immediate */
#define SUB_OPCODE_ALU_CNT 2    /*!< Arithmetic instruction, stage counter and an immediate */
#define ALU_SEL_ADD 0           /*!< Addition */
#define ALU_SEL_SUB 1           /*!< Subtraction */
#define ALU_SEL_AND 2           /*!< Logical AND */
#define ALU_SEL_OR  3           /*!< Logical OR */
#define ALU_SEL_MOV 4           /*!< Copy value (immediate to destination register or source register to destination register */
#define ALU_SEL_LSH 5           /*!< Shift left by given number of bits */
#define ALU_SEL_RSH 6           /*!< Shift right by given number of bits */
#define ALU_SEL_SINC  0         /*!< Increment the stage counter */
#define ALU_SEL_SDEC  1         /*!< Decrement the stage counter */
#define ALU_SEL_SRST  2         /*!< Reset the stage counter */

#define OPCODE_BRANCH 8         /*!< Branch instructions */
#define SUB_OPCODE_BX  0        /*!< Branch to absolute PC (immediate or in register) */
#define SUB_OPCODE_BR  1        /*!< Branch to relative PC, conditional on R0 */
#define SUB_OPCODE_BS  2        /*!< Branch to relative PC, conditional on the stage counter */
#define BX_JUMP_TYPE_DIRECT 0   /*!< Unconditional jump */
#define BX_JUMP_TYPE_ZERO 1     /*!< Branch if last ALU result is zero */
#define BX_JUMP_TYPE_OVF 2      /*!< Branch if last ALU operation caused and overflow */
#define SUB_OPCODE_B  1         /*!< Branch to a relative offset */
#define B_CMP_L 0               /*!< Branch if R0 is less than an immediate */
#define B_CMP_GE 1              /*!< Branch if R0 is greater than or equal to an immediate */
#define JUMPS_LT 0              /*!< Branch if the stage counter < */
#define JUMPS_GE 1              /*!< Branch if the stage counter >= */
#define JUMPS_LE 2              /*!< Branch if the stage counter <= */

#define OPCODE_END 9            /*!< Stop executing the program */
#define SUB_OPCODE_END 0        /*!< Stop executing the program and optionally wake up the chip */
#define SUB_OPCODE_SLEEP 1      /*!< Stop executing the program and run it again after selected interval */

#define OPCODE_TSENS 10         /*!< Instruction: temperature sensor measurement */

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
        uint32_t unused1 : 6;       /*!< Unused */
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
        uint32_t unused2 : 7;       /*!< Unused */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_LD) */
    } ld;                           /*!< Format of LD instruction */

    struct {
        uint32_t unused : 28;       /*!< Unused */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_HALT) */
    } halt;                         /*!< Format of HALT instruction */

    struct {
        uint32_t dreg : 2;          /*!< Register which contains target PC, expressed in words (used if .reg == 1) */
        uint32_t addr : 11;         /*!< Target PC, expressed in words (used if .reg == 0) */
        uint32_t unused : 8;        /*!< Unused */
        uint32_t reg : 1;           /*!< Target PC in register (1) or immediate (0) */
        uint32_t type : 3;          /*!< Jump condition (BX_JUMP_TYPE_xxx) */
        uint32_t sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_BX) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_BRANCH) */
    } bx;                           /*!< Format of BRANCH instruction (absolute address) */

    struct {
        uint32_t imm : 16;          /*!< Immediate value to compare against */
        uint32_t cmp : 1;           /*!< Comparison to perform: B_CMP_L or B_CMP_GE */
        uint32_t offset : 7;        /*!< Absolute value of target PC offset w.r.t. current PC, expressed in words */
        uint32_t sign : 1;          /*!< Sign of target PC offset: 0: positive, 1: negative */
        uint32_t sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_B) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_BRANCH) */
    } b;                            /*!< Format of BRANCH instruction (relative address, conditional on R0) */

    struct {
        uint32_t imm : 8;           /*!< Immediate value to compare against */
        uint32_t unused : 7;        /*!< Unused */
        uint32_t cmp : 2;           /*!< Comparison to perform: JUMPS_LT, JUMPS_GE or JUMPS_LE */
        uint32_t offset : 7;        /*!< Absolute value of target PC offset w.r.t. current PC, expressed in words */
        uint32_t sign : 1;          /*!< Sign of target PC offset: 0: positive, 1: negative */
        uint32_t sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_BS) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_BRANCH) */
    } bs;                           /*!< Format of BRANCH instruction (relative address, conditional on the stage counter) */

    struct {
        uint32_t dreg : 2;          /*!< Destination register */
        uint32_t sreg : 2;          /*!< Register with operand A */
        uint32_t treg : 2;          /*!< Register with operand B */
        uint32_t unused : 15;       /*!< Unused */
        uint32_t sel : 4;           /*!< Operation to perform, one of ALU_SEL_xxx */
        uint32_t sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_ALU_REG) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_ALU) */
    } alu_reg;                      /*!< Format of ALU instruction (both sources are registers) */

    struct {
        uint32_t unused1 : 4;       /*!< Unused */
        uint32_t imm : 8;           /*!< Immediate value of operand */
        uint32_t unused2 : 9;       /*!< Unused */
        uint32_t sel : 4;           /*!< Operation to perform, one of ALU_SEL_Sxxx */
        uint32_t sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_ALU_CNT) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_ALU) */
    } alu_reg_s;                    /*!< Format of ALU instruction (stage counter and an immediate) */

    struct {
        uint32_t dreg : 2;          /*!< Destination register */
        uint32_t sreg : 2;          /*!< Register with operand A */
        uint32_t imm : 16;          /*!< Immediate value of operand B */
        uint32_t unused : 1;        /*!< Unused */
        uint32_t sel : 4;           /*!< Operation to perform, one of ALU_SEL_xxx */
        uint32_t sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_ALU_IMM) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_ALU) */
    } alu_imm;                      /*!< Format of ALU instruction (one source is an immediate) */

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
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_WR_REG) */
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
        uint32_t data : 8;          /*!< 8 bits of data for write operation */
        uint32_t low_bits : 3;      /*!< low bit of range for write operation (lower bits are masked) */
        uint32_t high_bits : 3;     /*!< high bit of range for write operation (higher bits are masked) */
        uint32_t i2c_sel : 4;       /*!< index of slave address register [7:0] */
        uint32_t unused : 1;        /*!< Unused */
        uint32_t rw : 1;            /*!< Write (1) or read (0) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_I2C) */
    } i2c;                          /*!< Format of I2C instruction */

    struct {
        uint32_t wakeup : 1;        /*!< Set to 1 to wake up chip */
        uint32_t unused : 24;       /*!< Unused */
        uint32_t sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_WAKEUP) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_END) */
    } end;                          /*!< Format of END instruction with wakeup */

    struct {
        uint32_t cycle_sel : 4;     /*!< Select which one of SARADC_ULP_CP_SLEEP_CYCx_REG to get the sleep duration from */
        uint32_t unused : 21;       /*!< Unused */
        uint32_t sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_SLEEP) */
        uint32_t opcode : 4;        /*!< Opcode (OPCODE_END) */
    } sleep;                        /*!< Format of END instruction with sleep */

    struct {
        uint32_t dreg : 2;          /*!< Destination register (for SUB_OPCODE_MACRO_LABELPC) > */
        uint32_t label : 16;        /*!< Label number */
        uint32_t unused : 6;        /*!< Unused */
        uint32_t sub_opcode : 4;    /*!< SUB_OPCODE_MACRO_LABEL or SUB_OPCODE_MACRO_BRANCH or SUB_OPCODE_MACRO_LABELPC */
        uint32_t opcode: 4;         /*!< Opcode (OPCODE_MACRO) */
    } macro;                        /*!< Format of tokens used by MACROs */

    uint32_t instruction;           /*!< Encoded instruction for ULP coprocessor */

};

typedef union ulp_insn ulp_insn_t;

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
static inline uint32_t SOC_REG_TO_ULP_PERIPH_SEL(uint32_t reg) {
    uint32_t ret = 3;
    if (reg < DR_REG_RTCCNTL_BASE) {
        assert(0 && "invalid register base");
    } else if (reg < DR_REG_RTCIO_BASE) {
        ret = RD_REG_PERIPH_RTC_CNTL;
    } else if (reg < DR_REG_SENS_BASE) {
        ret = RD_REG_PERIPH_RTC_IO;
    } else if (reg < DR_REG_RTC_I2C_BASE){
        ret = RD_REG_PERIPH_SENS;
    } else if (reg < DR_REG_IO_MUX_BASE){
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
 * RTC_CNTL_ULP_CP_SLP_TIMER_EN_S bit of RTC_CNTL_STATE0_REG
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
    I_WR_REG_BIT(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN_S, 0)
/**
 * Select the time interval used to run ULP program.
 *
 * This instructions selects which of the SENS_SLEEP_CYCLES_Sx
 * registers' value is used by the ULP program timer.
 * When the ULP program stops at I_HALT instruction, ULP program
 * timer start counting. When the counter reaches the value of
 * the selected SENS_SLEEP_CYCLES_Sx register, ULP program
 * start running again from the start address (passed to the ulp_run
 * function).
 * There are 5 SENS_SLEEP_CYCLES_Sx registers, so 0 <= timer_idx < 5.
 *
 * By default, SENS_SLEEP_CYCLES_S0 register is used by the ULP
 * program timer.
 */
#define I_SLEEP_CYCLE_SEL(timer_idx) { .sleep = { \
        .cycle_sel = timer_idx, \
        .unused = 0, \
        .sub_opcode = SUB_OPCODE_SLEEP, \
        .opcode = OPCODE_END } }

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
 * Store value from register reg_val into RTC memory.
 *
 * The value is written to an offset calculated by adding value of
 * reg_addr register and offset_ field (this offset is expressed in 32-bit words).
 * 32 bits written to RTC memory are built as follows:
 * - bits [31:21] hold the PC of current instruction, expressed in 32-bit words
 * - bits [20:16] = 5'b1
 * - bits [15:0] are assigned the contents of reg_val
 *
 * RTC_SLOW_MEM[addr + offset_] = { 5'b0, insn_PC[10:0], val[15:0] }
 */
#define I_ST(reg_val, reg_addr, offset_) { .st = { \
    .dreg = reg_val, \
    .sreg = reg_addr, \
    .unused1 = 0, \
    .offset = offset_, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ST, \
    .opcode = OPCODE_ST } }


/**
 * Load value from RTC memory into reg_dest register.
 *
 * Loads 16 LSBs from RTC memory word given by the sum of value in reg_addr and
 * value of offset_.
 */
#define I_LD(reg_dest, reg_addr, offset_) { .ld = { \
    .dreg = reg_dest, \
    .sreg = reg_addr, \
    .unused1 = 0, \
    .offset = offset_, \
    .unused2 = 0, \
    .opcode = OPCODE_LD } }


/**
 *  Branch relative if R0 less than immediate value.
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
 *  Branch relative if R0 greater or equal than immediate value.
 *
 *  pc_offset is expressed in words, and can be from -127 to 127
 *  imm_value is a 16-bit value to compare R0 against
 */
#define I_BGE(pc_offset, imm_value) { .b = { \
    .imm = imm_value, \
    .cmp = B_CMP_GE, \
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
    .unused = 0, \
    .reg = 1, \
    .type = BX_JUMP_TYPE_DIRECT, \
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
    .unused = 0, \
    .reg = 0, \
    .type = BX_JUMP_TYPE_DIRECT, \
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
    .unused = 0, \
    .reg = 1, \
    .type = BX_JUMP_TYPE_ZERO, \
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
    .unused = 0, \
    .reg = 0, \
    .type = BX_JUMP_TYPE_ZERO, \
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
    .unused = 0, \
    .reg = 1, \
    .type = BX_JUMP_TYPE_OVF, \
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
    .unused = 0, \
    .reg = 0, \
    .type = BX_JUMP_TYPE_OVF, \
    .sub_opcode = SUB_OPCODE_BX, \
    .opcode = OPCODE_BRANCH } }


/**
 * Addition: dest = src1 + src2
 */
#define I_ADDR(reg_dest, reg_src1, reg_src2) { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src1, \
    .treg = reg_src2, \
    .unused = 0, \
    .sel = ALU_SEL_ADD, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }

/**
 * Subtraction: dest = src1 - src2
 */
#define I_SUBR(reg_dest, reg_src1, reg_src2) { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src1, \
    .treg = reg_src2, \
    .unused = 0, \
    .sel = ALU_SEL_SUB, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }

/**
 * Logical AND: dest = src1 & src2
 */
#define I_ANDR(reg_dest, reg_src1, reg_src2) { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src1, \
    .treg = reg_src2, \
    .unused = 0, \
    .sel = ALU_SEL_AND, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }

/**
 * Logical OR: dest = src1 | src2
 */
#define I_ORR(reg_dest, reg_src1, reg_src2)  { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src1, \
    .treg = reg_src2, \
    .unused = 0, \
    .sel = ALU_SEL_OR, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }

/**
 * Copy: dest = src
 */
#define I_MOVR(reg_dest, reg_src) { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .treg = 0, \
    .unused = 0, \
    .sel = ALU_SEL_MOV, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }

/**
 * Logical shift left: dest = src << shift
 */
#define I_LSHR(reg_dest, reg_src, reg_shift)  { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .treg = reg_shift, \
    .unused = 0, \
    .sel = ALU_SEL_LSH, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }


/**
 * Logical shift right: dest = src >> shift
 */
#define I_RSHR(reg_dest, reg_src, reg_shift)  { .alu_reg = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .treg = reg_shift, \
    .unused = 0, \
    .sel = ALU_SEL_RSH, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }

/**
 * Add register and an immediate value: dest = src1 + imm
 */
#define I_ADDI(reg_dest, reg_src, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .imm = imm_, \
    .unused = 0, \
    .sel = ALU_SEL_ADD, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }


/**
 *  Subtract register and an immediate value: dest = src - imm
 */
#define I_SUBI(reg_dest, reg_src, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .imm = imm_, \
    .unused = 0, \
    .sel = ALU_SEL_SUB, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }

/**
 * Logical AND register and an immediate value: dest = src & imm
 */
#define I_ANDI(reg_dest, reg_src, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .imm = imm_, \
    .unused = 0, \
    .sel = ALU_SEL_AND, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }

/**
 * Logical OR register and an immediate value: dest = src | imm
 */
#define I_ORI(reg_dest, reg_src, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .imm = imm_, \
    .unused = 0, \
    .sel = ALU_SEL_OR, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }

/**
 * Copy an immediate value into register: dest = imm
 */
#define I_MOVI(reg_dest, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = 0, \
    .imm = imm_, \
    .unused = 0, \
    .sel = ALU_SEL_MOV, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }

/**
 * Logical shift left register value by an immediate: dest = src << imm
 */
#define I_LSHI(reg_dest, reg_src, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .imm = imm_, \
    .unused = 0, \
    .sel = ALU_SEL_LSH, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }


/**
 * Logical shift right register value by an immediate: dest = val >> imm
 */
#define I_RSHI(reg_dest, reg_src, imm_) { .alu_imm = { \
    .dreg = reg_dest, \
    .sreg = reg_src, \
    .imm = imm_, \
    .unused = 0, \
    .sel = ALU_SEL_RSH, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
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
    .dreg = 0, \
    .label = label_num, \
    .unused = 0, \
    .sub_opcode = SUB_OPCODE_MACRO_LABEL, \
    .opcode = OPCODE_MACRO } }

/**
 * Token macro used by M_B and M_BX macros. Not to be used directly.
 */
#define M_BRANCH(label_num) { .macro = { \
    .dreg = 0, \
    .label = label_num, \
    .unused = 0, \
    .sub_opcode = SUB_OPCODE_MACRO_BRANCH, \
    .opcode = OPCODE_MACRO } }

/**
 * Token macro used by M_MOVL macro. Not to be used directly.
 */
#define M_LABELPC(label_num) { .macro = { \
    .dreg = 0, \
    .label = label_num, \
    .unused = 0, \
    .sub_opcode = SUB_OPCODE_MACRO_LABELPC, \
    .opcode = OPCODE_MACRO } }

/**
 * Macro: Move the program counter at the given label into the register.
 * This address can then be used with I_BXR, I_BXZR, I_BXFR, etc.
 *
 * This macro generates two ulp_insn_t values separated by a comma, and should
 * be used when defining contents of ulp_insn_t arrays. First value is not a
 * real instruction; it is a token which is removed by ulp_process_macros_and_load
 * function.
 */
#define M_MOVL(reg_dest, label_num) \
    M_LABELPC(label_num), \
    I_MOVI(reg_dest, 0)

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
 * Macro: branch to label label_num if R0 is greater or equal than immediate value
 *
 * This macro generates two ulp_insn_t values separated by a comma, and should
 * be used when defining contents of ulp_insn_t arrays. First value is not a
 * real instruction; it is a token which is removed by ulp_process_macros_and_load
 * function.
 */
#define M_BGE(label_num, imm_value) \
    M_BRANCH(label_num), \
    I_BGE(0, imm_value)

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

/**
 * Increment the stage counter by immediate value
 */
#define I_STAGE_INC(imm_) { .alu_reg_s = { \
    .unused1 = 0, \
    .imm = imm_, \
    .unused2 = 0, \
    .sel = ALU_SEL_SINC, \
    .sub_opcode = SUB_OPCODE_ALU_CNT, \
    .opcode = OPCODE_ALU } }

/**
 * Decrement the stage counter by immediate value
 */
#define I_STAGE_DEC(imm_) { .alu_reg_s = { \
    .unused1 = 0, \
    .imm = imm_, \
    .unused2 = 0, \
    .sel = ALU_SEL_SDEC, \
    .sub_opcode = SUB_OPCODE_ALU_CNT, \
    .opcode = OPCODE_ALU } }

/**
 * Reset the stage counter
 */
#define I_STAGE_RST() { .alu_reg_s = { \
    .unused1 = 0, \
    .imm = 0, \
    .unused2 = 0, \
    .sel = ALU_SEL_SRST, \
    .sub_opcode = SUB_OPCODE_ALU_CNT, \
    .opcode = OPCODE_ALU } }

/**
 * Macro: branch to label if the stage counter is less than immediate value
 *
 * This macro generates two ulp_insn_t values separated by a comma, and should
 * be used when defining contents of ulp_insn_t arrays. First value is not a
 * real instruction; it is a token which is removed by ulp_process_macros_and_load
 * function.
 */
#define M_BSLT(label_num, imm_value) \
    M_BRANCH(label_num), \
    I_JUMPS(0, imm_value, JUMPS_LT)

/**
 * Macro: branch to label if the stage counter is greater than or equal to immediate value
 *
 * This macro generates two ulp_insn_t values separated by a comma, and should
 * be used when defining contents of ulp_insn_t arrays. First value is not a
 * real instruction; it is a token which is removed by ulp_process_macros_and_load
 * function.
 */
#define M_BSGE(label_num, imm_value) \
    M_BRANCH(label_num), \
    I_JUMPS(0, imm_value, JUMPS_GE)

/**
 * Macro: branch to label if the stage counter is less than or equal to immediate value
 *
 * This macro generates two ulp_insn_t values separated by a comma, and should
 * be used when defining contents of ulp_insn_t arrays. First value is not a
 * real instruction; it is a token which is removed by ulp_process_macros_and_load
 * function.
 */
#define M_BSLE(label_num, imm_value) \
    M_BRANCH(label_num), \
    I_JUMPS(0, imm_value, JUMPS_LE)

/**
 * Macro: branch to label if the stage counter is equal to immediate value.
 *  Implemented using two JUMPS instructions:
 *      JUMPS next, imm_value, LT
 *      JUMPS label_num, imm_value, LE
 *
 * This macro generates three ulp_insn_t values separated by commas, and should
 * be used when defining contents of ulp_insn_t arrays. Second value is not a
 * real instruction; it is a token which is removed by ulp_process_macros_and_load
 * function.
 */
#define M_BSEQ(label_num, imm_value) \
    I_JUMPS(2, imm_value, JUMPS_LT), \
    M_BRANCH(label_num), \
    I_JUMPS(0, imm_value, JUMPS_LE)

/**
 * Macro: branch to label if the stage counter is greater than immediate value.
 *  Implemented using two instructions:
 *      JUMPS next, imm_value, LE
 *      JUMPS label_num, imm_value, GE
 *
 * This macro generates three ulp_insn_t values separated by commas, and should
 * be used when defining contents of ulp_insn_t arrays. Second value is not a
 * real instruction; it is a token which is removed by ulp_process_macros_and_load
 * function.
 */
#define M_BSGT(label_num, imm_value) \
    I_JUMPS(2, imm_value, JUMPS_LE), \
    M_BRANCH(label_num), \
    I_JUMPS(0, imm_value, JUMPS_GE)

/**
 *  Branch relative if (stage counter [comp_type] [imm_value]) evaluates to true.
 *
 *  pc_offset is expressed in words, and can be from -127 to 127
 *  imm_value is an 8-bit value to compare the stage counter against
 *  comp_type is the type of comparison to perform: JUMPS_LT (<), JUMPS_GE (>=) or JUMPS_LE (<=)
 */
#define I_JUMPS(pc_offset, imm_value, comp_type) { .bs = { \
    .imm = imm_value, \
    .unused = 0, \
    .cmp = comp_type, \
    .offset = abs(pc_offset), \
    .sign = (pc_offset >= 0) ? 0 : 1, \
    .sub_opcode = SUB_OPCODE_BS, \
    .opcode = OPCODE_BRANCH } }

/**
 *  Perform an I2C transaction with a slave device.
 *  I_I2C_READ and I_I2C_WRITE are provided for convenience, instead of using this directly.
 * 
 *  Slave address (in 7-bit format) has to be set in advance into SENS_I2C_SLAVE_ADDRx register field, where x == slave_sel. 
 *  For read operations, 8 bits of read result is stored into R0 register.
 *  For write operations, val will be written to sub_addr at [high_bit:low_bit]. Bits outside of this range are masked.
 */
#define I_I2C_RW(sub_addr, val, low_bit, high_bit, slave_sel, rw_bit) { .i2c = {\
        .i2c_addr = sub_addr, \
        .data = val, \
        .low_bits = low_bit, \
        .high_bits = high_bit, \
        .i2c_sel = slave_sel, \
        .unused = 0, \
        .rw = rw_bit, \
        .opcode = OPCODE_I2C } }

/**
 * Read a byte from the sub address of an I2C slave, and store the result in R0.
 * 
 * Slave address (in 7-bit format) has to be set in advance into SENS_I2C_SLAVE_ADDRx register field, where x == slave_sel. 
 */
#define I_I2C_READ(slave_sel, sub_addr) I_I2C_RW(sub_addr, 0, 0, 0, slave_sel, SUB_OPCODE_I2C_RD)

/**
 * Write a byte to the sub address of an I2C slave.
 * 
 * Slave address (in 7-bit format) has to be set in advance into SENS_I2C_SLAVE_ADDRx register field, where x == slave_sel.
 */
#define I_I2C_WRITE(slave_sel, sub_addr, val) I_I2C_RW(sub_addr, val, 0, 7, slave_sel, SUB_OPCODE_I2C_WR)

#define RTC_SLOW_MEM ((uint32_t*) 0x50000000)       /*!< RTC slow memory, 8k size */


#ifdef __cplusplus
}
#endif
