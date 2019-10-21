#!/usr/bin/env python
#
# 'idf.py' is a top-level config/build command line tool for ESP-IDF
#
# You don't have to use idf.py, you can use cmake directly
# (or use cmake in an IDE)
#
#
#
# Copyright 2019 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# WARNING: we don't check for Python build-time dependencies until
# check_environment() function below. If possible, avoid importing
# any external libraries here - put in external script, or import in
# their specific function instead.
import codecs
import json
import locale
import multiprocessing
import os
import os.path
import re
import shutil
import subprocess
import sys


class FatalError(RuntimeError):
    """
    Wrapper class for runtime errors that aren't caused by bugs in idf.py or the build proces.s
    """

    pass


# Use this Python interpreter for any subprocesses we launch
PYTHON = sys.executable

# note: os.environ changes don't automatically propagate to child processes,
# you have to pass env=os.environ explicitly anywhere that we create a process
os.environ["PYTHON"] = sys.executable

# Name of the program, normally 'idf.py'.
# Can be overridden from idf.bat using IDF_PY_PROGRAM_NAME
PROG = os.getenv("IDF_PY_PROGRAM_NAME", sys.argv[0])

# Make flavors, across the various kinds of Windows environments & POSIX...
if "MSYSTEM" in os.environ:  # MSYS
    MAKE_CMD = "make"
    MAKE_GENERATOR = "MSYS Makefiles"
elif os.name == "nt":  # other Windows
    MAKE_CMD = "mingw32-make"
    MAKE_GENERATOR = "MinGW Makefiles"
else:
    MAKE_CMD = "make"
    MAKE_GENERATOR = "Unix Makefiles"

GENERATORS = [
    # ('generator name', 'build command line', 'version command line', 'verbose flag')
    ("Ninja", ["ninja"], ["ninja", "--version"], "-v"),
    (
        MAKE_GENERATOR,
        [MAKE_CMD, "-j", str(multiprocessing.cpu_count() + 2)],
        [MAKE_CMD, "--version"],
        "VERBOSE=1",
    ),
]
GENERATOR_CMDS = dict((a[0], a[1]) for a in GENERATORS)
GENERATOR_VERBOSE = dict((a[0], a[3]) for a in GENERATORS)


def _run_tool(tool_name, args, cwd):
    def quote_arg(arg):
        " Quote 'arg' if necessary "
        if " " in arg and not (arg.startswith('"') or arg.startswith("'")):
            return "'" + arg + "'"
        return arg

    display_args = " ".join(quote_arg(arg) for arg in args)
    print("Running %s in directory %s" % (tool_name, quote_arg(cwd)))
    print('Executing "%s"...' % str(display_args))
    try:
        # Note: we explicitly pass in os.environ here, as we may have set IDF_PATH there during startup
        subprocess.check_call(args, env=os.environ, cwd=cwd)
    except subprocess.CalledProcessError as e:
        raise FatalError("%s failed with exit code %d" % (tool_name, e.returncode))


def _realpath(path):
    """
    Return the cannonical path with normalized case.

    It is useful on Windows to comparision paths in case-insensitive manner.
    On Unix and Mac OS X it works as `os.path.realpath()` only.
    """
    return os.path.normcase(os.path.realpath(path))


def check_environment():
    """
    Verify the environment contains the top-level tools we need to operate

    (cmake will check a lot of other things)
    """
    if not executable_exists(["cmake", "--version"]):
        raise FatalError("'cmake' must be available on the PATH to use %s" % PROG)
    # find the directory idf.py is in, then the parent directory of this, and assume this is IDF_PATH
    detected_idf_path = _realpath(os.path.join(os.path.dirname(__file__), ".."))
    if "IDF_PATH" in os.environ:
        set_idf_path = _realpath(os.environ["IDF_PATH"])
        if set_idf_path != detected_idf_path:
            print(
                "WARNING: IDF_PATH environment variable is set to %s but %s path indicates IDF directory %s. "
                "Using the environment variable directory, but results may be unexpected..."
                % (set_idf_path, PROG, detected_idf_path)
            )
    else:
        print("Setting IDF_PATH environment variable: %s" % detected_idf_path)
        os.environ["IDF_PATH"] = detected_idf_path

    # check Python dependencies
    print("Checking Python dependencies...")
    try:
        subprocess.check_call(
            [
                os.environ["PYTHON"],
                os.path.join(
                    os.environ["IDF_PATH"], "tools", "check_python_dependencies.py"
                ),
            ],
            env=os.environ,
        )
    except subprocess.CalledProcessError:
        raise SystemExit(1)


def executable_exists(args):
    try:
        subprocess.check_output(args)
        return True
    except Exception:
        return False


def detect_cmake_generator():
    """
    Find the default cmake generator, if none was specified. Raises an exception if no valid generator is found.
    """
    for (generator, _, version_check, _) in GENERATORS:
        if executable_exists(version_check):
            return generator
    raise FatalError(
        "To use %s, either the 'ninja' or 'GNU make' build tool must be available in the PATH"
        % PROG
    )


