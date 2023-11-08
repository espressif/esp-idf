# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import shutil
import sys
from pathlib import Path
from subprocess import run
from subprocess import SubprocessError
from tempfile import gettempdir
from tempfile import NamedTemporaryFile
from tempfile import TemporaryDirectory
from textwrap import dedent
from typing import Any
from typing import Callable
from typing import Dict
from typing import List
from typing import Optional
from typing import TextIO

try:
    # The ESP-IDF virtual environment hasn't been verified yet, so see if the rich library
    # can be imported to display error and status messages nicely.
    from rich.console import Console
except ImportError as e:
    sys.exit(f'error: Unable to import the rich module: {e}. Please execute the install script.')


def status_message(msg: str, rv_on_ok: bool=False, die_on_err: bool=True) -> Callable:
    def inner(func: Callable) -> Callable:
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            eprint(f'[dark_orange]*[/dark_orange] {msg} ... ', end='')

            try:
                rv = func(*args, **kwargs)
            except Exception as e:
                eprint('[red]FAILED[/red]')
                if ARGS.debug:
                    raise
                if not die_on_err:
                    return None
                die(str(e))

            if rv_on_ok:
                eprint(f'[green]{rv}[/green]')
            else:
                eprint('[green]OK[/green]')

            return rv
        return wrapper
    return inner


class Shell():
    def __init__(self, shell: str, deactivate_cmd: str, new_esp_idf_env: Dict[str,str]):
        self.shell = shell
        self.deactivate_cmd = deactivate_cmd
        self.new_esp_idf_env = new_esp_idf_env

        # TODO We are not removing the temporary activation scripts.
        self.tmp_dir_path = Path(gettempdir()) / 'esp_idf_activate'
        self.tmp_dir_path.mkdir(parents=True, exist_ok=True)
        with NamedTemporaryFile(dir=self.tmp_dir_path, delete=False, prefix='activate_') as fd:
            self.script_file_path = Path(fd.name)
        debug(f'Temporary script file path: {self.script_file_path}')

    def expanded_env(self) -> Dict[str, str]:
        expanded_env = self.new_esp_idf_env.copy()

        if 'PATH' not in expanded_env:
            return expanded_env

        # The PATH returned by idf_tools.py export is not expanded.
        # Note that for the export script, the PATH should remain unexpanded
        # to ensure proper deactivation. In the export script,
        # the expansion should occur after deactivation, when the PATH is adjusted.
        # But it has to be expanded for processes started with the new PATH.
        expanded_env['PATH'] = os.path.expandvars(expanded_env['PATH'])
        return expanded_env

    def spawn(self) -> None:
        # This method should likely work for all shells because we are delegating the initialization
        # purely to Python os.environ.
        new_env = os.environ.copy()
        new_env.update(self.expanded_env())
        run([self.shell], env=new_env)


class UnixShell(Shell):
    def __init__(self, shell: str, deactivate_cmd: str, new_esp_idf_env: Dict[str,str]):
        super().__init__(shell, deactivate_cmd, new_esp_idf_env)
        self.new_esp_idf_env['IDF_TOOLS_INSTALL_CMD'] = os.path.join(IDF_PATH, 'install.sh')
        self.new_esp_idf_env['IDF_TOOLS_EXPORT_CMD'] = os.path.join(IDF_PATH, 'export.sh')

    def export_file(self, fd: TextIO) -> None:
        fd.write(f'{self.deactivate_cmd}\n')
        for var, value in self.new_esp_idf_env.items():
            fd.write(f'export {var}="{value}"\n')
        prompt = self.get_prompt()
        fd.write(f'{prompt}\n')

    def get_prompt(self) -> str:
        return f'PS1="(ESP-IDF {IDF_VERSION}) $PS1"'

    def export(self) -> None:
        with open(self.script_file_path, 'w') as fd:
            self.export_file(fd)
            fd.write((f'echo "\nDone! You can now compile ESP-IDF projects.\n'
                      'Go to the project directory and run:\n\n  idf.py build\n"'))

        print(f'. {self.script_file_path}')

    def click_ver(self) -> int:
        return int(click.__version__.split('.')[0])


