#
# Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

from abc import abstractmethod


class ESPCoreDumpError(RuntimeError):
    pass


class ESPCoreDumpLoaderError(ESPCoreDumpError):
    pass


class _TargetMethodsBase(object):
    @staticmethod
    @abstractmethod
    def tcb_is_sane(tcb_addr, tcb_size):
        """
        Check tcb address if it is correct
        """
        return False

    @staticmethod
    @abstractmethod
    def stack_is_sane(sp):
        """
        Check stack address if it is correct
        """
        return False

    @staticmethod
    @abstractmethod
    def addr_is_fake(addr):
        """
        Check if address is in fake area
        """
        return False


class _ArchMethodsBase(object):
    @staticmethod
    @abstractmethod
    def get_registers_from_stack(data, grows_down):
        """
        Returns list of registers (in GDB format) from stack frame
        """
        return [], {}

    @staticmethod
    @abstractmethod
    def build_prstatus_data(tcb_addr, task_regs):
        return b''
