#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#
# 'idf.py' is a top-level config/build command line tool for ESP-IDF
#
# You don't have to use idf.py, you can use cmake directly
# (or use cmake in an IDE)

# WARNING: we don't check for Python build-time dependencies until
# check_environment() function below. If possible, avoid importing
# any external libraries here - put in external script, or import in
# their specific function instead.
import codecs
import json
import locale
import os
import os.path
import shlex
import subprocess
import sys
from collections import Counter, OrderedDict, _OrderedDictKeysView
from importlib import import_module
from pkgutil import iter_modules
from typing import Any, Callable, Dict, List, Optional, Union

# pyc files remain in the filesystem when switching between branches which might raise errors for incompatible
# idf.py extensions. Therefore, pyc file generation is turned off:
sys.dont_write_bytecode = True

import python_version_checker  # noqa: E402

try:
    from idf_py_actions.errors import FatalError  # noqa: E402
    from idf_py_actions.tools import (PROG, SHELL_COMPLETE_RUN, SHELL_COMPLETE_VAR, PropertyDict,  # noqa: E402
                                      debug_print_idf_version, get_target, merge_action_lists, print_warning)
    if os.getenv('IDF_COMPONENT_MANAGER') != '0':
        from idf_component_manager import idf_extensions
except ImportError as e:
    # For example, importing click could cause this.
    print((f'Cannot import module "{e.name}". This usually means that "idf.py" was not '
           f'spawned within an ESP-IDF shell environment or the python virtual '
           f'environment used by "idf.py" is corrupted.\n'
           f'Please use idf.py only in an ESP-IDF shell environment. If problem persists, '
           f'please try to install ESP-IDF tools again as described in the Get Started guide.'),
          file=sys.stderr)

    sys.exit(1)

# Use this Python interpreter for any subprocesses we launch
PYTHON = sys.executable

# note: os.environ changes don't automatically propagate to child processes,
# you have to pass env=os.environ explicitly anywhere that we create a process
os.environ['PYTHON'] = sys.executable


def check_environment() -> List:
    """
    Verify the environment contains the top-level tools we need to operate

    (cmake will check a lot of other things)
    """
    checks_output = []

    # verify that IDF_PATH env variable is set
    # find the directory idf.py is in, then the parent directory of this, and assume this is IDF_PATH
    detected_idf_path = os.path.realpath(os.path.join(os.path.dirname(__file__), '..'))
    if 'IDF_PATH' in os.environ:
        set_idf_path = os.path.realpath(os.environ['IDF_PATH'])
        if set_idf_path != detected_idf_path:
            print_warning(
                'WARNING: IDF_PATH environment variable is set to %s but %s path indicates IDF directory %s. '
                'Using the environment variable directory, but results may be unexpected...' %
                (set_idf_path, PROG, detected_idf_path))
    else:
        print_warning('Setting IDF_PATH environment variable: %s' % detected_idf_path)
        os.environ['IDF_PATH'] = detected_idf_path

    try:
        # The Python compatibility check could have been done earlier (tools/detect_python.{sh,fish}) but PATH is
        # not set for import at that time. Even if the check would be done before, the same check needs to be done
        # here as well (for example one can call idf.py from a not properly set-up environment).
        python_version_checker.check()
    except RuntimeError as e:
        raise FatalError(e)

    # check Python dependencies
    checks_output.append('Checking Python dependencies...')
    try:
        out = subprocess.check_output(
            [
                os.environ['PYTHON'],
                os.path.join(os.environ['IDF_PATH'], 'tools', 'idf_tools.py'),
                'check-python-dependencies',
            ],
            env=os.environ,
        )

        checks_output.append(out.decode('utf-8', 'ignore').strip())
    except subprocess.CalledProcessError as e:
        print_warning(e.output.decode('utf-8', 'ignore'), stream=sys.stderr)
        debug_print_idf_version()
        raise SystemExit(1)

    # Check used Python interpreter
    checks_output.append('Checking used Python interpreter...')
    try:
        python_venv_path = os.environ['IDF_PYTHON_ENV_PATH']
        if python_venv_path and not sys.executable.startswith(python_venv_path):
            print_warning(f'WARNING: Python interpreter "{sys.executable}" used to start idf.py is not from installed venv "{python_venv_path}"')
    except KeyError:
        print_warning('WARNING: The IDF_PYTHON_ENV_PATH is missing in environmental variables!')

    return checks_output


