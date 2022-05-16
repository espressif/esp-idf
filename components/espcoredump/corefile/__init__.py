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

__version__ = '0.4-dev'

import abc
import os
from abc import abstractmethod
from importlib import import_module

from future.utils import with_metaclass

try:
    from typing import Optional, Tuple
except ImportError:
    pass

IDF_PATH = os.path.normpath(os.getenv('IDF_PATH', '.'))
XTENSA_TARGETS = ['esp32', 'esp32s2', 'esp32s3']
RISCV_TARGETS = ['esp32c3']
SUPPORTED_TARGETS = XTENSA_TARGETS + RISCV_TARGETS


class ESPCoreDumpError(RuntimeError):
    pass


class ESPCoreDumpLoaderError(ESPCoreDumpError):
    pass


class BaseArchMethodsMixin(with_metaclass(abc.ABCMeta)):  # type: ignore
    @staticmethod
    @abstractmethod
    def get_registers_from_stack(data, grows_down):
        # type: (bytes, bool) -> Tuple[list[int], Optional[dict[int, int]]]
        """
        Parse stack data, growing up stacks are not supported for now.
        :param data: stack data
        :param grows_down: stack grow direction
        :return: return tuple (regs, exception_regs)
        """
        pass

    @staticmethod
    @abstractmethod
    def build_prstatus_data(tcb_addr, task_regs):  # type: (int, list[int]) -> str
        """
        Build PrStatus note section
        :param tcb_addr: tcb addr
        :param task_regs: registers
        :return: str
        """
        pass


class BaseTargetMethods(with_metaclass(abc.ABCMeta, BaseArchMethodsMixin)):  # type: ignore
    UNKNOWN = 'unknown'
    TARGET = UNKNOWN

    COREDUMP_FAKE_STACK_START = 0x20000000
    COREDUMP_FAKE_STACK_LIMIT = 0x30000000
    COREDUMP_MAX_TASK_STACK_SIZE = 64 * 1024

    def __init__(self):  # type: () -> None
        if self.TARGET == self.UNKNOWN:
            raise ValueError('Please use the derived child-class with valid TARGET')

        self._set_attr_from_soc_header()

    def _set_attr_from_soc_header(self):  # type: () -> None
        module = import_module('corefile.soc_headers.{}'.format(self.TARGET))
        for k, v in module.__dict__.items():
            if k.startswith('SOC_'):
                setattr(self, k, v)

    def _esp_ptr_in_dram(self, addr):  # type: (int) -> bool
        return self.SOC_DRAM_LOW <= addr < self.SOC_DRAM_HIGH  # type: ignore

    def _esp_ptr_in_iram(self, addr):  # type: (int) -> bool
        return self.SOC_IRAM_LOW <= addr < self.SOC_IRAM_HIGH  # type: ignore

    def _esp_ptr_in_rtc_slow(self, addr):  # type: (int) -> bool
        return self.SOC_RTC_DATA_LOW <= addr < self.SOC_RTC_DATA_HIGH  # type: ignore

    def _esp_ptr_in_rtc_dram_fast(self, addr):  # type: (int) -> bool
        return self.SOC_RTC_DRAM_LOW <= addr < self.SOC_RTC_DRAM_HIGH  # type: ignore

    def tcb_is_sane(self, tcb_addr, tcb_size):  # type: (int, int) -> bool
        for func in [self._esp_ptr_in_dram,
                     self._esp_ptr_in_iram,
                     self._esp_ptr_in_rtc_slow,
                     self._esp_ptr_in_rtc_dram_fast]:
            res = func(tcb_addr) and func(tcb_addr + tcb_size - 1)
            if res:
                return True
        return False

    def _esp_stack_ptr_in_dram(self, addr):  # type: (int) -> bool
        return not (addr < self.SOC_DRAM_LOW + 0x10
                    or addr > self.SOC_DRAM_HIGH - 0x10
                    or (addr & 0xF) != 0)

    def stack_is_sane(self, stack_start, stack_end):  # type: (int, int) -> bool
        return (self._esp_stack_ptr_in_dram(stack_start)
                and self._esp_ptr_in_dram(stack_end)
                and stack_start < stack_end
                and (stack_end - stack_start) < self.COREDUMP_MAX_TASK_STACK_SIZE)

    def addr_is_fake(self, addr):  # type: (int) -> bool
        return (self.COREDUMP_FAKE_STACK_START <= addr < self.COREDUMP_FAKE_STACK_LIMIT
                or addr > 2 ** 31 - 1)
