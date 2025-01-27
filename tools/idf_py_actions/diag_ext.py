# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import atexit
import difflib
import os
import platform
import re
import shutil
import sys
import textwrap
import traceback
import uuid
import zipfile
from pathlib import Path
from string import Template
from subprocess import run
from tempfile import TemporaryDirectory
from typing import Any
from typing import Dict
from typing import List
from typing import Optional
from typing import Tuple

import click
import yaml
from idf_py_actions.tools import PropertyDict
from idf_py_actions.tools import red_print
from idf_py_actions.tools import yellow_print


# Logging levels and configurations.
LOG_FATAL = 1
LOG_ERROR = 2
LOG_WARNING = 3
LOG_INFO = 4
LOG_DEBUG = 5
LOG_LEVEL = LOG_INFO
LOG_COLORS = True
LOG_PREFIX = False

# A temporary directory is used to store the report. Once it is completely
# generated, it is moved to its final location.
TMP_DIR = TemporaryDirectory()
TMP_DIR_PATH = Path(TMP_DIR.name)
TMP_DIR_REPORT_PATH = TMP_DIR_PATH / 'report'
TMP_DIR_REPORT_REDACTED_PATH = TMP_DIR_PATH / 'redacted'

# The full debug log is stored in the report directory alongside other
# collected files.
LOG_FILE = None
LOG_FILE_PATH = TMP_DIR_REPORT_PATH / 'diag.log'

# Fixed path for the built-in recipes
BUILTIN_RECIPES_PATH = Path(__file__).parent / 'diag' / 'recipes'


def cleanup() -> None:
    """Perform cleanup operations in case of unexpected termination."""
    try:
        if LOG_FILE:
            LOG_FILE.close()
        shutil.rmtree(TMP_DIR_PATH)
    except Exception:
        pass


atexit.register(cleanup)


def exception_tb() -> Optional[str]:
    """Return a string containing the message from the most recent exception,
    along with its traceback, if available.
    """
    ex_type, ex_value, ex_traceback = sys.exc_info()
    in_exception = ex_type is not None
    if not in_exception:
        return None
    ex_msg = f'exception {ex_type}:'
    if str(ex_value):
        ex_msg += f' {ex_value}'
    tb = ''.join(traceback.format_tb(ex_traceback))
    ex_msg += '\n' + tb.rstrip()
    ex_msg = textwrap.indent(ex_msg, prefix='> ')
    return ex_msg


def exception_msg() -> Optional[str]:
    """Return a string containing the message from the most recent exception,
    if available.
    """
    ex_type, ex_value, ex_traceback = sys.exc_info()
    in_exception = ex_type is not None
    if not in_exception or not str(ex_value):
        return None
    return str(ex_value)


def log(level: int, msg: str, prefix: str) -> None:
    """Logs a message with a specified severity level and prefix.

    This function outputs log messages to standard error (stderr) based on the
    provided severity level. All messages are also saved to a log file, which
    is part of the diagnostic report. The log file entries include a severity
    prefix but do not contain any color formatting.

    Parameters:
    level (int): The severity level of the log message.
    msg (str): The message to be logged.
    prefix (str): A character prefix to indicate the log severity.

    Returns:
    None
    """
    global LOG_FILE
    if LOG_PREFIX:
        log_prefix = f'{prefix} '
    else:
        log_prefix = ''

    msg = textwrap.indent(msg, prefix=log_prefix)

    if LOG_FILE:
        try:
            log_msg = textwrap.indent(msg, prefix=f'{prefix} ')
            LOG_FILE.write(log_msg + '\n')
            LOG_FILE.flush()
        except Exception:
            LOG_FILE.close()
            LOG_FILE = None
            err(f'Cannot write to log file "{LOG_FILE}". Logging to file is turned off.')

    if level > LOG_LEVEL:
        return

    if not LOG_COLORS or level not in (LOG_FATAL, LOG_ERROR, LOG_WARNING):
        print(msg, file=sys.stderr)
        sys.stderr.flush()
        return

    if level == LOG_ERROR or level == LOG_FATAL:
        red_print(msg)
    elif level == LOG_WARNING:
        yellow_print(msg)


def die(msg: str) -> None:
    """Irrecoverable fatal error."""
    fatal(msg)
    die_msg = 'ESP-IDF diagnostic command failed.'
    if LOG_LEVEL != LOG_DEBUG:
        # If the log level for stderr is not set to debug, suggest it.
        die_msg += f' Using the "-d/--debug" option may provide more information.'
    # Avoid calling fatal, as it may print the exception again if present.
    log(LOG_FATAL, die_msg, 'F')
    sys.exit(128)


def fatal(msg: str) -> None:
    """A fatal message, along with the exception traceback logged for
    debugging if available. Used by the die function."""
    ex_msg = exception_msg()
    if ex_msg:
        msg += f': {ex_msg}'
    log(LOG_FATAL, 'fatal: ' + msg, 'F')
    ex_tb = exception_tb()
    if ex_tb:
        dbg(ex_tb)


