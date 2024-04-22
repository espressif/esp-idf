# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import os
import shlex
import signal
import sys
from typing import Any
from typing import Dict
from typing import List
from typing import Optional

import click
from idf_py_actions.global_options import global_options
from idf_py_actions.tools import ensure_build_directory
from idf_py_actions.tools import get_default_serial_port
from idf_py_actions.tools import get_sdkconfig_value
from idf_py_actions.tools import PropertyDict
from idf_py_actions.tools import run_target
from idf_py_actions.tools import RunTool

PYTHON = sys.executable


BAUD_RATE = {
    'names': ['-b', '--baud'],
    'help': 'Baud rate for flashing. It can imply monitor baud rate as well if it hasn\'t been defined locally.',
    'scope': 'global',
    'envvar': 'ESPBAUD',
    'default': 460800,
}

PORT = {
    'names': ['-p', '--port'],
    'help': 'Serial port.',
    'scope': 'global',
    'envvar': 'ESPPORT',
    'type': click.Path(),
    'default': None,
}


def yellow_print(message: str, newline: Optional[str]='\n') -> None:
    """Print a message to stderr with yellow highlighting """
    sys.stderr.write('%s%s%s%s' % ('\033[0;33m', message, '\033[0m', newline))
    sys.stderr.flush()


