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
import os
import os.path
import subprocess
import sys
from collections import Counter, OrderedDict
from importlib import import_module
from pkgutil import iter_modules

from idf_py_actions.errors import FatalError
from idf_py_actions.tools import (executable_exists, idf_version, merge_action_lists, realpath)

# Use this Python interpreter for any subprocesses we launch
PYTHON = sys.executable

# note: os.environ changes don't automatically propagate to child processes,
# you have to pass env=os.environ explicitly anywhere that we create a process
os.environ["PYTHON"] = sys.executable

# Name of the program, normally 'idf.py'.
# Can be overridden from idf.bat using IDF_PY_PROGRAM_NAME
PROG = os.getenv("IDF_PY_PROGRAM_NAME", sys.argv[0])


def check_environment():
    """
    Verify the environment contains the top-level tools we need to operate

    (cmake will check a lot of other things)
    """
    checks_output = []

    if not executable_exists(["cmake", "--version"]):
        print_idf_version()
        raise FatalError("'cmake' must be available on the PATH to use %s" % PROG)

    # verify that IDF_PATH env variable is set
    # find the directory idf.py is in, then the parent directory of this, and assume this is IDF_PATH
    detected_idf_path = realpath(os.path.join(os.path.dirname(__file__), ".."))
    if "IDF_PATH" in os.environ:
        set_idf_path = realpath(os.environ["IDF_PATH"])
        if set_idf_path != detected_idf_path:
            print("WARNING: IDF_PATH environment variable is set to %s but %s path indicates IDF directory %s. "
                  "Using the environment variable directory, but results may be unexpected..." %
                  (set_idf_path, PROG, detected_idf_path))
    else:
        print("Setting IDF_PATH environment variable: %s" % detected_idf_path)
        os.environ["IDF_PATH"] = detected_idf_path

    # check Python dependencies
    checks_output.append("Checking Python dependencies...")
    try:
        out = subprocess.check_output(
            [
                os.environ["PYTHON"],
                os.path.join(os.environ["IDF_PATH"], "tools", "check_python_dependencies.py"),
            ],
            env=os.environ,
        )

        checks_output.append(out.decode('utf-8', 'ignore').strip())
    except subprocess.CalledProcessError as e:
        print(e.output.decode('utf-8', 'ignore'))
        print_idf_version()
        raise SystemExit(1)

    return checks_output


def _safe_relpath(path, start=None):
    """ Return a relative path, same as os.path.relpath, but only if this is possible.

    It is not possible on Windows, if the start directory and the path are on different drives.
    """
    try:
        return os.path.relpath(path, os.curdir if start is None else start)
    except ValueError:
        return os.path.abspath(path)


def print_idf_version():
    version = idf_version()
    if version:
        print("ESP-IDF %s" % version)
    else:
        print("ESP-IDF version unknown")


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


