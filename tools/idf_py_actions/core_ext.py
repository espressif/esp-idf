# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import fnmatch
import json
import locale
import os
import re
import shutil
import subprocess
import sys
from typing import Any, Dict, List, Optional
from urllib.error import URLError
from urllib.request import Request, urlopen
from webbrowser import open_new_tab

import click
from click.core import Context
from idf_py_actions.constants import GENERATORS, PREVIEW_TARGETS, SUPPORTED_TARGETS, URL_TO_DOC
from idf_py_actions.errors import FatalError
from idf_py_actions.global_options import global_options
from idf_py_actions.tools import (PropertyDict, TargetChoice, ensure_build_directory, generate_hints, get_target,
                                  idf_version, merge_action_lists, print_warning, run_target, yellow_print)


def action_extensions(base_actions: Dict, project_path: str) -> Any:
    def build_target(target_name: str, ctx: Context, args: PropertyDict) -> None:
        """
        Execute the target build system to build target 'target_name'

        Calls ensure_build_directory() which will run cmake to generate a build
        directory (with the specified generator) as needed.
        """
        ensure_build_directory(args, ctx.info_name)
        run_target(target_name, args, force_progression=GENERATORS[args.generator].get('force_progression', False))

    def size_target(target_name: str, ctx: Context, args: PropertyDict, output_format: str,
                    output_file: str, legacy: bool) -> None:
        """
        Builds the app and then executes a size-related target passed in 'target_name'.
        `tool_error_handler` handler is used to suppress errors during the build,
        so size action can run even in case of overflow.
        """

        def tool_error_handler(e: int, stdout: str, stderr: str) -> None:
            for hint in generate_hints(stdout, stderr):
                yellow_print(hint)

        if not legacy and output_format != 'json':
            try:
                import esp_idf_size.ng  # noqa: F401
            except ImportError:
                print_warning('WARNING: refactored esp-idf-size not installed, using legacy mode')
                legacy = True
            else:
                # Legacy mode is used only when explicitly requested with --legacy option
                # or when "--format json" option is specified. Here we enable the
                # esp-idf-size refactored version with ESP_IDF_SIZE_NG env. variable.
                os.environ['ESP_IDF_SIZE_NG'] = '1'
                # ESP_IDF_SIZE_FORCE_TERMINAL is set to force terminal control codes even
                # if stdout is not attached to terminal. This is set to pass color codes
                # from esp-idf-size to idf.py.
                os.environ['ESP_IDF_SIZE_FORCE_TERMINAL'] = '1'

        if legacy and output_format in ['json2', 'raw', 'tree']:
            # These formats are supported in new version only.
            # We would get error from the esp-idf-size anyway, so print error early.
            raise FatalError(f'Legacy esp-idf-size does not support {output_format} format')

        os.environ['SIZE_OUTPUT_FORMAT'] = output_format
        if output_file:
            os.environ['SIZE_OUTPUT_FILE'] = os.path.abspath(output_file)

        ensure_build_directory(args, ctx.info_name)
        run_target('all', args, force_progression=GENERATORS[args.generator].get('force_progression', False),
                   custom_error_handler=tool_error_handler)
        run_target(target_name, args)

    def list_build_system_targets(target_name: str, ctx: Context, args: PropertyDict) -> None:
        """Shows list of targets known to build sytem (make/ninja)"""
        build_target('help', ctx, args)

    def menuconfig(target_name: str, ctx: Context, args: PropertyDict, style: str) -> None:
        """
        Menuconfig target is build_target extended with the style argument for setting the value for the environment
        variable.
        """
        if sys.platform != 'win32':
            try:
                import curses  # noqa: F401
            except ImportError:
                raise FatalError('\n'.join(
                    ['', "menuconfig failed to import the standard Python 'curses' library.",
                     'Please re-run the install script which might be able to fix the issue.']))
        if sys.version_info[0] < 3:
            # The subprocess lib cannot accept environment variables as "unicode".
            # This encoding step is required only in Python 2.
            style = style.encode(sys.getfilesystemencoding() or 'utf-8')
        os.environ['MENUCONFIG_STYLE'] = style
        args.no_hints = True
        build_target(target_name, ctx, args)

    def save_defconfig(target_name: str, ctx: Context, args: PropertyDict, add_menu_labels: bool) -> None:
        if add_menu_labels:
            os.environ['ESP_IDF_KCONFIG_MIN_LABELS'] = '1'
        else:
            # unset variable
            os.environ.pop('ESP_IDF_KCONFIG_MIN_LABELS', None)
        build_target(target_name, ctx, args)

    def fallback_target(target_name: str, ctx: Context, args: PropertyDict) -> None:
        """
        Execute targets that are not explicitly known to idf.py
        """
        ensure_build_directory(args, ctx.info_name)

        try:
            subprocess.check_output(GENERATORS[args.generator]['dry_run'] + [target_name], cwd=args.build_dir)

        except Exception:
            if target_name in ['clang-check', 'clang-html-report']:
                raise FatalError('command "{}" requires an additional plugin "pyclang". '
                                 'Please install it via "pip install --upgrade pyclang"'.format(target_name))

            raise FatalError(
                'command "%s" is not known to idf.py and is not a %s target' % (target_name, args.generator))

        run_target(target_name, args)

    def verbose_callback(ctx: Context, param: List, value: str) -> Optional[str]:
        if not value or ctx.resilient_parsing:
            return None

        for line in ctx.command.verbose_output:
            print(line)

        return value

    def clean(action: str, ctx: Context, args: PropertyDict) -> None:
        if not os.path.isdir(args.build_dir):
            print("Build directory '%s' not found. Nothing to clean." % args.build_dir)
            return
        build_target('clean', ctx, args)

    def fullclean(action: str, ctx: Context, args: PropertyDict) -> None:
        build_dir = args.build_dir
        if not os.path.isdir(build_dir):
            print("Build directory '%s' not found. Nothing to clean." % build_dir)
            return
        if len(os.listdir(build_dir)) == 0:
            print("Build directory '%s' is empty. Nothing to clean." % build_dir)
            return

        if not os.path.exists(os.path.join(build_dir, 'CMakeCache.txt')):
            raise FatalError(
                "Directory '%s' doesn't seem to be a CMake build directory. Refusing to automatically "
                "delete files in this directory. Delete the directory manually to 'clean' it." % build_dir)
        red_flags = ['CMakeLists.txt', '.git', '.svn']
        for red in red_flags:
            red = os.path.join(build_dir, red)
            if os.path.exists(red):
                raise FatalError(
                    "Refusing to automatically delete files in directory containing '%s'. Delete files manually if you're sure."
                    % red)
        if args.verbose and len(build_dir) > 1:
            print('The following symlinks were identified and removed:\n%s' % '\n'.join(build_dir))
        for f in os.listdir(build_dir):  # TODO: once we are Python 3 only, this can be os.scandir()
            f = os.path.join(build_dir, f)
            if args.verbose:
                print('Removing: %s' % f)
            if os.path.isdir(f):
                shutil.rmtree(f)
            else:
                os.remove(f)

    def python_clean(action: str, ctx: Context, args: PropertyDict) -> None:
        for root, dirnames, filenames in os.walk(os.environ['IDF_PATH']):
            for d in dirnames:
                if d == '__pycache__':
                    dir_to_delete = os.path.join(root, d)
                    if args.verbose:
                        print('Removing: %s' % dir_to_delete)
                    shutil.rmtree(dir_to_delete)
            for filename in fnmatch.filter(filenames, '*.py[co]'):
                file_to_delete = os.path.join(root, filename)
                if args.verbose:
                    print('Removing: %s' % file_to_delete)
                os.remove(file_to_delete)

    def set_target(action: str, ctx: Context, args: PropertyDict, idf_target: str) -> None:
        if (not args['preview'] and idf_target in PREVIEW_TARGETS):
            raise FatalError(
                "%s is still in preview. You have to append '--preview' option after idf.py to use any preview feature."
                % idf_target)
        args.define_cache_entry.append('IDF_TARGET=' + idf_target)
        print(f'Set Target to: {idf_target}, new sdkconfig will be created.')
        env = {'_IDF_PY_SET_TARGET_ACTION': '1'}
        ensure_build_directory(args, ctx.info_name, True, env)

    def reconfigure(action: str, ctx: Context, args: PropertyDict) -> None:
        ensure_build_directory(args, ctx.info_name, True)

    def validate_root_options(ctx: Context, args: PropertyDict, tasks: List) -> None:
        args.project_dir = os.path.realpath(args.project_dir)
        if args.build_dir is not None and args.project_dir == os.path.realpath(args.build_dir):
            raise FatalError(
                'Setting the build directory to the project directory is not supported. Suggest dropping '
                "--build-dir option, the default is a 'build' subdirectory inside the project directory.")
        if args.build_dir is None:
            args.build_dir = os.path.join(args.project_dir, 'build')
        args.build_dir = os.path.realpath(args.build_dir)

    def idf_version_callback(ctx: Context, param: str, value: str) -> None:
        if not value or ctx.resilient_parsing:
            return

        version = idf_version()

        if not version:
            raise FatalError('ESP-IDF version cannot be determined')

        print('ESP-IDF %s' % version)
        sys.exit(0)

    def list_targets_callback(ctx: Context, param: List, value: int) -> None:
        if not value or ctx.resilient_parsing:
            return

        for target in SUPPORTED_TARGETS:
            print(target)

        if ctx.params.get('preview'):
            for target in PREVIEW_TARGETS:
                print(target)

        sys.exit(0)

    def show_docs(action: str, ctx: Context, args: PropertyDict, no_browser: bool, language: str, starting_page: str, version: str, target: str) -> None:
        if language == 'cn':
            language = 'zh_CN'
        if not version:
            # '0.0-dev' here because if 'dev' in version it will transform in to 'latest'
            version_search = re.search(r'v\d+\.\d+\.?\d*(-dev|-beta\d|-rc)?', idf_version() or '0.0-dev')
            version = version_search.group() if version_search else 'latest'
            if 'dev' in version:
                version = 'latest'
        elif version[0] != 'v':
            version = 'v' + version
        target = target or get_target(args.project_dir) or 'esp32'
        link = '/'.join([URL_TO_DOC, language, version, target, starting_page or ''])
        redirect_link = False
        try:
            req = Request(link)
            webpage = urlopen(req)
            redirect_link = webpage.geturl().endswith('404.html')
        except URLError:
            print("We can't check the link's functionality because you don't have an internet connection")
        if redirect_link:
            print('Target', target, 'doesn\'t exist for version', version)
            link = '/'.join([URL_TO_DOC, language, version, starting_page or ''])
        if not no_browser:
            print('Opening documentation in the default browser:')
            print(link)
            open_new_tab(link)
        else:
            print('Please open the documentation link in the browser:')
            print(link)
        sys.exit(0)

    def get_default_language() -> str:
        try:
            language = 'zh_CN' if locale.getdefaultlocale()[0] == 'zh_CN' else 'en'
        except ValueError:
            language = 'en'
        return language

    def help_and_exit(action: str, ctx: Context, param: List, json_option: bool, add_options: bool) -> None:
        if json_option:
            output_dict = {}
            output_dict['target'] = get_target(param.project_dir)  # type: ignore
            output_dict['actions'] = []
            actions = ctx.to_info_dict().get('command').get('commands')
            for a in actions:
                action_info = {}
                action_info['name'] = a
                action_info['description'] = actions[a].get('help')
                if add_options:
                    action_info['options'] = actions[a].get('params')
                output_dict['actions'].append(action_info)
            print(json.dumps(output_dict, sort_keys=True, indent=4))
        else:
            print(ctx.get_help())
        ctx.exit()

    root_options = {
        'global_options': [
            {
                'names': ['--version'],
                'help': 'Show IDF version and exit.',
                'is_flag': True,
                'expose_value': False,
                'callback': idf_version_callback,
            },
            {
                'names': ['--list-targets'],
                'help': 'Print list of supported targets and exit.',
                'is_flag': True,
                'expose_value': False,
                'callback': list_targets_callback,
            },
            {
                'names': ['-C', '--project-dir'],
                'scope': 'shared',
                'help': 'Project directory.',
                'type': click.Path(),
                'default': os.getcwd(),
            },
            {
                'names': ['-B', '--build-dir'],
                'help': 'Build directory.',
                'type': click.Path(),
                'default': None,
            },
            {
                'names': ['-w/-n', '--cmake-warn-uninitialized/--no-warnings'],
                'help': ('Enable CMake uninitialized variable warnings for CMake files inside the project directory. '
                         "(--no-warnings is now the default, and doesn't need to be specified.)"),
                'envvar': 'IDF_CMAKE_WARN_UNINITIALIZED',
                'is_flag': True,
                'default': False,
            },
            {
                'names': ['-v', '--verbose'],
                'help': 'Verbose build output.',
                'is_flag': True,
                'is_eager': True,
                'default': False,
                'callback': verbose_callback,
            },
            {
                'names': ['--preview'],
                'help': 'Enable IDF features that are still in preview.',
                'is_eager': True,
                'is_flag': True,
                'default': False,
            },
            {
                'names': ['--ccache/--no-ccache'],
                'help': 'Use ccache in build. Disabled by default.',
                'is_flag': True,
                'envvar': 'IDF_CCACHE_ENABLE',
                'default': False,
            },
            {
                'names': ['-G', '--generator'],
                'help': 'CMake generator.',
                'type': click.Choice(GENERATORS.keys()),
            },
            {
                'names': ['--dry-run'],
                'help': "Only process arguments, but don't execute actions.",
                'is_flag': True,
                'hidden': True,
                'default': False,
            },
            {
                'names': ['--no-hints'],
                'help': 'Disable hints on how to resolve errors and logging.',
                'is_flag': True,
                'default': False
            }
        ],
        'global_action_callbacks': [validate_root_options],
    }

    # 'default' is introduced instead of simply setting 'text' as the default so that we know
    # if the user explicitly specified the format or not. If the format is not specified, then
    # the legacy OUTPUT_JSON CMake variable will be taken into account.
    size_options = [{'names': ['--format', 'output_format'],
                     'type': click.Choice(['default', 'text', 'csv', 'json', 'json2', 'tree', 'raw']),
                     'help': 'Specify output format: text (same as "default"), csv, json, json2, tree or raw.',
                     'default': 'default'},
                    {'names': ['--legacy', '-l'],
                     'is_flag': True,
                     'default': os.environ.get('ESP_IDF_SIZE_LEGACY', '0') == '1',
                     'help': 'Use legacy esp-idf-size version'},
                    {'names': ['--output-file', 'output_file'],
                     'help': 'Print output to the specified file instead of to the standard output'}]

    build_actions = {
        'actions': {
            'all': {
                'aliases': ['build'],
                'callback': build_target,
                'short_help': 'Build the project.',
                'help': (
                    'Build the project. This can involve multiple steps:\n\n'
                    '1. Create the build directory if needed. '
                    "The sub-directory 'build' is used to hold build output, "
                    'although this can be changed with the -B option.\n\n'
                    '2. Run CMake as necessary to configure the project '
                    'and generate build files for the main build tool.\n\n'
                    '3. Run the main build tool (Ninja or GNU Make). '
                    'By default, the build tool is automatically detected '
                    'but it can be explicitly set by passing the -G option to idf.py.\n\n'),
                'options': global_options,
                'order_dependencies': [
                    'reconfigure',
                    'menuconfig',
                    'clean',
                    'fullclean',
                ],
            },
            'menuconfig': {
                'callback': menuconfig,
                'help': 'Run "menuconfig" project configuration tool.',
                'options': global_options + [
                    {
                        'names': ['--style', '--color-scheme', 'style'],
                        'help': (
                            'Menuconfig style.\n'
                            'The built-in styles include:\n\n'
                            '- default - a yellowish theme,\n\n'
                            '- monochrome -  a black and white theme, or\n\n'
                            '- aquatic - a blue theme.\n\n'
                            'It is possible to customize these themes further'
                            ' as it is described in the Color schemes section of the kconfiglib documentation.\n'
                            'The default value is \"aquatic\".'),
                        'envvar': 'MENUCONFIG_STYLE',
                        'default': 'aquatic',
                    }
                ],
            },
            'confserver': {
                'callback': build_target,
                'help': 'Run JSON configuration server.',
                'options': global_options,
            },
            'size': {
                'callback': size_target,
                'help': 'Print basic size information about the app.',
                'options': global_options + size_options,
            },
            'size-components': {
                'callback': size_target,
                'help': 'Print per-component size information.',
                'options': global_options + size_options,
            },
            'size-files': {
                'callback': size_target,
                'help': 'Print per-source-file size information.',
                'options': global_options + size_options,
            },
            'bootloader': {
                'callback': build_target,
                'help': 'Build only bootloader.',
                'options': global_options,
            },
            'app': {
                'callback': build_target,
                'help': 'Build only the app.',
                'order_dependencies': ['clean', 'fullclean', 'reconfigure'],
                'options': global_options,
            },
            'efuse-common-table': {
                'callback': build_target,
                'help': 'Generate C-source for IDF\'s eFuse fields.',
                'order_dependencies': ['reconfigure'],
                'options': global_options,
            },
            'efuse-custom-table': {
                'callback': build_target,
                'help': 'Generate C-source for user\'s eFuse fields.',
                'order_dependencies': ['reconfigure'],
                'options': global_options,
            },
            'show-efuse-table': {
                'callback': build_target,
                'help': 'Print eFuse table.',
                'order_dependencies': ['reconfigure'],
                'options': global_options,
            },
            'partition-table': {
                'callback': build_target,
                'help': 'Build only partition table.',
                'order_dependencies': ['reconfigure'],
                'options': global_options,
            },
            'build-system-targets': {
                'callback': list_build_system_targets,
                'help': 'Print list of build system targets.',
            },
            'fallback': {
                'callback': fallback_target,
                'help': 'Handle for targets not known for idf.py.',
                'hidden': True,
            },
            'docs': {
                'callback': show_docs,
                'help': 'Open web browser with documentation for ESP-IDF',
                'options': [
                    {
                        'names': ['--no-browser', '-nb'],
                        'is_flag': True,
                        'help': 'Don\'t open browser.'
                    },
                    {
                        'names': ['--language', '-l'],
                        'default': get_default_language(),
                        'type': click.Choice(['en', 'zh_CN', 'cn']),
                        'help': 'Documentation language. Your system language by default (en or cn)'
                    },
                    {
                        'names': ['--starting-page', '-sp'],
                        'help': 'Documentation page (get-started, api-reference etc).'
                    },
                    {
                        'names': ['--version', '-v'],
                        'help': 'Version of ESP-IDF.'
                    },
                    {
                        'names': ['--target', '-t'],
                        'type': TargetChoice(SUPPORTED_TARGETS + PREVIEW_TARGETS + ['']),
                        'help': 'Chip target.'
                    }
                ]
            },
            'save-defconfig': {
                'callback': save_defconfig,
                'help': 'Generate a sdkconfig.defaults with options different from the default ones',
                'options': global_options + [{
                    'names': ['--add-menu-labels'],
                    'is_flag': True,
                    'help': 'Add menu labels to minimal config.',
                }]
            }
        }
    }

    clean_actions = {
        'actions': {
            'reconfigure': {
                'callback': reconfigure,
                'short_help': 'Re-run CMake.',
                'help': (
                    "Re-run CMake even if it doesn't seem to need re-running. "
                    "This isn't necessary during normal usage, "
                    'but can be useful after adding/removing files from the source tree, '
                    'or when modifying CMake cache variables. '
                    "For example, \"idf.py -DNAME='VALUE' reconfigure\" "
                    'can be used to set variable "NAME" in CMake cache to value "VALUE".'),
                'options': global_options,
                'order_dependencies': ['menuconfig', 'fullclean'],
            },
            'set-target': {
                'callback': set_target,
                'short_help': 'Set the chip target to build.',
                'help': (
                    'Set the chip target to build. This will remove the '
                    'existing sdkconfig file and corresponding CMakeCache and '
                    'create new ones according to the new target.\nFor example, '
                    "\"idf.py set-target esp32\" will select esp32 as the new chip "
                    'target.'),
                'arguments': [
                    {
                        'names': ['idf-target'],
                        'nargs': 1,
                        'type': TargetChoice(SUPPORTED_TARGETS + PREVIEW_TARGETS),
                    },
                ],
                'dependencies': ['fullclean'],
            },
            'clean': {
                'callback': clean,
                'short_help': 'Delete build output files from the build directory.',
                'help': (
                    'Delete build output files from the build directory, '
                    "forcing a 'full rebuild' the next time "
                    "the project is built. Cleaning doesn't delete "
                    'CMake configuration output and some other files'),
                'order_dependencies': ['fullclean'],
            },
            'fullclean': {
                'callback': fullclean,
                'short_help': 'Delete the entire build directory contents.',
                'help': (
                    'Delete the entire build directory contents. '
                    'This includes all CMake configuration output.'
                    'The next time the project is built, '
                    'CMake will configure it from scratch. '
                    'Note that this option recursively deletes all files '
                    'in the build directory, so use with care.'
                    'Project configuration is not deleted.')
            },
            'python-clean': {
                'callback': python_clean,
                'short_help': 'Delete generated Python byte code from the IDF directory',
                'help': (
                    'Delete generated Python byte code from the IDF directory '
                    'which may cause issues when switching between IDF and Python versions. '
                    'It is advised to run this target after switching versions.')
            },
        }
    }

    help_action = {
        'actions': {
            'help': {
                'callback': help_and_exit,
                'help': 'Show help message and exit.',
                'hidden': True,
                'options': [
                    {
                        'names': ['--json', 'json_option'],
                        'is_flag': True,
                        'help': 'Print out actions in machine-readable format for selected target.'
                    },
                    {
                        'names': ['--add-options'],
                        'is_flag': True,
                        'help': 'Add options about actions to machine-readable format.'
                    }
                ],
            }
        }
    }

    return merge_action_lists(root_options, build_actions, clean_actions, help_action)
