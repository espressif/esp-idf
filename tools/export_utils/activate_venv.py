# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import sys
from types import SimpleNamespace
from typing import Any

import rich_click as click
from console_output import configure_output
from console_output import status_message
from esp_pylib.excepthook import install_exception_reporting
from esp_pylib.logger import log
from rich.markup import escape
from rich.text import Text
from shell_types import SHELL_CLASSES
from shell_types import SUPPORTED_SHELLS
from utils import conf
from utils import run_cmd


@status_message('Checking python version', rv_on_ok=True)
def check_python_version() -> str:
    # Check the Python version within a virtual environment
    python_version_checker = os.path.join(conf.IDF_PATH, 'tools', 'python_version_checker.py')
    run_cmd([sys.executable, python_version_checker])
    ver = sys.version_info
    return f'{ver[0]}.{ver[1]}.{ver[2]}'


@status_message('Checking python dependencies')
def check_python_dependencies() -> None:
    # Check Python dependencies within the virtual environment
    run_cmd([sys.executable, conf.IDF_TOOLS_PY, 'check-python-dependencies'])


@status_message('Deactivating the current ESP-IDF environment (if any)')
def get_deactivate_cmd() -> str:
    # Get previous ESP-IDF system environment variables
    cmd = [sys.executable, conf.IDF_TOOLS_PY, 'export', '--deactivate']
    stdout: str = run_cmd(cmd)
    return stdout


@status_message('Establishing a new ESP-IDF environment')
def get_idf_env() -> dict[str, str]:
    # Get ESP-IDF system environment variables
    extra_paths_list = [
        os.path.join('components', 'espcoredump'),
        os.path.join('components', 'partition_table'),
        os.path.join('components', 'app_update'),
    ]
    extra_paths = os.pathsep.join([os.path.join(conf.IDF_PATH, path) for path in extra_paths_list])
    cmd = [sys.executable, conf.IDF_TOOLS_PY, 'export', '--format', 'key-value', '--add_paths_extras', extra_paths]
    stdout = run_cmd(cmd)

    # idf_tools.py might not export certain environment variables if they are already set
    idf_env: dict[str, Any] = {
        'IDF_PATH': os.environ['IDF_PATH'],
        'ESP_IDF_VERSION': os.environ['ESP_IDF_VERSION'],
        'IDF_PYTHON_ENV_PATH': os.environ['IDF_PYTHON_ENV_PATH'],
    }

    try:
        for line in stdout.splitlines():
            var, val = line.split('=')
            idf_env[var] = val
    except ValueError as e:
        log.debug('\n'.join(['Output from `./tools/idf_tools.py export --format key-value`:', f'{stdout}']))
        raise ValueError(
            '\n'.join(
                [
                    'Please ensure your ESP-IDF installation is clean, especially file `./tools/idf_tools.py`.',
                    'The command `./tools/idf_tools.py export` appears to be returning unexpected values.',
                    f'Details: {e}',
                ]
            )
        )

    if 'PATH' in idf_env:
        idf_env['PATH'] = os.pathsep.join([extra_paths, idf_env['PATH']])

    return idf_env


@status_message('Identifying shell', rv_on_ok=True)
def detect_shell(args: Any) -> str:
    import psutil

    if args.shell != 'detect':
        log.debug(f'Shell explicitly stated: "{args.shell}"')
        return str(args.shell)

    current_pid = os.getpid()
    detected_shell_name = ''
    while True:
        parent_pid = psutil.Process(current_pid).ppid()
        parent = psutil.Process(parent_pid)
        parent_cmdline = parent.cmdline()
        parent_exe = parent_cmdline[0].lstrip('-')
        parent_name = os.path.basename(parent_exe)
        log.debug(f'Parent: pid: {parent_pid}, cmdline: {parent_cmdline}, exe: {parent_exe}, name: {parent_name}')
        if not parent_name.lower().startswith('python'):
            detected_shell_name = str(parent_name)
            break
        current_pid = parent_pid

    # On Windows shell names are case-insensitive, but ESP-IDF defines them in lowercase
    return detected_shell_name.lower() if sys.platform == 'win32' else detected_shell_name


