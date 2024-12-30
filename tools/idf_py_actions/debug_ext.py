# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import os
import re
import shlex
import subprocess
import sys
import threading
import time
from threading import Thread
from typing import Any
from typing import Dict
from typing import List
from typing import Optional
from typing import Tuple
from typing import Union

from click import INT
from click.core import Context
from esp_coredump import CoreDump
from idf_py_actions.errors import FatalError
from idf_py_actions.serial_ext import BAUD_RATE
from idf_py_actions.serial_ext import PORT
from idf_py_actions.tools import ensure_build_directory
from idf_py_actions.tools import generate_hints
from idf_py_actions.tools import get_default_serial_port
from idf_py_actions.tools import get_sdkconfig_value
from idf_py_actions.tools import PropertyDict
from idf_py_actions.tools import yellow_print


def chip_rev_to_int(chip_rev: Optional[str]) -> Union[int, None]:
    # The chip rev will be derived from the elf file if none are returned.
    # The chip rev must be supplied for coredump files generated with idf versions less than 5.1 in order to load
    # rom elf file.
    if not chip_rev or not all(c.isdigit() or c == '.' for c in chip_rev):
        return None
    if '.' not in chip_rev:
        chip_rev += '.0'
    major, minor = map(int, chip_rev.split('.'))
    return major * 100 + minor