def _strip_quotes(value, regexp=re.compile(r"^\"(.*)\"$|^'(.*)'$|^(.*)$")):
    """
    Strip quotes like CMake does during parsing cache entries
    """

    return [x for x in regexp.match(value).groups() if x is not None][0].rstrip()


def _new_cmakecache_entries(cache_path, new_cache_entries):
    if not os.path.exists(cache_path):
        return True

    if new_cache_entries:
        current_cache = parse_cmakecache(cache_path)

        for entry in new_cache_entries:
            key, value = entry.split("=", 1)
            current_value = current_cache.get(key, None)
            if current_value is None or _strip_quotes(value) != current_value:
                return True

    return False


def _ensure_build_directory(args, always_run_cmake=False):
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
            raise FatalError("Project directory %s does not exist" % project_dir)
        else:
            raise FatalError("%s must be a project directory" % project_dir)
    if not os.path.exists(os.path.join(project_dir, "CMakeLists.txt")):
        raise FatalError(
            "CMakeLists.txt not found in project directory %s" % project_dir
        )

    # Verify/create the build directory
    build_dir = args.build_dir
    if not os.path.isdir(build_dir):
        os.makedirs(build_dir)
    cache_path = os.path.join(build_dir, "CMakeCache.txt")

    args.define_cache_entry = list(args.define_cache_entry)
    args.define_cache_entry.append("CCACHE_ENABLE=%d" % args.ccache)

    if always_run_cmake or _new_cmakecache_entries(cache_path, args.define_cache_entry):
        if args.generator is None:
            args.generator = detect_cmake_generator()
        try:
            cmake_args = [
                "cmake",
                "-G",
                args.generator,
                "-DPYTHON_DEPS_CHECKED=1",
                "-DESP_PLATFORM=1",
            ]
            if not args.no_warnings:
                cmake_args += ["--warn-uninitialized"]

            if args.define_cache_entry:
                cmake_args += ["-D" + d for d in args.define_cache_entry]
            cmake_args += [project_dir]

            _run_tool("cmake", cmake_args, cwd=args.build_dir)
        except Exception:
            # don't allow partially valid CMakeCache.txt files,
            # to keep the "should I run cmake?" logic simple
            if os.path.exists(cache_path):
                os.remove(cache_path)
            raise

    # Learn some things from the CMakeCache.txt file in the build directory
    cache = parse_cmakecache(cache_path)
    try:
        generator = cache["CMAKE_GENERATOR"]
    except KeyError:
        generator = detect_cmake_generator()
    if args.generator is None:
        args.generator = (
            generator
        )  # reuse the previously configured generator, if none was given
    if generator != args.generator:
        raise FatalError(
            "Build is configured for generator '%s' not '%s'. Run '%s fullclean' to start again."
            % (generator, args.generator, PROG)
        )

    try:
        home_dir = cache["CMAKE_HOME_DIRECTORY"]
        if _realpath(home_dir) != _realpath(project_dir):
            raise FatalError(
                "Build directory '%s' configured for project '%s' not '%s'. Run '%s fullclean' to start again."
                % (build_dir, _realpath(home_dir), _realpath(project_dir), PROG)
            )
    except KeyError:
        pass  # if cmake failed part way, CMAKE_HOME_DIRECTORY may not be set yet


def parse_cmakecache(path):
    """
    Parse the CMakeCache file at 'path'.

    Returns a dict of name:value.

    CMakeCache entries also each have a "type", but this is currently ignored.
    """
    result = {}
    with open(path) as f:
        for line in f:
            # cmake cache lines look like: CMAKE_CXX_FLAGS_DEBUG:STRING=-g
            # groups are name, type, value
            m = re.match(r"^([^#/:=]+):([^:=]+)=(.*)\n$", line)
            if m:
                result[m.group(1)] = m.group(3)
    return result


def build_target(target_name, ctx, args):
    """
    Execute the target build system to build target 'target_name'

    Calls _ensure_build_directory() which will run cmake to generate a build
    directory (with the specified generator) as needed.
    """
    _ensure_build_directory(args)
    generator_cmd = GENERATOR_CMDS[args.generator]

    if args.ccache:
        # Setting CCACHE_BASEDIR & CCACHE_NO_HASHDIR ensures that project paths aren't stored in the ccache entries
        # (this means ccache hits can be shared between different projects. It may mean that some debug information
        # will point to files in another project, if these files are perfect duplicates of each other.)
        #
        # It would be nicer to set these from cmake, but there's no cross-platform way to set build-time environment
        # os.environ["CCACHE_BASEDIR"] = args.build_dir
        # os.environ["CCACHE_NO_HASHDIR"] = "1"
        pass
    if args.verbose:
        generator_cmd += [GENERATOR_VERBOSE[args.generator]]

    _run_tool(generator_cmd[0], generator_cmd + [target_name], args.build_dir)