def init_cli(verbose_output=None):
    # Click is imported here to run it after check_environment()
    import click

    class DeprecationMessage(object):
        """Construct deprecation notice for help messages"""

        def __init__(self, deprecated=False):
            self.deprecated = deprecated
            self.since = None
            self.removed = None
            self.custom_message = ""

            if isinstance(deprecated, dict):
                self.custom_message = deprecated.get("message", "")
                self.since = deprecated.get("since", None)
                self.removed = deprecated.get("removed", None)
            elif isinstance(deprecated, str):
                self.custom_message = deprecated

        def full_message(self, type="Option"):
            return "%s is deprecated %sand will be removed in%s.%s" % (
                type, "since %s " % self.since if self.since else "", " %s" % self.removed
                if self.removed else " future versions", " %s" % self.custom_message if self.custom_message else "")

        def help(self, text, type="Option", separator=" "):
            text = text or ""
            return self.full_message(type) + separator + text if self.deprecated else text

        def short_help(self, text):
            text = text or ""
            return ("Deprecated! " + text) if self.deprecated else text

    def print_deprecation_warning(ctx):
        """Prints deprectation warnings for arguments in given context"""
        for option in ctx.command.params:
            default = () if option.multiple else option.default
            if isinstance(option, Option) and option.deprecated and ctx.params[option.name] != default:
                print("Warning: %s" % DeprecationMessage(option.deprecated).full_message('Option "%s"' % option.name))

    class Task(object):
        def __init__(self, callback, name, aliases, dependencies, order_dependencies, action_args):
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
        def __init__(self,
                     name=None,
                     aliases=None,
                     deprecated=False,
                     dependencies=None,
                     order_dependencies=None,
                     hidden=False,
                     **kwargs):
            super(Action, self).__init__(name, **kwargs)

            self.name = self.name or self.callback.__name__
            self.deprecated = deprecated
            self.hidden = hidden

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

            if deprecated:
                deprecation = DeprecationMessage(deprecated)
                self.short_help = deprecation.short_help(self.short_help)
                self.help = deprecation.help(self.help, type="Command", separator="\n")

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

        def invoke(self, ctx):
            if self.deprecated:
                print("Warning: %s" % DeprecationMessage(self.deprecated).full_message('Command "%s"' % self.name))
                self.deprecated = False  # disable Click's built-in deprecation handling

            # Print warnings for options
            print_deprecation_warning(ctx)
            return super(Action, self).invoke(ctx)

    class Argument(click.Argument):
        """
        Positional argument

        names - alias of 'param_decls'
        """

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

        def __init__(self, scope=None, deprecated=False, hidden=False, **kwargs):
            """
            Keyword arguments additional to Click's Option class:

            names - alias of 'param_decls'
            deprecated - marks option as deprecated. May be boolean, string (with custom deprecation message)
            or dict with optional keys:
                since: version of deprecation
                removed: version when option will be removed
                custom_message:  Additional text to deprecation warning
            """

            kwargs["param_decls"] = kwargs.pop("names")
            super(Option, self).__init__(**kwargs)

            self.deprecated = deprecated
            self.scope = Scope(scope)
            self.hidden = hidden

            if deprecated:
                deprecation = DeprecationMessage(deprecated)
                self.help = deprecation.help(self.help)

            if self.scope.is_global:
                self.help += " This option can be used at most once either globally, or for one subcommand."

        def get_help_record(self, ctx):
            # Backport "hidden" parameter to click 5.0
            if self.hidden:
                return

            return super(Option, self).get_help_record(ctx)

    class CLI(click.MultiCommand):
        """Action list contains all actions with options available for CLI"""

        def __init__(self, all_actions=None, verbose_output=None, help=None):
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

            if verbose_output is None:
                verbose_output = []

            self.verbose_output = verbose_output

            if all_actions is None:
                all_actions = {}

            shared_options = []

            # Global options
            for option_args in all_actions.get("global_options", []):
                option = Option(**option_args)
                self.params.append(option)

                if option.scope.is_shared:
                    shared_options.append(option)

            # Global options validators
            self.global_action_callbacks = all_actions.get("global_action_callbacks", [])

            # Actions
            for name, action in all_actions.get("actions", {}).items():
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
                        raise FatalError('"%s" is defined for action "%s". '
                                         ' "shared" options can be declared only on global level' % (option.name, name))

                    # Promote options to global if see for the first time
                    if option.scope.is_global and option.name not in [o.name for o in self.params]:
                        self.params.append(option)

                    self._actions[name].params.append(option)

        def list_commands(self, ctx):
            return sorted(filter(lambda name: not self._actions[name].hidden, self._actions))

        def get_command(self, ctx, name):
            return self._actions.get(self.commands_with_aliases.get(name))

        def _print_closing_message(self, args, actions):
            # print a closing message of some kind
            #
            if "flash" in str(actions):
                print("Done")
                return

            if not os.path.exists(os.path.join(args.build_dir, "flasher_args.json")):
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
                    if (key == "bootloader"):  # bootloader needs --flash-mode, etc to be passed in
                        cmd = " ".join(flasher_args["write_flash_args"]) + " "

                    cmd += flasher_args[key]["offset"] + " "
                    cmd += flasher_path(flasher_args[key]["file"])
                else:  # flashing the whole project
                    cmd = " ".join(flasher_args["write_flash_args"]) + " "
                    flash_items = sorted(
                        ((o, f) for (o, f) in flasher_args["flash_files"].items() if len(o) > 0),
                        key=lambda x: int(x[0], 0),
                    )
                    for o, f in flash_items:
                        cmd += o + " " + flasher_path(f) + " "

                print("%s %s -p %s -b %s --before %s --after %s write_flash %s" % (
                    PYTHON,
                    _safe_relpath("%s/components/esptool_py/esptool/esptool.py" % os.environ["IDF_PATH"]),
                    args.port or "(PORT)",
                    args.baud,
                    flasher_args["extra_esptool_args"]["before"],
                    flasher_args["extra_esptool_args"]["after"],
                    cmd.strip(),
                ))
                print("or run 'idf.py -p %s %s'" % (
                    args.port or "(PORT)",
                    key + "-flash" if key != "project" else "flash",
                ))

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
            global_args = PropertyDict(kwargs)

            # Show warning if some tasks are present several times in the list
            dupplicated_tasks = sorted(
                [item for item, count in Counter(task.name for task in tasks).items() if count > 1])
            if dupplicated_tasks:
                dupes = ", ".join('"%s"' % t for t in dupplicated_tasks)
                print("WARNING: Command%s found in the list of commands more than once. " %
                      ("s %s are" % dupes if len(dupplicated_tasks) > 1 else " %s is" % dupes) +
                      "Only first occurence will be executed.")

            # Set propagated global options.
            # These options may be set on one subcommand, but available in the list of global arguments
            for task in tasks:
                for key in list(task.action_args):
                    option = next((o for o in ctx.command.params if o.name == key), None)

                    if option and (option.scope.is_global or option.scope.is_shared):
                        local_value = task.action_args.pop(key)
                        global_value = global_args[key]
                        default = () if option.multiple else option.default

                        if global_value != default and local_value != default and global_value != local_value:
                            raise FatalError('Option "%s" provided for "%s" is already defined to a different value. '
                                             "This option can appear at most once in the command line." %
                                             (key, task.name))
                        if local_value != default:
                            global_args[key] = local_value

            # Show warnings about global arguments
            print_deprecation_warning(ctx)

            # Make sure that define_cache_entry is mutable list and can be modified in callbacks
            global_args.define_cache_entry = list(global_args.define_cache_entry)

            # Execute all global action callback - first from idf.py itself, then from extensions
            for action_callback in ctx.command.global_action_callbacks:
                action_callback(ctx, global_args, tasks)

            # Always show help when command is not provided
            if not tasks:
                print(ctx.get_help())
                ctx.exit()

            # Build full list of tasks to and deal with dependencies and order dependencies
            tasks_to_run = OrderedDict()
            while tasks:
                task = tasks[0]
                tasks_dict = dict([(t.name, t) for t in tasks])

                dependecies_processed = True

                # If task have some dependecies they have to be executed before the task.
                for dep in task.dependencies:
                    if dep not in tasks_to_run.keys():
                        # If dependent task is in the list of unprocessed tasks move to the front of the list
                        if dep in tasks_dict.keys():
                            dep_task = tasks.pop(tasks.index(tasks_dict[dep]))
                        # Otherwise invoke it with default set of options
                        # and put to the front of the list of unprocessed tasks
                        else:
                            print('Adding "%s"\'s dependency "%s" to list of commands with default set of options.' %
                                  (task.name, dep))
                            dep_task = ctx.invoke(ctx.command.get_command(ctx, dep))

                            # Remove options with global scope from invoke tasks because they are alread in global_args
                            for key in list(dep_task.action_args):
                                option = next((o for o in ctx.command.params if o.name == key), None)
                                if option and (option.scope.is_global or option.scope.is_shared):
                                    dep_task.action_args.pop(key)

                        tasks.insert(0, dep_task)
                        dependecies_processed = False

                # Order only dependencies are moved to the front of the queue if they present in command list
                for dep in task.order_dependencies:
                    if dep in tasks_dict.keys() and dep not in tasks_to_run.keys():
                        tasks.insert(0, tasks.pop(tasks.index(tasks_dict[dep])))
                        dependecies_processed = False

                if dependecies_processed:
                    # Remove task from list of unprocessed tasks
                    tasks.pop(0)

                    # And add to the queue
                    if task.name not in tasks_to_run.keys():
                        tasks_to_run.update([(task.name, task)])

            # Run all tasks in the queue
            # when global_args.dry_run is true idf.py works in idle mode and skips actual task execution
            if not global_args.dry_run:
                for task in tasks_to_run.values():
                    name_with_aliases = task.name
                    if task.aliases:
                        name_with_aliases += " (aliases: %s)" % ", ".join(task.aliases)

                    print("Executing action: %s" % name_with_aliases)
                    task.run(ctx, global_args, task.action_args)

                self._print_closing_message(global_args, tasks_to_run.keys())

            return tasks_to_run

    # That's a tiny parser that parse project-dir even before constructing
    # fully featured click parser to be sure that extensions are loaded from the right place
    @click.command(
        add_help_option=False,
        context_settings={
            "allow_extra_args": True,
            "ignore_unknown_options": True
        },
    )
    @click.option("-C", "--project-dir", default=os.getcwd())
    def parse_project_dir(project_dir):
        return realpath(project_dir)

    project_dir = parse_project_dir(standalone_mode=False)

    all_actions = {}
    # Load extensions from components dir
    idf_py_extensions_path = os.path.join(os.environ["IDF_PATH"], "tools", "idf_py_actions")
    extra_paths = os.environ.get("IDF_EXTRA_ACTIONS_PATH", "").split(';')
    extension_dirs = [idf_py_extensions_path] + extra_paths
    extensions = {}

    for directory in extension_dirs:
        if directory and not os.path.exists(directory):
            print('WARNING: Directroy with idf.py extensions doesn\'t exist:\n    %s' % directory)
            continue

        sys.path.append(directory)
        for _finder, name, _ispkg in sorted(iter_modules([directory])):
            if name.endswith('_ext'):
                extensions[name] = import_module(name)

    for name, extension in extensions.items():
        try:
            all_actions = merge_action_lists(all_actions, extension.action_extensions(all_actions, project_dir))
        except AttributeError:
            print('WARNING: Cannot load idf.py extension "%s"' % name)

    # Load extensions from project dir
    if os.path.exists(os.path.join(project_dir, "idf_ext.py")):
        sys.path.append(project_dir)
        try:
            from idf_ext import action_extensions
        except ImportError:
            print("Error importing extension file idf_ext.py. Skipping.")
            print("Please make sure that it contains implementation (even if it's empty) of add_action_extensions")

        try:
            all_actions = merge_action_lists(all_actions, action_extensions(all_actions, project_dir))
        except NameError:
            pass

    return CLI(help="ESP-IDF build management", verbose_output=verbose_output, all_actions=all_actions)


