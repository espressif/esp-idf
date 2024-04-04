# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import asyncio
import importlib
import json
import os
import re
import subprocess
import sys
from asyncio.subprocess import Process
from pkgutil import iter_modules
from types import FunctionType
from typing import Any
from typing import Dict
from typing import Generator
from typing import List
from typing import Match
from typing import Optional
from typing import TextIO
from typing import Tuple
from typing import Union

import click
import yaml
from esp_idf_monitor import get_ansi_converter
from idf_py_actions.errors import NoSerialPortFoundError

from .constants import GENERATORS
from .errors import FatalError

# Name of the program, normally 'idf.py'.
# Can be overridden from idf.bat using IDF_PY_PROGRAM_NAME
PROG = os.getenv('IDF_PY_PROGRAM_NAME', 'idf.py')

# environment variable used during click shell completion run
SHELL_COMPLETE_VAR = '_IDF.PY_COMPLETE'

# was shell completion invoked?
SHELL_COMPLETE_RUN = SHELL_COMPLETE_VAR in os.environ


# The ctx dict "abuses" how python evaluates default parameter values.
# https://docs.python.org/3/reference/compound_stmts.html#function-definitions
# Default parameter values are evaluated from left to right
# when the function definition is executed
def get_build_context(ctx: Dict={}) -> Dict:
    """
    The build context is set in the ensure_build_directory function. It can be used
    in modules or other code, which don't have direct access to such information.
    It returns dictionary with the following keys:

    'proj_desc' - loaded project_description.json file

    Please make sure that ensure_build_directory was called otherwise the build
    context dictionary will be empty. Also note that it might not be thread-safe to
    modify the returned dictionary. It should be considered read-only.
    """
    return ctx


def _set_build_context(args: 'PropertyDict') -> None:
    # private helper to set global build context from ensure_build_directory
    ctx = get_build_context()

    proj_desc_fn = f'{args.build_dir}/project_description.json'
    try:
        with open(proj_desc_fn, 'r') as f:
            ctx['proj_desc'] = json.load(f)
    except (OSError, ValueError) as e:
        raise FatalError(f'Cannot load {proj_desc_fn}: {e}')


def executable_exists(args: List) -> bool:
    try:
        subprocess.check_output(args)
        return True

    except Exception:
        return False


def _idf_version_from_cmake() -> Optional[str]:
    version_path = os.path.join(os.environ['IDF_PATH'], 'tools/cmake/version.cmake')
    regex = re.compile(r'^\s*set\s*\(\s*IDF_VERSION_([A-Z]{5})\s+(\d+)')
    ver = {}
    try:
        with open(version_path) as f:
            for line in f:
                m = regex.match(line)

                if m:
                    ver[m.group(1)] = m.group(2)

        return 'v%s.%s.%s' % (ver['MAJOR'], ver['MINOR'], ver['PATCH'])
    except (KeyError, OSError):
        sys.stderr.write('WARNING: Cannot find ESP-IDF version in version.cmake\n')
        return None


def get_target(path: str, sdkconfig_filename: str='sdkconfig') -> Optional[str]:
    path = os.path.join(path, sdkconfig_filename)
    return get_sdkconfig_value(path, 'CONFIG_IDF_TARGET')


def idf_version() -> Optional[str]:
    """Print version of ESP-IDF"""

    #  Try to get version from git:
    try:
        version: Optional[str] = subprocess.check_output([
            'git',
            '--git-dir=%s' % os.path.join(os.environ['IDF_PATH'], '.git'),
            '--work-tree=%s' % os.environ['IDF_PATH'],
            'describe', '--tags', '--dirty', '--match', 'v*.*',
        ]).decode('utf-8', 'ignore').strip()
    except (subprocess.CalledProcessError, UnicodeError):
        # if failed, then try to parse cmake.version file
        sys.stderr.write('WARNING: Git version unavailable, reading from source\n')
        version = _idf_version_from_cmake()

    return version