def _get_esptool_args(args):
    esptool_path = os.path.join(
        os.environ["IDF_PATH"], "components/esptool_py/esptool/esptool.py"
    )
    if args.port is None:
        args.port = get_default_serial_port()
    result = [PYTHON, esptool_path]
    result += ["-p", args.port]
    result += ["-b", str(args.baud)]

    with open(os.path.join(args.build_dir, "flasher_args.json")) as f:
        flasher_args = json.load(f)

    extra_esptool_args = flasher_args["extra_esptool_args"]
    result += ["--after", extra_esptool_args["after"]]
    return result


def flash(action, ctx, args):
    """
    Run esptool to flash the entire project, from an argfile generated by the build system
    """
    flasher_args_path = {  # action -> name of flasher args file generated by build system
        "bootloader-flash": "flash_bootloader_args",
        "partition_table-flash": "flash_partition_table_args",
        "app-flash": "flash_app_args",
        "flash": "flash_project_args",
        "encrypted-app-flash": "flash_encrypted_app_args",
        "encrypted-flash": "flash_encrypted_project_args",
    }[
        action
    ]
    esptool_args = _get_esptool_args(args)
    esptool_args += ["write_flash", "@" + flasher_args_path]
    _run_tool("esptool.py", esptool_args, args.build_dir)


def erase_flash(action, ctx, args):
    esptool_args = _get_esptool_args(args)
    esptool_args += ["erase_flash"]
    _run_tool("esptool.py", esptool_args, args.build_dir)


def monitor(action, ctx, args, print_filter):
    """
    Run idf_monitor.py to watch build output
    """
    if args.port is None:
        args.port = get_default_serial_port()
    desc_path = os.path.join(args.build_dir, "project_description.json")
    if not os.path.exists(desc_path):
        _ensure_build_directory(args)
    with open(desc_path, "r") as f:
        project_desc = json.load(f)

    elf_file = os.path.join(args.build_dir, project_desc["app_elf"])
    if not os.path.exists(elf_file):
        raise FatalError(
            "ELF file '%s' not found. You need to build & flash the project before running 'monitor', "
            "and the binary on the device must match the one in the build directory exactly. "
            "Try '%s flash monitor'." % (elf_file, PROG)
        )
    idf_monitor = os.path.join(os.environ["IDF_PATH"], "tools/idf_monitor.py")
    monitor_args = [PYTHON, idf_monitor]
    if args.port is not None:
        monitor_args += ["-p", args.port]
    monitor_args += ["-b", project_desc["monitor_baud"]]
    if print_filter is not None:
        monitor_args += ["--print_filter", print_filter]
    monitor_args += [elf_file]

    idf_py = [PYTHON] + get_commandline_options(ctx)  # commands to re-run idf.py
    monitor_args += ["-m", " ".join("'%s'" % a for a in idf_py)]

    if "MSYSTEM" in os.environ:
        monitor_args = ["winpty"] + monitor_args
    _run_tool("idf_monitor", monitor_args, args.project_dir)


def clean(action, ctx, args):
    if not os.path.isdir(args.build_dir):
        print("Build directory '%s' not found. Nothing to clean." % args.build_dir)
        return
    build_target("clean", ctx, args)


def reconfigure(action, ctx, args):
    _ensure_build_directory(args, True)


def _delete_windows_symlinks(directory):
    """
    It deletes symlinks recursively on Windows. It is useful for Python 2 which doesn't detect symlinks on Windows.
    """
    deleted_paths = []
    if os.name == "nt":
        import ctypes

        for root, dirnames, _filenames in os.walk(directory):
            for d in dirnames:
                full_path = os.path.join(root, d)
                try:
                    full_path = full_path.decode("utf-8")
                except Exception:
                    pass
                if ctypes.windll.kernel32.GetFileAttributesW(full_path) & 0x0400:
                    os.rmdir(full_path)
                    deleted_paths.append(full_path)
    return deleted_paths


def fullclean(action, ctx, args):
    build_dir = args.build_dir
    if not os.path.isdir(build_dir):
        print("Build directory '%s' not found. Nothing to clean." % build_dir)
        return
    if len(os.listdir(build_dir)) == 0:
        print("Build directory '%s' is empty. Nothing to clean." % build_dir)
        return

    if not os.path.exists(os.path.join(build_dir, "CMakeCache.txt")):
        raise FatalError(
            "Directory '%s' doesn't seem to be a CMake build directory. Refusing to automatically "
            "delete files in this directory. Delete the directory manually to 'clean' it."
            % build_dir
        )
    red_flags = ["CMakeLists.txt", ".git", ".svn"]
    for red in red_flags:
        red = os.path.join(build_dir, red)
        if os.path.exists(red):
            raise FatalError(
                "Refusing to automatically delete files in directory containing '%s'. Delete files manually if you're sure."
                % red
            )
    # OK, delete everything in the build directory...
    # Note: Python 2.7 doesn't detect symlinks on Windows (it is supported form 3.2). Tools promising to not
    # follow symlinks will actually follow them. Deleting the build directory with symlinks deletes also items
    # outside of this directory.
    deleted_symlinks = _delete_windows_symlinks(build_dir)
    if args.verbose and len(deleted_symlinks) > 1:
        print(
            "The following symlinks were identified and removed:\n%s"
            % "\n".join(deleted_symlinks)
        )
    for f in os.listdir(
        build_dir
    ):  # TODO: once we are Python 3 only, this can be os.scandir()
        f = os.path.join(build_dir, f)
        if args.verbose:
            print("Removing: %s" % f)
        if os.path.isdir(f):
            shutil.rmtree(f)
        else:
            os.remove(f)