def action_extensions(base_actions: Dict, project_path: str) -> Dict:
    def _get_project_desc(ctx: click.core.Context, args: PropertyDict) -> Any:
        desc_path = os.path.join(args.build_dir, 'project_description.json')
        if not os.path.exists(desc_path):
            ensure_build_directory(args, ctx.info_name)
        with open(desc_path, 'r') as f:
            project_desc = json.load(f)
        return project_desc

    def _get_esptool_args(args: PropertyDict) -> List:
        esptool_path = os.path.join(os.environ['IDF_PATH'], 'components/esptool_py/esptool/esptool.py')
        esptool_wrapper_path = os.environ.get('ESPTOOL_WRAPPER', '')
        if args.port is None:
            args.port = get_default_serial_port()
        result = [PYTHON]
        if os.path.exists(esptool_wrapper_path):
            result += [esptool_wrapper_path]
        result += [esptool_path]
        result += ['-p', args.port]
        result += ['-b', str(args.baud)]

        with open(os.path.join(args.build_dir, 'flasher_args.json')) as f:
            flasher_args = json.load(f)

        extra_esptool_args = flasher_args['extra_esptool_args']
        result += ['--before', extra_esptool_args['before']]
        result += ['--after', extra_esptool_args['after']]
        result += ['--chip', extra_esptool_args['chip']]
        if not extra_esptool_args['stub']:
            result += ['--no-stub']
        return result

    def _get_commandline_options(ctx: click.core.Context) -> List:
        """ Return all the command line options up to first action """
        # This approach ignores argument parsing done Click
        result = []

        for arg in sys.argv:
            if arg in ctx.command.commands_with_aliases:
                break

            result.append(arg)

        return result

    def monitor(action: str, ctx: click.core.Context, args: PropertyDict, print_filter: str, monitor_baud: str, encrypted: bool,
                no_reset: bool, timestamps: bool, timestamp_format: str, force_color: bool) -> None:
        """
        Run esp_idf_monitor to watch build output
        """
        project_desc = _get_project_desc(ctx, args)
        elf_file = os.path.join(args.build_dir, project_desc['app_elf'])

        idf_monitor = os.path.join(os.environ['IDF_PATH'], 'tools/idf_monitor.py')
        monitor_args = [PYTHON, idf_monitor]

        if project_desc['target'] != 'linux':
            if no_reset and args.port is None:
                msg = ('WARNING: --no-reset is ignored. '
                       'Please specify the port with the --port argument in order to use this option.')
                yellow_print(msg)
                no_reset = False

            args.port = args.port or get_default_serial_port()
            monitor_args += ['-p', args.port]

            baud = monitor_baud or os.getenv('IDF_MONITOR_BAUD') or os.getenv('MONITORBAUD')

            if baud is None:
                # Baud hasn't been changed locally (by local baud argument nor by environment variables)
                #
                # Use the global baud rate if it has been changed by the command line.
                # Use project_desc['monitor_baud'] as the last option.

                global_baud_defined = ctx._parameter_source['baud'] == click.core.ParameterSource.COMMANDLINE
                baud = args.baud if global_baud_defined else project_desc['monitor_baud']

            monitor_args += ['-b', baud]

        monitor_args += ['--toolchain-prefix', project_desc['monitor_toolprefix']]

        coredump_decode = get_sdkconfig_value(project_desc['config_file'], 'CONFIG_ESP_COREDUMP_DECODE')
        if coredump_decode is not None:
            monitor_args += ['--decode-coredumps', coredump_decode]

        target_arch_riscv = get_sdkconfig_value(project_desc['config_file'], 'CONFIG_IDF_TARGET_ARCH_RISCV')
        monitor_args += ['--target', project_desc['target']]
        revision = project_desc.get('min_rev')
        if revision:
            monitor_args += ['--revision', revision]

        if target_arch_riscv:
            monitor_args += ['--decode-panic', 'backtrace']

        if print_filter is not None:
            monitor_args += ['--print_filter', print_filter]

        if elf_file:
            monitor_args += [elf_file]

        if encrypted:
            monitor_args += ['--encrypted']

        if no_reset:
            monitor_args += ['--no-reset']

        if timestamps:
            monitor_args += ['--timestamps']

        if timestamp_format:
            monitor_args += ['--timestamp-format', timestamp_format]

        if force_color or os.name == 'nt':
            monitor_args += ['--force-color']

        idf_py = [PYTHON] + _get_commandline_options(ctx)  # commands to re-run idf.py
        monitor_args += ['-m', ' '.join("'%s'" % a for a in idf_py)]
        hints = not args.no_hints

        # Temporally ignore SIGINT, which is used in idf_monitor to spawn gdb.
        old_handler = signal.getsignal(signal.SIGINT)
        signal.signal(signal.SIGINT, signal.SIG_IGN)

        try:
            RunTool('idf_monitor', monitor_args, args.project_dir, build_dir=args.build_dir, hints=hints, interactive=True, convert_output=True)()
        finally:
            signal.signal(signal.SIGINT, old_handler)

    def flash(action: str, ctx: click.core.Context, args: PropertyDict, force: bool, extra_args: str) -> None:
        """
        Run esptool to flash the entire project, from an argfile generated by the build system
        """
        ensure_build_directory(args, ctx.info_name)
        project_desc = _get_project_desc(ctx, args)
        if project_desc['target'] == 'linux':
            yellow_print('skipping flash since running on linux...')
            return

        args.port = args.port or get_default_serial_port()
        extra = list()
        if force:
            extra.append('--force')
        if extra_args:
            extra += shlex.split(extra_args)
        env = {'ESPBAUD': str(args.baud), 'ESPPORT': args.port, 'SERIAL_TOOL_EXTRA_ARGS': ';'.join(extra)}
        run_target(action, args, env, force_progression=True)

    def erase_flash(action: str, ctx: click.core.Context, args: PropertyDict) -> None:
        ensure_build_directory(args, ctx.info_name)
        esptool_args = _get_esptool_args(args)
        esptool_args += ['erase_flash']
        RunTool('esptool.py', esptool_args, args.build_dir, hints=not args.no_hints)()

    def global_callback(ctx: click.core.Context, global_args: Dict, tasks: PropertyDict) -> None:
        encryption = any([task.name in ('encrypted-flash', 'encrypted-app-flash') for task in tasks])
        if encryption:
            for task in tasks:
                if task.name == 'monitor':
                    task.action_args['encrypted'] = True
                    break

    def ota_targets(target_name: str, ctx: click.core.Context, args: PropertyDict) -> None:
        """
        Execute the target build system to build target 'target_name'.
        Additionally set global variables for baud and port.
        Calls ensure_build_directory() which will run cmake to generate a build
        directory (with the specified generator) as needed.
        """
        args.port = args.port or get_default_serial_port()
        ensure_build_directory(args, ctx.info_name)
        run_target(target_name, args, {'ESPBAUD': str(args.baud), 'ESPPORT': args.port})

    def merge_bin(action: str,
                  ctx: click.core.Context,
                  args: PropertyDict,
                  output: str,
                  format: str,
                  md5_disable: str,
                  flash_offset: str,
                  fill_flash_size: str) -> None:
        ensure_build_directory(args, ctx.info_name)
        project_desc = _get_project_desc(ctx, args)
        merge_bin_args = [PYTHON, '-m', 'esptool']
        target = project_desc['target']
        merge_bin_args += ['--chip', target]
        merge_bin_args += ['merge_bin']  # needs to be after the --chip option
        if not output:
            if format in ('raw', 'uf2'):
                output = 'merged-binary.bin'
            elif format == 'hex':
                output = 'merged-binary.hex'
        merge_bin_args += ['-o', output]
        if format:
            merge_bin_args += ['-f', format]
        if md5_disable:
            if format != 'uf2':
                yellow_print('idf.py merge-bin: --md5-disable is only valid for UF2 format. Option will be ignored.')
            else:
                merge_bin_args += ['--md5-disable']
        if flash_offset:
            if format != 'raw':
                yellow_print('idf.py merge-bin: --flash-offset is only valid for RAW format. Option will be ignored.')
            else:
                merge_bin_args += ['-t', flash_offset]
        if fill_flash_size:
            if format != 'raw':
                yellow_print('idf.py merge-bin: --fill-flash-size is only valid for RAW format, option will be ignored.')
            else:
                merge_bin_args += ['--fill-flash-size', fill_flash_size]
        merge_bin_args += ['@flash_args']
        print(f'Merged binary {output} will be created in the build directory...')
        RunTool('merge_bin', merge_bin_args, args.build_dir, build_dir=args.build_dir, hints=not args.no_hints)()

    BAUD_AND_PORT = [BAUD_RATE, PORT]
    flash_options = BAUD_AND_PORT + [
        {
            'names': ['--force'],
            'is_flag': True,
            'help': 'Force write, skip security and compatibility checks. Use with caution!',
        },
        {
            'names': ['--extra-args'],
            'help': (
                'Pass extra arguments to esptool separated by space. For more details see `esptool.py write_flash --help`. '
                'For example to compress and verify data use: `idf.py flash --extra-args="--compress --verify"`. Use with caution!'
            )
        }
    ]
    serial_actions = {
        'global_action_callbacks': [global_callback],
        'actions': {
            'flash': {
                'callback': flash,
                'help': 'Flash the project.',
                'options': global_options + flash_options,
                'order_dependencies': ['all', 'erase-flash'],
            },
            'erase-flash': {
                'callback': erase_flash,
                'help': 'Erase entire flash chip.',
                'options': BAUD_AND_PORT,
            },
            'erase_flash': {
                'callback': erase_flash,
                'deprecated': {
                    'since': 'v4.4',
                    'removed': 'next major release',
                    'message': 'Have you wanted to run "erase-flash" instead?',
                },
                'hidden': True,
                'help': 'Erase entire flash chip.',
                'options': BAUD_AND_PORT,
            },
            'merge-bin': {
                'callback': merge_bin,
                'options': [
                    {
                        'names': ['--output', '-o'],
                        'help': ('Output filename'),
                        'type': click.Path(),
                    },
                    {
                        'names': ['--format', '-f'],
                        'help': ('Format of the output file'),
                        'type': click.Choice(['hex', 'uf2', 'raw']),
                        'default': 'raw',
                    },
                    {
                        'names': ['--md5-disable'],
                        'is_flag': True,
                        'help': ('[ONLY UF2] Disable MD5 checksum in UF2 output.'),
                    },
                    {
                        'names': ['--flash-offset', '-t'],
                        'help': ('[ONLY RAW] Flash offset where the output file will be flashed.'),
                    },
                    {
                        'names': ['--fill-flash-size'],
                        'help': ('[ONLY RAW] If set, the final binary file will be padded with FF bytes up to this flash size.'),
                        'type': click.Choice(['256KB', '512KB', '1MB', '2MB', '4MB', '8MB', '16MB', '32MB', '64MB', '128MB']),
                    },
                ],
                'dependencies': ['all'],  # all = build
            },
            'monitor': {
                'callback':
                monitor,
                'help':
                'Display serial output.',
                'options': [
                    PORT, {
                        'names': ['--print-filter', '--print_filter'],
                        'help':
                        ('Filter monitor output. '
                         'Restrictions on what to print can be specified as a series of <tag>:<log_level> items '
                         'where <tag> is the tag string and <log_level> is a character from the set '
                         '{N, E, W, I, D, V, *} referring to a level. '
                         'For example, "tag1:W" matches and prints only the outputs written with '
                         'ESP_LOGW("tag1", ...) or at lower verbosity level, i.e. ESP_LOGE("tag1", ...). '
                         'Not specifying a <log_level> or using "*" defaults to Verbose level. '
                         'Please see the IDF Monitor section of the ESP-IDF documentation '
                         'for a more detailed description and further examples.'),
                        'default':
                        None,
                    }, {
                        'names': ['--monitor-baud', '-b'],
                        'type':
                        click.INT,
                        'help': ('Baud rate for monitor. '
                                 'If this option is not provided IDF_MONITOR_BAUD and MONITORBAUD '
                                 'environment variables, global baud rate and project_description.json in build directory '
                                 "(generated by CMake from project's sdkconfig) "
                                 'will be checked for default value.'),
                    }, {
                        'names': ['--encrypted', '-E'],
                        'is_flag': True,
                        'help': ('Enable encrypted flash targets. '
                                 'IDF Monitor will invoke encrypted-flash and encrypted-app-flash targets '
                                 'if this option is set. This option is set by default if IDF Monitor was invoked '
                                 'together with encrypted-flash or encrypted-app-flash target.'),
                    }, {
                        'names': ['--no-reset'],
                        'is_flag': True,
                        'help': ('Disable reset on monitor startup. '
                                 'IDF Monitor will not reset the MCU target by toggling DTR/RTS lines on startup '
                                 'if this option is set.'),
                    }, {
                        'names': ['--timestamps'],
                        'is_flag': True,
                        'help': 'Print a time stamp in the beginning of each line.',
                    }, {
                        'names': ['--timestamp-format'],
                        'help': ('Set the formatting of timestamps compatible with strftime(). '
                                 'For example, "%Y-%m-%d %H:%M:%S".'),
                        'default': None
                    }, {
                        'names': ['--force-color'],
                        'is_flag': True,
                        'help': 'Always print ANSI for colors',
                    }

                ],
                'order_dependencies': [
                    'flash',
                    'encrypted-flash',
                    'partition-table-flash',
                    'bootloader-flash',
                    'app-flash',
                    'encrypted-app-flash',
                ],
            },
            'partition-table-flash': {
                'callback': flash,
                'help': 'Flash partition table only.',
                'options': flash_options,
                'order_dependencies': ['partition-table', 'erase-flash'],
            },
            'bootloader-flash': {
                'callback': flash,
                'help': 'Flash bootloader only.',
                'options': flash_options,
                'order_dependencies': ['bootloader', 'erase-flash'],
            },
            'app-flash': {
                'callback': flash,
                'help': 'Flash the app only.',
                'options': flash_options,
                'order_dependencies': ['app', 'erase-flash'],
            },
            'encrypted-app-flash': {
                'callback': flash,
                'help': 'Flash the encrypted app only.',
                'options': flash_options,
                'order_dependencies': ['app', 'erase-flash'],
            },
            'encrypted-flash': {
                'callback': flash,
                'help': 'Flash the encrypted project.',
                'options': flash_options,
                'order_dependencies': ['all', 'erase-flash'],
            },
            'erase-otadata': {
                'callback': ota_targets,
                'help': 'Erase otadata partition.',
                'options': global_options + BAUD_AND_PORT,
            },
            'read-otadata': {
                'callback': ota_targets,
                'help': 'Read otadata partition.',
                'options': global_options + BAUD_AND_PORT,
            },
        },
    }

    return serial_actions
