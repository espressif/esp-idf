# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import collections
import multiprocessing
import os
import platform
from typing import Dict
from typing import Union

GENERATORS: Dict[str, Union[str, Dict, list]] = collections.OrderedDict([
    # - command: build command line
    # - version: version command line
    # - dry_run: command to run in dry run mode
    # - verbose_flag: verbose flag
    # - force_progression: one liner status of the progress
    ('Ninja', {
        'command': ['ninja'],
        'version': ['ninja', '--version'],
        'dry_run': ['ninja', '-n'],
        'verbose_flag': '-v',
        # as opposed to printing the status updates each in a in new line
        'force_progression': True,
    }),
])

if os.name != 'nt':
    MAKE_CMD = 'gmake' if platform.system() == 'FreeBSD' else 'make'
    GENERATORS['Unix Makefiles'] = {'command': [MAKE_CMD, '-j', str(multiprocessing.cpu_count() + 2)],
                                    'version': [MAKE_CMD, '--version'],
                                    'dry_run': [MAKE_CMD, '-n'],
                                    'verbose_flag': 'VERBOSE=1',
                                    'force_progression': False}

URL_TO_DOC = 'https://docs.espressif.com/projects/esp-idf'

SUPPORTED_TARGETS = ['esp32', 'esp32s2', 'esp32c3', 'esp32s3', 'esp32c2', 'esp32c6', 'esp32h2', 'esp32p4']
PREVIEW_TARGETS = ['linux', 'esp32c5', 'esp32c61']

OPENOCD_TAGET_CONFIG_DEFAULT = '-f interface/ftdi/esp32_devkitj_v1.cfg -f target/{target}.cfg'
OPENOCD_TAGET_CONFIG: Dict[str, str] = {
    'esp32': '-f board/esp32-wrover-kit-3.3v.cfg',
    'esp32s2': '-f board/esp32s2-kaluga-1.cfg',
    'esp32c3': '-f board/esp32c3-builtin.cfg',
    'esp32s3': '-f board/esp32s3-builtin.cfg',
    'esp32c6': '-f board/esp32c6-builtin.cfg',
    'esp32c5': '-f board/esp32c5-builtin.cfg',
    'esp32h2': '-f board/esp32h2-builtin.cfg',
    'esp32p4': '-f board/esp32p4-builtin.cfg',
}
