# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import asyncio
import os
import re
import subprocess
import sys
from asyncio.subprocess import Process
from io import open
from types import FunctionType
from typing import Any, Dict, List, Optional, TextIO, Tuple, Union

import click
import yaml
from idf_monitor_base.output_helpers import yellow_print

from .constants import GENERATORS
from .errors import FatalError


def executable_exists(args: List) -> bool:
    try:
        subprocess.check_output(args)
        return True

    except Exception:
        return False


def realpath(path: str) -> str:
    """
    Return the cannonical path with normalized case.

    It is useful on Windows to comparision paths in case-insensitive manner.
    On Unix and Mac OS X it works as `os.path.realpath()` only.
    """
    return os.path.normcase(os.path.realpath(path))


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


def print_hints(*filenames: str) -> None:
    """Getting output files and printing hints on how to resolve errors based on the output."""
    with open(os.path.join(os.path.dirname(__file__), 'hints.yml'), 'r') as file:
        hints = yaml.safe_load(file)
    for file_name in filenames:
        with open(file_name, 'r') as file:
            output = ' '.join(line.strip() for line in file if line.strip())
        for hint in hints:
            try:
                match = re.compile(hint['re']).findall(output)
            except KeyError:
                raise KeyError("Argument 're' missing in {}. Check hints.yml file.".format(hint))
            except re.error as e:
                raise re.error('{} from hints.yml have {} problem. Check hints.yml file.'.format(hint['re'], e))
            if match:
                extra_info = ', '.join(match) if hint.get('match_to_output', '') else ''
                try:
                    yellow_print(' '.join(['HINT:', hint['hint'].format(extra_info)]))
                except KeyError:
                    raise KeyError("Argument 'hint' missing in {}. Check hints.yml file.".format(hint))


def fit_text_in_terminal(out: str) -> str:
    """Fit text in terminal, if the string is not fit replace center with `...`"""
    space_for_dots = 3  # Space for "..."
    terminal_width, _ = os.get_terminal_size()
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
                 hints: bool=False, force_progression: bool=False, interactive: bool=False) -> None:
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
            print_hints(stderr_output_file, stdout_output_file)
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
            sys.exit(f'ERROR: {sys.executable} doesn\'t support asyncio. The issue can be worked around by re-running idf.py with the "--no-hints" argument.')

        stderr_output_file = os.path.join(self.build_dir, log_dir_name, f'idf_py_stderr_output_{p.pid}')
        stdout_output_file = os.path.join(self.build_dir, log_dir_name, f'idf_py_stdout_output_{p.pid}')
        if p.stderr and p.stdout:  # it only to avoid None type in p.std
            await asyncio.gather(
                self.read_and_write_stream(p.stderr, stderr_output_file, sys.stderr),
                self.read_and_write_stream(p.stdout, stdout_output_file))
        await p.wait()  # added for avoiding None returncode
        return p, stderr_output_file, stdout_output_file

    async def read_and_write_stream(self, input_stream: asyncio.StreamReader, output_filename: str,
                                    output_stream: TextIO=sys.stdout) -> None:
        """read the output of the `input_stream` and then write it into `output_filename` and `output_stream`"""
        def delete_ansi_escape(text: str) -> str:
            ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')
            return ansi_escape.sub('', text)

        def prepare_for_print(out: bytes) -> str:
            # errors='ignore' is here because some chips produce some garbage bytes
            result = out.decode(errors='ignore')
            if not output_stream.isatty():
                # delete escape sequence if we printing in environments where ANSI coloring is disabled
                return delete_ansi_escape(result)
            return result

        def print_progression(output: str) -> None:
            # Print a new line on top of the previous line
            sys.stdout.write('\x1b[K')
            print('\r', end='')
            print(fit_text_in_terminal(output.strip('\n\r')), end='', file=output_stream)

        try:
            with open(output_filename, 'w') as output_file:
                while True:
                    if self.interactive:
                        out = await input_stream.read(1)
                    else:
                        out = await input_stream.readline()
                    if not out:
                        break
                    output = prepare_for_print(out)
                    output_file.write(output)

                    # print output in progression way but only the progression related (that started with '[') and if verbose flag is not set
                    if self.force_progression and output[0] == '[' and '-v' not in self.args and output_stream.isatty():
                        print_progression(output)
                    else:
                        output_stream.write(output)
                        output_stream.flush()
        except (RuntimeError, EnvironmentError) as e:
            yellow_print('WARNING: The exception {} was raised and we can\'t capture all your {} and '
                         'hints on how to resolve errors can be not accurate.'.format(e, output_stream.name.strip('<>')))


def run_tool(*args: Any, **kwargs: Any) -> None:
    # Added in case some one use run_tool externally in a idf.py extensions
    return RunTool(*args, **kwargs)()