def get_default_serial_port() -> Any:
    # Import is done here in order to move it after the check_environment()
    # ensured that pyserial has been installed
    try:
        import esptool
        import serial.tools.list_ports
        ports = list(sorted(p.device for p in serial.tools.list_ports.comports()))
        # high baud rate could cause the failure of creation of the connection
        esp = esptool.get_default_connected_device(serial_list=ports, port=None, connect_attempts=4,
                                                   initial_baud=115200)
        if esp is None:
            raise NoSerialPortFoundError(
                "No serial ports found. Connect a device, or use '-p PORT' option to set a specific port.")

        serial_port = esp.serial_port
        esp._port.close()

        return serial_port

    except NoSerialPortFoundError:
        raise
    except Exception as e:
        raise FatalError('An exception occurred during detection of the serial port: {}'.format(e))


# function prints warning when autocompletion is not being performed
# set argument stream to sys.stderr for errors and exceptions
def print_warning(message: str, stream: TextIO=None) -> None:
    if not SHELL_COMPLETE_RUN:
        print(message, file=stream or sys.stderr)


def color_print(message: str, color: str, newline: Optional[str]='\n') -> None:
    """ Print a message to stderr with colored highlighting """
    ansi_normal = '\033[0m'
    sys.stderr.write('%s%s%s%s' % (color, message, ansi_normal, newline))
    sys.stderr.flush()


def yellow_print(message: str, newline: Optional[str]='\n') -> None:
    ansi_yellow = '\033[0;33m'
    color_print(message, ansi_yellow, newline)


def red_print(message: str, newline: Optional[str]='\n') -> None:
    ansi_red = '\033[1;31m'
    color_print(message, ansi_red, newline)


def debug_print_idf_version() -> None:
    print_warning(f'ESP-IDF {idf_version() or "version unknown"}')


def load_hints() -> Dict:
    """Helper function to load hints yml file"""
    hints: Dict = {
        'yml': [],
        'modules': []
    }

    current_module_dir = os.path.dirname(__file__)
    with open(os.path.join(current_module_dir, 'hints.yml'), 'r') as file:
        hints['yml'] = yaml.safe_load(file)

    hint_modules_dir = os.path.join(current_module_dir, 'hint_modules')
    if not os.path.exists(hint_modules_dir):
        return hints

    sys.path.append(hint_modules_dir)
    for _, name, _ in iter_modules([hint_modules_dir]):
        # Import modules for hint processing and add list of their 'generate_hint' functions into hint dict.
        # If the module doesn't have the function 'generate_hint', it will raise an exception
        try:
            hints['modules'].append(getattr(importlib.import_module(name), 'generate_hint'))
        except ModuleNotFoundError:
            red_print(f'Failed to import "{name}" from "{hint_modules_dir}" as a module')
            raise SystemExit(1)
        except AttributeError:
            red_print('Module "{}" does not have function generate_hint.'.format(name))
            raise SystemExit(1)

    return hints


def generate_hints_buffer(output: str, hints: Dict) -> Generator:
    """Helper function to process hints within a string buffer"""
    # Call modules for possible hints with unchanged output. Note that
    # hints in hints.yml expect new line trimmed, but modules should
    # get the output unchanged. Please see tools/idf_py_actions/hints.yml
    for generate_hint in hints['modules']:
        module_hint = generate_hint(output)
        if module_hint:
            yield module_hint

    # hints expect new lines trimmed
    output = ' '.join(line.strip() for line in output.splitlines() if line.strip())
    for hint in hints['yml']:
        variables_list = hint.get('variables')
        hint_list, hint_vars, re_vars = [], [], []
        match: Optional[Match[str]] = None
        try:
            if variables_list:
                for variables in variables_list:
                    hint_vars = variables['hint_variables']
                    re_vars = variables['re_variables']
                    regex = hint['re'].format(*re_vars)
                    if re.compile(regex).search(output):
                        try:
                            hint_list.append(hint['hint'].format(*hint_vars))
                        except KeyError as e:
                            red_print('Argument {} missing in {}. Check hints.yml file.'.format(e, hint))
                            sys.exit(1)
            else:
                match = re.compile(hint['re']).search(output)
        except KeyError as e:
            red_print('Argument {} missing in {}. Check hints.yml file.'.format(e, hint))
            sys.exit(1)
        except re.error as e:
            red_print('{} from hints.yml have {} problem. Check hints.yml file.'.format(hint['re'], e))
            sys.exit(1)
        if hint_list:
            for message in hint_list:
                yield ' '.join(['HINT:', message])
        elif match:
            extra_info = ', '.join(match.groups()) if hint.get('match_to_output', '') else ''
            try:
                yield ' '.join(['HINT:', hint['hint'].format(extra_info)])
            except KeyError:
                raise KeyError("Argument 'hint' missing in {}. Check hints.yml file.".format(hint))