def main():
    checks_output = check_environment()
    cli = init_cli(verbose_output=checks_output)
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
        locales = subprocess.Popen(["locale", "-a"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0]
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
            " Please refer to the manual for your operating system for details on locale reconfiguration.")

    return usable_locales[0]


if __name__ == "__main__":
    try:
        # On MSYS2 we need to run idf.py with "winpty" in order to be able to cancel the subprocesses properly on
        # keyboard interrupt (CTRL+C).
        # Using an own global variable for indicating that we are running with "winpty" seems to be the most suitable
        # option as os.environment['_'] contains "winpty" only when it is run manually from console.
        WINPTY_VAR = "WINPTY"
        WINPTY_EXE = "winpty"
        if ("MSYSTEM" in os.environ) and (not os.environ.get("_", "").endswith(WINPTY_EXE)
                                          and WINPTY_VAR not in os.environ):

            if 'menuconfig' in sys.argv:
                # don't use winpty for menuconfig because it will print weird characters
                main()
            else:
                os.environ[WINPTY_VAR] = "1"  # the value is of no interest to us
                # idf.py calls itself with "winpty" and WINPTY global variable set
                ret = subprocess.call([WINPTY_EXE, sys.executable] + sys.argv, env=os.environ)
                if ret:
                    raise SystemExit(ret)

        elif os.name == "posix" and not _valid_unicode_config():
            # Trying to find best utf-8 locale available on the system and restart python with it
            best_locale = _find_usable_locale()

            print("Your environment is not configured to handle unicode filenames outside of ASCII range."
                  " Environment variable LC_ALL is temporary set to %s for unicode support." % best_locale)

            os.environ["LC_ALL"] = best_locale
            ret = subprocess.call([sys.executable] + sys.argv, env=os.environ)
            if ret:
                raise SystemExit(ret)

        else:
            main()

    except FatalError as e:
        print(e)
        sys.exit(2)
