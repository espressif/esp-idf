# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import os
import re
import shlex
import shutil
import subprocess
import sys
import threading
import time
from textwrap import indent
from threading import Thread
from typing import Any, Dict, List, Optional, Union

from click import INT
from click.core import Context
from esp_coredump import CoreDump
from idf_py_actions.constants import OPENOCD_TAGET_CONFIG, OPENOCD_TAGET_CONFIG_DEFAULT
from idf_py_actions.errors import FatalError
from idf_py_actions.serial_ext import BAUD_RATE, PORT
from idf_py_actions.tools import (PropertyDict, ensure_build_directory, generate_hints, get_default_serial_port,
                                  get_sdkconfig_value, yellow_print)

PYTHON = sys.executable
ESP_ROM_INFO_FILE = 'roms.json'
GDBINIT_PYTHON_TEMPLATE = '''
# Add Python GDB extensions
python
import sys
sys.path = {sys_path}
import freertos_gdb
end
'''
GDBINIT_PYTHON_NOT_SUPPORTED = '''
# Python scripting is not supported in this copy of GDB.
# Please make sure that your Python distribution contains Python shared library.
'''
GDBINIT_BOOTLOADER_ADD_SYMBOLS = '''
# Load bootloader symbols
set confirm off
  add-symbol-file {boot_elf}
set confirm on
'''
GDBINIT_BOOTLOADER_NOT_FOUND = '''
# Bootloader elf was not found
'''
GDBINIT_APP_ADD_SYMBOLS = '''
# Load application file
file {app_elf}
'''
GDBINIT_CONNECT = '''
# Connect to the default openocd-esp port and break on app_main()
target remote :3333
monitor reset halt
maintenance flush register-cache
thbreak app_main
continue
'''
GDBINIT_MAIN = '''
source {py_extensions}
source {symbols}
source {connect}
'''


