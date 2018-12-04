# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

# Convenience functions for commonly used data type conversions


def str_to_hexstr(string):
    # Form hexstr by appending ASCII codes (in hex) corresponding to
    # each character in the input string
    return ''.join('{:02x}'.format(ord(c)) for c in string)


def hexstr_to_str(hexstr):
    # Prepend 0 (if needed) to make the hexstr length an even number
    if len(hexstr) % 2 == 1:
        hexstr = '0' + hexstr
    # Interpret consecutive pairs of hex characters as 8 bit ASCII codes
    # and append characters corresponding to each code to form the string
    return ''.join(chr(int(hexstr[2 * i: 2 * i + 2], 16)) for i in range(len(hexstr) // 2))
