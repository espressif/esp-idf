#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#
import json
import logging
import os.path
from typing import Any

try:
    from esp_coredump import CoreDump
except ImportError:
    raise ModuleNotFoundError(
        'No module named "esp_coredump" please install esp_coredump by running "python -m pip install esp-coredump"'
    )

from esp_coredump.cli_ext import parser


def get_prefix_map_gdbinit_path(prog_path: str) -> Any:
    build_dir = os.path.abspath(os.path.dirname(prog_path))
    desc_path = os.path.abspath(os.path.join(build_dir, 'project_description.json'))
    if not os.path.isfile(desc_path):
        logging.warning('%s does not exist. Please build the app with "idf.py build"', desc_path)
        return ''

    with open(desc_path, encoding='utf-8') as f:
        project_desc = json.load(f)

    return project_desc['gdbinit_files']['02_prefix_map']


def main() -> None:
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
    # pass the extra_gdbinit_file if the build is reproducible
    kwargs['extra_gdbinit_file'] = get_prefix_map_gdbinit_path(kwargs['prog'])

    del kwargs['debug']
    del kwargs['operation']

    espcoredump = CoreDump(**kwargs)
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
