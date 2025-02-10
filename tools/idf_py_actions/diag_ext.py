# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import sys
import uuid
from typing import Any
from typing import Dict
from typing import Optional
from typing import Tuple

import click
from idf_py_actions.tools import PropertyDict
from idf_py_actions.tools import RunTool
from idf_py_actions.tools import yellow_print


def diag(action: str,
         ctx: click.core.Context,
         args: PropertyDict,
         debug: bool,
         log_prefix: bool,
         force: bool,
         no_color: bool,
         zip_directory: Optional[str],
         list_recipes: bool,
         check_recipes: bool,
         cmdl_recipes: Tuple,
         cmdl_tags: Tuple,
         purge_file: Optional[str],
         append: bool,
         output: Optional[str]) -> None:

    diag_args: list = [sys.executable, '-m', 'esp_idf_diag']

    def add_common_arguments(diag_args: list) -> None:
        # Common arguments for all esp-idf-diag commands
        diag_args += ['--no-hints']
        if debug:
            diag_args += ['--debug']
        if log_prefix:
            diag_args += ['--log-prefix']
        if no_color:
            diag_args += ['--no-color']
        else:
            diag_args += ['--force-terminal']

    def add_recipe_arguments(diag_args: list) -> None:
        # Recipe related arguments for create, list and check esp-idf-diag commands
        diag_args += ['--project-dir', args.project_dir]
        diag_args += ['--build-dir', args.build_dir]

        for recipe in cmdl_recipes:
            diag_args += ['--recipe', recipe]

        for tag in cmdl_tags:
            diag_args += ['--tag', tag]

        if append:
            diag_args += ['--append']

    if zip_directory:
        # zip command
        command = 'zip'
        diag_args += [command]
        add_common_arguments(diag_args)

        if output:
            diag_args += ['--output', output]
        if force:
            diag_args += ['--force']

        diag_args += [zip_directory]

    elif list_recipes:
        # list command
        command = 'list'
        diag_args += [command]
        add_common_arguments(diag_args)
        add_recipe_arguments(diag_args)

    elif check_recipes:
        # check command
        command = 'check'
        diag_args += [command]
        add_common_arguments(diag_args)
        add_recipe_arguments(diag_args)

    else:
        # create command
        command = 'create'
        diag_args += [command]
        add_common_arguments(diag_args)
        add_recipe_arguments(diag_args)

        if not output:
            output = f'idf-diag-{uuid.uuid4()}'

        diag_args += ['--output', output]

        if force:
            diag_args += ['--force']
        if purge_file:
            diag_args += ['--purge', purge_file]

    try:
        RunTool('idf_diag', diag_args, args.project_dir, hints=not args.no_hints)()
    except Exception:
        raise

    if command == 'create':
        yellow_print(
            (
                f'Please make sure to thoroughly check it for any sensitive '
                f'information before sharing and remove files you do not want '
                f'to share. Kindly include any additional files you find '
                f'relevant that were not automatically added. Please archive '
                f'the contents of the final report directory using the command:\n'
                f'"idf.py diag --zip {output}".'
            )
        )


def action_extensions(base_actions: Dict, project_path: str) -> Any:
    return {
        'actions': {
            'diag': {
                'callback': diag,
                'help': 'Create diagnostic report.',
                'options': [
                    {
                        'names': ['-d', '--debug'],
                        'is_flag': True,
                        'help': 'Print debug information, including exception tracebacks.',
                    },
                    {
                        'names': ['--no-color'],
                        'is_flag': True,
                        'help': 'Do not emit ANSI color codes.',
                    },
                    {
                        'names': ['--log-prefix'],
                        'is_flag': True,
                        'help': 'Add a severity character at the beginning of log messages.',
                    },
                    {
                        'names': ['-z', '--zip', 'zip_directory'],
                        'metavar': 'PATH',
                        'help': 'Create zip archive for diagnostic report in PATH.',
                    },
                    {
                        'names': ['-l', '--list', 'list_recipes'],
                        'is_flag': True,
                        'help': 'Show information about available recipes.',
                    },
                    {
                        'names': ['-c', '--check', 'check_recipes'],
                        'is_flag': True,
                        'help': 'Validate recipes.',
                    },
                    {
                        'names': ['-r', '--recipe', 'cmdl_recipes'],
                        'multiple': True,
                        'metavar': 'RECIPE',
                        'type': str,
                        'help': ('Recipe to use. This option can be specified multiple times. '
                                 'By default, all built-in recipes are used. RECIPE refers to '
                                 'the recipe file path or the file name stem for built-in recipes.'),
                    },
                    {
                        'names': ['-t', '--tag', 'cmdl_tags'],
                        'multiple': True,
                        'metavar': 'TAG',
                        'type': str,
                        'help': ('Consider only recipes containing TAG. This option can be specified '
                                 'multiple times. By default, all recipes are used. Use -l/--list-recipes '
                                 'option to see recipe TAG information.'),
                    },
                    {
                        'names': ['-a', '--append'],
                        'is_flag': True,
                        'help': ('Use recipes specified with the -r/--recipe option in '
                                 'combination with the built-in recipes.'),
                    },
                    {
                        'names': ['-f', '--force'],
                        'is_flag': True,
                        'help': 'Delete the target file or directory if it already exists before creating it.',
                    },
                    {
                        'names': ['-o', '--output'],
                        'metavar': 'PATH',
                        'type': str,
                        'help': ('Diagnostic report directory PATH or zip file archive PATH. '
                                 'If not specified, the report-UUID is used as the report directory, '
                                 'and the report directory specified with the --zip option with a zip '
                                 'extension is used for the zip file archive.')
                    },
                    {
                        'names': ['-p', '--purge', 'purge_file'],
                        'metavar': 'PATH',
                        'type': str,
                        'help': ('Use a custom purge file to remove sensitive information from the report.')
                    },
                ],
            },
        },
    }
