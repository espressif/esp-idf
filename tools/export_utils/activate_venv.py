# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import sys
from typing import Any
from typing import Dict

from console_output import CONSOLE_STDERR
from console_output import CONSOLE_STDOUT
from console_output import debug
from console_output import die
from console_output import eprint
from console_output import oprint
from console_output import status_message
from console_output import warn
from shell_types import SHELL_CLASSES
from shell_types import SUPPORTED_SHELLS
from utils import conf
from utils import run_cmd


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(prog='activate',
                                     description='Activate ESP-IDF environment',
                                     epilog='On Windows, run `python activate.py` to execute this script in the current terminal window.')
    parser.add_argument('-s', '--shell',
                        metavar='SHELL',
                        default=os.environ.get('ESP_IDF_SHELL', 'detect'),
                        help='Explicitly specify shell to start. For example bash, zsh, powershell.exe, cmd.exe')
    parser.add_argument('-l', '--list',
                        action='store_true',
                        help=('List supported shells.'))
    parser.add_argument('-e', '--export',
                        action='store_true',
                        help=('Generate commands to run in the terminal.'))
    parser.add_argument('-n', '--no-color',
                        action='store_true',
                        help=('Disable ANSI color escape sequences.'))
    parser.add_argument('-d', '--debug',
                        action='store_true',
                        default=bool(os.environ.get('ESP_IDF_EXPORT_DEBUG')),
                        help=('Enable debug information.'))
    parser.add_argument('-q', '--quiet',
                        action='store_true',
                        help=('Suppress all output.'))

    return parser.parse_args()


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
def get_idf_env() -> Dict[str,str]:
    # Get ESP-IDF system environment variables
    extra_paths_list = [os.path.join('components', 'espcoredump'),
                        os.path.join('components', 'partition_table'),
                        os.path.join('components', 'app_update')]
    extra_paths = os.pathsep.join([os.path.join(conf.IDF_PATH, path) for path in extra_paths_list])
    cmd = [sys.executable, conf.IDF_TOOLS_PY, 'export', '--format', 'key-value', '--add_paths_extras', extra_paths]
    stdout = run_cmd(cmd)

    # idf_tools.py might not export certain environment variables if they are already set
    idf_env: Dict[str, Any] = {
        'IDF_PATH': os.environ['IDF_PATH'],
        'ESP_IDF_VERSION': os.environ['ESP_IDF_VERSION'],
        'IDF_PYTHON_ENV_PATH': os.environ['IDF_PYTHON_ENV_PATH'],
    }

    try:
        for line in stdout.splitlines():
            var, val = line.split('=')
            idf_env[var] = val
    except ValueError as e:
        debug('\n'.join(['Output from `./tools/idf_tools.py export --format key-value`:',
                         f'{stdout}']))
        raise ValueError('\n'.join(['Please ensure your ESP-IDF installation is clean, especially file `./tools/idf_tools.py`.',
                                    'The command `./tools/idf_tools.py export` appears to be returning unexpected values.',
                                    f'Details: {e}']))

    if 'PATH' in idf_env:
        idf_env['PATH'] = os.pathsep.join([extra_paths, idf_env['PATH']])

    return idf_env


@status_message('Identifying shell', rv_on_ok=True)
def detect_shell(args: Any) -> str:
    import psutil

    if args.shell != 'detect':
        debug(f'Shell explicitly stated: "{args.shell}"')
        return str(args.shell)

    current_pid = os.getpid()
    detected_shell_name = ''
    while True:
        parent_pid = psutil.Process(current_pid).ppid()
        parent = psutil.Process(parent_pid)
        parent_cmdline = parent.cmdline()
        parent_exe = parent_cmdline[0].lstrip('-')
        parent_name = os.path.basename(parent_exe)
        debug(f'Parent: pid: {parent_pid}, cmdline: {parent_cmdline}, exe: {parent_exe}, name: {parent_name}')
        if not parent_name.lower().startswith('python'):
            detected_shell_name = parent_name
            break
        current_pid = parent_pid

    return detected_shell_name


@status_message('Detecting outdated tools in system', rv_on_ok=True)
def print_uninstall_msg() -> Any:
    stdout = run_cmd([sys.executable, conf.IDF_TOOLS_PY, 'uninstall', '--dry-run'])
    if stdout:
        python_cmd = 'python.exe' if sys.platform == 'win32' else 'python'
        msg = (f'Found tools that are not used by active ESP-IDF version.\n'
               f'[bright_cyan]{stdout}\n'
               f'To free up even more space, remove installation packages of those tools.\n'
               f'Use option {python_cmd} {conf.IDF_TOOLS_PY} uninstall --remove-archives.')
    else:
        msg = 'OK - no outdated tools found'

    return msg


def main() -> None:
    args = parse_arguments()

    # Setup parsed arguments
    CONSOLE_STDERR.no_color = args.no_color
    CONSOLE_STDOUT.no_color = args.no_color
    CONSOLE_STDERR.quiet = args.quiet
    CONSOLE_STDOUT.quiet = args.quiet
    # Fill config global holder
    conf.ARGS = args

    debug(f'command line: {sys.argv}')
    if conf.ARGS.list:
        oprint(SUPPORTED_SHELLS)
        sys.exit()

    eprint(f'[dark_orange]Activating ESP-IDF {conf.IDF_VERSION}')
    if conf.IDF_PATH_OLD and conf.IDF_PATH != conf.IDF_PATH_OLD:
        warn(f"IDF_PATH is changed from '{conf.IDF_PATH_OLD}' to '{conf.IDF_PATH}'.")
    else:
        eprint(f"Setting IDF_PATH to '{conf.IDF_PATH}'.")

    debug(f'IDF_PYTHON_ENV_PATH {conf.IDF_PYTHON_ENV_PATH}')

    check_python_version()
    check_python_dependencies()

    deactivate_cmd = get_deactivate_cmd()
    new_esp_idf_env = get_idf_env()
    detected_shell = detect_shell(conf.ARGS)
    print_uninstall_msg()

    if detected_shell not in SHELL_CLASSES:
        die(f'"{detected_shell}" shell is not among the supported options: "{SUPPORTED_SHELLS}"')

    shell = SHELL_CLASSES[detected_shell](detected_shell, deactivate_cmd, new_esp_idf_env)

    if conf.ARGS.export:
        shell.export()
        sys.exit()

    eprint(f'[dark_orange]Starting new \'{shell.shell}\' shell with ESP-IDF environment... (use "exit" command to quit)')
    shell.spawn()
    eprint(f'[dark_orange]ESP-IDF environment exited.')


if __name__ == '__main__':
    main()
