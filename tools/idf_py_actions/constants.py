import collections
import multiprocessing
import os
import platform

GENERATORS = collections.OrderedDict([
    # - command: build command line
    # - version: version command line
    # - dry_run: command to run in dry run mode
    # - verbose_flag: verbose flag
    ('Ninja', {
        'command': ['ninja'],
        'version': ['ninja', '--version'],
        'dry_run': ['ninja', '-n'],
        'verbose_flag': '-v'
    }),
])

if os.name != 'nt':
    MAKE_CMD = 'gmake' if platform.system() == 'FreeBSD' else 'make'
    GENERATORS['Unix Makefiles'] = {'command': [MAKE_CMD, '-j', str(multiprocessing.cpu_count() + 2)],
                                    'version': [MAKE_CMD, '--version'],
                                    'dry_run': [MAKE_CMD, '-n'],
                                    'verbose_flag': 'VERBOSE=1'}

URL_TO_DOC = 'https://docs.espressif.com/projects/esp-idf'

SUPPORTED_TARGETS = ['esp32', 'esp32s2', 'esp32c3', 'esp32s3']
PREVIEW_TARGETS = ['linux', 'esp32h2', 'esp32c2']