def err(msg: str) -> None:
    """A error message, along with the exception traceback logged for
    debugging if available."""
    ex_msg = exception_msg()
    if ex_msg:
        msg += f': {ex_msg}'
    log(LOG_ERROR, 'error: ' + msg, 'E')
    ex_tb = exception_tb()
    if ex_tb:
        dbg(ex_tb)


def warn(msg: str) -> None:
    """A warning message, along with the exception traceback logged for
    debugging if available."""
    ex_msg = exception_msg()
    if ex_msg:
        msg += f': {ex_msg}'
    log(LOG_WARNING, 'warning: ' + msg, 'W')
    ex_tb = exception_tb()
    if ex_tb:
        dbg(ex_tb)


def info(msg: str) -> None:
    log(LOG_INFO, msg, 'I')


def dbg(msg: str) -> None:
    log(LOG_DEBUG, msg, 'D')


def set_logger(debug: bool,
               prefix: bool,
               file: bool,
               colors: bool) -> None:
    """Configure the logging settings for the application.

    Parameters:
    debug (bool): If True, enables debug logging to standard error.
    prefix (bool): If True, adds a one-character prefix to each log message to identify the log level.
    file (bool): If True, specifies that all log messages should be stored in a file, regardless of the log level setting.
    colors (bool): If True, enables the use of ANSI color codes in log messages.

    Returns:
    None
    """
    global LOG_LEVEL
    global LOG_FILE
    global LOG_COLORS
    global LOG_PREFIX

    if not colors:
        LOG_COLORS = False

    if debug:
        LOG_LEVEL = LOG_DEBUG

    if prefix:
        LOG_PREFIX = True

    if file:
        try:
            LOG_FILE_PATH.parent.mkdir(parents=True, exist_ok=True)
            LOG_FILE = open(LOG_FILE_PATH, 'w')
        except Exception:
            err(f'Cannot open log file "{LOG_FILE}". Log file will not be generated.')


def diff_dirs(dir1: Path, dir2: Path) -> None:
    """Show differences in files between two directories."""
    dir1_root_path = Path(dir1).resolve()
    dir2_root_path = Path(dir2).resolve()
    dbg(f'diff "{dir1_root_path}" to "{dir2_root_path}"')
    for dir1_file_path in dir1_root_path.rglob('*'):
        if not dir1_file_path.is_file():
            continue
        dir2_file_path = dir2_root_path / dir1_file_path.relative_to(dir1_root_path)

        with open(dir1_file_path, 'r') as f1, open(dir2_file_path, 'r') as f2:
            diff = difflib.unified_diff(
                f1.readlines(),
                f2.readlines(),
                fromfile=str(dir1_file_path.relative_to(dir1_root_path.parent)),
                tofile=str(dir2_file_path.relative_to(dir2_root_path.parent)),
                n=0
            )
            for line in diff:
                dbg(line.strip())


def redact_files(dir1: Path, dir2: Path) -> None:
    """Show differences in files between two directories."""
    purge_path = Path(__file__).parent / 'diag' / 'purge.yml'
    with open(purge_path, 'r') as f:
        purge = yaml.safe_load(f.read())

    regexes: List = []
    for entry in purge:
        regex = re.compile(entry['regex'])
        repl = entry['repl']
        regexes.append((regex, repl))

    dir1_root_path = Path(dir1).resolve()
    dir2_root_path = Path(dir2).resolve()
    dbg(f'redacting files in "{dir1_root_path}" into "{dir2_root_path}"')
    for dir1_file_path in dir1_root_path.rglob('*'):
        if not dir1_file_path.is_file():
            continue
        dir2_file_path = dir2_root_path / dir1_file_path.relative_to(dir1_root_path)
        dir2_file_path.parent.mkdir(parents=True, exist_ok=True)

        with open(dir1_file_path, 'r') as f1, open(dir2_file_path, 'w') as f2:
            data = f1.read()
            for regex, repl in regexes:
                data = regex.sub(repl, data)
            f2.write(data)

    diff_dirs(dir1, dir2)