def _safe_relpath(path, start=None):
    """ Return a relative path, same as os.path.relpath, but only if this is possible.

    It is not possible on Windows, if the start directory and the path are on different drives.
    """
    try:
        return os.path.relpath(path, os.curdir if start is None else start)
    except ValueError:
        return os.path.abspath(path)


def get_commandline_options(ctx):
    """ Return all the command line options up to first action """
    # This approach ignores argument parsing done Click
    result = []

    for arg in sys.argv:
        if arg in ctx.command.commands_with_aliases:
            break

        result.append(arg)

    return result


def get_default_serial_port():
    """ Return a default serial port. esptool can do this (smarter), but it can create
    inconsistencies where esptool.py uses one port and idf_monitor uses another.

    Same logic as esptool.py search order, reverse sort by name and choose the first port.
    """
    # Import is done here in order to move it after the check_environment() ensured that pyserial has been installed
    import serial.tools.list_ports

    ports = list(reversed(sorted(p.device for p in serial.tools.list_ports.comports())))
    try:
        print(
            "Choosing default port %s (use '-p PORT' option to set a specific serial port)"
            % ports[0].encode("ascii", "ignore")
        )
        return ports[0]
    except IndexError:
        raise RuntimeError(
            "No serial ports found. Connect a device, or use '-p PORT' option to set a specific port."
        )


class PropertyDict(dict):
    def __getattr__(self, name):
        if name in self:
            return self[name]
        else:
            raise AttributeError("'PropertyDict' object has no attribute '%s'" % name)

    def __setattr__(self, name, value):
        self[name] = value

    def __delattr__(self, name):
        if name in self:
            del self[name]
        else:
            raise AttributeError("'PropertyDict' object has no attribute '%s'" % name)


