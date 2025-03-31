# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import getpass
import os
import re
import shutil
import sys
import textwrap
from datetime import datetime
from datetime import timedelta
from pathlib import Path
from subprocess import run
from tempfile import gettempdir
from tempfile import NamedTemporaryFile
from tempfile import TemporaryDirectory
from typing import Dict
from typing import List
from typing import TextIO
from typing import Union

import click
from console_output import debug
from console_output import status_message
from console_output import warn
from utils import conf
from utils import run_cmd


class Shell():
    def __init__(self, shell: str, deactivate_cmd: str, new_esp_idf_env: Dict[str,str]):
        self.shell = shell
        self.deactivate_cmd = deactivate_cmd
        self.new_esp_idf_env = new_esp_idf_env

        try:
            self.tmp_dir_path = Path(gettempdir()) / ('esp_idf_activate_' + getpass.getuser())
        except Exception as e:
            self.tmp_dir_path = Path(gettempdir()) / 'esp_idf_activate'
            warn(f'Failed to get username with error: {e}. Using default temporary directory {self.tmp_dir_path}.')

        if not conf.ARGS.debug and os.path.exists(self.tmp_dir_path):
            # Do not cleanup temporary directory when debugging
            for item in self.tmp_dir_path.iterdir():
                try:
                    if item.is_file():
                        current_time = datetime.now()
                        file_creation_time = datetime.fromtimestamp(os.path.getctime(item))
                        # delete files older than 1 hour to avoid datarace when using activation in parallel
                        if current_time - file_creation_time > timedelta(hours=1):
                            item.unlink()
                except Exception as e:
                    warn(f'Failed to clean temp activation directory with file {item}: {e}')

        self.tmp_dir_path.mkdir(parents=True, exist_ok=True)

    def export(self) -> None:
        raise NotImplementedError('Subclass must implement abstract method "export"')

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

        with NamedTemporaryFile(dir=self.tmp_dir_path, delete=False, prefix='activate_') as fd:
            self.script_file_path = Path(fd.name)
        debug(f'Temporary script file path: {self.script_file_path}')

        self.new_esp_idf_env['IDF_TOOLS_INSTALL_CMD'] = os.path.join(conf.IDF_PATH, 'install.sh')
        self.new_esp_idf_env['IDF_TOOLS_EXPORT_CMD'] = os.path.join(conf.IDF_PATH, 'export.sh')

    def autocompletion(self) -> None:
        # Basic POSIX shells does not support autocompletion
        return None

    def export_file(self, fd: TextIO) -> None:
        fd.write(f'{self.deactivate_cmd}\n')
        for var, value in self.new_esp_idf_env.items():
            fd.write(f'export {var}="{value}"\n')
        stdout = self.autocompletion()  # type: ignore
        if stdout is not None:
            fd.write(f'{stdout}\n')
        fd.write((f'echo "\nDone! You can now compile ESP-IDF projects.\n'
                  'Go to the project directory and run:\n\n  idf.py build"\n'))

    def export(self) -> None:
        with open(self.script_file_path, 'w', encoding='utf-8') as fd:
            self.export_file(fd)
        print(f'. {self.script_file_path}')

    def click_ver(self) -> int:
        return int(click.__version__.split('.')[0])


class BashShell(UnixShell):
    @status_message('Shell completion', msg_result='Autocompletion code generated')
    def autocompletion(self) -> str:
        bash_source = 'bash_source' if self.click_ver() >= 8 else 'source_bash'
        autocom = textwrap.dedent(f"""
            WARNING_MSG="WARNING: Failed to load shell autocompletion for bash version: $BASH_VERSION!"
            if test ${{BASH_VERSINFO[0]}} -lt 4
            then
                echo "$WARNING_MSG"
            else
                if ! eval "$(env LANG=en _IDF.PY_COMPLETE={bash_source} idf.py)"
                then
                    echo "$WARNING_MSG"
                fi
            fi
            """)

        return autocom

    def init_file(self) -> None:
        with open(self.script_file_path, 'w', encoding='utf-8') as fd:
            # We will use the --init-file option to pass a custom rc file, which will ignore .bashrc,
            # so we need to source .bashrc first.
            bashrc_path = os.path.expanduser('~/.bashrc')
            if os.path.isfile(bashrc_path):
                fd.write(f'source {bashrc_path}\n')
            self.export_file(fd)

    def spawn(self) -> None:
        self.init_file()
        new_env = os.environ.copy()
        run([self.shell, '--init-file', str(self.script_file_path)], env=new_env)


