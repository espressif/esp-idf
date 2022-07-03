# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

# Convenience functions for commonly used data type conversions
import binascii

from future.utils import tobytes


def str_to_hexstr(string):
    # Form hexstr by appending ASCII codes (in hex) corresponding to
    # each character in the input string
    return binascii.hexlify(tobytes(string)).decode('latin-1')


def hexstr_to_str(hexstr):
    # Prepend 0 (if needed) to make the hexstr length an even number
    if len(hexstr) % 2 == 1:
        hexstr = '0' + hexstr
    # Interpret consecutive pairs of hex characters as 8 bit ASCII codes
    # and append characters corresponding to each code to form the string
    return binascii.unhexlify(tobytes(hexstr)).decode('latin-1')