def init_cli():
    # Click is imported here to run it after check_environment()
    import click

    class Task(object):
        def __init__(
            self, callback, name, aliases, dependencies, order_dependencies, action_args
        ):
            self.callback = callback
            self.name = name
            self.dependencies = dependencies
            self.order_dependencies = order_dependencies
            self.action_args = action_args
            self.aliases = aliases

        def run(self, context, global_args, action_args=None):
            if action_args is None:
                action_args = self.action_args

            self.callback(self.name, context, global_args, **action_args)

    class Action(click.Command):
        def __init__(
            self,
            name=None,
            aliases=None,
            dependencies=None,
            order_dependencies=None,
            **kwargs
        ):
            super(Action, self).__init__(name, **kwargs)

            self.name = self.name or self.callback.__name__

            if aliases is None:
                aliases = []
            self.aliases = aliases

            self.help = self.help or self.callback.__doc__
            if self.help is None:
                self.help = ""

            if dependencies is None:
                dependencies = []

            if order_dependencies is None:
                order_dependencies = []

            # Show first line of help if short help is missing
            self.short_help = self.short_help or self.help.split("\n")[0]

            # Add aliases to help string
            if aliases:
                aliases_help = "Aliases: %s." % ", ".join(aliases)

                self.help = "\n".join([self.help, aliases_help])
                self.short_help = " ".join([aliases_help, self.short_help])

            if self.callback is not None:
                callback = self.callback

                def wrapped_callback(**action_args):
                    return Task(
                        callback=callback,
                        name=self.name,
                        dependencies=dependencies,
                        order_dependencies=order_dependencies,
                        action_args=action_args,
                        aliases=self.aliases,
                    )

                self.callback = wrapped_callback

    class Argument(click.Argument):
        """Positional argument"""

        def __init__(self, **kwargs):
            names = kwargs.pop("names")
            super(Argument, self).__init__(names, **kwargs)

    class Scope(object):
        """
            Scope for sub-command option.
            possible values:
            - default - only available on defined level (global/action)
            - global - When defined for action, also available as global
            - shared - Opposite to 'global': when defined in global scope, also available for all actions
        """

        SCOPES = ("default", "global", "shared")

        def __init__(self, scope=None):
            if scope is None:
                self._scope = "default"
            elif isinstance(scope, str) and scope in self.SCOPES:
                self._scope = scope
            elif isinstance(scope, Scope):
                self._scope = str(scope)
            else:
                raise FatalError("Unknown scope for option: %s" % scope)

        @property
        def is_global(self):
            return self._scope == "global"

        @property
        def is_shared(self):
            return self._scope == "shared"

        def __str__(self):
            return self._scope

    class Option(click.Option):
        """Option that knows whether it should be global"""

        def __init__(self, scope=None, **kwargs):
            kwargs["param_decls"] = kwargs.pop("names")
            super(Option, self).__init__(**kwargs)

            self.scope = Scope(scope)

            if self.scope.is_global:
                self.help += " This option can be used at most once either globally, or for one subcommand."

    class CLI(click.MultiCommand):
        """Action list contains all actions with options available for CLI"""

        def __init__(self, action_lists=None, help=None):
            super(CLI, self).__init__(
                chain=True,
                invoke_without_command=True,
                result_callback=self.execute_tasks,
                context_settings={"max_content_width": 140},
                help=help,
            )
            self._actions = {}
            self.global_action_callbacks = []
            self.commands_with_aliases = {}

            if action_lists is None:
                action_lists = []

            shared_options = []

            for action_list in action_lists:
                # Global options
                for option_args in action_list.get("global_options", []):
                    option = Option(**option_args)
                    self.params.append(option)

                    if option.scope.is_shared:
                        shared_options.append(option)

            for action_list in action_lists:
                # Global options validators
                self.global_action_callbacks.extend(
                    action_list.get("global_action_callbacks", [])
                )

            for action_list in action_lists:
                # Actions
                for name, action in action_list.get("actions", {}).items():
                    arguments = action.pop("arguments", [])
                    options = action.pop("options", [])

                    if arguments is None:
                        arguments = []

                    if options is None:
                        options = []

                    self._actions[name] = Action(name=name, **action)
                    for alias in [name] + action.get("aliases", []):
                        self.commands_with_aliases[alias] = name

                    for argument_args in arguments:
                        self._actions[name].params.append(Argument(**argument_args))

                    # Add all shared options
                    for option in shared_options:
                        self._actions[name].params.append(option)

                    for option_args in options:
                        option = Option(**option_args)

                        if option.scope.is_shared:
                            raise FatalError(
                                '"%s" is defined for action "%s". '
                                ' "shared" options can be declared only on global level' % (option.name, name)
                            )

                        # Promote options to global if see for the first time
                        if option.scope.is_global and option.name not in [o.name for o in self.params]:
                            self.params.append(option)

                        self._actions[name].params.append(option)

        def list_commands(self, ctx):
            return sorted(self._actions)

        def get_command(self, ctx, name):
            return self._actions.get(self.commands_with_aliases.get(name))

        def _print_closing_message(self, args, actions):
            # print a closing message of some kind
            #
            if "flash" in str(actions):
                print("Done")
                return

            # Otherwise, if we built any binaries print a message about
            # how to flash them
            def print_flashing_message(title, key):
                print("\n%s build complete. To flash, run this command:" % title)

                with open(os.path.join(args.build_dir, "flasher_args.json")) as f:
                    flasher_args = json.load(f)

                def flasher_path(f):
                    return _safe_relpath(os.path.join(args.build_dir, f))

                if key != "project":  # flashing a single item
                    cmd = ""
                    if (
                        key == "bootloader"
                    ):  # bootloader needs --flash-mode, etc to be passed in
                        cmd = " ".join(flasher_args["write_flash_args"]) + " "

                    cmd += flasher_args[key]["offset"] + " "
                    cmd += flasher_path(flasher_args[key]["file"])
                else:  # flashing the whole project
                    cmd = " ".join(flasher_args["write_flash_args"]) + " "
                    flash_items = sorted(
                        (
                            (o, f)
                            for (o, f) in flasher_args["flash_files"].items()
                            if len(o) > 0
                        ),
                        key=lambda x: int(x[0], 0),
                    )
                    for o, f in flash_items:
                        cmd += o + " " + flasher_path(f) + " "

                print(
                    "%s -p %s -b %s --after %s write_flash %s"
                    % (
                        _safe_relpath(
                            "%s/components/esptool_py/esptool/esptool.py"
                            % os.environ["IDF_PATH"]
                        ),
                        args.port or "(PORT)",
                        args.baud,
                        flasher_args["extra_esptool_args"]["after"],
                        cmd.strip(),
                    )
                )
                print(
                    "or run 'idf.py -p %s %s'"
                    % (
                        args.port or "(PORT)",
                        key + "-flash" if key != "project" else "flash",
                    )
                )

            if "all" in actions or "build" in actions:
                print_flashing_message("Project", "project")
            else:
                if "app" in actions:
                    print_flashing_message("App", "app")
                if "partition_table" in actions:
                    print_flashing_message("Partition Table", "partition_table")
                if "bootloader" in actions:
                    print_flashing_message("Bootloader", "bootloader")

        def execute_tasks(self, tasks, **kwargs):
            ctx = click.get_current_context()
            global_args = PropertyDict(ctx.params)

            # Set propagated global options
            for task in tasks:
                for key in list(task.action_args):
                    option = next((o for o in ctx.command.params if o.name == key), None)
                    if option and (option.scope.is_global or option.scope.is_shared):
                        local_value = task.action_args.pop(key)
                        global_value = global_args[key]
                        default = () if option.multiple else option.default

                        if global_value != default and local_value != default and global_value != local_value:
                            raise FatalError(
                                'Option "%s" provided for "%s" is already defined to a different value. '
                                "This option can appear at most once in the command line." % (key, task.name)
                            )
                        if local_value != default:
                            global_args[key] = local_value

            # Validate global arguments
            for action_callback in ctx.command.global_action_callbacks:
                action_callback(ctx, global_args, tasks)

            # very simple dependency management
            completed_tasks = set()

            if not tasks:
                print(ctx.get_help())
                ctx.exit()

            while tasks:
                task = tasks[0]
                tasks_dict = dict([(t.name, t) for t in tasks])

                name_with_aliases = task.name
                if task.aliases:
                    name_with_aliases += " (aliases: %s)" % ", ".join(task.aliases)

                ready_to_run = True
                for dep in task.dependencies:
                    if dep not in completed_tasks:
                        print(
                            'Adding %s\'s dependency "%s" to list of actions'
                            % (task.name, dep)
                        )
                        dep_task = ctx.invoke(ctx.command.get_command(ctx, dep))

                        # Remove global options from dependent tasks
                        for key in list(dep_task.action_args):
                            option = next((o for o in ctx.command.params if o.name == key), None)
                            if option and (option.scope.is_global or option.scope.is_shared):
                                dep_task.action_args.pop(key)

                        tasks.insert(0, dep_task)
                        ready_to_run = False

                for dep in task.order_dependencies:
                    if dep in tasks_dict.keys() and dep not in completed_tasks:
                        tasks.insert(0, tasks.pop(tasks.index(tasks_dict[dep])))
                        ready_to_run = False

                if ready_to_run:
                    tasks.pop(0)

                    if task.name in completed_tasks:
                        print(
                            "Skipping action that is already done: %s"
                            % name_with_aliases
                        )
                    else:
                        print("Executing action: %s" % name_with_aliases)
                        task.run(ctx, global_args, task.action_args)

                    completed_tasks.add(task.name)

            self._print_closing_message(global_args, completed_tasks)

        @staticmethod
        def merge_action_lists(*action_lists):
            merged_actions = {
                "global_options": [],
                "actions": {},
                "global_action_callbacks": [],
            }
            for action_list in action_lists:
                merged_actions["global_options"].extend(
                    action_list.get("global_options", [])
                )
                merged_actions["actions"].update(action_list.get("actions", {}))
                merged_actions["global_action_callbacks"].extend(
                    action_list.get("global_action_callbacks", [])
                )
            return merged_actions

    # That's a tiny parser that parse project-dir even before constructing
    # fully featured click parser to be sure that extensions are loaded from the right place
    @click.command(
        add_help_option=False,
        context_settings={"allow_extra_args": True, "ignore_unknown_options": True},
    )
    @click.option("-C", "--project-dir", default=os.getcwd())
    def parse_project_dir(project_dir):
        return _realpath(project_dir)

    project_dir = parse_project_dir(standalone_mode=False)

    # Load base idf commands
    def validate_root_options(ctx, args, tasks):
        args.project_dir = _realpath(args.project_dir)
        if args.build_dir is not None and args.project_dir == _realpath(args.build_dir):
            raise FatalError(
                "Setting the build directory to the project directory is not supported. Suggest dropping "
                "--build-dir option, the default is a 'build' subdirectory inside the project directory."
            )
        if args.build_dir is None:
            args.build_dir = os.path.join(args.project_dir, "build")
        args.build_dir = _realpath(args.build_dir)

    # Possible keys for action dict are: global_options, actions and global_action_callbacks
    global_options = [
        {
            "names": ["-D", "--define-cache-entry"],
            "help": "Create a cmake cache entry.",
            "scope": "global",
            "multiple": True,
        }
    ]

    root_options = {
        "global_options": [
            {
                "names": ["-C", "--project-dir"],
                "help": "Project directory.",
                "type": click.Path(),
                "default": os.getcwd(),
            },
            {
                "names": ["-B", "--build-dir"],
                "help": "Build directory.",
                "type": click.Path(),
                "default": None,
            },
            {
                "names": ["-n", "--no-warnings"],
                "help": "Disable Cmake warnings.",
                "is_flag": True,
                "default": False,
            },
            {
                "names": ["-v", "--verbose"],
                "help": "Verbose build output.",
                "is_flag": True,
                "default": False,
            },
            {
                "names": ["--ccache/--no-ccache"],
                "help": "Use ccache in build. Disabled by default.",
                "is_flag": True,
                "default": False,
            },
            {
                "names": ["-G", "--generator"],
                "help": "CMake generator.",
                "type": click.Choice(GENERATOR_CMDS.keys()),
            },
        ],
        "global_action_callbacks": [validate_root_options],
    }

    build_actions = {
        "actions": {
            "all": {
                "aliases": ["build"],
                "callback": build_target,
                "short_help": "Build the project.",
                "help": "Build the project. This can involve multiple steps:\n\n"
                + "1. Create the build directory if needed. The sub-directory 'build' is used to hold build output, "
                + "although this can be changed with the -B option.\n\n"
                + "2. Run CMake as necessary to configure the project and generate build files for the main build tool.\n\n"
                + "3. Run the main build tool (Ninja or GNU Make). By default, the build tool is automatically detected "
                + "but it can be explicitly set by passing the -G option to idf.py.\n\n",
                "options": global_options,
                "order_dependencies": [
                    "reconfigure",
                    "menuconfig",
                    "clean",
                    "fullclean",
                ],
            },
            "menuconfig": {
                "callback": build_target,
                "help": 'Run "menuconfig" project configuration tool.',
                "options": global_options,
            },
            "confserver": {
                "callback": build_target,
                "help": "Run JSON configuration server.",
                "options": global_options,
            },
            "size": {
                "callback": build_target,
                "help": "Print basic size information about the app.",
                "options": global_options,
                "dependencies": ["app"],
            },
            "size-components": {
                "callback": build_target,
                "help": "Print per-component size information.",
                "options": global_options,
                "dependencies": ["app"],
            },
            "size-files": {
                "callback": build_target,
                "help": "Print per-source-file size information.",
                "options": global_options,
                "dependencies": ["app"],
            },
            "bootloader": {
                "callback": build_target,
                "help": "Build only bootloader.",
                "options": global_options,
            },
            "app": {
                "callback": build_target,
                "help": "Build only the app.",
                "order_dependencies": ["clean", "fullclean", "reconfigure"],
                "options": global_options,
            },
            "efuse_common_table": {
                "callback": build_target,
                "help": "Genereate C-source for IDF's eFuse fields.",
                "order_dependencies": ["reconfigure"],
                "options": global_options,
            },
            "efuse_custom_table": {
                "callback": build_target,
                "help": "Genereate C-source for user's eFuse fields.",
                "order_dependencies": ["reconfigure"],
                "options": global_options,
            },
            "show_efuse_table": {
                "callback": build_target,
                "help": "Print eFuse table.",
                "order_dependencies": ["reconfigure"],
                "options": global_options,
            },
            "partition_table": {
                "callback": build_target,
                "help": "Build only partition table.",
                "order_dependencies": ["reconfigure"],
                "options": global_options,
            },
            "erase_otadata": {
                "callback": build_target,
                "help": "Erase otadata partition.",
                "options": global_options,
            },
            "read_otadata": {
                "callback": build_target,
                "help": "Read otadata partition.",
                "options": global_options,
            },
        }
    }

    clean_actions = {
        "actions": {
            "reconfigure": {
                "callback": reconfigure,
                "short_help": "Re-run CMake.",
                "help": "Re-run CMake even if it doesn't seem to need re-running. This isn't necessary during normal usage, "
                + "but can be useful after adding/removing files from the source tree, or when modifying CMake cache variables. "
                + "For example, \"idf.py -DNAME='VALUE' reconfigure\" "
                + 'can be used to set variable "NAME" in CMake cache to value "VALUE".',
                "options": global_options,
                "order_dependencies": ["menuconfig"],
            },
            "clean": {
                "callback": clean,
                "short_help": "Delete build output files from the build directory.",
                "help": "Delete build output files from the build directory , forcing a 'full rebuild' the next time "
                + "the project is built. Cleaning doesn't delete CMake configuration output and some other files",
                "order_dependencies": ["fullclean"],
            },
            "fullclean": {
                "callback": fullclean,
                "short_help": "Delete the entire build directory contents.",
                "help": "Delete the entire build directory contents. This includes all CMake configuration output."
                + "The next time the project is built, CMake will configure it from scratch. "
                + "Note that this option recursively deletes all files in the build directory, so use with care."
                + "Project configuration is not deleted.",
            },
        }
    }

    baud_rate = {
        "names": ["-b", "--baud"],
        "help": "Baud rate.",
        "scope": "global",
        "envvar": "ESPBAUD",
        "default": 460800,
    }

    port = {
        "names": ["-p", "--port"],
        "help": "Serial port.",
        "scope": "global",
        "envvar": "ESPPORT",
        "default": None,
    }

    serial_actions = {
        "actions": {
            "flash": {
                "callback": flash,
                "help": "Flash the project.",
                "options": global_options + [baud_rate, port],
                "dependencies": ["all"],
                "order_dependencies": ["erase_flash"],
            },
            "erase_flash": {
                "callback": erase_flash,
                "help": "Erase entire flash chip.",
                "options": [baud_rate, port],
            },
            "monitor": {
                "callback": monitor,
                "help": "Display serial output.",
                "options": [
                    port,
                    {
                        "names": ["--print-filter", "--print_filter"],
                        "help": (
                            "Filter monitor output.\n"
                            "Restrictions on what to print can be specified as a series of <tag>:<log_level> items "
                            "where <tag> is the tag string and <log_level> is a character from the set "
                            "{N, E, W, I, D, V, *} referring to a level. "
                            'For example, "tag1:W" matches and prints only the outputs written with '
                            'ESP_LOGW("tag1", ...) or at lower verbosity level, i.e. ESP_LOGE("tag1", ...). '
                            'Not specifying a <log_level> or using "*" defaults to Verbose level.\n'
                            'Please see the IDF Monitor section of the ESP-IDF documentation '
                            'for a more detailed description and further examples.'),
                        "default": None,
                    },
                ],
                "order_dependencies": [
                    "flash",
                    "partition_table-flash",
                    "bootloader-flash",
                    "app-flash",
                ],
            },
            "partition_table-flash": {
                "callback": flash,
                "help": "Flash partition table only.",
                "options": [baud_rate, port],
                "dependencies": ["partition_table"],
                "order_dependencies": ["erase_flash"],
            },
            "bootloader-flash": {
                "callback": flash,
                "help": "Flash bootloader only.",
                "options": [baud_rate, port],
                "dependencies": ["bootloader"],
                "order_dependencies": ["erase_flash"],
            },
            "app-flash": {
                "callback": flash,
                "help": "Flash the app only.",
                "options": [baud_rate, port],
                "dependencies": ["app"],
                "order_dependencies": ["erase_flash"],
            },
            "encrypted-app-flash": {
                "callback": flash,
                "help": "Flash the encrypted app only.",
                "dependencies": ["app"],
                "order_dependencies": ["erase_flash"],
            },
            "encrypted-flash": {
                "callback": flash,
                "help": "Flash the encrypted project.",
                "dependencies": ["all"],
                "order_dependencies": ["erase_flash"],
            },
        },
    }

    base_actions = CLI.merge_action_lists(
        root_options, build_actions, clean_actions, serial_actions
    )
    all_actions = [base_actions]

    # Load extensions
    if os.path.exists(os.path.join(project_dir, "idf_ext.py")):
        sys.path.append(project_dir)
        try:
            from idf_ext import action_extensions
        except ImportError:
            print("Error importing extension file idf_ext.py. Skipping.")
            print(
                "Please make sure that it contains implementation (even if it's empty) of add_action_extensions"
            )

    # Add actions extensions
    try:
        all_actions.append(action_extensions(base_actions, project_dir))
    except NameError:
        pass

    return CLI(help="ESP-IDF build management", action_lists=all_actions)