def action_extensions(base_actions: Dict, project_path: str) -> Dict:
    OPENOCD_OUT_FILE = 'openocd_out.txt'
    GDBGUI_OUT_FILE = 'gdbgui_out.txt'
    # Internal dictionary of currently active processes, threads and their output files
    processes: Dict = {'threads_to_join': [], 'allow_hints': True}

    def _print_hints(file_name: str) -> None:
        if not processes['allow_hints']:
            return

        for hint in generate_hints(file_name):
            if sys.stderr.isatty():
                yellow_print(hint)
            else:
                # Hints go to stderr. Flush stdout, so hints are printed last.
                sys.stdout.flush()
                print(hint, file=sys.stderr)

    def _check_openocd_errors(fail_if_openocd_failed: Dict, target: str, ctx: Context) -> None:
        if fail_if_openocd_failed:
            if 'openocd' in processes and processes['openocd'] is not None:
                p = processes['openocd']
                name = processes['openocd_outfile_name']
                # watch OpenOCD (for 5x500ms) to check if it hasn't terminated or outputs an error
                for _ in range(5):
                    if p.poll() is not None:
                        print('OpenOCD exited with {}'.format(p.poll()))
                        break
                    with open(name, 'r', encoding='utf-8') as f:
                        content = f.read()
                        if re.search(r'Listening on port \d+ for gdb connections', content):
                            # expect OpenOCD has started successfully - stop watching
                            return
                    time.sleep(0.5)

                # OpenOCD exited or is not listening -> print full log and terminate
                with open(name, 'r', encoding='utf-8') as f:
                    print(f.read())

                raise FatalError('Action "{}" failed due to errors in OpenOCD'.format(target), ctx)

    def _terminate_async_target(target: str) -> None:
        if target in processes and processes[target] is not None:
            try:
                if target + '_outfile' in processes:
                    processes[target + '_outfile'].close()
                p = processes[target]
                if p.poll() is None:
                    p.terminate()
                    # waiting 10x100ms for the process to terminate gracefully
                    for _ in range(10):
                        if p.poll() is not None:
                            break
                        time.sleep(0.1)
                    else:
                        p.kill()
                if target + '_outfile_name' in processes:
                    _print_hints(processes[target + '_outfile_name'])
            except Exception as e:
                print(e)
                print('Failed to close/kill {}'.format(target))
            processes[target] = None  # to indicate this has ended

    def _get_espcoredump_instance(ctx: Context,
                                  args: PropertyDict,
                                  gdb_timeout_sec: Optional[int] = None,
                                  core: Optional[str] = None,
                                  chip_rev: Optional[str] = None,
                                  save_core: Optional[str] = None) -> CoreDump:

        ensure_build_directory(args, ctx.info_name)
        project_desc = get_project_desc(args, ctx)
        coredump_to_flash_config = get_sdkconfig_value(project_desc['config_file'],
                                                       'CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH')
        coredump_to_flash = coredump_to_flash_config.rstrip().endswith('y') if coredump_to_flash_config else False

        prog = os.path.join(project_desc['build_dir'], project_desc['app_elf'])

        espcoredump_kwargs = dict()

        espcoredump_kwargs['baud'] = args.baud
        espcoredump_kwargs['gdb_timeout_sec'] = gdb_timeout_sec
        espcoredump_kwargs['chip_rev'] = chip_rev_to_int(chip_rev)

        # for reproducible builds
        extra_gdbinit_file = project_desc.get('debug_prefix_map_gdbinit', None)

        if extra_gdbinit_file:
            espcoredump_kwargs['extra_gdbinit_file'] = extra_gdbinit_file

        if core:
            espcoredump_kwargs['core'] = core
            espcoredump_kwargs['core_format'] = 'auto'
            espcoredump_kwargs['chip'] = get_sdkconfig_value(project_desc['config_file'], 'CONFIG_IDF_TARGET')
        elif coredump_to_flash:
            #  If the core dump is read from flash, we don't need to specify the --core-format argument at all.
            #  The format will be determined automatically
            args.port = args.port or get_default_serial_port()
        else:
            print('Path to core dump file is not provided. '
                  "Core dump can't be read from flash since this option is not enabled in menuconfig")
            sys.exit(1)

        espcoredump_kwargs['port'] = args.port
        espcoredump_kwargs['parttable_off'] = get_sdkconfig_value(project_desc['config_file'],
                                                                  'CONFIG_PARTITION_TABLE_OFFSET')

        if save_core:
            espcoredump_kwargs['save_core'] = save_core

        espcoredump_kwargs['prog'] = prog

        # compatibility check for esp-coredump < 1.5.2
        try:
            coredump = CoreDump(**espcoredump_kwargs)
        except TypeError as e:
            # 'parttable_off' was added in esp-coredump 1.5.2
            # remove argument and retry without it
            if 'parttable_off' in str(e):
                espcoredump_kwargs.pop('parttable_off')
                coredump = CoreDump(**espcoredump_kwargs)
            else:
                raise
        return coredump

    def is_gdb_with_python(gdb: str) -> bool:
        # execute simple python command to check is it supported
        return subprocess.run([gdb, '--batch-silent', '--ex', 'python import os'],
                              stderr=subprocess.DEVNULL).returncode == 0

    def debug_cleanup() -> None:
        print('cleaning up debug targets')
        for t in processes['threads_to_join']:
            if threading.currentThread() != t:
                t.join()
        _terminate_async_target('openocd')
        _terminate_async_target('gdbgui')
        _terminate_async_target('gdb')

    def post_debug(action: str, ctx: Context, args: PropertyDict, **kwargs: str) -> None:
        """ Deal with asynchronous targets, such as openocd running in background """
        if kwargs['block'] == 1:
            for target in ['openocd', 'gdbgui']:
                if target in processes and processes[target] is not None:
                    break
            else:
                return
            try:
                p = processes[target]
                name = processes[target + '_outfile_name']
                pos = 0
                while True:
                    with open(name, 'r', encoding='utf-8') as f:
                        f.seek(pos)
                        for line in f:
                            print(line.rstrip())
                        pos = f.tell()
                    if p.poll() is not None:
                        print('"{}" exited with {}'.format(target, p.poll()))
                        break
                    time.sleep(0.5)
            except KeyboardInterrupt:
                print('Terminated -> exiting debug utility targets')
        _terminate_async_target('openocd')
        _terminate_async_target('gdbgui')

    def get_project_desc(args: PropertyDict, ctx: Context) -> Any:
        desc_path = os.path.join(args.build_dir, 'project_description.json')
        if not os.path.exists(desc_path):
            ensure_build_directory(args, ctx.info_name)
        with open(desc_path, 'r', encoding='utf-8') as f:
            project_desc = json.load(f)
            return project_desc

    def openocd(action: str, ctx: Context, args: PropertyDict, openocd_scripts: Optional[str],
                openocd_commands: str) -> None:
        """
        Execute openocd as external tool
        """
        if os.getenv('OPENOCD_SCRIPTS') is None:
            raise FatalError('OPENOCD_SCRIPTS not found in the environment: Please run export.sh/export.bat', ctx)
        openocd_arguments = os.getenv('OPENOCD_COMMANDS') if openocd_commands is None else openocd_commands
        project_desc = get_project_desc(args, ctx)
        if openocd_arguments is None:
            # use default value if commands not defined in the environment nor command line
            openocd_arguments = project_desc.get('debug_arguments_openocd', '')
            print(
                'Note: OpenOCD cfg not found (via env variable OPENOCD_COMMANDS nor as a --openocd-commands argument)\n'
                'OpenOCD arguments default to: "{}"'.format(openocd_arguments))
        # script directory is taken from the environment by OpenOCD, update only if command line arguments to override
        if openocd_scripts is not None:
            openocd_arguments += ' -s {}'.format(openocd_scripts)
        local_dir = project_desc['build_dir']
        args = ['openocd'] + shlex.split(openocd_arguments)
        openocd_out_name = os.path.join(local_dir, OPENOCD_OUT_FILE)
        openocd_out = open(openocd_out_name, 'w', encoding='utf-8')
        try:
            process = subprocess.Popen(args, stdout=openocd_out, stderr=subprocess.STDOUT, bufsize=1)
        except Exception as e:
            print(e)
            raise FatalError(
                'Error starting openocd. Please make sure it is installed and is present in executable paths', ctx)

        processes['openocd'] = process
        processes['openocd_outfile'] = openocd_out
        processes['openocd_outfile_name'] = openocd_out_name
        print('OpenOCD started as a background task {}'.format(process.pid))

    def get_gdb_args(project_desc: Dict[str, Any], gdb_x: Tuple, gdb_ex: Tuple, gdb_commands: Optional[str]) -> List[str]:
        # check if the application was built and ELF file is in place.
        app_elf = os.path.join(project_desc.get('build_dir', ''), project_desc.get('app_elf', ''))
        if not os.path.exists(app_elf):
            raise FatalError('ELF file not found. You need to build & flash the project before running debug targets')
        # debugger application name (xtensa-esp32-elf-gdb, riscv32-esp-elf-gdb, ...)
        gdb_name = project_desc.get('monitor_toolprefix', '') + 'gdb'
        gdb_args = [gdb_name]
        gdbinit_files = project_desc.get('gdbinit_files')
        if not gdbinit_files:
            raise FatalError('Please check if the project was configured correctly ("gdbinit_files" not found in "project_description.json").')
        gdbinit_files = sorted(gdbinit_files.items())
        gdb_x_list = list(gdb_x)
        gdb_x_names = [os.path.basename(x) for x in gdb_x_list]
        # compile predefined gdbinit files options.
        for name, path in gdbinit_files:
            name = name[len('xx_'):]
            if name == 'py_extensions':
                if not is_gdb_with_python(gdb_name):
                    continue
            # Replace predefined gdbinit with user's if passed with the same name.
            if name in gdb_x_names:
                gdb_x_index = gdb_x_names.index(name)
                gdb_args.append(f'-x={gdb_x_list[gdb_x_index]}')
                gdb_x_list.pop(gdb_x_index)
                continue
            if name == 'connect' and gdb_x_list:  # TODO IDF-11692
                continue
            gdb_args.append(f'-x={path}')
        # append user-defined gdbinit files
        for x in gdb_x_list:
            gdb_args.append(f'-x={x}')
        # add user-defined commands
        if gdb_ex:
            for ex in gdb_ex:
                gdb_args.append('-ex')
                gdb_args.append(ex)
        # add user defined options
        if gdb_commands:
            gdb_args += shlex.split(gdb_commands)

        return gdb_args

    def _get_gdbgui_version(ctx: Context) -> Tuple[int, ...]:
        subprocess_success = False
        try:
            completed_process = subprocess.run(['gdbgui', '--version'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            captured_output = completed_process.stdout.decode('utf-8', 'ignore')
            subprocess_success = True
        except FileNotFoundError:
            # This is happening at least with Python 3.12 when gdbgui is not installed
            pass

        if not subprocess_success or completed_process.returncode != 0:
            if sys.version_info[:2] >= (3, 11) and sys.platform == 'win32':
                raise SystemExit('Unfortunately, gdbgui is supported only with Python 3.10 or older. '
                                 'See: https://github.com/espressif/esp-idf/issues/10116. '
                                 'Please use "idf.py gdb" or debug in Eclipse/Vscode instead.')
            if sys.version_info[:2] >= (3, 13) and sys.platform != 'win32':
                raise SystemExit('Unfortunately, gdbgui is supported only with Python 3.12 or older. '
                                 'See: https://github.com/cs01/gdbgui/issues/494. '
                                 'Please use "idf.py gdb" or debug in Eclipse/Vscode instead.')
            raise FatalError('Error starting gdbgui. Please make sure gdbgui has been installed with '
                             '"install.{sh,bat,ps1,fish} --enable-gdbgui" and can be started. '
                             f'Error: {captured_output if subprocess_success else "Unknown"}', ctx)

        v = re.search(r'(\d+)(?:\.(\d+))?(?:\.(\d+))?(?:\.(\d+))?', captured_output)
        if not v:
            raise SystemExit(f'Error: "gdbgui --version" returned "{captured_output}"')
        return tuple(int(i) if i else 0 for i in (v[1], v[2], v[3], v[4]))

    def gdbui(action: str, ctx: Context, args: PropertyDict, gdbgui_port: Optional[str], gdbinit: Tuple,
              ex: Tuple, gdb_commands: Optional[str], require_openocd: bool) -> None:
        """
        Asynchronous GDB-UI target
        """
        project_desc = get_project_desc(args, ctx)
        local_dir = project_desc['build_dir']
        gdb = project_desc['monitor_toolprefix'] + 'gdb'

        gdbgui_version = _get_gdbgui_version(ctx)
        gdb_args = get_gdb_args(project_desc, gdbinit, ex, gdb_commands)
        if gdbgui_version >= (0, 14, 0, 0):
            # See breaking changes https://github.com/cs01/gdbgui/blob/master/CHANGELOG.md#01400, especially the
            # replacement of command line arguments.
            gdbgui_args = ['gdbgui', '-g', ' '.join(gdb_args)]
        else:
            # this is a workaround for gdbgui
            # gdbgui is using shlex.split for the --gdb-args option. When the input is:
            # - '"-x=foo -x=bar"', would return ['foo bar']
            # - '-x=foo', would return ['-x', 'foo'] and mess up the former option '--gdb-args'
            # so for one item, use extra double quotes. for more items, use no extra double quotes.
            gdb = gdb_args[0]
            gdb_args_list = gdb_args[1:]
            gdb_args_str = '"{}"'.format(' '.join(gdb_args_list)) if len(gdb_args_list) == 1 else ' '.join(gdb_args_list)
            gdbgui_args = ['gdbgui', '-g', gdb, '--gdb-args', gdb_args_str]

        if gdbgui_port is not None:
            gdbgui_args += ['--port', gdbgui_port]
        gdbgui_out_name = os.path.join(local_dir, GDBGUI_OUT_FILE)
        gdbgui_out = open(gdbgui_out_name, 'w', encoding='utf-8')
        env = os.environ.copy()
        # The only known solution for https://github.com/cs01/gdbgui/issues/359 is to set the following environment
        # variable. The greenlet package cannot be downgraded for compatibility with other requirements (gdbgui,
        # pygdbmi).
        env['PURE_PYTHON'] = '1'
        try:
            print('Running: ', gdbgui_args)
            process = subprocess.Popen(gdbgui_args, stdout=gdbgui_out, stderr=subprocess.STDOUT, bufsize=1, env=env)
        except (OSError, subprocess.CalledProcessError) as e:
            print(e)
            raise FatalError('Error starting gdbgui', ctx)

        processes['gdbgui'] = process
        processes['gdbgui_outfile'] = gdbgui_out
        processes['gdbgui_outfile_name'] = gdbgui_out_name
        print('gdbgui started as a background task {}'.format(process.pid))
        _check_openocd_errors(fail_if_openocd_failed, action, ctx)

    def global_callback(ctx: Context, global_args: PropertyDict, tasks: List) -> None:
        def move_to_front(task_name: str) -> None:
            for index, task in enumerate(tasks):
                if task.name == task_name:
                    tasks.insert(0, tasks.pop(index))
                    break

        processes['allow_hints'] = not ctx.params['no_hints']
        debug_targets = any([task.name in ('openocd', 'gdbgui') for task in tasks])
        if debug_targets:
            # Register the meta cleanup callback -> called on FatalError
            ctx.meta['cleanup'] = debug_cleanup
            move_to_front('gdbgui')  # possibly 2nd
            move_to_front('openocd')  # always 1st
            # followed by "monitor", "gdb" or "gdbtui" in any order

            post_action = ctx.invoke(ctx.command.get_command(ctx, 'post_debug'))
            if any([task.name in ('monitor', 'gdb', 'gdbtui') for task in tasks]):
                post_action.action_args['block'] = 0
            else:
                post_action.action_args['block'] = 1
            tasks.append(post_action)  # always last
        if any([task.name == 'openocd' for task in tasks]):
            for task in tasks:
                if task.name in ('gdb', 'gdbgui', 'gdbtui'):
                    task.action_args['require_openocd'] = True

    def gdbtui(action: str, ctx: Context, args: PropertyDict, gdbinit: Tuple, ex: Tuple, gdb_commands: str, require_openocd: bool) -> None:
        """
        Synchronous GDB target with text ui mode
        """
        gdb(action, ctx, args, False, 1, gdbinit, ex, gdb_commands, require_openocd)

    def gdb(action: str, ctx: Context, args: PropertyDict, batch: bool, gdb_tui: Optional[int], gdbinit: Tuple,
            ex: Tuple, gdb_commands: Optional[str], require_openocd: bool) -> None:
        """
        Synchronous GDB target
        """
        watch_openocd = Thread(target=_check_openocd_errors, args=(fail_if_openocd_failed, action, ctx,))
        watch_openocd.start()
        processes['threads_to_join'].append(watch_openocd)
        project_desc = get_project_desc(args, ctx)
        gdb_args = get_gdb_args(project_desc, gdbinit, ex, gdb_commands)
        if gdb_tui is not None:
            gdb_args += ['-tui']
        if batch:
            gdb_args += ['--batch']
        p = subprocess.Popen(gdb_args)
        processes['gdb'] = p
        while True:
            try:
                p.wait()
                break
            except KeyboardInterrupt:
                # Catching Keyboard interrupt, as this is used for breaking running program in gdb
                continue
            finally:
                watch_openocd.join()
                try:
                    processes['threads_to_join'].remove(watch_openocd)
                except ValueError:
                    # Valid scenario: watch_openocd task won't be in the list if openocd not started from idf.py
                    pass

    def coredump_info(action: str,
                      ctx: Context,
                      args: PropertyDict,
                      gdb_timeout_sec: int,
                      core: Optional[str] = None,
                      chip_rev: Optional[str] = None,
                      save_core: Optional[str] = None) -> None:
        espcoredump = _get_espcoredump_instance(ctx=ctx, args=args, gdb_timeout_sec=gdb_timeout_sec, core=core,
                                                chip_rev=chip_rev,
                                                save_core=save_core)

        espcoredump.info_corefile()

    def coredump_debug(action: str,
                       ctx: Context,
                       args: PropertyDict,
                       core: Optional[str] = None,
                       chip_rev: Optional[str] = None,
                       save_core: Optional[str] = None) -> None:
        espcoredump = _get_espcoredump_instance(ctx=ctx, args=args, core=core, chip_rev=chip_rev, save_core=save_core)

        espcoredump.dbg_corefile()

    coredump_base = [
        {
            'names': ['--core', '-c'],
            'help': 'Path to core dump file (if skipped core dump will be read from flash)',
        },
        {
            'names': ['--chip-rev'],
            'help': 'Specify the chip revision (e.g., 0.1). If provided, the corresponding ROM ELF file will be used '
                    'for decoding the core dump, improving stack traces. This is only needed for core dumps from IDF '
                    '<v5.1. Newer versions already contain chip revision information.',
        },
        {
            'names': ['--save-core', '-s'],
            'help': 'Save core to file. Otherwise temporary core file will be deleted.',
        },
    ]
    gdb_timeout_sec_opt = {
        'names': ['--gdb-timeout-sec'],
        'type': INT,
        'default': 3,
        'help': 'Overwrite the default internal delay for gdb responses',
    }
    fail_if_openocd_failed = {
        'names': ['--require-openocd', '--require_openocd'],
        'help': 'Fail this target if openocd (this targets dependency) failed.\n',
        'is_flag': True,
        'default': False,
    }
    gdbinit = {
        'names': ['--gdbinit', '--x', '-x'],
        'help': 'Specify the name of gdbinit file to use\n',
        'default': None,
        'multiple': True,
    }
    ex = {
        'names': ['--ex', '-ex'],
        'help':
            ('Execute given GDB command.\n'),
        'default': None,
        'multiple': True,
    }
    gdb_commands = {
        'names': ['--gdb-commands', '--gdb_commands'],
        'help':
            ('Command line arguments for gdb.\n'),
        'default': None,
    }
    debug_actions = {
        'global_action_callbacks': [global_callback],
        'actions': {
            'openocd': {
                'callback': openocd,
                'help': 'Run openocd from current path',
                'options': [
                    {
                        'names': ['--openocd-scripts', '--openocd_scripts'],
                        'help':
                            ('Script directory for openocd cfg files.\n'),
                        'default':
                            None,
                    },
                    {
                        'names': ['--openocd-commands', '--openocd_commands'],
                        'help':
                            ('Command line arguments for openocd.\n'),
                        'default': None,
                    }
                ],
                'order_dependencies': ['all', 'flash'],
            },
            'gdb': {
                'callback': gdb,
                'help': 'Run the GDB.',
                'options': [
                    {
                        'names': ['--batch'],
                        'help': ('exit after processing gdbinit.\n'),
                        'hidden': True,
                        'is_flag': True,
                        'default': False,
                    },
                    {
                        'names': ['--gdb-tui', '--gdb_tui'],
                        'help': ('run gdb in TUI mode\n'),
                        'default': None,
                    }, gdbinit, ex, gdb_commands, fail_if_openocd_failed
                ],
                'order_dependencies': ['all', 'flash'],
            },
            'gdbgui': {
                'callback': gdbui,
                'help': 'GDB UI in default browser.',
                'options': [
                    {
                        'names': ['--gdbgui-port', '--gdbgui_port'],
                        'help':
                            ('The port on which gdbgui will be hosted. Default: 5000\n'),
                        'default':
                            None,
                    }, gdbinit, ex, gdb_commands, fail_if_openocd_failed
                ],
                'order_dependencies': ['all', 'flash'],
            },
            'gdbtui': {
                'callback': gdbtui,
                'help': 'GDB TUI mode.',
                'options': [gdbinit, ex, gdb_commands, fail_if_openocd_failed],
                'order_dependencies': ['all', 'flash'],
            },
            'coredump-info': {
                'callback': coredump_info,
                'help': 'Print crashed task’s registers, callstack, list of available tasks in the system, '
                        'memory regions and contents of memory stored in core dump (TCBs and stacks)',
                'options': coredump_base + [PORT, BAUD_RATE, gdb_timeout_sec_opt],  # type: ignore
                'order_dependencies': ['all', 'flash'],
            },
            'coredump-debug': {
                'callback': coredump_debug,
                'help': 'Create core dump ELF file and run GDB debug session with this file.',
                'options': coredump_base + [PORT, BAUD_RATE],  # type: ignore
                'order_dependencies': ['all', 'flash'],
            },
            'post-debug': {
                'callback': post_debug,
                'help': 'Utility target to read the output of async debug action and stop them.',
                'options': [
                    {
                        'names': ['--block', '--block'],
                        'help':
                            ('Set to 1 for blocking the console on the outputs of async debug actions\n'),
                        'default': 0,
                    },
                ],
                'order_dependencies': [],
            },
            'post_debug': {
                'callback': post_debug,
                'deprecated': {
                    'since': 'v4.4',
                    'removed': 'v5.0',
                    'exit_with_error': True,
                    'message': 'Have you wanted to run "post-debug" instead?',
                },
                'hidden': True,
                'help': 'Utility target to read the output of async debug action and stop them.',
                'options': [
                    {
                        'names': ['--block', '--block'],
                        'help':
                            ('Set to 1 for blocking the console on the outputs of async debug actions\n'),
                        'default': 0,
                    },
                ],
                'order_dependencies': [],
            },
        },
    }

    return debug_actions