class BashShell(UnixShell):
    def get_bash_major(self) -> int:
        env = self.expanded_env()
        stdout = run_cmd(['bash', '-c', 'echo ${BASH_VERSINFO[0]}'], env=env)
        bash_maj = int(stdout)
        return bash_maj

    @status_message('Shell completion', die_on_err=False)
    def autocompletion(self) -> str:
        bash_maj = self.get_bash_major()
        if bash_maj < 4:
            raise RuntimeError('Autocompletion not supported')

        env = self.expanded_env()
        env['LANG'] = 'en'
        env['_IDF.PY_COMPLETE'] = 'bash_source' if self.click_ver() >= 8 else 'source_bash'
        stdout = run_cmd([IDF_PY], env=env)
        return stdout

    def export_file(self, fd: TextIO) -> None:
        super().export_file(fd)
        stdout = self.autocompletion()
        if stdout is not None:
            fd.write(f'{stdout}\n')

    def init_file(self) -> None:
        with open(self.script_file_path, 'w') as fd:
            # We will use the --init-file option to pass a custom rc file, which will ignore .bashrc,
            # so we need to source .bashrc first.
            fd.write(f'source ~/.bashrc\n')

            stdout = self.autocompletion()
            if stdout is not None:
                fd.write(f'{stdout}\n')

            prompt = self.get_prompt()
            fd.write(f'{prompt}\n')

    def spawn(self) -> None:
        self.init_file()
        new_env = os.environ.copy()
        new_env.update(self.expanded_env())
        run([self.shell, '--init-file', str(self.script_file_path)], env=new_env)


class ZshShell(UnixShell):
    @status_message('Shell completion', die_on_err=False)
    def autocompletion(self) -> str:
        env = self.expanded_env()
        env['LANG'] = 'en'
        env['_IDF.PY_COMPLETE'] = 'zsh_source' if self.click_ver() >= 8 else 'source_zsh'
        stdout = run_cmd([IDF_PY], env=env)
        return f'autoload -Uz compinit && compinit -u\n{stdout}'

    def export_file(self, fd: TextIO) -> None:
        super().export_file(fd)
        stdout = self.autocompletion()
        # Add autocompletion
        if stdout is not None:
            fd.write(f'{stdout}\n')

    def init_file(self) -> None:
        # If ZDOTDIR is unset, HOME is used instead.
        # https://zsh.sourceforge.io/Doc/Release/Files.html#Startup_002fShutdown-Files
        zdotdir = os.environ.get('ZDOTDIR', str(Path.home()))
        with open(self.script_file_path, 'w') as fd:
            # We will use the ZDOTDIR env variable to load our custom script in the newly spawned shell
            # so we need to source .zshrc first.
            zshrc_path = Path(zdotdir) / '.zshrc'
            if zshrc_path.is_file():
                fd.write(f'source {zshrc_path}\n')

            # Add autocompletion
            stdout = self.autocompletion()
            if stdout is not None:
                fd.write(f'{stdout}\n')

            prompt = self.get_prompt()
            fd.write(f'{prompt}\n')

            # TODO This might not be needed, or consider resetting it to the original value
            fd.write('unset ZDOTDIR\n')

    def spawn(self) -> None:
        self.init_file()

        # Create a temporary directory to use as ZDOTDIR
        tmpdir = TemporaryDirectory()
        tmpdir_path = Path(tmpdir.name)
        debug(f'Temporary ZDOTDIR {tmpdir_path} with .zshrc file')

        # Copy init script to the custom ZDOTDIR
        zshrc_path = tmpdir_path / '.zshrc'
        shutil.copy(str(self.script_file_path), str(zshrc_path))

        new_env = os.environ.copy()
        new_env.update(self.expanded_env())
        # Set new ZDOTDIR in the new environment
        new_env['ZDOTDIR'] = str(tmpdir_path)

        run([self.shell], env=new_env)