def validate_recipe(recipe: Dict) -> None:
    """Validate the loaded recipe file. This is done manually to avoid any
    dependencies and to provide more informative error messages.
    """
    recipe_keys = ['description', 'tags', 'output', 'steps']
    step_keys = ['name', 'cmds', 'output', 'system']
    recipe_description = recipe.get('description')
    recipe_tags = recipe.get('tags')
    recipe_output = recipe.get('output')
    recipe_steps = recipe.get('steps')

    for key in recipe:
        if key not in recipe_keys:
            raise RuntimeError(f'Unknown recipe key "{key}", expecting "{recipe_keys}"')

    if not recipe_description:
        raise RuntimeError(f'Recipe is missing "description" key')

    if type(recipe_description) is not str:
        raise RuntimeError(f'Recipe "description" key is not of type "str"')

    if recipe_tags:
        if type(recipe_tags) is not list:
            raise RuntimeError(f'Recipe "tags" key is not of type "list"')
        for tag in recipe_tags:
            if type(tag) is not str:
                raise RuntimeError(f'Recipe tag value "{tag}" is not of type "str"')

    if recipe_output:
        if type(recipe_output) is not str:
            raise RuntimeError(f'Recipe "output" key is not of type "str"')

    if not recipe_steps:
        raise RuntimeError(f'Recipe is missing "steps" key')

    if type(recipe_steps) is not list:
        raise RuntimeError(f'Recipe "steps" key is not of type "list"')

    for step in recipe_steps:
        for key in step:
            if key not in step_keys:
                raise RuntimeError(f'Unknown recipe step key "{key}", expecting "{step_keys}"')

        step_name = step.get('name')
        step_output = step.get('output')
        step_cmds = step.get('cmds')
        step_system = step.get('system')

        if not step_name:
            raise RuntimeError(f'Recipe step is missing "name" key')
        if type(step_name) is not str:
            raise RuntimeError(f'Recipe step "name" key is not of type "str"')
        if not step_cmds:
            raise RuntimeError(f'Recipe step is missing "cmds" key')
        if type(step_cmds) is not list:
            raise RuntimeError(f'Recipe step "cmds" key is not of type "list"')
        if step_output:
            if type(step_output) is not str:
                raise RuntimeError(f'Step "output" key is not of type "str"')
        if step_system:
            if type(step_system) is not str:
                raise RuntimeError(f'Step "system" key is not of type "str"')
            if step_system not in ['Linux', 'Windows', 'Darwin']:
                raise RuntimeError((f'Unknown "system" key value "{step_system}", '
                                    f'expecting "Linux", "Windows" or "Darwin"'))

        for cmd in step_cmds:
            if 'exec' in cmd:
                cmd_exec_keys = ['exec', 'cmd', 'output', 'stderr', 'timeout', 'append']

                exec_cmd = cmd.get('cmd')
                output = cmd.get('output')
                stderr = cmd.get('stderr')
                timeout = cmd.get('timeout')
                append = cmd.get('append')

                for key in cmd:
                    if key not in cmd_exec_keys:
                        raise RuntimeError((f'Unknown "exec" command argument "{key}" in step "{step_name}", '
                                            f'expecting "{cmd_exec_keys}"'))

                # Required arguments
                if not exec_cmd:
                    raise RuntimeError(f'Command "exec" in step "{step_name}" is missing "cmd" argument')
                if type(exec_cmd) is list:
                    for arg in exec_cmd:
                        if type(arg) is not str:
                            raise RuntimeError((f'List entry "{arg}" in "cmd" argument for command "exec" '
                                                f'in step "{step_name}" is not of type "str"'))
                elif type(exec_cmd) is not str:
                    raise RuntimeError(f'Command "exec" in step "{step_name}" is not of type "list" or "str"')

                # Optional arguments
                if output and type(output) is not str:
                    raise RuntimeError(f'Argument "output" for command "exec" in step "{step_name}" is not of type "str"')
                if stderr and type(stderr) is not str:
                    raise RuntimeError(f'Argument "stderr" for command "exec" in step "{step_name}" is not of type "str"')
                if timeout and type(timeout) is not int:
                    raise RuntimeError(f'Argument "timeout" for command "exec" in step "{step_name}" is not of type "int"')
                if append and type(append) is not bool:
                    raise RuntimeError(f'Argument "append" for command "exec" in step "{step_name}" is not of type "bool"')

            elif 'file' in cmd:
                cmd_file_keys = ['file', 'path', 'output']

                path = cmd.get('path')
                output = cmd.get('output')

                for key in cmd:
                    if key not in cmd_file_keys:
                        raise RuntimeError((f'Unknown "file" command argument "{key}" in step "{step_name}", '
                                            f'expecting "{cmd_file_keys}"'))

                # Required arguments
                if not path:
                    raise RuntimeError(f'Command "file" in step "{step_name}" is missing "path" argument')
                if type(path) is not str:
                    raise RuntimeError(f'Argument "path" for command "file" in step "{step_name}" is not of type "str"')

                # Optional arguments
                if output and type(output) is not str:
                    raise RuntimeError(f'Argument "output" for command "file" in step "{step_name}" is not of type "str"')

            elif 'env' in cmd:
                cmd_env_keys = ['env', 'vars', 'regex', 'output', 'append']

                variables = cmd.get('vars')
                regex = cmd.get('regex')
                output = cmd.get('output')
                append = cmd.get('append')

                for key in cmd:
                    if key not in cmd_env_keys:
                        raise RuntimeError((f'Unknown "env" command argument "{key}" in step "{step_name}", '
                                            f'expecting "{cmd_env_keys}"'))

                # Required arguments
                if not variables and not regex:
                    raise RuntimeError(f'Command "env" in step "{step_name}" is missing both "vars" and "regex" arguments')
                if variables:
                    if type(variables) is not list:
                        raise RuntimeError(f'Argument "vars" for command "env" in step "{step_name}" is not of type "list"')
                    for var in variables:
                        if type(var) is not str:
                            raise RuntimeError((f'List entry "{var}" in "vars" argument for command "env" '
                                                f'in step "{step_name}" is not of type "str"'))
                if regex:
                    if type(regex) is not str:
                        raise RuntimeError(f'Argument "regex" for command "env" in step "{step_name}" is not of type "str"')
                    try:
                        re.compile(regex)
                    except re.error as e:
                        raise RuntimeError((f'Argument "regex" for command "env" in step "{step_name}" is not '
                                            f'a valid regular expression: {e}'))

                # Optional arguments
                if output and type(output) is not str:
                    raise RuntimeError(f'Argument "output" for command "env" in step "{step_name}" is not of type "str"')
                if append and type(append) is not bool:
                    raise RuntimeError(f'Argument "append" for command "env" in step "{step_name}" is not of type "bool"')

            elif 'glob' in cmd:
                cmd_glob_keys = ['glob', 'pattern', 'path', 'regex', 'mtime', 'recursive', 'relative', 'output']

                pattern = cmd.get('pattern')
                path = cmd.get('path')
                regex = cmd.get('regex')
                mtime = cmd.get('mtime')
                recursive = cmd.get('recursive')
                relative = cmd.get('relative')
                output = cmd.get('output')

                for key in cmd:
                    if key not in cmd_glob_keys:
                        raise RuntimeError((f'Unknown "glob" command argument "{key}" in step "{step_name}", '
                                            f'expecting "{cmd_glob_keys}"'))
                # Required arguments
                if not pattern:
                    raise RuntimeError(f'Command "glob" in step "{step_name}" is missing "pattern" argument')
                if type(pattern) is not str:
                    raise RuntimeError(f'Argument "pattern" for command "glob" in step "{step_name}" is not of type "str"')
                if not path:
                    raise RuntimeError(f'Command "glob" in step "{step_name}" is missing "path" argument')
                if type(path) is not str:
                    raise RuntimeError(f'Argument "path" for command "glob" in step "{step_name}" is not of type "str"')

                # Optional arguments
                if regex:
                    if type(regex) is not str:
                        raise RuntimeError(f'Argument "regex" for command "glob" in step "{step_name}" is not of type "str"')
                    try:
                        re.compile(regex)
                    except re.error as e:
                        raise RuntimeError((f'Argument "regex" for command "glob" in step "{step_name}" is not '
                                            f'a valid regular expression: {e}'))
                if mtime and type(mtime) is not bool:
                    raise RuntimeError(f'Argument "mtime" for command "glob" in step "{step_name}" is not of type "bool"')
                if recursive and type(recursive) is not bool:
                    raise RuntimeError(f'Argument "recursive" for command "glob" in step "{step_name}" is not of type "bool"')
                if relative and type(relative) is not bool:
                    raise RuntimeError(f'Argument "relative" for command "glob" in step "{step_name}" is not of type "bool"')
                if output and type(output) is not str:
                    raise RuntimeError(f'Argument "output" for command "glob" in step "{step_name}" is not of type "str"')

            else:
                raise RuntimeError(f'Unknown command "{cmd}" in step "{step_name}"')