def _safe_relpath(path: str, start: Optional[str]=None) -> str:
    """ Return a relative path, same as os.path.relpath, but only if this is possible.

    It is not possible on Windows, if the start directory and the path are on different drives.
    """
    try:
        return os.path.relpath(path, os.curdir if start is None else start)
    except ValueError:
        return os.path.abspath(path)


def init_cli(verbose_output: List=None) -> Any:
    # Click is imported here to run it after check_environment()
    import click

    class Deprecation(object):
        """Construct deprecation notice for help messages"""
        def __init__(self, deprecated: Union[Dict, str, bool]=False) -> None:
            self.deprecated = deprecated
            self.since = None
            self.removed = None
            self.exit_with_error = None
            self.custom_message = ''

            if isinstance(deprecated, dict):
                self.custom_message = deprecated.get('message', '')
                self.since = deprecated.get('since', None)
                self.removed = deprecated.get('removed', None)
                self.exit_with_error = deprecated.get('exit_with_error', None)
            elif isinstance(deprecated, str):
                self.custom_message = deprecated

        def full_message(self, type: str='Option') -> str:
            if self.exit_with_error:
                return '%s is deprecated %sand was removed%s.%s' % (
                    type,
                    'since %s ' % self.since if self.since else '',
                    ' in %s' % self.removed if self.removed else '',
                    ' %s' % self.custom_message if self.custom_message else '',
                )
            else:
                return '%s is deprecated %sand will be removed in%s.%s' % (
                    type,
                    'since %s ' % self.since if self.since else '',
                    ' %s' % self.removed if self.removed else ' future versions',
                    ' %s' % self.custom_message if self.custom_message else '',
                )

        def help(self, text: str, type: str='Option', separator: str=' ') -> str:
            text = text or ''
            return self.full_message(type) + separator + text if self.deprecated else text

        def short_help(self, text: str) -> str:
            text = text or ''
            return ('Deprecated! ' + text) if self.deprecated else text

    def check_deprecation(ctx: click.core.Context) -> None:
        """Prints deprecation warnings for arguments in given context"""
        for option in ctx.command.params:
            default = () if option.multiple else option.default
            if isinstance(option, Option) and option.deprecated and ctx.params[option.name] != default:
                deprecation = Deprecation(option.deprecated)
                if deprecation.exit_with_error:
                    raise FatalError('Error: %s' % deprecation.full_message('Option "%s"' % option.name))
                else:
                    print_warning('Warning: %s' % deprecation.full_message('Option "%s"' % option.name))

    class Task(object):
        def __init__(self, callback: Callable, name: str, aliases: List, dependencies: Optional[List],
                     order_dependencies: Optional[List], action_args: Dict) -> None:
            self.callback = callback
            self.name = name
            self.dependencies = dependencies
            self.order_dependencies = order_dependencies
            self.action_args = action_args
            self.aliases = aliases

        def __call__(self, context: click.core.Context, global_args: PropertyDict, action_args: Dict=None) -> None:
            if action_args is None:
                action_args = self.action_args

            self.callback(self.name, context, global_args, **action_args)

    class Action(click.Command):
        def __init__(
                self,
                name: Optional[str]=None,
                aliases: Optional[List]=None,
                deprecated: Union[Dict, str, bool]=False,
                dependencies: Optional[List]=None,
                order_dependencies: Optional[List]=None,
                hidden: bool=False,
                **kwargs: Any) -> None:
            super(Action, self).__init__(name, **kwargs)

            self.name: str = self.name or self.callback.__name__
            self.deprecated: Union[Dict, str, bool] = deprecated
            self.hidden: bool = hidden

            if aliases is None:
                aliases = []
            self.aliases = aliases

            self.help: str = self.help or self.callback.__doc__ or ''

            if dependencies is None:
                dependencies = []

            if order_dependencies is None:
                order_dependencies = []

            # Show first line of help if short help is missing
            self.short_help: str = self.short_help or self.help.split('\n')[0]

            if deprecated:
                deprecation = Deprecation(deprecated)
                self.short_help = deprecation.short_help(self.short_help)
                self.help = deprecation.help(self.help, type='Command', separator='\n')

            # Add aliases to help string
            if aliases:
                aliases_help = 'Aliases: %s.' % ', '.join(aliases)

                self.help = '\n'.join([self.help, aliases_help])
                self.short_help = ' '.join([aliases_help, self.short_help])

            self.unwrapped_callback = self.callback
            if self.callback is not None:

                def wrapped_callback(**action_args: Any) -> Task:
                    return Task(
                        callback=self.unwrapped_callback,
                        name=self.name,
                        dependencies=dependencies,
                        order_dependencies=order_dependencies,
                        action_args=action_args,
                        aliases=self.aliases,
                    )

                self.callback = wrapped_callback

        def invoke(self, ctx: click.core.Context) -> click.core.Context:
            if self.deprecated:
                deprecation = Deprecation(self.deprecated)
                message = deprecation.full_message('Command "%s"' % self.name)

                if deprecation.exit_with_error:
                    raise FatalError('Error: %s' % message)
                else:
                    print_warning('Warning: %s' % message)

                self.deprecated = False  # disable Click's built-in deprecation handling

            # Print warnings for options
            check_deprecation(ctx)
            return super(Action, self).invoke(ctx)

    class Argument(click.Argument):
        """
        Positional argument

        names - alias of 'param_decls'
        """
        def __init__(self, **kwargs: str):
            names = kwargs.pop('names')
            super(Argument, self).__init__(names, **kwargs)

    class Scope(object):
        """
            Scope for sub-command option.
            possible values:
            - default - only available on defined level (global/action)
            - global - When defined for action, also available as global
            - shared - Opposite to 'global': when defined in global scope, also available for all actions
        """

        SCOPES = ('default', 'global', 'shared')

        def __init__(self, scope: Union['Scope', str]=None) -> None:  # noqa: F821
            if scope is None:
                self._scope = 'default'
            elif isinstance(scope, str) and scope in self.SCOPES:
                self._scope = scope
            elif isinstance(scope, Scope):
                self._scope = str(scope)
            else:
                raise FatalError('Unknown scope for option: %s' % scope)

        @property
        def is_global(self) -> bool:
            return self._scope == 'global'

        @property
        def is_shared(self) -> bool:
            return self._scope == 'shared'

        def __str__(self) -> str:
            return self._scope

    class Option(click.Option):
        """Option that knows whether it should be global"""
        def __init__(self, scope: Union[Scope, str]=None, deprecated: Union[Dict, str, bool]=False, hidden: bool=False, **kwargs: str) -> None:
            """
            Keyword arguments additional to Click's Option class:

            names - alias of 'param_decls'
            deprecated - marks option as deprecated. May be boolean, string (with custom deprecation message)
            or dict with optional keys:
                since: version of deprecation
                removed: version when option will be removed
                custom_message:  Additional text to deprecation warning
            """

            kwargs['param_decls'] = kwargs.pop('names')
            super(Option, self).__init__(**kwargs)

            self.deprecated = deprecated
            self.scope = Scope(scope)
            self.hidden = hidden

            if deprecated:
                deprecation = Deprecation(deprecated)
                self.help: str = deprecation.help(self.help)

            if self.envvar:
                self.help += ' The default value can be set with the %s environment variable.' % self.envvar

            if self.scope.is_global:
                self.help += ' This option can be used at most once either globally, or for one subcommand.'

        def get_help_record(self, ctx: click.core.Context) -> Any:
            # Backport "hidden" parameter to click 5.0
            if self.hidden:
                return None

            return super(Option, self).get_help_record(ctx)

    class CLI(click.MultiCommand):
        """Action list contains all actions with options available for CLI"""
        def __init__(self, all_actions: Dict=None, verbose_output: List=None, help: str=None) -> None:
            super(CLI, self).__init__(
                chain=True,
                invoke_without_command=True,
                result_callback=self.execute_tasks,
                no_args_is_help=True,
                context_settings={'max_content_width': 140},
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
            for option_args in all_actions.get('global_options', []):
                option = Option(**option_args)
                self.params.append(option)

                if option.scope.is_shared:
                    shared_options.append(option)

            # Global options validators
            self.global_action_callbacks = all_actions.get('global_action_callbacks', [])

            # Actions
            for name, action in all_actions.get('actions', {}).items():
                arguments = action.pop('arguments', [])
                options = action.pop('options', [])

                if arguments is None:
                    arguments = []

                if options is None:
                    options = []

                self._actions[name] = Action(name=name, **action)
                for alias in [name] + action.get('aliases', []):
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
                            ' "shared" options can be declared only on global level' % (option.name, name))

                    # Promote options to global if see for the first time
                    if option.scope.is_global and option.name not in [o.name for o in self.params]:
                        self.params.append(option)

                    self._actions[name].params.append(option)

        def list_commands(self, ctx: click.core.Context) -> List:
            return sorted(filter(lambda name: not self._actions[name].hidden, self._actions))

        def get_command(self, ctx: click.core.Context, name: str) -> Optional[Action]:
            if name in self.commands_with_aliases:
                return self._actions.get(self.commands_with_aliases.get(name))

            # Trying fallback to build target (from "all" action) if command is not known
            else:
                callback = self._actions.get('fallback')
                if callback:
                    return Action(name=name, callback=callback.unwrapped_callback)
                return None

        def _print_closing_message(self, args: PropertyDict, actions: _OrderedDictKeysView) -> None:
            # print a closing message of some kind,
            # except if any of the following actions were requested
            if any(t in str(actions) for t in ('flash', 'dfu', 'uf2', 'uf2-app', 'qemu')):
                print('Done')
                return

            if not os.path.exists(os.path.join(args.build_dir, 'flasher_args.json')):
                print('Done')
                return

            # Otherwise, if we built any binaries print a message about
            # how to flash them
            def print_flashing_message(title: str, key: str) -> None:
                with open(os.path.join(args.build_dir, 'flasher_args.json')) as file:
                    flasher_args: Dict[str, Any] = json.load(file)

                def flasher_path(f: Union[str, 'os.PathLike[str]']) -> str:
                    if type(args.build_dir) is bytes:
                        args.build_dir = args.build_dir.decode()
                    return _safe_relpath(os.path.join(args.build_dir, f))

                if key != 'project':  # flashing a single item
                    if key not in flasher_args:
                        # This is the case for 'idf.py bootloader' if Secure Boot is on, need to follow manual flashing steps
                        print('\n%s build complete.' % title)
                        return
                    cmd = ''
                    if (key == 'bootloader'):  # bootloader needs --flash-mode, etc to be passed in
                        cmd = ' '.join(flasher_args['write_flash_args']) + ' '

                    cmd += flasher_args[key]['offset'] + ' '
                    cmd += flasher_path(flasher_args[key]['file'])
                else:  # flashing the whole project
                    cmd = ' '.join(flasher_args['write_flash_args']) + ' '
                    flash_items = sorted(
                        ((o, f) for (o, f) in flasher_args['flash_files'].items() if len(o) > 0),
                        key=lambda x: int(x[0], 0),
                    )
                    for o, f in flash_items:
                        cmd += o + ' ' + flasher_path(f) + ' '

                flash_target = 'flash' if key == 'project' else f'{key}-flash'
                print(f'{os.linesep}{title} build complete. To flash, run:')
                print(f' idf.py {flash_target}')
                if args.port:
                    print('or')
                    print(f' idf.py -p {args.port} {flash_target}')
                print('or')
                print(f' idf.py -p PORT {flash_target}')

                esptool_cmd = ['python -m esptool',
                               '--chip {}'.format(flasher_args['extra_esptool_args']['chip']),
                               f'-b {args.baud}',
                               '--before {}'.format(flasher_args['extra_esptool_args']['before']),
                               '--after {}'.format(flasher_args['extra_esptool_args']['after'])]

                if not flasher_args['extra_esptool_args']['stub']:
                    esptool_cmd += ['--no-stub']

                if args.port:
                    esptool_cmd += [f'-p {args.port}']

                esptool_cmd += ['write_flash']

                print('or')
                print(' {}'.format(' '.join(esptool_cmd + [cmd.strip()])))

                if os.path.exists(os.path.join(args.build_dir, 'flash_args')):
                    print(f'or from the "{args.build_dir}" directory')
                    print(' {}'.format(' '.join(esptool_cmd + ['"@flash_args"'])))

            if 'all' in actions or 'build' in actions:
                print_flashing_message('Project', 'project')
            else:
                if 'app' in actions:
                    print_flashing_message('App', 'app')
                if 'partition-table' in actions:
                    print_flashing_message('Partition Table', 'partition-table')
                if 'bootloader' in actions:
                    print_flashing_message('Bootloader', 'bootloader')

        def execute_tasks(self, tasks: List, **kwargs: str) -> OrderedDict:
            ctx = click.get_current_context()
            global_args = PropertyDict(kwargs)

            # Show warning if some tasks are present several times in the list
            dupplicated_tasks = sorted(
                [item for item, count in Counter(task.name for task in tasks).items() if count > 1])
            if dupplicated_tasks:
                dupes = ', '.join('"%s"' % t for t in dupplicated_tasks)

                print_warning(
                    'WARNING: Command%s found in the list of commands more than once. ' %
                    ('s %s are' % dupes if len(dupplicated_tasks) > 1 else ' %s is' % dupes) +
                    'Only first occurrence will be executed.')

            for task in tasks:
                # Set propagated global options.
                # These options may be set on one subcommand, but available in the list of global arguments
                for key in list(task.action_args):
                    option = next((o for o in ctx.command.params if o.name == key), None)

                    if option and (option.scope.is_global or option.scope.is_shared):
                        local_value = task.action_args.pop(key)
                        global_value = global_args[key]
                        default = () if option.multiple else option.default

                        if global_value != default and local_value != default and global_value != local_value:
                            if hasattr(option, 'envvar') and option.envvar and os.getenv(option.envvar) != default:
                                msg = (f'This option cannot be set in command line if the {option.envvar} '
                                       'environment variable is set to a different value.')
                            else:
                                msg = 'This option can appear at most once in the command line.'

                            raise FatalError(f'Option "{key}" provided for "{task.name}" is already defined to '
                                             f'a different value. {msg}')
                        if local_value != default:
                            global_args[key] = local_value

            # Show warnings about global arguments
            check_deprecation(ctx)

            # Make sure that define_cache_entry is mutable list and can be modified in callbacks
            global_args.define_cache_entry = list(global_args.define_cache_entry)

            # Execute all global action callback - first from idf.py itself, then from extensions
            for action_callback in ctx.command.global_action_callbacks:
                action_callback(ctx, global_args, tasks)

            # Build full list of tasks to and deal with dependencies and order dependencies
            tasks_to_run: OrderedDict = OrderedDict()
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
                            print(
                                'Adding "%s"\'s dependency "%s" to list of commands with default set of options.' %
                                (task.name, dep))
                            dep_task = ctx.invoke(ctx.command.get_command(ctx, dep))

                            # Remove options with global scope from invoke tasks because they are already in global_args
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
                        name_with_aliases += ' (aliases: %s)' % ', '.join(task.aliases)

                    # When machine-readable json format for help is printed, don't show info about executing action so the output is deserializable
                    if name_with_aliases != 'help' or not task.action_args.get('json_option', False):
                        print('Executing action: %s' % name_with_aliases)
                    task(ctx, global_args, task.action_args)

                self._print_closing_message(global_args, tasks_to_run.keys())

            return tasks_to_run

    # That's a tiny parser that parse project-dir even before constructing
    # fully featured click parser to be sure that extensions are loaded from the right place
    @click.command(
        add_help_option=False,
        context_settings={
            'allow_extra_args': True,
            'ignore_unknown_options': True
        },
    )
    @click.option('-C', '--project-dir', default=os.getcwd(), type=click.Path())
    def parse_project_dir(project_dir: str) -> Any:
        return os.path.realpath(project_dir)

    # Set `complete_var` to not existing environment variable name to prevent early cmd completion
    project_dir = parse_project_dir(standalone_mode=False, complete_var='_IDF.PY_COMPLETE_NOT_EXISTING')

    all_actions: Dict = {}
    # Load extensions from components dir
    idf_py_extensions_path = os.path.join(os.environ['IDF_PATH'], 'tools', 'idf_py_actions')
    extension_dirs = [os.path.realpath(idf_py_extensions_path)]
    extra_paths = os.environ.get('IDF_EXTRA_ACTIONS_PATH')
    if extra_paths is not None:
        for path in extra_paths.split(';'):
            path = os.path.realpath(path)
            if path not in extension_dirs:
                extension_dirs.append(path)

    extensions = []
    for directory in extension_dirs:
        if directory and not os.path.exists(directory):
            print_warning('WARNING: Directory with idf.py extensions doesn\'t exist:\n    %s' % directory)
            continue

        sys.path.append(directory)
        for _finder, name, _ispkg in sorted(iter_modules([directory])):
            if name.endswith('_ext'):
                extensions.append((name, import_module(name)))

    # Load component manager idf.py extensions if not explicitly disabled
    if os.getenv('IDF_COMPONENT_MANAGER') != '0':
        extensions.append(('component_manager_ext', idf_extensions))

    # Optional load `pyclang` for additional clang-tidy related functionalities
    try:
        from pyclang import idf_extension

        extensions.append(('idf_clang_tidy_ext', idf_extension))
    except ImportError:
        pass

    for name, extension in extensions:
        try:
            all_actions = merge_action_lists(all_actions, extension.action_extensions(all_actions, project_dir))
        except AttributeError:
            print_warning('WARNING: Cannot load idf.py extension "%s"' % name)

    # Load extensions from project dir
    if os.path.exists(os.path.join(project_dir, 'idf_ext.py')):
        sys.path.append(project_dir)
        try:
            from idf_ext import action_extensions
        except ImportError:
            print_warning('Error importing extension file idf_ext.py. Skipping.')
            print_warning(
                "Please make sure that it contains implementation (even if it's empty) of add_action_extensions")

        try:
            all_actions = merge_action_lists(all_actions, action_extensions(all_actions, project_dir))
        except NameError:
            pass

    cli_help = (
        'ESP-IDF CLI build management tool. '
        'For commands that are not known to idf.py an attempt to execute it as a build system target will be made. '
        'Selected target: {}'.format(get_target(project_dir)))

    return CLI(help=cli_help, verbose_output=verbose_output, all_actions=all_actions)