def get_openocd_arguments(target: str) -> str:
    default_args = OPENOCD_TAGET_CONFIG_DEFAULT.format(target=target)
    return str(OPENOCD_TAGET_CONFIG.get(target, default_args))


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
                    with open(name, 'r') as f:
                        content = f.read()
                        if re.search(r'Listening on port \d+ for gdb connections', content):
                            # expect OpenOCD has started successfully - stop watching
                            return
                    time.sleep(0.5)

                # OpenOCD exited or is not listening -> print full log and terminate
                with open(name, 'r') as f:
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
                                  gdb_timeout_sec: int = None,
                                  core: str = None,
                                  chip_rev: str = None,
                                  save_core: str = None) -> CoreDump:

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

    def get_normalized_path(path: str) -> str:
        if os.name == 'nt':
            return os.path.normpath(path).replace('\\', '\\\\')
        return path

    def get_rom_if_condition_str(date_addr: int, date_str: str) -> str:
        r = []
        for i in range(0, len(date_str), 4):
            value = hex(int.from_bytes(bytes(date_str[i:i + 4], 'utf-8'), 'little'))
            r.append(f'(*(int*) {hex(date_addr + i)}) == {value}')
        return 'if ' + ' && '.join(r)

    def generate_gdbinit_rom_add_symbols(target: str) -> str:
        base_ident = '  '
        rom_elfs_dir = os.getenv('ESP_ROM_ELF_DIR')
        if not rom_elfs_dir:
            raise FatalError(
                'ESP_ROM_ELF_DIR environment variable is not defined. Please try to run IDF "install" and "export" scripts.')
        with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), ESP_ROM_INFO_FILE), 'r') as f:
            roms = json.load(f)
            if target not in roms:
                msg_body = f'Target "{target}" was not found in "{ESP_ROM_INFO_FILE}". Please check IDF integrity.'
                if os.getenv('ESP_IDF_GDB_TESTING'):
                    raise FatalError(msg_body)
                print(f'Warning: {msg_body}')
                return f'# {msg_body}'
            r = ['', f'# Load {target} ROM ELF symbols']
            r.append('define target hookpost-remote')
            r.append('set confirm off')
            # Since GDB does not have 'else if' statement than we use nested 'if..else' instead.
            for i, k in enumerate(roms[target], 1):
                indent_str = base_ident * i
                rom_file = get_normalized_path(os.path.join(rom_elfs_dir, f'{target}_rev{k["rev"]}_rom.elf'))
                build_date_addr = int(k['build_date_str_addr'], base=16)
                r.append(indent(f'# if $_streq((char *) {hex(build_date_addr)}, "{k["build_date_str"]}")', indent_str))
                r.append(indent(get_rom_if_condition_str(build_date_addr, k['build_date_str']), indent_str))
                r.append(indent(f'add-symbol-file {rom_file}', indent_str + base_ident))
                r.append(indent('else', indent_str))
                if i == len(roms[target]):
                    # In case no one known ROM ELF fits - print error and exit with error code 1
                    indent_str += base_ident
                    msg_body = f'unknown {target} ROM revision.'
                    if os.getenv('ESP_IDF_GDB_TESTING'):
                        r.append(indent(f'echo Error: {msg_body}\\n', indent_str))
                        r.append(indent('quit 1', indent_str))
                    else:
                        r.append(indent(f'echo Warning: {msg_body}\\n', indent_str))
            # Close 'else' operators
            for i in range(len(roms[target]), 0, -1):
                r.append(indent('end', base_ident * i))
            r.append('set confirm on')
            r.append('end')
            r.append('')
            return '\n'.join(r)
        raise FatalError(f'{ESP_ROM_INFO_FILE} file not found. Please check IDF integrity.')

    def generate_gdbinit_files(gdb: str, gdbinit: Optional[str], project_desc: Dict[str, Any]) -> None:
        app_elf = get_normalized_path(os.path.join(project_desc['build_dir'], project_desc['app_elf']))
        if not os.path.exists(app_elf):
            raise FatalError('ELF file not found. You need to build & flash the project before running debug targets')

        # Recreate empty 'gdbinit' directory
        gdbinit_dir = '/'.join([project_desc['build_dir'], 'gdbinit'])
        if os.path.isfile(gdbinit_dir):
            os.remove(gdbinit_dir)
        elif os.path.isdir(gdbinit_dir):
            shutil.rmtree(gdbinit_dir)
        os.mkdir(gdbinit_dir)

        # Prepare gdbinit for Python GDB extensions import
        py_extensions = '/'.join([gdbinit_dir, 'py_extensions'])
        with open(py_extensions, 'w') as f:
            if is_gdb_with_python(gdb):
                f.write(GDBINIT_PYTHON_TEMPLATE.format(sys_path=sys.path))
            else:
                f.write(GDBINIT_PYTHON_NOT_SUPPORTED)

        # Prepare gdbinit for related ELFs symbols load
        symbols = '/'.join([gdbinit_dir, 'symbols'])
        with open(symbols, 'w') as f:
            boot_elf = get_normalized_path(project_desc['bootloader_elf']) if 'bootloader_elf' in project_desc else None
            if boot_elf and os.path.exists(boot_elf):
                f.write(GDBINIT_BOOTLOADER_ADD_SYMBOLS.format(boot_elf=boot_elf))
            else:
                f.write(GDBINIT_BOOTLOADER_NOT_FOUND)
            f.write(generate_gdbinit_rom_add_symbols(project_desc['target']))
            f.write(GDBINIT_APP_ADD_SYMBOLS.format(app_elf=app_elf))

        # Generate the gdbinit for target connect if no custom gdbinit is present
        if not gdbinit:
            gdbinit = '/'.join([gdbinit_dir, 'connect'])
            with open(gdbinit, 'w') as f:
                f.write(GDBINIT_CONNECT)

        with open(os.path.join(gdbinit_dir, 'gdbinit'), 'w') as f:
            f.write(GDBINIT_MAIN.format(py_extensions=py_extensions, symbols=symbols, connect=gdbinit))

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
                    with open(name, 'r') as f:
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
        with open(desc_path, 'r') as f:
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
            target = project_desc['target']
            openocd_arguments = get_openocd_arguments(target)
            print(
                'Note: OpenOCD cfg not found (via env variable OPENOCD_COMMANDS nor as a --openocd-commands argument)\n'
                'OpenOCD arguments default to: "{}"'.format(openocd_arguments))
        # script directory is taken from the environment by OpenOCD, update only if command line arguments to override
        if openocd_scripts is not None:
            openocd_arguments += ' -s {}'.format(openocd_scripts)
        local_dir = project_desc['build_dir']
        args = ['openocd'] + shlex.split(openocd_arguments)
        openocd_out_name = os.path.join(local_dir, OPENOCD_OUT_FILE)
        openocd_out = open(openocd_out_name, 'w')
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

    def get_gdb_args(project_desc: Dict[str, Any]) -> List:
        gdbinit = os.path.join(project_desc['build_dir'], 'gdbinit', 'gdbinit')
        args = ['-x={}'.format(gdbinit)]
        debug_prefix_gdbinit = project_desc.get('debug_prefix_map_gdbinit')
        if debug_prefix_gdbinit:
            args.append('-ix={}'.format(debug_prefix_gdbinit))
        return args

    def gdbui(action: str, ctx: Context, args: PropertyDict, gdbgui_port: Optional[str], gdbinit: Optional[str],
              require_openocd: bool) -> None:
        """
        Asynchronous GDB-UI target
        """
        project_desc = get_project_desc(args, ctx)
        local_dir = project_desc['build_dir']
        gdb = project_desc['monitor_toolprefix'] + 'gdb'
        generate_gdbinit_files(gdb, gdbinit, project_desc)

        gdb_args_list = get_gdb_args(project_desc)
        if sys.version_info[:2] >= (3, 11):
            # If we use Python 3.11+ then the only compatible gdbgui doesn't support the --gdb-args argument. This
            # check is easier than checking gdbgui version or re-running the process in case of gdb-args-related
            # failure.
            gdb_args = ' '.join(gdb_args_list)
            args = ['gdbgui', '-g', ' '.join((gdb, gdb_args))]
        else:
            # this is a workaround for gdbgui
            # gdbgui is using shlex.split for the --gdb-args option. When the input is:
            # - '"-x=foo -x=bar"', would return ['foo bar']
            # - '-x=foo', would return ['-x', 'foo'] and mess up the former option '--gdb-args'
            # so for one item, use extra double quotes. for more items, use no extra double quotes.
            gdb_args = '"{}"'.format(' '.join(gdb_args_list)) if len(gdb_args_list) == 1 else ' '.join(gdb_args_list)
            args = ['gdbgui', '-g', gdb, '--gdb-args', gdb_args]

        if gdbgui_port is not None:
            args += ['--port', gdbgui_port]
        gdbgui_out_name = os.path.join(local_dir, GDBGUI_OUT_FILE)
        gdbgui_out = open(gdbgui_out_name, 'w')
        env = os.environ.copy()
        # The only known solution for https://github.com/cs01/gdbgui/issues/359 is to set the following environment
        # variable. The greenlet package cannot be downgraded for compatibility with other requirements (gdbgui,
        # pygdbmi).
        env['PURE_PYTHON'] = '1'
        try:
            print('Running: ', args)
            process = subprocess.Popen(args, stdout=gdbgui_out, stderr=subprocess.STDOUT, bufsize=1, env=env)
        except (OSError, subprocess.CalledProcessError) as e:
            print(e)
            if sys.version_info[:2] >= (3, 11) and sys.platform == 'win32':
                raise SystemExit('Unfortunately, gdbgui is supported only with Python 3.10 or older. '
                                 'See: https://github.com/espressif/esp-idf/issues/10116. '
                                 'Please use "idf.py gdb" or debug in Eclipse/Vscode instead.')
            raise FatalError('Error starting gdbgui. Please make sure gdbgui has been installed with '
                             '"install.{sh,bat,ps1,fish} --enable-gdbgui" and can be started.', ctx)

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

    def gdbtui(action: str, ctx: Context, args: PropertyDict, gdbinit: str, require_openocd: bool) -> None:
        """
        Synchronous GDB target with text ui mode
        """
        gdb(action, ctx, args, False, 1, gdbinit, require_openocd)

    def gdb(action: str, ctx: Context, args: PropertyDict, batch: bool, gdb_tui: Optional[int], gdbinit: Optional[str],
            require_openocd: bool) -> None:
        """
        Synchronous GDB target
        """
        watch_openocd = Thread(target=_check_openocd_errors, args=(fail_if_openocd_failed, action, ctx,))
        watch_openocd.start()
        processes['threads_to_join'].append(watch_openocd)
        project_desc = get_project_desc(args, ctx)
        gdb = project_desc['monitor_toolprefix'] + 'gdb'
        generate_gdbinit_files(gdb, gdbinit, project_desc)
        args = [gdb, *get_gdb_args(project_desc)]
        if gdb_tui is not None:
            args += ['-tui']
        if batch:
            args += ['--batch']
        p = subprocess.Popen(args)
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
                      core: str = None,
                      chip_rev: str = None,
                      save_core: str = None) -> None:
        espcoredump = _get_espcoredump_instance(ctx=ctx, args=args, gdb_timeout_sec=gdb_timeout_sec, core=core,
                                                chip_rev=chip_rev,
                                                save_core=save_core)

        espcoredump.info_corefile()

    def coredump_debug(action: str,
                       ctx: Context,
                       args: PropertyDict,
                       core: str = None,
                       chip_rev: str = None,
                       save_core: str = None) -> None:
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
        'names': ['--gdbinit'],
        'help': 'Specify the name of gdbinit file to use\n',
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
                    }, gdbinit, fail_if_openocd_failed
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
                    }, gdbinit, fail_if_openocd_failed
                ],
                'order_dependencies': ['all', 'flash'],
            },
            'gdbtui': {
                'callback': gdbtui,
                'help': 'GDB TUI mode.',
                'options': [gdbinit, fail_if_openocd_failed],
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
