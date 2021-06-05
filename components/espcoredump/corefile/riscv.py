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

from construct import Int16ul, Int32ul, Padding, Struct
from corefile import BaseArchMethodsMixin, BaseTargetMethods, ESPCoreDumpLoaderError

try:
    from typing import Any, Optional, Tuple
except ImportError:
    pass

RISCV_GP_REGS_COUNT = 32
PRSTATUS_SIZE = 204
PRSTATUS_OFFSET_PR_CURSIG = 12
PRSTATUS_OFFSET_PR_PID = 24
PRSTATUS_OFFSET_PR_REG = 72
ELF_GREGSET_T_SIZE = 128

PrStruct = Struct(
    Padding(PRSTATUS_OFFSET_PR_CURSIG),
    'pr_cursig' / Int16ul,
    Padding(PRSTATUS_OFFSET_PR_PID - PRSTATUS_OFFSET_PR_CURSIG - Int16ul.sizeof()),
    'pr_pid' / Int32ul,
    Padding(PRSTATUS_OFFSET_PR_REG - PRSTATUS_OFFSET_PR_PID - Int32ul.sizeof()),
    'regs' / Int32ul[RISCV_GP_REGS_COUNT],
    Padding(PRSTATUS_SIZE - PRSTATUS_OFFSET_PR_REG - ELF_GREGSET_T_SIZE)
)


class RiscvMethodsMixin(BaseArchMethodsMixin):
    @staticmethod
    def get_registers_from_stack(data, grows_down):
        # type: (bytes, bool) -> Tuple[list[int], Optional[dict[int, int]]]
        regs = Int32ul[RISCV_GP_REGS_COUNT].parse(data)
        if not grows_down:
            raise ESPCoreDumpLoaderError('Growing up stacks are not supported for now!')
        return regs, None

    @staticmethod
    def build_prstatus_data(tcb_addr, task_regs):  # type: (int, list[int]) -> Any
        return PrStruct.build({
            'pr_cursig': 0,
            'pr_pid': tcb_addr,
            'regs': task_regs,
        })


class Esp32c3Methods(BaseTargetMethods, RiscvMethodsMixin):
    TARGET = 'esp32c3'
