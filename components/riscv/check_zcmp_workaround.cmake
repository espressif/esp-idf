# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# Post-link check for RISC-V ZCMP workaround compliance.
#
# Required variables:
#   ELF_FILE       - path to the linked executable
#   CMAKE_OBJDUMP  - objdump tool from the toolchain
#
# Pure CMake implementation: objdump output is written to a temporary <elf>.objdump file,
# scanned line-by-line with file(STRINGS), then removed. Empty objdump lines are skipped.
#
# For every instruction that clears mstatus.MIE (bit 3) this check verifies that a
# mintthresh write (csrrw ...,0x347,...) of 0xff appears earlier in the same function.
# Instructions considered to clear MIE:
#   csrrci mstatus,8                         - immediate clear of bit 3
#   csrrw  ...,mstatus,...                   - full mstatus overwrite
#   csrrc  ...,mstatus,...                   - register clear, only when the mask loaded
#                                              on the previous line has bit 3 set
# csrrs (bit set) is intentionally ignored: it cannot clear MIE.

set(_zcmp_disasm_file "${ELF_FILE}.objdump")
set(_mstatus_mie_re "csrrci.*mstatus,8|csrrw.*mstatus")
set(_mstatus_csrrc_re "csrrc.*mstatus")
set(_mintthresh_csrrw_re "csrrw.*,0x347,")
set(_mintthresh_value_base_re "(255|-256|0xff)")
set(_mintthresh_stack_move_re "(^|[ \t])(c\\.sw|c\\.lw|sw|lw)")
set(_mintthresh_call_re "(^|[ \t])(c\\.jal|jal)")
set(_function_start_re "^[0-9a-fA-F]+ <([^>]+)>:$")

