# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from elftools.elf.constants import SH_FLAGS
from elftools.elf.elffile import ELFFile


class PcAddressMatcher(object):
    """
    Class for detecting potential addresses which will consequently run through the external addr2line command to
    identify and print information about it.

    The input to this class is the path to the ELF file. Addresses of sections with executable flag are stored and
    used later for lookup.
    """

    def __init__(self, elf_path):  # type: (str) -> None
        self.intervals = []
        try:
            with open(elf_path, 'rb') as f:
                # Is this an ELF file?
                elf_magic = f.read(4)
                if elf_magic != b'\x7fELF':
                    # Probably not an ELF file
                    # (could be Mach-O format on macOS, for example)
                    raise NotImplementedError()
                f.seek(0)

                elf = ELFFile(f)

                for section in elf.iter_sections():
                    if section['sh_flags'] & SH_FLAGS.SHF_EXECINSTR:
                        start = section['sh_addr']
                        size = section['sh_size']
                        end = start + size
                        self.intervals.append((start, end))

        except FileNotFoundError:
            # ELF file is just an optional argument
            pass
        except NotImplementedError:
            pass

        # sort them in order to have faster lookup
        self.intervals = sorted(self.intervals)

    def is_executable_address(self, addr):  # type: (int) -> bool
        """
        Returns True/False if "addr" is in one of the ELF sections with executable flag set.
        """

        for start, end in self.intervals:
            if start > addr:
                # The intervals are sorted. This means that loop can end because all remaining intervals starts are
                # greater than the current start
                return False
            if start <= addr < end:
                return True

        return False