class ZshShell(UnixShell):
    @status_message('Shell completion', msg_result='Autocompletion code generated')
    def autocompletion(self) -> str:
        zsh_source = 'zsh_source' if self.click_ver() >= 8 else 'source_zsh'
        autocom = textwrap.dedent(f"""
            WARNING_MSG="WARNING: Failed to load shell autocompletion for zsh version: $ZSH_VERSION!"
            autoload -Uz compinit && compinit -u
            if ! eval "$(env _IDF.PY_COMPLETE={zsh_source} idf.py)"
            then
                echo "$WARNING_MSG"
            fi
            """)

        return autocom

    def init_file(self) -> None:
        # If ZDOTDIR is unset, HOME is used instead.
        # https://zsh.sourceforge.io/Doc/Release/Files.html#Startup_002fShutdown-Files
        zdotdir = os.environ.get('ZDOTDIR', str(Path.home()))
        with open(self.script_file_path, 'w', encoding='utf-8') as fd:
            # We will use the ZDOTDIR env variable to load our custom script in the newly spawned shell
            # so we need to source .zshrc first.
            zshrc_path = Path(zdotdir) / '.zshrc'
            if zshrc_path.is_file():
                fd.write(f'source {zshrc_path}\n')

            self.export_file(fd)

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
        # Set new ZDOTDIR in the new environment
        new_env['ZDOTDIR'] = str(tmpdir_path)

        run([self.shell], env=new_env)


class FishShell(UnixShell):
    def __init__(self, shell: str, deactivate_cmd: str, new_esp_idf_env: Dict[str,str]):
        super().__init__(shell, deactivate_cmd, new_esp_idf_env)
        self.new_esp_idf_env['IDF_TOOLS_INSTALL_CMD'] = os.path.join(conf.IDF_PATH, 'install.fish')
        self.new_esp_idf_env['IDF_TOOLS_EXPORT_CMD'] = os.path.join(conf.IDF_PATH, 'export.fish')

    @status_message('Shell completion', die_on_err=False)
    def autocompletion(self) -> str:
        env = self.expanded_env()
        env['LANG'] = 'en'
        env['_IDF.PY_COMPLETE'] = 'fish_source' if self.click_ver() >= 8 else 'source_fish'
        stdout: str = run_cmd([sys.executable, conf.IDF_PY], env=env)
        return stdout

    def init_file(self) -> None:
        with open(self.script_file_path, 'w', encoding='utf-8') as fd:
            self.export_file(fd)

    def spawn(self) -> None:
        self.init_file()
        new_env = os.environ.copy()
        run([self.shell, f'--init-command=source {self.script_file_path}'], env=new_env)


class PowerShell(Shell):
    def __init__(self, shell: str, deactivate_cmd: str, new_esp_idf_env: Dict[str,str]):
        super().__init__(shell, deactivate_cmd, new_esp_idf_env)

        with NamedTemporaryFile(dir=self.tmp_dir_path, delete=False, prefix='activate_', suffix='.ps1') as fd:
            self.script_file_path = Path(fd.name)
        debug(f'Temporary script file path: {self.script_file_path}')

        self.new_esp_idf_env['IDF_TOOLS_INSTALL_CMD'] = os.path.join(conf.IDF_PATH, 'install.ps1')
        self.new_esp_idf_env['IDF_TOOLS_EXPORT_CMD'] = os.path.join(conf.IDF_PATH, 'export.ps1')

    def get_functions(self) -> str:
        return '\n'.join([
            r'function idf.py { &python "$Env:IDF_PATH\tools\idf.py" $args }',
            r'function global:esptool.py { &python -m esptool $args }',
            r'function global:espefuse.py { &python -m espefuse $args }',
            r'function global:espsecure.py { &python -m espsecure $args }',
            r'function global:otatool.py { &python "$Env:IDF_PATH\components\app_update\otatool.py" $args }',
            r'function global:parttool.py { &python "$Env:IDF_PATH\components\partition_table\parttool.py" $args }',
        ])

    def export(self) -> None:
        self.init_file()
        # Powershell is the only Shell class that does not return the script name in dot sourcing style
        # since PSAnalyzer complains about using `InvokeExpression` command
        print(f'{self.script_file_path}')

    def init_file(self) -> None:
        with open(self.script_file_path, 'w', encoding='utf-8') as fd:
            # fd.write(f'{self.deactivate_cmd}\n')  TODO in upcoming task IDF-10292
            for var, value in self.new_esp_idf_env.items():
                if var == 'PATH':
                    value = re.sub(r'(%PATH%|\$PATH)', r'$Env:PATH', value)
                fd.write(f'$Env:{var}="{value}"\n')
            functions = self.get_functions()
            fd.write(f'{functions}\n')
            fd.write((f'echo "\nDone! You can now compile ESP-IDF projects.\n'
                      'Go to the project directory and run:\n\n  idf.py build\n"'))

    def spawn(self) -> None:
        self.init_file()
        new_env = os.environ.copy()
        arguments = ['-NoExit', '-Command', f'{self.script_file_path}']
        cmd: Union[str, List[str]] = [self.shell] + arguments
        run(cmd, env=new_env)