def generate_hints(*filenames: str) -> Generator:
    """Getting output files and printing hints on how to resolve errors based on the output."""
    hints = load_hints()
    for file_name in filenames:
        with open(file_name, 'r') as file:
            yield from generate_hints_buffer(file.read(), hints)


def fit_text_in_terminal(out: str) -> str:
    """Fit text in terminal, if the string is not fit replace center with `...`"""
    space_for_dots = 3  # Space for "..."
    terminal_width, _ = os.get_terminal_size()
    if not terminal_width:
        return out
    if terminal_width <= space_for_dots:
        # if the wide of the terminal is too small just print dots
        return '.' * terminal_width
    if len(out) >= terminal_width:
        elide_size = (terminal_width - space_for_dots) // 2
        # cut out the middle part of the output if it does not fit in the terminal
        return '...'.join([out[:elide_size], out[len(out) - elide_size:]])
    return out


class RunTool:
    def __init__(self, tool_name: str, args: List, cwd: str, env: Dict=None, custom_error_handler: FunctionType=None, build_dir: str=None,
                 hints: bool=True, force_progression: bool=False, interactive: bool=False, convert_output: bool=False) -> None:
        self.tool_name = tool_name
        self.args = args
        self.cwd = cwd
        self.env = env
        self.custom_error_handler = custom_error_handler
        # build_dir sets by tools that do not use build directory as cwd
        self.build_dir = build_dir or cwd
        self.hints = hints
        self.force_progression = force_progression
        self.interactive = interactive
        self.convert_output = convert_output

    def __call__(self) -> None:
        def quote_arg(arg: str) -> str:
            """ Quote the `arg` with whitespace in them because it can cause problems when we call it from a subprocess."""
            if re.match(r"^(?![\'\"]).*\s.*", arg):
                return ''.join(["'", arg, "'"])
            return arg

        self.args = [str(arg) for arg in self.args]
        display_args = ' '.join(quote_arg(arg) for arg in self.args)
        print('Running %s in directory %s' % (self.tool_name, quote_arg(self.cwd)))
        print('Executing "%s"...' % str(display_args))

        env_copy = dict(os.environ)
        env_copy.update(self.env or {})

        process: Union[Process, subprocess.CompletedProcess[bytes]]
        if self.hints:
            process, stderr_output_file, stdout_output_file = asyncio.run(self.run_command(self.args, env_copy))
        else:
            process = subprocess.run(self.args, env=env_copy, cwd=self.cwd)
            stderr_output_file, stdout_output_file = None, None
        if process.returncode == 0:
            return

        if self.custom_error_handler:
            self.custom_error_handler(process.returncode, stderr_output_file, stdout_output_file)
            return

        if stderr_output_file and stdout_output_file:
            # hints in interactive mode were already processed, don't print them again
            if not self.interactive:
                for hint in generate_hints(stderr_output_file, stdout_output_file):
                    yellow_print(hint)
            raise FatalError('{} failed with exit code {}, output of the command is in the {} and {}'.format(self.tool_name, process.returncode,
                             stderr_output_file, stdout_output_file))

        raise FatalError('{} failed with exit code {}'.format(self.tool_name, process.returncode))

    async def run_command(self, cmd: List, env_copy: Dict) -> Tuple[Process, Optional[str], Optional[str]]:
        """ Run the `cmd` command with capturing stderr and stdout from that function and return returncode
        and of the command, the id of the process, paths to captured output """
        log_dir_name = 'log'
        try:
            os.mkdir(os.path.join(self.build_dir, log_dir_name))
        except FileExistsError:
            pass
        # Note: we explicitly pass in os.environ here, as we may have set IDF_PATH there during startup
        # limit was added for avoiding error in idf.py confserver
        try:
            p = await asyncio.create_subprocess_exec(*cmd, env=env_copy, limit=1024 * 256, cwd=self.cwd, stdout=asyncio.subprocess.PIPE,
                                                     stderr=asyncio.subprocess.PIPE)
        except NotImplementedError:
            message = f'ERROR: {sys.executable} doesn\'t support asyncio. The issue can be worked around by re-running idf.py with the "--no-hints" argument.'
            if sys.platform == 'win32':
                message += ' To fix the issue use the Windows Installer for setting up your python environment, ' \
                    'available from: https://dl.espressif.com/dl/esp-idf/'
            sys.exit(message)

        stderr_output_file = os.path.join(self.build_dir, log_dir_name, f'idf_py_stderr_output_{p.pid}')
        stdout_output_file = os.path.join(self.build_dir, log_dir_name, f'idf_py_stdout_output_{p.pid}')
        if p.stderr and p.stdout:  # it only to avoid None type in p.std
            try:
                await asyncio.gather(
                    self.read_and_write_stream(p.stderr, stderr_output_file, sys.stderr),
                    self.read_and_write_stream(p.stdout, stdout_output_file, sys.stdout))
            except asyncio.CancelledError:
                # The process we are trying to read from was terminated. Print the
                # message here and let the asyncio to finish, because
                # Runner context in asyncio.run is closing the event loop and
                # if exception is raised(unhandled here) the transport is not closed before
                # the even loop is closed and we get RuntimeError: Event loop is closed
                # in the transport __del__ function because it's trying to use the closed
                # even loop.
                red_print(f'\n{self.tool_name} process terminated\n')
        await p.wait()  # added for avoiding None returncode
        return p, stderr_output_file, stdout_output_file

    async def read_and_write_stream(self, input_stream: asyncio.StreamReader, output_filename: str,
                                    output_stream: TextIO) -> None:
        """read the output of the `input_stream` and then write it into `output_filename` and `output_stream`"""
        def delete_ansi_escape(text: str) -> str:
            ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')
            return ansi_escape.sub('', text)

        def print_progression(output: str) -> None:
            # Print a new line on top of the previous line
            print('\r' + fit_text_in_terminal(output.strip('\n\r')) + '\x1b[K', end='', file=output_stream)
            output_stream.flush()

        def is_progression(output: str) -> bool:
            # try to find possible progression by a pattern match
            if re.match(r'^\[\d+/\d+\]|.*\(\d+ \%\)$', output):
                return True
            return False

        async def read_stream() -> Optional[str]:
            try:
                output_b = await input_stream.readline()
                return output_b.decode(errors='ignore')
            except (asyncio.LimitOverrunError, asyncio.IncompleteReadError) as e:
                print(e, file=sys.stderr)
                return None
            except AttributeError:
                return None

        async def read_interactive_stream() -> Optional[str]:
            buffer = b''
            while True:
                output_b = await input_stream.read(1)
                if not output_b:
                    return None
                try:
                    return (buffer + output_b).decode()
                except UnicodeDecodeError:
                    buffer += output_b
                    if len(buffer) > 4:
                        # Multi-byte character contain up to 4 bytes and if buffer have more then 4 bytes
                        # and still can not decode it we can just ignore some bytes
                        return buffer.decode(errors='replace')

        # use ANSI color converter for Monitor on Windows
        output_converter = get_ansi_converter(output_stream) if self.convert_output else output_stream

        # used in interactive mode to print hints after matched line
        hints = load_hints()
        last_line = ''
        is_progression_last_line = False
        is_progression_processing_enabled = self.force_progression and output_stream.isatty() and '-v' not in self.args

        try:
            # The command output from asyncio stream already contains OS specific line ending,
            # because it's read in as bytes and decoded to string. On Windows "output" already
            # contains CRLF. Use "newline=''" to prevent python to convert CRLF into CRCRLF.
            # Please see "newline" description at https://docs.python.org/3/library/functions.html#open
            with open(output_filename, 'w', encoding='utf8', newline='') as output_file:
                while True:
                    if self.interactive:
                        output = await read_interactive_stream()
                    else:
                        output = await read_stream()
                    if not output:
                        break

                    output_noescape = delete_ansi_escape(output)
                    # Always remove escape sequences when writing the build log.
                    output_file.write(output_noescape)
                    # If idf.py output is redirected and the output stream is not a TTY,
                    # strip the escape sequences as well.
                    # (There shouldn't be any, but just in case.)
                    if not output_stream.isatty():
                        output = output_noescape

                    if is_progression_processing_enabled and is_progression(output):
                        print_progression(output)
                        is_progression_last_line = True
                    else:
                        if is_progression_last_line:
                            output_converter.write(os.linesep)
                            is_progression_last_line = False
                        output_converter.write(output)
                        output_converter.flush()

                        # process hints for last line and print them right away
                        if self.interactive:
                            last_line += output
                            if last_line[-1] == '\n':
                                for hint in generate_hints_buffer(last_line, hints):
                                    yellow_print(hint)
                                last_line = ''
        except (RuntimeError, EnvironmentError) as e:
            yellow_print('WARNING: The exception {} was raised and we can\'t capture all your {} and '
                         'hints on how to resolve errors can be not accurate.'.format(e, output_stream.name.strip('<>')))


