#
# Copyright 2021 Espressif Systems (Shanghai) CO., LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

from construct import Int16ul, Int32ul, Int64ul, Struct

from . import BaseArchMethodsMixin, BaseTargetMethods, ESPCoreDumpLoaderError

try:
    from typing import Any, Optional, Tuple
except ImportError:
    pass

INVALID_CAUSE_VALUE = 0xFFFF
XCHAL_EXCCAUSE_NUM = 64

# Exception cause dictionary to get translation of exccause register
# From 4.4.1.5 table 4-64 Exception Causes of Xtensa
# Instruction Set Architecture (ISA) Reference Manual

XTENSA_EXCEPTION_CAUSE_DICT = {
    0: ('IllegalInstructionCause', 'Illegal instruction'),
    1: ('SyscallCause', 'SYSCALL instruction'),
    2: ('InstructionFetchErrorCause',
        'Processor internal physical address or data error during instruction fetch. (See EXCVADDR for more information)'),
    3: ('LoadStoreErrorCause',
        'Processor internal physical address or data error during load or store. (See EXCVADDR for more information)'),
    4: ('Level1InterruptCause', 'Level-1 interrupt as indicated by set level-1 bits in the INTERRUPT register'),
    5: ('AllocaCause', 'MOVSP instruction, if caller`s registers are not in the register file'),
    6: ('IntegerDivideByZeroCause', 'QUOS: QUOU, REMS: or REMU divisor operand is zero'),
    8: ('PrivilegedCause', 'Attempt to execute a privileged operation when CRING ? 0'),
    9: ('LoadStoreAlignmentCause', 'Load or store to an unaligned address. (See EXCVADDR for more information)'),
    12: ('InstrPIFDataErrorCause', 'PIF data error during instruction fetch. (See EXCVADDR for more information)'),
    13: ('LoadStorePIFDataErrorCause',
         'Synchronous PIF data error during LoadStore access. (See EXCVADDR for more information)'),
    14: ('InstrPIFAddrErrorCause', 'PIF address error during instruction fetch. (See EXCVADDR for more information)'),
    15: ('LoadStorePIFAddrErrorCause',
         'Synchronous PIF address error during LoadStore access. (See EXCVADDR for more information)'),
    16: ('InstTLBMissCause', 'Error during Instruction TLB refill. (See EXCVADDR for more information)'),
    17: ('InstTLBMultiHitCause', 'Multiple instruction TLB entries matched. (See EXCVADDR for more information)'),
    18: ('InstFetchPrivilegeCause',
         'An instruction fetch referenced a virtual address at a ring level less than CRING. (See EXCVADDR for more information)'),
    20: ('InstFetchProhibitedCause',
         'An instruction fetch referenced a page mapped with an attribute that does not permit instruction fetch (EXCVADDR).'),
    24: ('LoadStoreTLBMissCause', 'Error during TLB refill for a load or store. (See EXCVADDR for more information)'),
    25: ('LoadStoreTLBMultiHitCause',
         'Multiple TLB entries matched for a load or store. (See EXCVADDR for more information)'),
    26: ('LoadStorePrivilegeCause',
         'A load or store referenced a virtual address at a ring level less than CRING. (See EXCVADDR for more information)'),
    28: ('LoadProhibitedCause',
         'A load referenced a page mapped with an attribute that does not permit loads. (See EXCVADDR for more information)'),
    29: ('StoreProhibitedCause',
         'A store referenced a page mapped with an attribute that does not permit stores [Region Protection Option or MMU Option].'),
    32: ('Coprocessor0Disabled', 'Coprocessor 0 instruction when cp0 disabled'),
    33: ('Coprocessor1Disabled', 'Coprocessor 1 instruction when cp1 disabled'),
    34: ('Coprocessor2Disabled', 'Coprocessor 2 instruction when cp2 disabled'),
    35: ('Coprocessor3Disabled', 'Coprocessor 3 instruction when cp3 disabled'),
    36: ('Coprocessor4Disabled', 'Coprocessor 4 instruction when cp4 disabled'),
    37: ('Coprocessor5Disabled', 'Coprocessor 5 instruction when cp5 disabled'),
    38: ('Coprocessor6Disabled', 'Coprocessor 6 instruction when cp6 disabled'),
    39: ('Coprocessor7Disabled', 'Coprocessor 7 instruction when cp7 disabled'),
    INVALID_CAUSE_VALUE: (
        'InvalidCauseRegister', 'Invalid EXCCAUSE register value or current task is broken and was skipped'),
    # ESP panic pseudo reasons
    XCHAL_EXCCAUSE_NUM + 0: ('UnknownException', 'Unknown exception'),
    XCHAL_EXCCAUSE_NUM + 1: ('DebugException', 'Unhandled debug exception'),
    XCHAL_EXCCAUSE_NUM + 2: ('DoubleException', 'Double exception'),
    XCHAL_EXCCAUSE_NUM + 3: ('KernelException', 'Unhandled kernel exception'),
    XCHAL_EXCCAUSE_NUM + 4: ('CoprocessorException', 'Coprocessor exception'),
    XCHAL_EXCCAUSE_NUM + 5: ('InterruptWDTTimoutCPU0', 'Interrupt wdt timeout on CPU0'),
    XCHAL_EXCCAUSE_NUM + 6: ('InterruptWDTTimoutCPU1', 'Interrupt wdt timeout on CPU1'),
    XCHAL_EXCCAUSE_NUM + 7: ('CacheError', 'Cache disabled but cached memory region accessed'),
}