class FishShell(UnixShell):
    def __init__(self, shell: str, deactivate_cmd: str, new_esp_idf_env: Dict[str,str]):
        super().__init__(shell, deactivate_cmd, new_esp_idf_env)
        self.new_esp_idf_env['IDF_TOOLS_INSTALL_CMD'] = os.path.join(IDF_PATH, 'install.fish')
        self.new_esp_idf_env['IDF_TOOLS_EXPORT_CMD'] = os.path.join(IDF_PATH, 'export.fish')

    @status_message('Shell completion', die_on_err=False)
    def autocompletion(self) -> str:
        env = self.expanded_env()
        env['LANG'] = 'en'
        env['_IDF.PY_COMPLETE'] = 'fish_source' if self.click_ver() >= 8 else 'source_fish'
        stdout = run_cmd([IDF_PY], env=env)
        return stdout

    def get_prompt(self) -> str:
        prompt = dedent(f'''
            functions -c fish_prompt _old_fish_prompt
            function fish_prompt
                printf "(ESP-IDF {IDF_VERSION}) "
                _old_fish_prompt
            end
            ''')
        return prompt

    def export_file(self, fd: TextIO) -> None:
        fd.write(f'{self.deactivate_cmd}\n')
        for var, value in self.new_esp_idf_env.items():
            fd.write(f'export {var}="{value}"\n')

        # Add autocompletion
        stdout = self.autocompletion()
        if stdout is not None:
            fd.write(f'{stdout}\n')

        # Adjust fish prompt
        prompt = self.get_prompt()
        fd.write(f'{prompt}\n')

    def init_file(self) -> None:
        with open(self.script_file_path, 'w') as fd:
            # Add autocompletion
            stdout = self.autocompletion()
            if stdout is not None:
                fd.write(f'{stdout}\n')
            # Adjust fish prompt
            prompt = self.get_prompt()
            fd.write(f'{prompt}\n')

    def spawn(self) -> None:
        self.init_file()
        new_env = os.environ.copy()
        new_env.update(self.expanded_env())
        run([self.shell, f'--init-command=source {self.script_file_path}'], env=new_env)


SHELL_CLASSES = {
    'bash': BashShell,
    'zsh': ZshShell,
    'fish': FishShell,
    'sh': UnixShell,
    'ksh': UnixShell,
    'dash': UnixShell,
    'nu': UnixShell,
}

SUPPORTED_SHELLS = ' '.join(SHELL_CLASSES.keys())

CONSOLE_STDERR = None
CONSOLE_STDOUT = None


def err(*args: Any, **kwargs: Any) -> None:
    CONSOLE_STDERR.print('[red]error[/red]: ', *args, **kwargs)  # type: ignore


def warn(*args: Any, **kwargs: Any) -> None:
    CONSOLE_STDERR.print('[yellow]warning[/yellow]: ', *args, **kwargs)  # type: ignore


def debug(*args: Any, **kwargs: Any) -> None:
    if not ARGS.debug:
        return
    CONSOLE_STDERR.print('[green_yellow]debug[/green_yellow]: ', *args, **kwargs)  # type: ignore


def die(*args: Any, **kwargs: Any) -> None:
    err(*args, **kwargs)
    sys.exit(1)


def eprint(*args: Any, **kwargs: Any) -> None:
    CONSOLE_STDERR.print(*args, **kwargs)  # type: ignore


def oprint(*args: Any, **kwargs: Any) -> None:
    CONSOLE_STDOUT.print(*args, **kwargs)  # type: ignore


def run_cmd(cmd: List[str], env: Optional[Dict[str, Any]]=None) -> str:
    new_env = os.environ.copy()
    if env is not None:
        new_env.update(env)

    cmd_str = '"{}"'.format(' '.join(cmd))
    try:
        p = run(cmd, env=new_env, text=True, capture_output=True)
    except (OSError, SubprocessError) as e:
        raise RuntimeError(f'Command {cmd_str} failed: {e}')

    stdout = p.stdout.strip()
    stderr = p.stderr.strip()
    if p.returncode:
        raise RuntimeError(f'Command {cmd_str} failed with error code {p.returncode}\n{stdout}\n{stderr}')

    return stdout


parser = argparse.ArgumentParser(prog='activate',
                                 description='Activate ESP-IDF environment')
parser.add_argument('-s', '--shell',
                    metavar='SHELL',
                    default=os.environ.get('ESP_IDF_SHELL', None),
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
                    help=('Enable debug information.'))
parser.add_argument('-q', '--quiet',
                    action='store_true',
                    help=('Suppress all output.'))

ARGS = parser.parse_args()

CONSOLE_STDERR = Console(stderr=True, quiet=ARGS.quiet, no_color=ARGS.no_color)
CONSOLE_STDOUT = Console(quiet=ARGS.quiet, no_color=ARGS.no_color)

