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
PREVIEW_TARGETS = ['linux', 'esp32c5', 'esp32c61', 'esp32h21', 'esp32h4']