def run_tool(*args: Any, **kwargs: Any) -> None:
    # Added in case someone uses run_tool externally in idf.py extensions
    return RunTool(*args, **kwargs)()


def run_target(target_name: str, args: 'PropertyDict', env: Optional[Dict]=None,
               custom_error_handler: FunctionType=None, force_progression: bool=False, interactive: bool=False) -> None:
    """Run target in build directory."""
    if env is None:
        env = {}

    generator_cmd = GENERATORS[args.generator]['command']

    if args.verbose:
        generator_cmd += [GENERATORS[args.generator]['verbose_flag']]

    # By default, GNU Make and Ninja strip away color escape sequences when they see that their stdout is redirected.
    # If idf.py's stdout is not redirected, the final output is a TTY, so we can tell Make/Ninja to disable stripping
    # of color escape sequences. (Requires Ninja v1.9.0 or later.)
    if sys.stdout.isatty():
        if 'CLICOLOR_FORCE' not in env:
            env['CLICOLOR_FORCE'] = '1'

    RunTool(generator_cmd[0], generator_cmd + [target_name], args.build_dir, env, custom_error_handler, hints=not args.no_hints,
            force_progression=force_progression, interactive=interactive)()


def _strip_quotes(value: str, regexp: re.Pattern=re.compile(r"^\"(.*)\"$|^'(.*)'$|^(.*)$")) -> Optional[str]:
    """
    Strip quotes like CMake does during parsing cache entries
    """
    matching_values = regexp.match(value)
    return [x for x in matching_values.groups() if x is not None][0].rstrip() if matching_values is not None else None


