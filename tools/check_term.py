#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function

import os
import sys

if __name__ == '__main__':
    # Checks for the content of environment variable TERM to use with the Python- and curses-based menuconfig. It is
    # not implemented in shell so calling this script could not use some other shell environment where TERM is set
    # differently. Implemented here so it could be checked at one place for make and cmake as well.
    if sys.platform == 'win32' and 'MSYSTEM' not in os.environ:
        # no TERM is used in Windows command line
        exit(0)

    term = os.environ.get('TERM', None)

    if term is None:
        print('WARNING: The TERM environment variable is not defined. The curses-based menuconfig '
              'will probably fail to run. Please consult the documentation of your terminal to set it up.')
    else:
        if term.endswith('256color') or term in ['alacritty']:
            print('TERM environment variable is set to "{}"'.format(term))
        else:
            print('WARNING: Menuconfig may fail because of the TERM environment variable is set '
                  'to "{}". Please consult the documentation of your terminal to set it up. '
                  'Some good, proved to been working setups include xterm-256color, screen-256color, '
                  'rxvt-unicode-256color, alacritty.'.format(term))
