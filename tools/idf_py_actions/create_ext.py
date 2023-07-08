# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from __future__ import print_function

import os
import re
import sys
from distutils.dir_util import copy_tree
from typing import Dict

import click
from idf_py_actions.tools import PropertyDict


def get_type(action: str) -> str:
    return action.split('-')[1]


def replace_in_file(filename: str, pattern: str, replacement: str) -> None:
    with open(filename, 'r+') as f:
        content = f.read()
        overwritten_content = re.sub(pattern, replacement, content, flags=re.M)
        f.seek(0)
        f.write(overwritten_content)
        f.truncate()


def is_empty_and_create(path: str, action: str) -> None:
    abspath = os.path.abspath(path)
    if not os.path.exists(abspath):
        os.makedirs(abspath)
    elif not os.path.isdir(abspath):
        print('Your target path is not a directory. Please remove the', os.path.abspath(abspath),
              'or use different target path.')
        sys.exit(4)
    elif len(os.listdir(path)) > 0:
        print('The directory', abspath, 'is not empty. To create a', get_type(action),
              'you must empty the directory or choose a different path.')
        sys.exit(3)


def create_project(target_path: str, name: str) -> None:
    copy_tree(
        os.path.join(os.environ['IDF_PATH'], 'examples', 'get-started', 'sample_project'),
        target_path,
        preserve_mode=0,
    )
    main_folder = os.path.join(target_path, 'main')
    os.rename(os.path.join(main_folder, 'main.c'), os.path.join(main_folder, '.'.join((name, 'c'))))
    replace_in_file(os.path.join(main_folder, 'CMakeLists.txt'), 'main', name)
    replace_in_file(os.path.join(target_path, 'CMakeLists.txt'), 'main', name)
    os.remove(os.path.join(target_path, 'README.md'))


def create_component(target_path: str, name: str) -> None:
    copy_tree(
        os.path.join(os.environ['IDF_PATH'], 'tools', 'templates', 'sample_component'),
        target_path,
        preserve_mode=0,
    )
    os.rename(os.path.join(target_path, 'main.c'), os.path.join(target_path, '.'.join((name, 'c'))))
    os.rename(os.path.join(target_path, 'include', 'main.h'),
              os.path.join(target_path, 'include', '.'.join((name, 'h'))))

    replace_in_file(os.path.join(target_path, '.'.join((name, 'c'))), 'main', name)
    replace_in_file(os.path.join(target_path, 'CMakeLists.txt'), 'main', name)


def action_extensions(base_actions: Dict, project_path: str) -> Dict:
    def create_new(action: str, ctx: click.core.Context, global_args: PropertyDict, **action_args: str) -> Dict:
        target_path = action_args.get('path') or os.path.join(project_path, action_args['name'])

        is_empty_and_create(target_path, action)

        func_action_map = {'create-project': create_project, 'create-component': create_component}
        func_action_map[action](target_path, action_args['name'])

        print('The', get_type(action), 'was created in', os.path.abspath(target_path))

        # after the command execution, no other commands are accepted and idf.py terminates
        sys.exit(0)

    return {
        'actions': {
            'create-project': {
                'callback': create_new,
                'short_help': 'Create a new project.',
                'help': ('Create a new project with the name NAME specified as argument. '
                         'For example: '
                         '`idf.py create-project new_proj` '
                         'will create a new project in subdirectory called `new_proj` '
                         'of the current working directory. '
                         "For specifying the new project's path, use either the option --path for specifying the "
                         'destination directory, or the global option -C if the project should be created as a '
                         'subdirectory of the specified directory. '
                         'If the target path does not exist it will be created. If the target folder is not empty '
                         'then the operation will fail with return code 3. '
                         'If the target path is not a folder, the script will fail with return code 4. '
                         'After the execution idf.py terminates '
                         'so this operation should be used alone.'),
                'arguments': [{'names': ['name']}],
                'options': [
                    {
                        'names': ['-p', '--path'],
                        'help': ('Set the path for the new project. The project '
                                 'will be created directly in the given folder if it does not contain anything'),
                    },
                ],

            },
            'create-component': {
                'callback': create_new,
                'short_help': 'Create a new component.',
                'help': ('Create a new component with the name NAME specified as argument. '
                         'For example: '
                         '`idf.py create-component new_comp` '
                         'will create a new component in subdirectory called `new_comp` '
                         'of the current working directory. '
                         "For specifying the new component's path use the option -C. "
                         'If the target path does not exist then it will be created. '
                         'If the target folder is not empty '
                         'then the operation will fail with return code 3. '
                         'If the target path is not a folder, the script will fail with return code 4. '
                         'After the execution idf.py terminates '
                         'so this operation should be used alone.'),
                'arguments': [{'names': ['name']}],
            }
        }
    }