@status_message('Detecting outdated tools in system', rv_on_ok=True)
def print_uninstall_msg() -> Text:
    stdout = run_cmd([sys.executable, conf.IDF_TOOLS_PY, 'uninstall', '--dry-run'])
    if stdout:
        python_cmd = 'python.exe' if sys.platform == 'win32' else 'python'
        msg = Text('Found tools that are not used by active ESP-IDF version.\n', style='green')
        msg.append(
            f'{stdout}\nTo free up even more space, remove installation packages of those tools.\n'
            f'Use option {python_cmd} {conf.IDF_TOOLS_PY} uninstall --remove-archives.',
            style='bright_cyan',
        )
    else:
        msg = Text('OK - no outdated tools found', style='green')

    return msg


@click.command(
    context_settings={'help_option_names': ['-h', '--help']},
    epilog='On Windows, run `python activate.py` to execute this script in the current terminal window.',
)
@click.option(
    '-s',
    '--shell',
    metavar='SHELL',
    default=os.environ.get('ESP_IDF_SHELL', 'detect'),
    show_default=True,
    help='Explicitly specify shell to start. For example bash, zsh, powershell.exe, cmd.exe',
)
@click.option('-l', '--list', 'list_shells', is_flag=True, help='List supported shells.')
@click.option('-e', '--export', is_flag=True, help='Generate commands to run in the terminal.')
@click.option('-n', '--no-color', is_flag=True, help='Disable ANSI color escape sequences.')
@click.option(
    '-d',
    '--debug',
    'debug_flag',
    is_flag=True,
    default=bool(os.environ.get('ESP_IDF_EXPORT_DEBUG')),
    help='Enable debug information.',
)
@click.option('-q', '--quiet', is_flag=True, help='Suppress all output.')
def main(shell: str, list_shells: bool, export: bool, no_color: bool, debug_flag: bool, quiet: bool) -> None:
    install_exception_reporting()

    # Fill config global holder before configure_output
    conf.ARGS = SimpleNamespace(
        shell=shell,
        list=list_shells,
        export=export,
        no_color=no_color,
        debug=debug_flag,
        quiet=quiet,
    )
    configure_output(no_color=no_color, quiet=quiet, debug=debug_flag)

    log.debug(f'command line: {sys.argv}')
    if conf.ARGS.list:
        log.print(SUPPORTED_SHELLS)
        sys.exit()

    log.print(f'[dark_orange]Activating ESP-IDF {conf.IDF_VERSION}', file=sys.stderr)
    if conf.IDF_PATH_OLD and conf.IDF_PATH != conf.IDF_PATH_OLD:
        log.warn(f"IDF_PATH is changed from '{escape(conf.IDF_PATH_OLD)}' to '{escape(conf.IDF_PATH)}'.")
    else:
        log.print(f"Setting IDF_PATH to '{escape(conf.IDF_PATH)}'.", file=sys.stderr)

    log.debug(f'IDF_PYTHON_ENV_PATH {conf.IDF_PYTHON_ENV_PATH}')

    check_python_version()
    check_python_dependencies()

    deactivate_cmd = get_deactivate_cmd()
    new_esp_idf_env = get_idf_env()
    detected_shell = detect_shell(conf.ARGS)
    print_uninstall_msg()

    if detected_shell not in SHELL_CLASSES:
        log.die(f'"{escape(str(detected_shell))}" shell is not among the supported options: "{SUPPORTED_SHELLS}"')

    shell_obj = SHELL_CLASSES[detected_shell](detected_shell, deactivate_cmd, new_esp_idf_env)

    if conf.ARGS.export:
        shell_obj.export()
        sys.exit()

    log.print(
        f"[dark_orange]Starting new '{escape(str(shell_obj.shell))}' shell with ESP-IDF environment..."
        ' (use "exit" command to quit)',
        file=sys.stderr,
    )
    shell_obj.spawn()
    log.print('[dark_orange]ESP-IDF environment exited.', file=sys.stderr)


if __name__ == '__main__':
    main()