def main(argv: List[Any] = None) -> None:
    # Check the environment only when idf.py is invoked regularly from command line.
    checks_output = None if SHELL_COMPLETE_RUN else check_environment()

    # Check existance of the current working directory to prevent exceptions from click cli.
    try:
        os.getcwd()
    except FileNotFoundError as e:
        raise FatalError(f'ERROR: {e}. Working directory cannot be established. Check its existence.')

    try:
        cli = init_cli(verbose_output=checks_output)
    except ImportError:
        if SHELL_COMPLETE_RUN:
            pass
        else:
            raise
    else:
        argv = expand_file_arguments(argv or sys.argv[1:])

        cli(argv, prog_name=PROG, complete_var=SHELL_COMPLETE_VAR)


def expand_file_arguments(argv: List[Any]) -> List[Any]:
    """
    Any argument starting with "@" gets replaced with all values read from a text file.
    Text file arguments can be split by newline or by space.
    Values are added "as-is", as if they were specified in this order
    on the command line.
    """
    visited = set()
    expanded = False

    def expand_args(args: List[Any], parent_path: str, file_stack: List[str]) -> List[str]:
        expanded_args = []
        for arg in args:
            if not arg.startswith('@'):
                expanded_args.append(arg)
            else:
                nonlocal expanded, visited
                expanded = True

                file_name = arg[1:]
                rel_path = os.path.normpath(os.path.join(parent_path, file_name))

                if rel_path in visited:
                    file_stack_str = ' -> '.join(['@' + f for f in file_stack + [file_name]])
                    raise FatalError(f'Circular dependency in file argument expansion: {file_stack_str}')
                visited.add(rel_path)

                try:
                    with open(rel_path, 'r') as f:
                        for line in f:
                            expanded_args.extend(expand_args(shlex.split(line), os.path.dirname(rel_path), file_stack + [file_name]))
                except IOError:
                    file_stack_str = ' -> '.join(['@' + f for f in file_stack + [file_name]])
                    raise FatalError(f"File '{rel_path}' (expansion of {file_stack_str}) could not be opened. "
                                     'Please ensure the file exists and you have the necessary permissions to read it.')
        return expanded_args

    argv = expand_args(argv, os.getcwd(), [])

    if expanded:
        print(f'Running: idf.py {" ".join(argv)}')

    return argv


