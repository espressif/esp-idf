#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#

import json
import logging
import os.path
import re
from typing import Any, Optional

try:
    from esp_coredump import CoreDump
except ImportError:
    raise ModuleNotFoundError('No module named "esp_coredump" please install esp_coredump by running '
                              '"python -m pip install esp-coredump"')

from esp_coredump.cli_ext import parser


def get_sdkconfig_value(sdkconfig_file, key):  # type: (str, str) -> Optional[str]
    """
    Return the value of given key from sdkconfig_file.
    If sdkconfig_file does not exist or the option is not present, returns None.
    """
    assert key.startswith('CONFIG_')
    if not os.path.exists(sdkconfig_file):
        return None
    # keep track of the last seen value for the given key
    value = None
    # if the value is quoted, this excludes the quotes from the value
    pattern = re.compile(r"^{}=\"?([^\"]*)\"?$".format(key))
    with open(sdkconfig_file, 'r') as f:
        for line in f:
            match = re.match(pattern, line)
            if match:
                value = match.group(1)
    return value


def get_project_desc(prog_path):  # type: (str) -> Any
    build_dir = os.path.abspath(os.path.dirname(prog_path))
    desc_path = os.path.abspath(os.path.join(build_dir, 'project_description.json'))
    if not os.path.isfile(desc_path):
        logging.warning('%s does not exist. Please build the app with "idf.py build"', desc_path)
        return ''

    with open(desc_path, 'r') as f:
        project_desc = json.load(f)

    return project_desc


def main():  # type: () -> None
    args = parser.parse_args()

    if args.debug == 0:
        log_level = logging.CRITICAL
    elif args.debug == 1:
        log_level = logging.ERROR
    elif args.debug == 2:
        log_level = logging.WARNING
    elif args.debug == 3:
        log_level = logging.INFO
    else:
        log_level = logging.DEBUG
    logging.basicConfig(format='%(levelname)s: %(message)s', level=log_level)

    kwargs = {k: v for k, v in vars(args).items() if v is not None}
    project_desc = get_project_desc(kwargs['prog'])
    # pass the extra_gdbinit_file if the build is reproducible
    if project_desc:
        kwargs['extra_gdbinit_file'] = project_desc.get('debug_prefix_map_gdbinit')
        kwargs['parttable_off'] = get_sdkconfig_value(project_desc['config_file'], 'CONFIG_PARTITION_TABLE_OFFSET')

    del kwargs['debug']
    del kwargs['operation']

    try:
        espcoredump = CoreDump(**kwargs)
    except TypeError as e:
        # 'parttable_off' was added in esp-coredump 1.5.2
        # remove argument and retry without it
        if 'parttable_off' in str(e):
            kwargs.pop('parttable_off')
            espcoredump = CoreDump(**kwargs)
        else:
            raise
    temp_core_files = None

    try:
        if args.operation == 'info_corefile':
            temp_core_files = espcoredump.info_corefile()
        elif args.operation == 'dbg_corefile':
            temp_core_files = espcoredump.dbg_corefile()
        else:
            raise ValueError('Please specify action, should be info_corefile or dbg_corefile')
    finally:
        if temp_core_files:
            for f in temp_core_files:
                try:
                    os.remove(f)
                except OSError:
                    pass


if __name__ == '__main__':
    main()
