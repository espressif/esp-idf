# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from rich_click import Context

from idf_py_actions.errors import FatalError
from idf_py_actions.tools import PropertyDict
from idf_py_actions.tools import ensure_build_directory
from idf_py_actions.tools import get_sdkconfig_filename
from idf_py_actions.tools import get_sdkconfig_value
from idf_py_actions.tools import red_print
from idf_py_actions.tools import run_target

SOC_USB_DFU_SUPPORTED = 'CONFIG_SOC_USB_DFU_SUPPORTED'


def action_extensions(base_actions: dict, project_path: str) -> dict:
    def check_dfu_supported(target_name: str, ctx: Context, args: PropertyDict, **kwargs: str) -> bool:
        sdkconfig_path = get_sdkconfig_filename(args)
        if get_sdkconfig_value(sdkconfig_path, SOC_USB_DFU_SUPPORTED) == 'y':
            return True

        target = get_sdkconfig_value(sdkconfig_path, 'CONFIG_IDF_TARGET') or 'unknown'
        red_print(f'DFU is not supported for this target: {target}')
        return False

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
            print(
                'Please have a look at the "Device Firmware Upgrade through USB" chapter in API Guides of the '
                'ESP-IDF documentation for solving common dfu-util issues.'
            )
            raise

    dfu_actions = {
        'actions': {
            'dfu': {
                'callback': dfu_target,
                'check': check_dfu_supported,
                'short_help': 'Build the DFU binary',
                'dependencies': ['all'],
                'options': [
                    {
                        'names': ['--part-size'],
                        'help': 'Large files are split up into smaller partitions in order to avoid timeout during '
                        'erasing flash. This option allows to overwrite the default partition size of '
                        'mkdfu.py.',
                    }
                ],
            },
            'dfu-list': {
                'callback': dfu_list_target,
                'check': check_dfu_supported,
                'short_help': 'List DFU capable devices',
                'dependencies': [],
            },
            'dfu-flash': {
                'callback': dfu_flash_target,
                'check': check_dfu_supported,
                'short_help': 'Flash the DFU binary',
                'order_dependencies': ['dfu'],
                'options': [
                    {
                        'names': ['--path'],
                        'default': '',
                        'help': 'Specify path to DFU device. The default empty path works if there is just one '
                        'ESP device with the same product identifier. See the device list for paths '
                        'of available devices.',
                    }
                ],
            },
        }
    }

    return dfu_actions