def get_output_path(src: Optional[str],
                    dst: Optional[str],
                    step: Dict,
                    recipe: Dict,
                    src_root: Optional[str] = None) -> Path:
    """Construct the output file path based on source, destination, and recipe output.

    Parameters:
    src (Optional[str]): The source file path. This can be None, for example,
                         when used in an exec command.
    dst (Optional[str]): The destination file path or directory. If it ends with
                         a '/' character, it is considered a directory, and the
                         src file name is appended to it. Otherwise it is the
                         file where the output should be saved. This can also be
                         None, in which case the src file name is used as the
                         output file name.
    step (Dict): The step this file belongs to, used to obtain the step'
                 global output directory.
    recipe (Dict): The recipe this file belongs to, used to obtain the recipe's
                   global output directory.
    src_root (Optional[str]): The src file directory, used to determine the
                              relative source file path for constructing the
                              relative destination path. For example, if src
                              is "/dir/dir2/dir3/file.txt" and src_root is
                              "/dir/" and dst is "/output/", the destination
                              file path will be "/output/dir2/dir3/file.txt".

    Returns:
    Path: The constructed output file path.
    """
    dst_path = TMP_DIR_REPORT_PATH
    # recipe global output directory
    recipe_root = recipe.get('output')
    # step global output directory
    step_root = step.get('output')

    if recipe_root:
        dst_path = dst_path / recipe_root

    if step_root:
        dst_path = dst_path / step_root

    if dst:
        dst_path = dst_path / dst
        if dst.endswith('/') and src:
            if src_root:
                src_rel_path = Path(src).relative_to(src_root)
                dst_path = dst_path / src_rel_path
            else:
                dst_path = dst_path / Path(src).name
    elif src:
        dst_path = dst_path / Path(src).name

    return dst_path