def run_target(target_name: str, args: 'PropertyDict', env: Optional[Dict]=None,
               custom_error_handler: FunctionType=None, force_progression: bool=False, hints: bool=False, interactive: bool=False) -> None:
    """Run target in build directory."""
    if env is None:
        env = {}

    generator_cmd = GENERATORS[args.generator]['command']
    env.update(GENERATORS[args.generator]['envvar'])

    if args.verbose:
        generator_cmd += [GENERATORS[args.generator]['verbose_flag']]

    RunTool(generator_cmd[0], generator_cmd + [target_name], args.build_dir, env, custom_error_handler, hints=hints,
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


def _new_cmakecache_entries(cache_path: str, new_cache_entries: List) -> bool:
    if not os.path.exists(cache_path):
        return True

    if new_cache_entries:
        current_cache = _parse_cmakecache(cache_path)

        for entry in new_cache_entries:
            key, value = entry.split('=', 1)
            current_value = current_cache.get(key, None)
            if current_value is None or _strip_quotes(value) != current_value:
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


def ensure_build_directory(args: 'PropertyDict', prog_name: str, always_run_cmake: bool=False) -> None:
    """Check the build directory exists and that cmake has been run there.

    If this isn't the case, create the build directory (if necessary) and
    do an initial cmake run to configure it.

    This function will also check args.generator parameter. If the parameter is incompatible with
    the build directory, an error is raised. If the parameter is None, this function will set it to
    an auto-detected default generator or to the value already configured in the build directory.
    """
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

    # Validate or set IDF_TARGET
    _guess_or_check_idf_target(args, prog_name, cache)

    args.define_cache_entry.append('CCACHE_ENABLE=%d' % args.ccache)

    if always_run_cmake or _new_cmakecache_entries(cache_path, args.define_cache_entry):
        if args.generator is None:
            args.generator = _detect_cmake_generator(prog_name)
        try:
            cmake_args = [
                'cmake',
                '-G',
                args.generator,
                '-DPYTHON_DEPS_CHECKED=1',
                '-DESP_PLATFORM=1',
            ]
            if args.cmake_warn_uninitialized:
                cmake_args += ['--warn-uninitialized']

            if args.define_cache_entry:
                cmake_args += ['-D' + d for d in args.define_cache_entry]
            cmake_args += [project_dir]

            hints = not args.no_hints
            RunTool('cmake', cmake_args, cwd=args.build_dir, hints=hints)()
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
        if realpath(home_dir) != realpath(project_dir):
            raise FatalError(
                "Build directory '%s' configured for project '%s' not '%s'. Run '%s fullclean' to start again." %
                (build_dir, realpath(home_dir), realpath(project_dir), prog_name))
    except KeyError:
        pass  # if cmake failed part way, CMAKE_HOME_DIRECTORY may not be set yet


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


def _guess_or_check_idf_target(args: 'PropertyDict', prog_name: str, cache: Dict) -> None:
    """
    If CMakeCache.txt doesn't exist, and IDF_TARGET is not set in the environment, guess the value from
    sdkconfig or sdkconfig.defaults, and pass it to CMake in IDF_TARGET variable.

    Otherwise, cross-check the three settings (sdkconfig, CMakeCache, environment) and if there is
    mismatch, fail with instructions on how to fix this.
    """
    # Default locations of sdkconfig files.
    # FIXME: they may be overridden in the project or by a CMake variable (IDF-1369).
    # These are used to guess the target from sdkconfig, or set the default target by sdkconfig.defaults.
    idf_target_from_sdkconfig = get_target(args.project_dir)
    idf_target_from_sdkconfig_defaults = get_target(args.project_dir, 'sdkconfig.defaults')
    idf_target_from_env = os.environ.get('IDF_TARGET')
    idf_target_from_cache = cache.get('IDF_TARGET')

    if not cache and not idf_target_from_env:
        # CMakeCache.txt does not exist yet, and IDF_TARGET is not set in the environment.
        guessed_target = idf_target_from_sdkconfig or idf_target_from_sdkconfig_defaults
        if guessed_target:
            if args.verbose:
                print("IDF_TARGET is not set, guessed '%s' from sdkconfig" % (guessed_target))
            args.define_cache_entry.append('IDF_TARGET=' + guessed_target)

    elif idf_target_from_env:
        # Let's check that IDF_TARGET values are consistent
        if idf_target_from_sdkconfig and idf_target_from_sdkconfig != idf_target_from_env:
            raise FatalError("Project sdkconfig was generated for target '{t_conf}', but environment variable IDF_TARGET "
                             "is set to '{t_env}'. Run '{prog} set-target {t_env}' to generate new sdkconfig file for target {t_env}."
                             .format(t_conf=idf_target_from_sdkconfig, t_env=idf_target_from_env, prog=prog_name))

        if idf_target_from_cache and idf_target_from_cache != idf_target_from_env:
            raise FatalError("Target settings are not consistent: '{t_env}' in the environment, '{t_cache}' in CMakeCache.txt. "
                             "Run '{prog} fullclean' to start again."
                             .format(t_env=idf_target_from_env, t_cache=idf_target_from_cache, prog=prog_name))

    elif idf_target_from_cache and idf_target_from_sdkconfig and idf_target_from_cache != idf_target_from_sdkconfig:
        # This shouldn't happen, unless the user manually edits CMakeCache.txt or sdkconfig, but let's check anyway.
        raise FatalError("Project sdkconfig was generated for target '{t_conf}', but CMakeCache.txt contains '{t_cache}'. "
                         "To keep the setting in sdkconfig ({t_conf}) and re-generate CMakeCache.txt, run '{prog} fullclean'. "
                         "To re-generate sdkconfig for '{t_cache}' target, run '{prog} set-target {t_cache}'."
                         .format(t_conf=idf_target_from_sdkconfig, t_cache=idf_target_from_cache, prog=prog_name))


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
