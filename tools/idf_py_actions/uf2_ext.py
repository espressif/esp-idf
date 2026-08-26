# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from rich_click import Context

from idf_py_actions.tools import PropertyDict
from idf_py_actions.tools import ensure_build_directory
from idf_py_actions.tools import run_target


def action_extensions(base_actions: dict, project_path: list) -> dict:
    def uf2_target(target_name: str, ctx: Context, args: PropertyDict, md5_disable: bool) -> None:
        ensure_build_directory(args, ctx.info_name)
        extra = list()
        if md5_disable:
            extra.append('--md5-disable')
        run_target(target_name, args, env={'SERIAL_TOOL_EXTRA_ARGS': ' '.join(extra)})

    uf2_options = [
        {
            'names': ['--md5-disable'],
            'is_flag': True,
            'help': 'Disable MD5 checksum',
        },
    ]

    uf2_actions = {
        'actions': {
            'uf2': {
                'callback': uf2_target,
                'options': uf2_options,
                'short_help': 'Generate the UF2 binary with all the binaries included',
                'dependencies': ['all'],
            },
            'uf2-app': {
                'callback': uf2_target,
                'options': uf2_options,
                'short_help': 'Generate an UF2 binary for the application only',
                'dependencies': ['all'],
            },
        }
    }

    return uf2_actions