def main():
    check_environment()
    cli = init_cli()
    cli(prog_name=PROG)


def _valid_unicode_config():
    # Python 2 is always good
    if sys.version_info[0] == 2:
        return True

    # With python 3 unicode environment is required
    try:
        return codecs.lookup(locale.getpreferredencoding()).name != "ascii"
    except Exception:
        return False


def _find_usable_locale():
    try:
        locales = subprocess.Popen(
            ["locale", "-a"], stdout=subprocess.PIPE, stderr=subprocess.PIPE
        ).communicate()[0]
    except OSError:
        locales = ""
    if isinstance(locales, bytes):
        locales = locales.decode("ascii", "replace")

    usable_locales = []
    for line in locales.splitlines():
        locale = line.strip()
        locale_name = locale.lower().replace("-", "")

        # C.UTF-8 is the best option, if supported
        if locale_name == "c.utf8":
            return locale

        if locale_name.endswith(".utf8"):
            # Make a preference of english locales
            if locale.startswith("en_"):
                usable_locales.insert(0, locale)
            else:
                usable_locales.append(locale)

    if not usable_locales:
        raise FatalError(
            "Support for Unicode filenames is required, but no suitable UTF-8 locale was found on your system."
            " Please refer to the manual for your operating system for details on locale reconfiguration."
        )

    return usable_locales[0]