def cmd_file(args: Dict, step: Dict, recipe: Dict) -> None:
    """file command"""
    src = args['path']
    dst = args.get('output')

    dst_path = get_output_path(src, dst, step, recipe)

    try:
        dst_path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy(src, dst_path)
    except Exception:
        warn(f'Cannot copy file "{src}"')


def cmd_exec(args: Dict, step: Dict, recipe: Dict) -> None:
    """exec command"""
    cmd = args['cmd']
    stdout = args.get('output')
    stderr = args.get('stderr')
    timeout = args.get('timeout')
    append = args.get('append', False)

    stdout_path = get_output_path(None, stdout, step, recipe)
    stderr_path = get_output_path(None, stderr, step, recipe)

    # If cmd is a string, execute it using the shell.
    if isinstance(cmd, list):
        shell = False
    else:
        shell = True

    try:
        p = run(cmd, shell=shell, text=True, capture_output=True, timeout=timeout)
    except Exception:
        warn(f'Exec command "{cmd}" failed')
        return

    if p.returncode:
        warn(f'Exec command "{cmd}" failed with exit code {p.returncode}')
        if p.stderr:
            dbg(f'stderr: "{p.stderr}"')

    if stdout and p.stdout:
        try:
            stdout_path.parent.mkdir(parents=True, exist_ok=True)
            with open(stdout_path, 'a' if append else 'w') as f:
                f.write(p.stdout)
        except Exception:
            warn(f'Cannot write exec command "{cmd}" stdout to "{stdout}"')

    if stderr and p.stderr:
        try:
            stderr_path.parent.mkdir(parents=True, exist_ok=True)
            with open(stderr_path, 'a' if append else 'w') as f:
                f.write(p.stderr)
        except Exception:
            warn(f'Cannot write exec command "{cmd}" stderr to "{stderr}"')


def cmd_env(args: Dict, step: Dict, recipe: Dict) -> None:
    """env command"""
    variables = args.get('vars', [])
    regex_str = args.get('regex')
    output = args.get('output')
    append = args.get('append', False)
    regex = re.compile(regex_str) if regex_str else None

    output_path = get_output_path(None, output, step, recipe)
    found_list: List = []
    out_list: List = []

    for var, val in os.environ.items():
        if var in variables:
            found_list.append(var)
            continue

        if not regex:
            continue

        match = regex.match(var)
        if match:
            found_list.append(var)

    for var in found_list:
        val = os.environ[var]
        out_list.append(f'{var}={val}')

    if output:
        try:
            output_path.parent.mkdir(parents=True, exist_ok=True)
            with open(output_path, 'a' if append else 'w') as f:
                f.write('\n'.join(out_list))
        except Exception:
            warn(f'Cannot write env command output to "{output}"')


def get_latest_modified_file(file_paths: List[Path]) -> Optional[Path]:
    """Return the most recently modified file from the file_paths list"""
    file_path = None
    file_mtime = 0.0

    for file in file_paths:
        mtime = file.stat().st_mtime
        if mtime < file_mtime:
            continue
        file_mtime = mtime
        file_path = file

    return file_path


def cmd_glob(args: Dict, step: Dict, recipe: Dict) -> None:
    """glob command"""
    pattern = args['pattern']
    dir_path = Path(args['path'])
    output = args.get('output')
    mtime = args.get('mtime', False)
    recursive = args.get('recursive', False)
    relative = args.get('relative', False)
    regex_str = args.get('regex')

    try:
        if recursive:
            file_paths = list(dir_path.rglob(pattern))
        else:
            file_paths = list(dir_path.glob(pattern))
    except Exception:
        warn(f'Cannot glob "{pattern}" in "{dir_path}"')
        return

    file_paths = [file_path for file_path in file_paths if file_path.is_file()]
    if not file_paths:
        warn(f'No files matching glob "{pattern}" found in "{dir_path}"')
        return

    if regex_str:
        file_paths_match = []
        regex = re.compile(regex_str, flags=re.MULTILINE)
        for file_path in file_paths:
            try:
                with open(file_path, 'r') as f:
                    data = f.read()
                    match = regex.search(data)
                    if match:
                        file_paths_match.append(file_path)
            except Exception:
                err(f'Failed to search for the regex "{regex_str}" in "{file_path}"')

        if not file_paths_match:
            warn(f'No files with content matching regex "{regex_str}" found in "{dir_path}"')
            return
        file_paths = file_paths_match

    if mtime:
        last_modified_file = get_latest_modified_file(file_paths)
        if not last_modified_file:
            err(f'No last modified file found for "{pattern}" found in "{dir_path}"')
            return
        file_paths = [last_modified_file]

    for file_path in file_paths:
        # If the relative flag is enabled, save the file in the output directory while
        # maintaining the same relative path as in the source directory.
        dst_path = get_output_path(str(file_path), output, step, recipe, str(dir_path) if relative else None)
        try:
            dst_path.parent.mkdir(parents=True, exist_ok=True)
            if dst_path.is_file():
                # A file already exists in the report directory. Attempt to
                # create a new name by appending numerical suffixes.
                cnt = 1
                while True:
                    new_dst_path = dst_path.with_name(dst_path.name + f'.{cnt}')
                    if not new_dst_path.exists():
                        warn(f'File "{dst_path.name}" for "{file_path}" already exists. Using "{new_dst_path.name}"')
                        dst_path = new_dst_path
                        break
                    cnt += 1
            dbg(f'copy "{file_path}" to "{dst_path}"')
            shutil.copy(file_path, dst_path)
        except Exception:
            warn(f'Cannot copy glob file "{file_path}"')


