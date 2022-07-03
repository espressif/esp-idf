# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from typing import Dict, List

from click.core import Context
from idf_py_actions.tools import PropertyDict, ensure_build_directory, run_target


def action_extensions(base_actions: Dict, project_path: List) -> Dict:
    def uf2_target(target_name: str, ctx: Context, args: PropertyDict) -> None:
        ensure_build_directory(args, ctx.info_name)
        run_target(target_name, args)

    uf2_actions = {
        'actions': {
            'uf2': {
                'callback': uf2_target,
                'short_help': 'Generate the UF2 binary with all the binaries included',
                'dependencies': ['all'],
            },
            'uf2-app': {
                'callback': uf2_target,
                'short_help': 'Generate an UF2 binary for the application only',
                'dependencies': ['all'],
            },
        }
    }

    return uf2_actions
