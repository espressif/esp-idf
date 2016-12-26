// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp32/ulp.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"

#include "sdkconfig.h"

static const char* TAG = "ulp";

typedef struct {
    uint32_t label : 16;
    uint32_t addr : 11;
    uint32_t unused : 1;
    uint32_t type : 4;
} reloc_info_t;

#define RELOC_TYPE_LABEL   0
#define RELOC_TYPE_BRANCH  1

/* This record means: there is a label at address
 * insn_addr, with number label_num.
 */
#define RELOC_INFO_LABEL(label_num, insn_addr) (reloc_info_t) { \
    .label = label_num, \
    .addr = insn_addr, \
    .unused = 0, \
    .type = RELOC_TYPE_LABEL }

/* This record means: there is a branch instruction at
 * insn_addr, it needs to be changed to point to address
 * of label label_num.
 */
#define RELOC_INFO_BRANCH(label_num, insn_addr) (reloc_info_t) { \
    .label = label_num, \
    .addr = insn_addr, \
    .unused = 0, \
    .type = RELOC_TYPE_BRANCH }


/* Processing branch and label macros involves four steps:
 *
 * 1. Iterate over program and count all instructions
 *    with "macro" opcode. Allocate relocations array
 *    with number of entries equal to number of macro
 *    instructions.
 *
 * 2. Remove all fake instructions with "macro" opcode
 *    and record their locations into relocations array.
 *    Removal is done using two pointers. Instructions
 *    are read from read_ptr, and written to write_ptr.
 *    When a macro instruction is encountered,
 *    its contents are recorded into the appropriate
 *    table, and then read_ptr is advanced again.
 *    When a real instruction is encountered, it is
 *    read via read_ptr and written to write_ptr.
 *    In the end, all macro instructions are removed,
 *    size of the program (expressed in words) is
 *    reduced by the total number of macro instructions
 *    which were present.
 *
 * 3. Sort relocations array by label number, and then
 *    by type ("label" or "branch") if label numbers
 *    match. This is done to simplify lookup on the next
 *    step.
 *
 * 4. Iterate over entries of relocations table.
 *    For each label number, label entry comes first
 *    because the array was sorted at the previous step.
 *    Label address is recorded, and all subsequent
 *    "branch" entries which point to the same label number
 *    are processed. For each branch entry, correct offset
 *    or absolute address is calculated, depending on branch
 *    type, and written into the appropriate field of
 *    the instruction.
 *
 */

static esp_err_t do_single_reloc(ulp_insn_t* program, uint32_t load_addr,
        reloc_info_t label_info, reloc_info_t branch_info)
{
    size_t insn_offset = branch_info.addr - load_addr;
    ulp_insn_t* insn = &program[insn_offset];
    // B and BX have the same layout of opcode/sub_opcode fields,
    // and share the same opcode
    assert(insn->b.opcode == OPCODE_BRANCH
            && "branch macro was applied to a non-branch instruction");
    switch (insn->b.sub_opcode) {
        case SUB_OPCODE_B: {
            int32_t offset = ((int32_t) label_info.addr) - ((int32_t) branch_info.addr);
            uint32_t abs_offset = abs(offset);
            uint32_t sign = (offset >= 0) ? 0 : 1;
            if (abs_offset > 127) {
                ESP_LOGW(TAG, "target out of range: branch from %x to %x",
                        branch_info.addr, label_info.addr);
                return ESP_ERR_ULP_BRANCH_OUT_OF_RANGE;
            }
            insn->b.offset = abs_offset;
            insn->b.sign = sign;
            break;
        }
        case SUB_OPCODE_BX: {
            assert(insn->bx.reg == 0 &&
                    "relocation applied to a jump with offset in register");
            insn->bx.addr = label_info.addr;
            break;
        }
        default:
            assert(false && "unexpected sub-opcode");
    }
    return ESP_OK;
}