def _valid_unicode_config() -> Union[codecs.CodecInfo, bool]:
    # Python 2 is always good
    if sys.version_info[0] == 2:
        return True

    # With python 3 unicode environment is required
    try:
        return codecs.lookup(locale.getpreferredencoding()).name != 'ascii'
    except Exception:
        return False


def _find_usable_locale() -> str:
    try:
        locales = subprocess.Popen(['locale', '-a'], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0].decode('ascii', 'replace')
    except OSError:
        locales = ''

    usable_locales: List[str] = []
    for line in locales.splitlines():
        locale = line.strip()
        locale_name = locale.lower().replace('-', '')

        # C.UTF-8 is the best option, if supported
        if locale_name == 'c.utf8':
            return locale

        if locale_name.endswith('.utf8'):
            # Make a preference of english locales
            if locale.startswith('en_'):
                usable_locales.insert(0, locale)
            else:
                usable_locales.append(locale)

    if not usable_locales:
        raise FatalError(
            'Support for Unicode filenames is required, but no suitable UTF-8 locale was found on your system.'
            ' Please refer to the manual for your operating system for details on locale reconfiguration.')

    return usable_locales[0]


if __name__ == '__main__':
    try:
        if 'MSYSTEM' in os.environ:
            print_warning(
                'MSys/Mingw is no longer supported. Please follow the getting started guide of the '
                'documentation in order to set up a suitiable environment, or continue at your own risk.')
        elif os.name == 'posix' and not _valid_unicode_config():
            # Trying to find best utf-8 locale available on the system and restart python with it
            best_locale = _find_usable_locale()

            print_warning(
                'Your environment is not configured to handle unicode filenames outside of ASCII range.'
                ' Environment variable LC_ALL is temporary set to %s for unicode support.' % best_locale)

            os.environ['LC_ALL'] = best_locale
            ret = subprocess.call([sys.executable] + sys.argv, env=os.environ)
            if ret:
                raise SystemExit(ret)

        else:
            main()

    except FatalError as e:
        print(e, file=sys.stderr)
        sys.exit(2)