class WinCmd(Shell):
    def __init__(self, shell: str, deactivate_cmd: str, new_esp_idf_env: Dict[str,str]):
        super().__init__(shell, deactivate_cmd, new_esp_idf_env)

        with NamedTemporaryFile(dir=self.tmp_dir_path, delete=False, prefix='activate_', suffix='.bat') as fd:
            self.script_file_path = Path(fd.name)
        debug(f'Temporary script file path: {self.script_file_path}')

        self.new_esp_idf_env['IDF_TOOLS_INSTALL_CMD'] = os.path.join(conf.IDF_PATH, 'install.bat')
        self.new_esp_idf_env['IDF_TOOLS_EXPORT_CMD'] = os.path.join(conf.IDF_PATH, 'export.bat')
        self.new_esp_idf_env['IDF_TOOLS_JSON_PATH'] = os.path.join(conf.IDF_PATH, 'tools', 'tools.json')
        self.new_esp_idf_env['IDF_TOOLS_PY_PATH'] = conf.IDF_TOOLS_PY

    def get_functions(self) -> str:
        return '\n'.join([
            r'DOSKEY idf.py=python.exe "%IDF_PATH%\tools\idf.py" $*',
            r'DOSKEY esptool.py=python.exe -m esptool $*',
            r'DOSKEY espefuse.py=python.exe -m espefuse $*',
            r'DOSKEY espsecure.py=python.exe -m espsecure $*',
            r'DOSKEY otatool.py=python.exe "%IDF_PATH%\components\app_update\otatool.py" $*',
            r'DOSKEY parttool.py=python.exe "%IDF_PATH%\components\partition_table\parttool.py" $*',
        ])

    def export(self) -> None:
        self.init_file()
        print(f'call {self.script_file_path}')

    def init_file(self) -> None:
        with open(self.script_file_path, 'w', encoding='utf-8') as fd:
            fd.write('@echo off\n')
            # fd.write(f'{self.deactivate_cmd}\n')  TODO in upcoming task IDF-10292
            for var, value in self.new_esp_idf_env.items():
                fd.write(f'set {var}={value}\n')
            functions = self.get_functions()
            fd.write(f'{functions}\n')
            fd.write('\n'.join([
                'echo.',
                'echo Done! You can now compile ESP-IDF projects.',
                'echo Go to the project directory and run:',
                'echo.',
                'echo   idf.py build',
                'echo.',
            ]))

    def spawn(self) -> None:
        self.init_file()
        new_env = os.environ.copy()
        arguments = ['/k', f'{self.script_file_path}']
        cmd: Union[str, List[str]] = [self.shell] + arguments
        cmd = ' '.join(cmd)
        run(cmd, env=new_env)


SHELL_CLASSES = {
    'bash': BashShell,
    'zsh': ZshShell,
    'fish': FishShell,
    'sh': UnixShell,
    'ksh': UnixShell,
    'dash': UnixShell,
    'nu': UnixShell,
    'pwsh.exe': PowerShell,
    'pwsh': PowerShell,
    'powershell.exe': PowerShell,
    'powershell': PowerShell,
    'cmd.exe': WinCmd,
    'cmd': WinCmd
}

SUPPORTED_SHELLS = ' '.join(SHELL_CLASSES.keys())