class ExceptionRegisters(object):
    # extra regs IDs used in EXTRA_INFO note
    EXCCAUSE_IDX = 0
    EXCVADDR_IDX = 1
    EPC1_IDX = 177
    EPC2_IDX = 178
    EPC3_IDX = 179
    EPC4_IDX = 180
    EPC5_IDX = 181
    EPC6_IDX = 182
    EPC7_IDX = 183
    EPS2_IDX = 194
    EPS3_IDX = 195
    EPS4_IDX = 196
    EPS5_IDX = 197
    EPS6_IDX = 198
    EPS7_IDX = 199

    @property
    def registers(self):  # type: () -> dict[str, int]
        return {k: v for k, v in self.__class__.__dict__.items()
                if not k.startswith('__') and isinstance(v, int)}


# Following structs are based on source code
# IDF_PATH/components/espcoredump/src/core_dump_port.c
PrStatus = Struct(
    'si_signo' / Int32ul,
    'si_code' / Int32ul,
    'si_errno' / Int32ul,
    'pr_cursig' / Int16ul,
    'pr_pad0' / Int16ul,
    'pr_sigpend' / Int32ul,
    'pr_sighold' / Int32ul,
    'pr_pid' / Int32ul,
    'pr_ppid' / Int32ul,
    'pr_pgrp' / Int32ul,
    'pr_sid' / Int32ul,
    'pr_utime' / Int64ul,
    'pr_stime' / Int64ul,
    'pr_cutime' / Int64ul,
    'pr_cstime' / Int64ul,
)


def print_exc_regs_info(extra_info):  # type: (list[int]) -> None
    """
    Print the register info by parsing extra_info
    :param extra_info: extra info data str
    :return: None
    """
    exccause = extra_info[1 + 2 * ExceptionRegisters.EXCCAUSE_IDX + 1]
    exccause_str = XTENSA_EXCEPTION_CAUSE_DICT.get(exccause)
    if not exccause_str:
        exccause_str = ('Invalid EXCCAUSE code', 'Invalid EXCAUSE description or not found.')
    print('exccause       0x%x (%s)' % (exccause, exccause_str[0]))
    print('excvaddr       0x%x' % extra_info[1 + 2 * ExceptionRegisters.EXCVADDR_IDX + 1])

    # skip crashed_task_tcb, exccause, and excvaddr
    for i in range(5, len(extra_info), 2):
        if (extra_info[i] >= ExceptionRegisters.EPC1_IDX and extra_info[i] <= ExceptionRegisters.EPC7_IDX):
            print('epc%d           0x%x' % ((extra_info[i] - ExceptionRegisters.EPC1_IDX + 1), extra_info[i + 1]))

    # skip crashed_task_tcb, exccause, and excvaddr
    for i in range(5, len(extra_info), 2):
        if (extra_info[i] >= ExceptionRegisters.EPS2_IDX and extra_info[i] <= ExceptionRegisters.EPS7_IDX):
            print('eps%d           0x%x' % ((extra_info[i] - ExceptionRegisters.EPS2_IDX + 2), extra_info[i + 1]))


# from "gdb/xtensa-tdep.h"
# typedef struct
# {
# 0    xtensa_elf_greg_t pc;
# 1    xtensa_elf_greg_t ps;
# 2    xtensa_elf_greg_t lbeg;
# 3    xtensa_elf_greg_t lend;
# 4    xtensa_elf_greg_t lcount;
# 5    xtensa_elf_greg_t sar;
# 6    xtensa_elf_greg_t windowstart;
# 7    xtensa_elf_greg_t windowbase;
# 8..63 xtensa_elf_greg_t reserved[8+48];
# 64   xtensa_elf_greg_t ar[64];
# } xtensa_elf_gregset_t;
REG_PC_IDX = 0
REG_PS_IDX = 1
REG_LB_IDX = 2
REG_LE_IDX = 3
REG_LC_IDX = 4
REG_SAR_IDX = 5
# REG_WS_IDX = 6
# REG_WB_IDX = 7
REG_AR_START_IDX = 64
# REG_AR_NUM = 64
# FIXME: acc to xtensa_elf_gregset_t number of regs must be 128,
# but gdb complains when it less then 129
REG_NUM = 129