def process_recipe(recipe: Dict) -> None:
    """execute commands for every stage in a recipe"""
    for step in recipe['steps']:
        step_name = step['name']
        step_system = step.get('system')

        if step_system and step_system != platform.system():
            dbg(f'Skipping step "{step_name}" for "{step_system}"')
            continue

        dbg(f'Processing step "{step_name}"')
        print(f'* {step_name}')
        for cmd in step['cmds']:
            dbg(f'cmd: "{cmd}"')
            if 'file' in cmd:
                cmd_file(cmd, step, recipe)
            elif 'exec' in cmd:
                cmd_exec(cmd, step, recipe)
            elif 'env' in cmd:
                cmd_env(cmd, step, recipe)
            elif 'glob' in cmd:
                cmd_glob(cmd, step, recipe)
            else:
                err(f'Unknow command "{cmd}" in step "{step_name}"')


def get_recipes(cmdl_recipes: Optional[Tuple],
                cmdl_tags: Optional[Tuple],
                append: bool,
                variables: Dict) -> Dict:
    """Load and return a dictionary of recipes.

    This function retrieves recipes based on the provided command line inputs
    and filters them using specified tags. It can also append additional
    recipes to a set of built-in recipes.

    Args:
        cmdl_recipes (Optional[Tuple]): A tuple containing recipe names for
                                        built-in recipes or paths to
                                        user-provided recipes.
        cmdl_tags (Optional[Tuple]): A tuple of tags used to filter the loaded
                                     recipes.
        append (bool): A flag indicating whether to append the `cmdl_recipes`
                       to the built-in recipes. If False, only the recipes
                       specified in `cmdl_recipes` and filtered by `cmdl_tags`
                       are used. Used to allow run additional recipes along
                       with the built-in ones.
        variables (Dict): A dictionary of variables to be replaced within the
                          recipes.

    Returns:
        Dict: A dictionary where each key is a recipe filename and each value
        is a dictionary representing the recipe.
    """

    builtin_recipe_files: Dict = {}
    recipe_files: List = []
    recipes: Dict = {}

    for recipe_path in BUILTIN_RECIPES_PATH.glob('*.yml'):
        builtin_recipe_files[recipe_path.stem] = str(recipe_path.resolve())
    dbg(f'Builtin recipes "{builtin_recipe_files}"')

    if cmdl_recipes:
        for recipe_file in cmdl_recipes:
            recipe_path = Path(recipe_file).resolve()
            if recipe_path.is_file():
                recipe_files.append(str(recipe_path))
                continue

            if recipe_file in builtin_recipe_files:
                recipe_files.append(builtin_recipe_files[recipe_file])
                continue

            die(f'Cannot find recipe "{recipe_file}"')

        if append:
            recipe_files += list(builtin_recipe_files.values())
    else:
        recipe_files += list(builtin_recipe_files.values())

    recipe_files = list(set(recipe_files))
    recipe_files.sort()
    dbg(f'Recipe files to use "{recipe_files}"')

    # Load recipes
    for recipe_file in recipe_files:
        dbg(f'Loading recipe file "{recipe_file}"')
        try:
            with open(recipe_file, 'r') as f:
                data = f.read()
                formatted = Template(data).safe_substitute(**variables)
                recipe = yaml.safe_load(formatted)
                recipes[recipe_file] = recipe
        except Exception:
            die(f'Cannot load diagnostic recipe "{recipe_file}"')

    if cmdl_tags:
        dbg('Filtering recipe file with tags "{}"'.format(', '.join(cmdl_tags)))
        recipes_tagged: Dict = {}
        for recipe_file, recipe in recipes.items():
            recipe_tags = recipe.get('tags')

            if not recipe_tags:
                continue

            for cmdl_tag in cmdl_tags:
                if cmdl_tag in recipe_tags:
                    recipes_tagged[recipe_file] = recipe
                    break

        recipes = recipes_tagged

    if not recipes:
        die(f'No recipes available')

    return recipes