if __name__ == "__main__":
    try:
        # On MSYS2 we need to run idf.py with "winpty" in order to be able to cancel the subprocesses properly on
        # keyboard interrupt (CTRL+C).
        # Using an own global variable for indicating that we are running with "winpty" seems to be the most suitable
        # option as os.environment['_'] contains "winpty" only when it is run manually from console.
        WINPTY_VAR = "WINPTY"
        WINPTY_EXE = "winpty"
        if ("MSYSTEM" in os.environ) and (
            not os.environ.get("_", "").endswith(WINPTY_EXE) and WINPTY_VAR not in os.environ
        ):

            if 'menuconfig' in sys.argv:
                # don't use winpty for menuconfig because it will print weird characters
                main()
            else:
                os.environ[WINPTY_VAR] = "1"  # the value is of no interest to us
                # idf.py calls itself with "winpty" and WINPTY global variable set
                ret = subprocess.call(
                    [WINPTY_EXE, sys.executable] + sys.argv, env=os.environ
                )
                if ret:
                    raise SystemExit(ret)

        elif os.name == "posix" and not _valid_unicode_config():
            # Trying to find best utf-8 locale available on the system and restart python with it
            best_locale = _find_usable_locale()

            print(
                "Your environment is not configured to handle unicode filenames outside of ASCII range."
                " Environment variable LC_ALL is temporary set to %s for unicode support."
                % best_locale
            )

            os.environ["LC_ALL"] = best_locale
            ret = subprocess.call([sys.executable] + sys.argv, env=os.environ)
            if ret:
                raise SystemExit(ret)

        else:
            main()

    except FatalError as e:
        print(e)
        sys.exit(2)