# XT_SOL_EXIT = 0
XT_SOL_PC = 1
XT_SOL_PS = 2
# XT_SOL_NEXT = 3
XT_SOL_AR_START = 4
XT_SOL_AR_NUM = 4
# XT_SOL_FRMSZ = 8

XT_STK_EXIT = 0
XT_STK_PC = 1
XT_STK_PS = 2
XT_STK_AR_START = 3
XT_STK_AR_NUM = 16
XT_STK_SAR = 19
XT_STK_EXCCAUSE = 20
XT_STK_EXCVADDR = 21
XT_STK_LBEG = 22
XT_STK_LEND = 23
XT_STK_LCOUNT = 24
XT_STK_FRMSZ = 25


class XtensaMethodsMixin(BaseArchMethodsMixin):
    @staticmethod
    def get_registers_from_stack(data, grows_down):
        # type: (bytes, bool) -> Tuple[list[int], Optional[dict[int, int]]]
        extra_regs = {v: 0 for v in ExceptionRegisters().registers.values()}
        regs = [0] * REG_NUM
        # TODO: support for growing up stacks
        if not grows_down:
            raise ESPCoreDumpLoaderError('Growing up stacks are not supported for now!')
        ex_struct = Struct(
            'stack' / Int32ul[XT_STK_FRMSZ]
        )
        if len(data) < ex_struct.sizeof():
            raise ESPCoreDumpLoaderError('Too small stack to keep frame: %d bytes!' % len(data))

        stack = ex_struct.parse(data).stack
        # Stack frame type indicator is always the first item
        rc = stack[XT_STK_EXIT]
        if rc != 0:
            regs[REG_PC_IDX] = stack[XT_STK_PC]
            regs[REG_PS_IDX] = stack[XT_STK_PS]
            for i in range(XT_STK_AR_NUM):
                regs[REG_AR_START_IDX + i] = stack[XT_STK_AR_START + i]
            regs[REG_SAR_IDX] = stack[XT_STK_SAR]
            regs[REG_LB_IDX] = stack[XT_STK_LBEG]
            regs[REG_LE_IDX] = stack[XT_STK_LEND]
            regs[REG_LC_IDX] = stack[XT_STK_LCOUNT]
            # FIXME: crashed and some running tasks (e.g. prvIdleTask) have EXCM bit set
            #   and GDB can not unwind callstack properly (it implies not windowed call0)
            if regs[REG_PS_IDX] & (1 << 5):
                regs[REG_PS_IDX] &= ~(1 << 4)
            if stack[XT_STK_EXCCAUSE] in XTENSA_EXCEPTION_CAUSE_DICT:
                extra_regs[ExceptionRegisters.EXCCAUSE_IDX] = stack[XT_STK_EXCCAUSE]
            else:
                extra_regs[ExceptionRegisters.EXCCAUSE_IDX] = INVALID_CAUSE_VALUE
            extra_regs[ExceptionRegisters.EXCVADDR_IDX] = stack[XT_STK_EXCVADDR]
        else:
            regs[REG_PC_IDX] = stack[XT_SOL_PC]
            regs[REG_PS_IDX] = stack[XT_SOL_PS]
            for i in range(XT_SOL_AR_NUM):
                regs[REG_AR_START_IDX + i] = stack[XT_SOL_AR_START + i]
            # nxt = stack[XT_SOL_NEXT]
        return regs, extra_regs

    @staticmethod
    def build_prstatus_data(tcb_addr, task_regs):  # type: (int, list[int]) -> Any
        return PrStatus.build({
            'si_signo': 0,
            'si_code': 0,
            'si_errno': 0,
            'pr_cursig': 0,  # TODO: set sig only for current/failed task
            'pr_pad0': 0,
            'pr_sigpend': 0,
            'pr_sighold': 0,
            'pr_pid': tcb_addr,
            'pr_ppid': 0,
            'pr_pgrp': 0,
            'pr_sid': 0,
            'pr_utime': 0,
            'pr_stime': 0,
            'pr_cutime': 0,
            'pr_cstime': 0,
        }) + Int32ul[len(task_regs)].build(task_regs)


class Esp32Methods(BaseTargetMethods, XtensaMethodsMixin):
    TARGET = 'esp32'


class Esp32S2Methods(BaseTargetMethods, XtensaMethodsMixin):
    TARGET = 'esp32s2'


class Esp32S3Methods(BaseTargetMethods, XtensaMethodsMixin):
    TARGET = 'esp32s3'