def _parse_cmakecache(path: str) -> Dict:
    """
    Parse the CMakeCache file at 'path'.

    Returns a dict of name:value.

    CMakeCache entries also each have a "type", but this is currently ignored.
    """
    result = {}
    with open(path, encoding='utf-8') as f:
        for line in f:
            # cmake cache lines look like: CMAKE_CXX_FLAGS_DEBUG:STRING=-g
            # groups are name, type, value
            m = re.match(r'^([^#/:=]+):([^:=]+)=(.*)\n$', line)
            if m:
                result[m.group(1)] = m.group(3)
    return result


def _parse_cmdl_cmakecache(entries: List) -> Dict[str, str]:
    """
    Parse list of CMake cache entries passed in via the -D option.

    Returns a dict of name:value.
    """
    result: Dict = {}
    for entry in entries:
        key, value = entry.split('=', 1)
        value = _strip_quotes(value)
        result[key] = value

    return result


def _new_cmakecache_entries(cache: Dict, cache_cmdl: Dict) -> bool:
    for entry in cache_cmdl:
        if entry not in cache:
            return True
        if cache_cmdl[entry] != cache[entry]:
            return True
    return False


def _detect_cmake_generator(prog_name: str) -> Any:
    """
    Find the default cmake generator, if none was specified. Raises an exception if no valid generator is found.
    """
    for (generator_name,  generator) in GENERATORS.items():
        if executable_exists(generator['version']):
            return generator_name
    raise FatalError("To use %s, either the 'ninja' or 'GNU make' build tool must be available in the PATH" % prog_name)