def act_list_recipes(cmdl_recipes: Optional[Tuple],
                     cmdl_tags: Optional[Tuple],
                     append: bool,
                     variables: Dict) -> None:

    """Display a list of available recipes along with their details"""
    try:
        recipes = get_recipes(cmdl_recipes, cmdl_tags, append, variables)
    except Exception:
        die(f'Unable to create list of recipe files')

    for recipe_file, recipe in recipes.items():
        builtin = BUILTIN_RECIPES_PATH == Path(recipe_file).parent

        try:
            validate_recipe(recipe)
            valid = True
        except Exception:
            valid = False

        print(recipe_file)
        print('   description: {}'.format(recipe.get('description', '')))
        print('   short name: {}'.format(Path(recipe_file).stem if builtin else ''))
        print('   valid: {}'.format(valid))
        print('   builtin: {}'.format(builtin))
        print('   tags: {}'.format(', '.join(recipe.get('tags', ''))))


def act_check_recipes(cmdl_recipes: Optional[Tuple],
                      cmdl_tags: Optional[Tuple],
                      append: bool,
                      variables: Dict) -> None:
    """Verify recipes"""
    try:
        recipes = get_recipes(cmdl_recipes, cmdl_tags, append, variables)
    except Exception:
        die(f'Unable to create list of recipe files')

    error = False
    for recipe_file, recipe in recipes.items():
        print(f'Checking recipe "{recipe_file}"')
        try:
            validate_recipe(recipe)
            print('Recipe is valid')
        except Exception:
            err('validation failed')
            print('Recipe is invalid.')
            error = True

    if error:
        die('Recipes validation failed')


def act_zip(directory: str, output: Optional[str], force: bool) -> None:
    """Compress the report directory into a zip file"""
    archive_dir_path = Path(directory).expanduser()
    archive_path = Path(output or directory).with_suffix('.zip').expanduser()

    info(f'Creating archive "{archive_path}"')

    if not archive_dir_path.exists() or not archive_dir_path.is_dir():
        die(f'The path "{archive_dir_path}" either does not exist or is not a directory.')

    if archive_path.exists():
        if not archive_path.is_file():
            die((f'Directory entry "{archive_path}" already exists and is not a regular file. '
                 f'Please use the --output option or remove "{archive_path}" manually.'))
        if not force:
            die((f'Archive file "{archive_path}" already exists. '
                 f'Please use the --output option or --force option to overwrite the existing '
                 f'"{archive_path}" archive.'))
    try:
        with zipfile.ZipFile(archive_path, 'w', zipfile.ZIP_DEFLATED) as f:
            for file in archive_dir_path.rglob('*'):
                print(f'adding: {file}')
                f.write(file, file.relative_to(archive_dir_path.parent))
    except Exception:
        die(f'Cannot create zip archive for "{directory}" directory.')

    info(f'The archive "{archive_path}" is prepared and can be included with your issue report.')


def create(action: str,
           ctx: click.core.Context,
           args: PropertyDict,
           debug: bool,
           log_prefix: bool,
           force: bool,
           no_color: bool,
           zip_directory: Optional[str],
           list_recipes: bool,
           check_recipes: bool,
           cmdl_recipes: Tuple,
           cmdl_tags: Tuple,
           append: bool,
           output: Optional[str]) -> None:

    if list_recipes or check_recipes or zip_directory:
        # Generate a log file only when the report is produced.
        log_to_file = False
    else:
        log_to_file = True

    set_logger(debug=debug,
               prefix=log_prefix,
               file=log_to_file,
               colors=not no_color)

    if zip_directory:
        # Archive recipes command
        act_zip(zip_directory, output, force)
        return

    project_dir = args.project_dir

    # Set up variables that can be utilized in the recipe.
    recipe_variables: Dict = {}

    if project_dir:
        recipe_variables['PROJECT_DIR'] = project_dir
    else:
        warn(f'Project directory is not set')

    build_dir = args.build_dir
    if build_dir:
        recipe_variables['BUILD_DIR'] = build_dir
    else:
        warn(f'Build directory is not set')

    recipe_variables['IDF_PATH'] = os.environ['IDF_PATH']
    recipe_variables['REPORT_DIR'] = str(TMP_DIR_REPORT_PATH)

    dbg(f'Recipe variables: {recipe_variables}')
    dbg(f'Project directory: {project_dir}')
    dbg(f'Build directory: {build_dir}')
    dbg(f'System: {platform.system()}')

    if list_recipes:
        # List recipes command
        act_list_recipes(cmdl_recipes, cmdl_tags, append, recipe_variables)
        return

    if check_recipes:
        # Validate recipes command
        act_check_recipes(cmdl_recipes, cmdl_tags, append, recipe_variables)
        return

    recipes: Dict = {}

    if not output:
        output_dir_path = Path('idf-diag-{}'.format(uuid.uuid4())).expanduser()
    else:
        output_dir_path = Path(output).expanduser()

    info(f'Creating report in "{output_dir_path}" directory.')

    # Check output directory
    dbg(f'Using "{output_dir_path}" as report directory')

    try:
        output_dir_path_exists = output_dir_path.exists()
    except Exception:
        die(f'Cannot get report directory "{output_dir_path}" status')

    if output_dir_path_exists:
        if not output_dir_path.is_dir():
            die((f'Directory entry "{output_dir_path}" already exists and is not a directory. '
                 f'Please select a directory that does not exist or remove "{output_dir_path}" '
                 f'manually.'))
        if not force:
            die((f'Report directory "{output_dir_path}" already exists. '
                 f'Please select a directory that does not exist or use the "-f/--force" '
                 f'option to delete the existing "{output_dir_path}" directory.'))
        try:
            dbg(f'Removing existing report "{output_dir_path}" directory')
            shutil.rmtree(output_dir_path)
        except Exception:
            die(f'Cannot remove existing "{output_dir_path}" directory')

    # Get recipe files
    try:
        recipes = get_recipes(cmdl_recipes, cmdl_tags, append, recipe_variables)
    except Exception:
        die(f'Unable to create list of recipe files')

    # Validate recipes
    try:
        for recipe_file, recipe in recipes.items():
            dbg(f'Validating recipe file "{recipe_file}"')
            validate_recipe(recipe)
    except Exception:
        die(f'File "{recipe_file}" is not a valid diagnostic file')

    # Cook recipes
    try:
        for recipe_file, recipe in recipes.items():
            desc = recipe.get('description')
            dbg(f'Processing recipe "{desc} "file "{recipe_file}"')
            print(f'{desc}')
            process_recipe(recipe)
    except Exception:
        die(f'Cannot process diagnostic file "{recipe_file}"')

    dbg(f'Report is done.')

    global LOG_FILE
    if LOG_FILE:
        LOG_FILE.close()
        LOG_FILE = None

    try:
        redact_files(TMP_DIR_REPORT_PATH, TMP_DIR_REPORT_REDACTED_PATH)
    except Exception:
        err(f'The redaction was unsuccessful.')

    try:
        shutil.move(TMP_DIR_REPORT_REDACTED_PATH, output_dir_path)
    except Exception:
        die(f'Cannot move diagnostic report directory from "{TMP_DIR_REPORT_REDACTED_PATH}" to "{output_dir_path}"')

    info((f'The report has been created in the "{output_dir_path}" directory. '
          f'Please make sure to thoroughly check it for any sensitive information '
          f'before sharing and remove files you do not want to share. Kindly include '
          f'any additional files you find relevant that were not automatically added. '
          f'Please archive the contents of the final report directory using the command:\n'
          f'"idf.py diag --zip {output_dir_path}".'))