if ARGS.list:
    oprint(SUPPORTED_SHELLS)
    sys.exit()

# The activate.py script sets the following environment variables
IDF_PATH = os.environ['IDF_PATH']
IDF_VERSION = os.environ['ESP_IDF_VERSION']
IDF_PYTHON_ENV_PATH = os.environ['IDF_PYTHON_ENV_PATH']
IDF_TOOLS_PY = os.path.join(IDF_PATH, 'tools', 'idf_tools.py')
IDF_PY = os.path.join(IDF_PATH, 'tools', 'idf.py')

eprint(f'[dark_orange]Activating ESP-IDF {IDF_VERSION}')
debug(f'IDF_PATH {IDF_PATH}')
debug(f'IDF_PYTHON_ENV_PATH {IDF_PYTHON_ENV_PATH}')


@status_message('Checking python version', rv_on_ok=True)
def check_python_version() -> str:
    # Check the Python version within a virtual environment
    python_version_checker = os.path.join(IDF_PATH, 'tools', 'python_version_checker.py')
    run_cmd([sys.executable, python_version_checker])
    ver = sys.version_info[:3]
    return f'{ver[0]}.{ver[1]}.{ver[2]}'


@status_message('Checking python dependencies')
def check_python_dependencies() -> None:
    # Check Python dependencies within the virtual environment
    run_cmd([sys.executable, IDF_TOOLS_PY, 'check-python-dependencies'])


check_python_version()
check_python_dependencies()
# TODO Report installed tools that are not currently used by active ESP-IDF version

# From this point forward, we are functioning within a fully validated ESP-IDF environment.

# TODO Verify the architectures supported by psutils. We might need to create a wheel for it or
# substitute it with ps and tasklist commands.
import psutil  # noqa: E402
import click   # noqa: E402


@status_message('Deactivating the current ESP-IDF environment')
def get_deactivate_cmd() -> str:
    # Get previous ESP-IDF system environment variables
    cmd = [sys.executable, IDF_TOOLS_PY, 'export', '--deactivate']
    stdout = run_cmd(cmd)
    return stdout


@status_message('Establishing a new ESP-IDF environment')
def get_idf_env() -> Dict[str,str]:
    # Get ESP-IDF system environment variables
    extra_paths_list = [os.path.join('components', 'espcoredump'),
                        os.path.join('components', 'partition_table'),
                        os.path.join('components', 'app_update')]
    extra_paths = ':'.join([os.path.join(IDF_PATH, path) for path in extra_paths_list])
    cmd = [sys.executable, IDF_TOOLS_PY, 'export', '--format', 'key-value', '--add_paths_extras', extra_paths]
    stdout = run_cmd(cmd)

    # idf_tools.py might not export certain environment variables if they are already set
    idf_env: Dict[str, Any] = {
        'IDF_PATH': os.environ['IDF_PATH'],
        'ESP_IDF_VERSION': os.environ['ESP_IDF_VERSION'],
        'IDF_PYTHON_ENV_PATH': os.environ['IDF_PYTHON_ENV_PATH'],
    }

    for line in stdout.splitlines():
        var, val = line.split('=')
        idf_env[var] = val

    if 'PATH' in idf_env:
        idf_env['PATH'] = ':'.join([extra_paths, idf_env['PATH']])

    return idf_env


@status_message('Identifying shell', rv_on_ok=True)
def detect_shell() -> str:
    if ARGS.shell is not None:
        return str(ARGS.shell)
    ppid = psutil.Process(os.getpid()).ppid()
    # Look for grandparent, because we started from activate.py.
    pppid = psutil.Process(ppid).ppid()
    return str(psutil.Process(pppid).name())


deactivate_cmd = get_deactivate_cmd()
new_esp_idf_env = get_idf_env()
detected_shell = detect_shell()

if detected_shell not in SHELL_CLASSES:
    die(f'"{detected_shell}" shell is not among the supported options: "{SUPPORTED_SHELLS}"')

shell = SHELL_CLASSES[detected_shell](detected_shell, deactivate_cmd, new_esp_idf_env)

if ARGS.export:
    shell.export()
    sys.exit()

shell.spawn()
eprint(f'[dark_orange]ESP-IDF environment exited.')