def ensure_build_directory(args: 'PropertyDict', prog_name: str, always_run_cmake: bool=False,
                           env: Dict=None) -> None:
    """Check the build directory exists and that cmake has been run there.

    If this isn't the case, create the build directory (if necessary) and
    do an initial cmake run to configure it.

    This function will also check args.generator parameter. If the parameter is incompatible with
    the build directory, an error is raised. If the parameter is None, this function will set it to
    an auto-detected default generator or to the value already configured in the build directory.
    """

    if not executable_exists(['cmake', '--version']):
        debug_print_idf_version()
        raise FatalError(f'"cmake" must be available on the PATH to use {PROG}')

    project_dir = args.project_dir
    # Verify the project directory
    if not os.path.isdir(project_dir):
        if not os.path.exists(project_dir):
            raise FatalError('Project directory %s does not exist' % project_dir)
        else:
            raise FatalError('%s must be a project directory' % project_dir)
    if not os.path.exists(os.path.join(project_dir, 'CMakeLists.txt')):
        raise FatalError('CMakeLists.txt not found in project directory %s' % project_dir)

    # Verify/create the build directory
    build_dir = args.build_dir
    if not os.path.isdir(build_dir):
        os.makedirs(build_dir)

    # Parse CMakeCache, if it exists
    cache_path = os.path.join(build_dir, 'CMakeCache.txt')
    cache = _parse_cmakecache(cache_path) if os.path.exists(cache_path) else {}

    args.define_cache_entry.append('CCACHE_ENABLE=%d' % args.ccache)

    cache_cmdl = _parse_cmdl_cmakecache(args.define_cache_entry)

    # Validate IDF_TARGET
    _check_idf_target(args, prog_name, cache, cache_cmdl, env)

    if always_run_cmake or _new_cmakecache_entries(cache, cache_cmdl):
        if args.generator is None:
            args.generator = _detect_cmake_generator(prog_name)
        try:
            cmake_args = [
                'cmake',
                '-G',
                args.generator,
                '-DPYTHON_DEPS_CHECKED=1',
                '-DPYTHON={}'.format(sys.executable),
                '-DESP_PLATFORM=1',
            ]
            if args.cmake_warn_uninitialized:
                cmake_args += ['--warn-uninitialized']

            if args.define_cache_entry:
                cmake_args += ['-D' + d for d in args.define_cache_entry]
            cmake_args += [project_dir]

            hints = not args.no_hints
            RunTool('cmake', cmake_args, cwd=args.build_dir, env=env, hints=hints)()
        except Exception:
            # don't allow partially valid CMakeCache.txt files,
            # to keep the "should I run cmake?" logic simple
            if os.path.exists(cache_path):
                os.remove(cache_path)
            raise

    # need to update cache so subsequent access in this method would reflect the result of the previous cmake run
    cache = _parse_cmakecache(cache_path) if os.path.exists(cache_path) else {}

    try:
        generator = cache['CMAKE_GENERATOR']
    except KeyError:
        generator = _detect_cmake_generator(prog_name)
    if args.generator is None:
        args.generator = (generator)  # reuse the previously configured generator, if none was given
    if generator != args.generator:
        raise FatalError("Build is configured for generator '%s' not '%s'. Run '%s fullclean' to start again." %
                         (generator, args.generator, prog_name))

    try:
        home_dir = cache['CMAKE_HOME_DIRECTORY']
        if os.path.realpath(home_dir) != os.path.realpath(project_dir):
            raise FatalError(
                "Build directory '%s' configured for project '%s' not '%s'. Run '%s fullclean' to start again." %
                (build_dir, os.path.realpath(home_dir), os.path.realpath(project_dir), prog_name))
    except KeyError:
        pass  # if cmake failed part way, CMAKE_HOME_DIRECTORY may not be set yet

    try:
        python = cache['PYTHON']
        if os.path.normcase(python) != os.path.normcase(sys.executable):
            raise FatalError(
                "'{}' is currently active in the environment while the project was configured with '{}'. "
                "Run '{} fullclean' to start again.".format(sys.executable, python, prog_name))
    except KeyError:
        pass

    # set global build context
    _set_build_context(args)