def action_extensions(base_actions: Dict, project_path: str) -> Any:
    return {
        'actions': {
            'diag': {
                'callback': create,
                'help': 'Create diagnostic report.',
                'options': [
                    {
                        'names': ['-d', '--debug'],
                        'is_flag': True,
                        'help': 'Print debug information, including exception tracebacks.',
                    },
                    {
                        'names': ['--no-color'],
                        'is_flag': True,
                        'help': 'Do not emit ANSI color codes.',
                    },
                    {
                        'names': ['--log-prefix'],
                        'is_flag': True,
                        'help': 'Add a severity character at the beginning of log messages.',
                    },
                    {
                        'names': ['-z', '--zip', 'zip_directory'],
                        'metavar': 'PATH',
                        'help': 'Create zip archive for diagnostic report in PATH.',
                    },
                    {
                        'names': ['-l', '--list', 'list_recipes'],
                        'is_flag': True,
                        'help': 'Show information about available recipes.',
                    },
                    {
                        'names': ['-c', '--check', 'check_recipes'],
                        'is_flag': True,
                        'help': 'Validate recipes.',
                    },
                    {
                        'names': ['-r', '--recipe', 'cmdl_recipes'],
                        'multiple': True,
                        'metavar': 'RECIPE',
                        'type': str,
                        'help': ('Recipe to use. This option can be specified multiple times. '
                                 'By default, all built-in recipes are used. RECIPE refers to '
                                 'the recipe file path or the file name stem for built-in recipes.'),
                    },
                    {
                        'names': ['-t', '--tag', 'cmdl_tags'],
                        'multiple': True,
                        'metavar': 'TAG',
                        'type': str,
                        'help': ('Consider only recipes containing TAG. This option can be specified '
                                 'multiple times. By default, all recipes are used. Use -l/--list-recipes '
                                 'option to see recipe TAG information.'),
                    },
                    {
                        'names': ['-a', '--append'],
                        'is_flag': True,
                        'help': ('Use recipes specified with the -r/--recipe option in '
                                 'combination with the built-in recipes.'),
                    },
                    {
                        'names': ['-f', '--force'],
                        'is_flag': True,
                        'help': 'Delete the target file or directory if it already exists before creating it.',
                    },
                    {
                        'names': ['-o', '--output'],
                        'metavar': 'PATH',
                        'type': str,
                        'help': ('Diagnostic report directory PATH or zip file archive PATH. '
                                 'If not specified, the report-UUID is used as the report directory, '
                                 'and the report directory specified with the --zip option with a zip '
                                 'extension is used for the zip file archive.')
                    },
                ],
            },
        },
    }
