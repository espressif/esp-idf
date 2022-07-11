# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from typing import Dict

from click.core import Context
from idf_py_actions.errors import FatalError
from idf_py_actions.tools import PropertyDict, ensure_build_directory, is_target_supported, run_target


def action_extensions(base_actions: Dict, project_path: str) -> Dict:

    SUPPORTED_TARGETS = ['esp32s2', 'esp32s3']

    def dfu_target(target_name: str, ctx: Context, args: PropertyDict, part_size: str) -> None:
        ensure_build_directory(args, ctx.info_name)
        run_target(target_name, args, {'ESP_DFU_PART_SIZE': part_size} if part_size else {})

    def dfu_list_target(target_name: str, ctx: Context, args: PropertyDict) -> None:
        ensure_build_directory(args, ctx.info_name)
        run_target(target_name, args)

    def dfu_flash_target(target_name: str, ctx: Context, args: PropertyDict, path: str) -> None:
        ensure_build_directory(args, ctx.info_name)

        try:
            run_target(target_name, args, {'ESP_DFU_PATH': path})
        except FatalError:
            # Cannot capture the error from dfu-util here so the best advise is:
            print('Please have a look at the "Device Firmware Upgrade through USB" chapter in API Guides of the '
                  'ESP-IDF documentation for solving common dfu-util issues.')
            raise

    dfu_actions = {
        'actions': {
            'dfu': {
                'callback': dfu_target,
                'short_help': 'Build the DFU binary',
                'dependencies': ['all'],
                'options': [
                    {
                        'names': ['--part-size'],
                        'help': 'Large files are split up into smaller partitions in order to avoid timeout during '
                                'erasing flash. This option allows to overwrite the default partition size of '
                                'mkdfu.py.'
                    }
                ],
            },
            'dfu-list': {
                'callback': dfu_list_target,
                'short_help': 'List DFU capable devices',
                'dependencies': [],
            },
            'dfu-flash': {
                'callback': dfu_flash_target,
                'short_help': 'Flash the DFU binary',
                'order_dependencies': ['dfu'],
                'options': [
                    {
                        'names': ['--path'],
                        'default': '',
                        'help': 'Specify path to DFU device. The default empty path works if there is just one '
                                'ESP device with the same product identifier. See the device list for paths '
                                'of available devices.'
                    }
                ],
            },
        }
    }

    return dfu_actions if is_target_supported(project_path, SUPPORTED_TARGETS) else {}