def merge_action_lists(*action_lists: Dict) -> Dict:
    merged_actions: Dict = {
        'global_options': [],
        'actions': {},
        'global_action_callbacks': [],
    }
    for action_list in action_lists:
        merged_actions['global_options'].extend(action_list.get('global_options', []))
        merged_actions['actions'].update(action_list.get('actions', {}))
        merged_actions['global_action_callbacks'].extend(action_list.get('global_action_callbacks', []))
    return merged_actions


def get_sdkconfig_filename(args: 'PropertyDict', cache_cmdl: Dict=None) -> str:
    """
    Get project's sdkconfig file name.
    """
    if not cache_cmdl:
        cache_cmdl = _parse_cmdl_cmakecache(args.define_cache_entry)
    config = cache_cmdl.get('SDKCONFIG')
    if config:
        return os.path.abspath(config)

    proj_desc_path = os.path.join(args.build_dir, 'project_description.json')
    try:
        with open(proj_desc_path, 'r') as f:
            proj_desc = json.load(f)
        return str(proj_desc['config_file'])
    except (OSError, KeyError):
        pass

    return os.path.join(args.project_dir, 'sdkconfig')


def get_sdkconfig_value(sdkconfig_file: str, key: str) -> Optional[str]:
    """
    Return the value of given key from sdkconfig_file.
    If sdkconfig_file does not exist or the option is not present, returns None.
    """
    assert key.startswith('CONFIG_')
    if not os.path.exists(sdkconfig_file):
        return None
    # keep track of the last seen value for the given key
    value = None
    # if the value is quoted, this excludes the quotes from the value
    pattern = re.compile(r"^{}=\"?([^\"]*)\"?$".format(key))
    with open(sdkconfig_file, 'r') as f:
        for line in f:
            match = re.match(pattern, line)
            if match:
                value = match.group(1)
    return value


def is_target_supported(project_path: str, supported_targets: List) -> bool:
    """
    Returns True if the active target is supported, or False otherwise.
    """
    return get_target(project_path) in supported_targets