# Extract the last comma-separated operand of a disassembly line (e.g. "li a5,8" -> "8").
function(_zcmp_last_operand line out_var)
    set(${out_var} "" PARENT_SCOPE)
    if(line MATCHES ",([^, \t]+)[ \t]*$")
        set(${out_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
    endif()
endfunction()

# TRUE when the numeric operand (decimal or hex) has bit 3 set, i.e. it would clear MIE.
function(_zcmp_operand_clears_mie operand out_var)
    set(_value 0)
    if(operand MATCHES "^-?(0[xX][0-9a-fA-F]+|[0-9]+)$")
        string(TOLOWER "${operand}" _norm)
        math(EXPR _value "${_norm}")
    endif()
    math(EXPR _bit3 "${_value} & 0x8")
    if(_bit3 EQUAL 0)
        set(${out_var} FALSE PARENT_SCOPE)
    else()
        set(${out_var} TRUE PARENT_SCOPE)
    endif()
endfunction()

if(NOT DEFINED ELF_FILE)
    message(FATAL_ERROR "ELF_FILE is not set")
endif()

if(NOT DEFINED CMAKE_OBJDUMP)
    message(FATAL_ERROR "CMAKE_OBJDUMP is not set")
endif()

if(NOT EXISTS "${ELF_FILE}")
    message(FATAL_ERROR "ELF file not found: ${ELF_FILE}")
endif()

execute_process(
    COMMAND "${CMAKE_OBJDUMP}" -d -M no-aliases "${ELF_FILE}"
    OUTPUT_FILE "${_zcmp_disasm_file}"
    ERROR_VARIABLE _zcmp_objdump_err
    RESULT_VARIABLE _zcmp_objdump_result
)

if(NOT _zcmp_objdump_result EQUAL 0)
    message(FATAL_ERROR "Failed to disassemble ${ELF_FILE}: ${_zcmp_objdump_err}")
endif()

file(STRINGS "${_zcmp_disasm_file}" _zcmp_lines)
file(REMOVE "${_zcmp_disasm_file}")

set(_zcmp_function "")
set(_zcmp_function_lines "")
set(_zcmp_skip_function FALSE)

# Functions exempt from the workaround (hand-audited / run with interrupts already masked).
set(_idf_trusted_function "rv_core_critical_regs_restore")

foreach(_line IN LISTS _zcmp_lines)
    string(STRIP "${_line}" _line)
    if(_line STREQUAL "")
        continue()
    endif()
    string(REPLACE ";" "\\;" _line "${_line}")

    # New function: reset the accumulated instruction window. The header line itself
    # is not part of the window.
    if(_line MATCHES "${_function_start_re}")
        set(_zcmp_function "${CMAKE_MATCH_1}")
        set(_zcmp_function_lines "")
        # Trusted functions are exempt: skip every instruction until the next function header.
        set(_zcmp_skip_function FALSE)
        foreach(_trusted_function IN LISTS _idf_trusted_function)
            if(_zcmp_function STREQUAL "${_trusted_function}")
                set(_zcmp_skip_function TRUE)
                break()
            endif()
        endforeach()
        continue()
    endif()

    if(_zcmp_skip_function)
        continue()
    endif()

    list(APPEND _zcmp_function_lines "${_line}")
    list(LENGTH _zcmp_function_lines _line_count)
    math(EXPR _cur_index "${_line_count} - 1")

    # ---- Does this line clear mstatus.MIE? ----
    set(_clears_mie FALSE)
    if(_line MATCHES "${_mstatus_mie_re}")
        set(_clears_mie TRUE)
    elseif(_line MATCHES "${_mstatus_csrrc_re}")
        set(_prev_operand "")
        if(_cur_index GREATER 0)
            math(EXPR _prev_index "${_cur_index} - 1")
            list(GET _zcmp_function_lines ${_prev_index} _prev_line)
            _zcmp_last_operand("${_prev_line}" _prev_operand)
        endif()
        _zcmp_operand_clears_mie("${_prev_operand}" _clears_mie)
    endif()

    if(NOT _clears_mie)
        continue()
    endif()

    # ---- Require a preceding mintthresh write of 0xff in this function ----
    set(_has_mintthresh FALSE)
    foreach(_scan_index RANGE 0 ${_cur_index})
        list(GET _zcmp_function_lines ${_scan_index} _scan_line)
        if(NOT _scan_line MATCHES "${_mintthresh_csrrw_re}")
            continue()
        endif()

        # Found the mintthresh write. Extract its source register to narrow the value search.
        _zcmp_last_operand("${_scan_line}" _mintthresh_src_reg)
        if(_mintthresh_src_reg STREQUAL "")
            continue()
        endif()
        set(_mintthresh_value_re "(^|[ \t])${_mintthresh_src_reg},(zero,)?${_mintthresh_value_base_re}")

        # Walk backward to the instruction that loaded 0xff into that register,
        # skipping sw/lw stack traffic (-O0) and (c.)jal calls (-Os) the compiler may emit.
        math(EXPR _value_index "${_scan_index} - 1")
        while(_value_index GREATER_EQUAL 0)
            list(GET _zcmp_function_lines ${_value_index} _value_line)
            if(_value_line MATCHES "${_mintthresh_stack_move_re}" OR
               _value_line MATCHES "${_mintthresh_call_re}")
                math(EXPR _value_index "${_value_index} - 1")
                continue()
            endif()
            if(NOT _value_line MATCHES "${_mintthresh_value_re}")
                message(FATAL_ERROR
                    "ZCMP workaround violation: mintthresh write not preceded by value 0xff "
                    "(or equivalent 255/-256) in function ${_zcmp_function} in ${ELF_FILE}:\n${_value_line}")
            endif()
            set(_has_mintthresh TRUE)
            break()
        endwhile()
        if(_has_mintthresh)
            break()
        endif()
    endforeach()

    if(NOT _has_mintthresh)
        message(FATAL_ERROR
            "ZCMP workaround violation: mstatus access without mintthresh write "
            "earlier in function ${_zcmp_function} in ${ELF_FILE}:\n${_line}")
    endif()
endforeach()