esp_err_t ulp_process_macros_and_load(uint32_t load_addr, const ulp_insn_t* program, size_t* psize)
{
    const ulp_insn_t* read_ptr = program;
    const ulp_insn_t* end = program + *psize;
    size_t macro_count = 0;
    // step 1: calculate number of macros
    while (read_ptr < end) {
        ulp_insn_t r_insn = *read_ptr;
        if (r_insn.macro.opcode == OPCODE_MACRO) {
            ++macro_count;
        }
        ++read_ptr;
    }
    size_t real_program_size = *psize - macro_count;
    const size_t ulp_mem_end = CONFIG_ULP_COPROC_RESERVE_MEM / sizeof(ulp_insn_t);
    if (load_addr > ulp_mem_end) {
        ESP_LOGW(TAG, "invalid load address %x, max is %x",
                load_addr, ulp_mem_end);
        return ESP_ERR_ULP_INVALID_LOAD_ADDR;
    }
    if (real_program_size + load_addr > ulp_mem_end) {
        ESP_LOGE(TAG, "program too big: %d words, max is %d words",
                real_program_size, ulp_mem_end);
        return ESP_ERR_ULP_SIZE_TOO_BIG;
    }
    // If no macros found, copy the program and return.
    if (macro_count == 0) {
        memcpy(((ulp_insn_t*) RTC_SLOW_MEM) + load_addr, program, *psize * sizeof(ulp_insn_t));
        return ESP_OK;
    }
    reloc_info_t* reloc_info =
            (reloc_info_t*) malloc(sizeof(reloc_info_t) * macro_count);
    if (reloc_info == NULL) {
        return ESP_ERR_NO_MEM;
    }

    // step 2: record macros into reloc_info array
    // and remove them from then program
    read_ptr = program;
    ulp_insn_t* output_program = ((ulp_insn_t*) RTC_SLOW_MEM) + load_addr;
    ulp_insn_t* write_ptr = output_program;
    uint32_t cur_insn_addr = load_addr;
    reloc_info_t* cur_reloc = reloc_info;
    while (read_ptr < end) {
        ulp_insn_t r_insn = *read_ptr;
        if (r_insn.macro.opcode == OPCODE_MACRO) {
            switch(r_insn.macro.sub_opcode) {
                case SUB_OPCODE_MACRO_LABEL:
                    *cur_reloc = RELOC_INFO_LABEL(r_insn.macro.label,
                            cur_insn_addr);
                    break;
                case SUB_OPCODE_MACRO_BRANCH:
                    *cur_reloc = RELOC_INFO_BRANCH(r_insn.macro.label,
                            cur_insn_addr);
                    break;
                default:
                    assert(0 && "invalid sub_opcode for macro insn");
            }
            ++read_ptr;
            assert(read_ptr != end && "program can not end with macro insn");
            ++cur_reloc;
        } else {
            // normal instruction (not a macro)
            *write_ptr = *read_ptr;
            ++read_ptr;
            ++write_ptr;
            ++cur_insn_addr;
        }
    }

    // step 3: sort relocations array
    int reloc_sort_func(const void* p_lhs, const void* p_rhs) {
        const reloc_info_t lhs = *(const reloc_info_t*) p_lhs;
        const reloc_info_t rhs = *(const reloc_info_t*) p_rhs;
        if (lhs.label < rhs.label) {
            return -1;
        } else if (lhs.label > rhs.label) {
            return 1;
        }
        // label numbers are equal
        if (lhs.type < rhs.type) {
            return -1;
        } else if (lhs.type > rhs.type) {
            return 1;
        }

        // both label number and type are equal
        return 0;
    }
    qsort(reloc_info, macro_count, sizeof(reloc_info_t),
            reloc_sort_func);

    // step 4: walk relocations array and fix instructions
    reloc_info_t* reloc_end = reloc_info + macro_count;
    cur_reloc = reloc_info;
    while(cur_reloc < reloc_end) {
        reloc_info_t label_info = *cur_reloc;
        assert(label_info.type == RELOC_TYPE_LABEL);
        ++cur_reloc;
        while (cur_reloc < reloc_end) {
            if (cur_reloc->type == RELOC_TYPE_LABEL) {
                if(cur_reloc->label == label_info.label) {
                    ESP_LOGE(TAG, "duplicate label definition: %d",
                            label_info.label);
                    free(reloc_info);
                    return ESP_ERR_ULP_DUPLICATE_LABEL;
                }
                break;
            }
            if (cur_reloc->label != label_info.label) {
                ESP_LOGE(TAG, "branch to an inexistent label: %d",
                        cur_reloc->label);
                free(reloc_info);
                return ESP_ERR_ULP_UNDEFINED_LABEL;
            }
            esp_err_t rc = do_single_reloc(output_program, load_addr,
                    label_info, *cur_reloc);
            if (rc != ESP_OK) {
                free(reloc_info);
                return rc;
            }
            ++cur_reloc;
        }
    }
    free(reloc_info);
    *psize = real_program_size;
    return ESP_OK;
}

esp_err_t ulp_run(uint32_t entry_point)
{
    // disable ULP timer
    CLEAR_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
    // set entry point
    SET_PERI_REG_BITS(SENS_SAR_START_FORCE_REG, SENS_PC_INIT_V, entry_point, SENS_PC_INIT_S);
    // disable force start
    CLEAR_PERI_REG_MASK(SENS_SAR_START_FORCE_REG, SENS_ULP_CP_FORCE_START_TOP_M);
    // make sure voltage is raised when RTC 8MCLK is enabled
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FOLW_8M);
    // enable ULP timer
    SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
    return ESP_OK;
}