def _check_idf_target(args: 'PropertyDict', prog_name: str, cache: Dict,
                      cache_cmdl: Dict, env: Dict=None) -> None:
    """
    Cross-check the three settings (sdkconfig, CMakeCache, environment) and if there is
    mismatch, fail with instructions on how to fix this.
    """
    sdkconfig = get_sdkconfig_filename(args, cache_cmdl)
    idf_target_from_sdkconfig = get_sdkconfig_value(sdkconfig, 'CONFIG_IDF_TARGET')
    idf_target_from_env = os.environ.get('IDF_TARGET')
    idf_target_from_cache = cache.get('IDF_TARGET')
    idf_target_from_cache_cmdl = cache_cmdl.get('IDF_TARGET')

    # Called from set-target action. The original sdkconfig will be renamed
    # in cmake, so ignore any CONFIG_IDF_TARGET which may be defined in
    # stale sdkconfig.
    if env and env.get('_IDF_PY_SET_TARGET_ACTION') == '1':
        idf_target_from_sdkconfig = None

    if idf_target_from_env:
        # Let's check that IDF_TARGET values are consistent
        if idf_target_from_sdkconfig and idf_target_from_sdkconfig != idf_target_from_env:
            raise FatalError("Project sdkconfig '{cfg}' was generated for target '{t_conf}', but environment variable IDF_TARGET "
                             "is set to '{t_env}'. Run '{prog} set-target {t_env}' to generate new sdkconfig file for target {t_env}."
                             .format(cfg=sdkconfig, t_conf=idf_target_from_sdkconfig, t_env=idf_target_from_env, prog=prog_name))

        if idf_target_from_cache and idf_target_from_cache != idf_target_from_env:
            raise FatalError("Target settings are not consistent: '{t_env}' in the environment, '{t_cache}' in CMakeCache.txt. "
                             "Run '{prog} fullclean' to start again."
                             .format(t_env=idf_target_from_env, t_cache=idf_target_from_cache, prog=prog_name))

        if idf_target_from_cache_cmdl and idf_target_from_cache_cmdl != idf_target_from_env:
            raise FatalError("Target '{t_cmdl}' specified on command line is not consistent with "
                             "target '{t_env}' in the environment."
                             .format(t_cmdl=idf_target_from_cache_cmdl, t_env=idf_target_from_env))
    elif idf_target_from_cache_cmdl:
        # Check if -DIDF_TARGET is consistent with target in CMakeCache.txt
        if idf_target_from_cache and idf_target_from_cache != idf_target_from_cache_cmdl:
            raise FatalError("Target '{t_cmdl}' specified on command line is not consistent with "
                             "target '{t_cache}' in CMakeCache.txt. Run '{prog} set-target {t_cmdl}' to re-generate "
                             'CMakeCache.txt.'
                             .format(t_cache=idf_target_from_cache, t_cmdl=idf_target_from_cache_cmdl, prog=prog_name))

    elif idf_target_from_cache:
        # This shouldn't happen, unless the user manually edits CMakeCache.txt or sdkconfig, but let's check anyway.
        if idf_target_from_sdkconfig and idf_target_from_cache != idf_target_from_sdkconfig:
            raise FatalError("Project sdkconfig '{cfg}' was generated for target '{t_conf}', but CMakeCache.txt contains '{t_cache}'. "
                             "To keep the setting in sdkconfig ({t_conf}) and re-generate CMakeCache.txt, run '{prog} fullclean'. "
                             "To re-generate sdkconfig for '{t_cache}' target, run '{prog} set-target {t_cache}'."
                             .format(cfg=sdkconfig, t_conf=idf_target_from_sdkconfig, t_cache=idf_target_from_cache, prog=prog_name))


class TargetChoice(click.Choice):
    """
    A version of click.Choice with two special features:
    - ignores hyphens
    - not case sensitive
    """
    def __init__(self, choices: List) -> None:
        super(TargetChoice, self).__init__(choices, case_sensitive=False)

    def convert(self, value: Any, param: click.Parameter, ctx: click.Context) -> Any:
        def normalize(string: str) -> str:
            return string.lower().replace('-', '')

        saved_token_normalize_func = ctx.token_normalize_func
        ctx.token_normalize_func = normalize

        try:
            return super(TargetChoice, self).convert(value, param, ctx)
        finally:
            ctx.token_normalize_func = saved_token_normalize_func


class PropertyDict(dict):
    def __getattr__(self, name: str) -> Any:
        if name in self:
            return self[name]
        else:
            raise AttributeError("'PropertyDict' object has no attribute '%s'" % name)

    def __setattr__(self, name: str, value: Any) -> None:
        self[name] = value

    def __delattr__(self, name: str) -> None:
        if name in self:
            del self[name]
        else:
            raise AttributeError("'PropertyDict' object has no attribute '%s'" % name)
