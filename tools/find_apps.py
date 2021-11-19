#!/usr/bin/env python
# coding=utf-8
#
# ESP-IDF helper script to enumerate the builds of multiple configurations of multiple apps.
# Produces the list of builds. The list can be consumed by build_apps.py, which performs the actual builds.

import argparse
import glob
import json
import logging
import os
import re
import sys
import typing

from find_build_apps import (BUILD_SYSTEM_CMAKE, BUILD_SYSTEMS, DEFAULT_TARGET, BuildItem, BuildSystem, ConfigRule,
                             config_rules_from_str, setup_logging)


# Helper functions
def dict_from_sdkconfig(path):
    """
    Parse the sdkconfig file at 'path', return name:value pairs as a dict
    """
    regex = re.compile(r'^([^#=]+)=(.+)$')
    result = {}
    with open(path) as f:
        for line in f:
            m = regex.match(line)
            if m:
                val = m.group(2)
                if val.startswith('"') and val.endswith('"'):
                    val = val[1:-1]
                result[m.group(1)] = val
    return result


# Main logic: enumerating apps and builds


def find_builds_for_app(app_path, work_dir, build_dir, build_log, target_arg,
                        build_system, config_rules, preserve_artifacts=True):
    # type: (str, str, str, str, str, str, typing.List[ConfigRule], bool) -> typing.List[BuildItem]
    """
    Find configurations (sdkconfig file fragments) for the given app, return them as BuildItem objects
    :param app_path: app directory (can be / usually will be a relative path)
    :param work_dir: directory where the app should be copied before building.
                     May contain env. variables and placeholders.
    :param build_dir: directory where the build will be done, relative to the work_dir. May contain placeholders.
    :param build_log: path of the build log. May contain placeholders. May be None, in which case the log should go
                      into stdout/stderr.
    :param target_arg: the value of IDF_TARGET passed to the script. Used to filter out configurations with
                       a different CONFIG_IDF_TARGET value.
    :param build_system: name of the build system, index into BUILD_SYSTEMS dictionary
    :param config_rules: mapping of sdkconfig file name patterns to configuration names
    :param preserve_artifacts: determine if the built binary will be uploaded as artifacts.
    :return: list of BuildItems representing build configuration of the app
    """
    build_items = []  # type: typing.List[BuildItem]
    default_config_name = ''

    for rule in config_rules:
        if not rule.file_name:
            default_config_name = rule.config_name
            continue

        sdkconfig_paths = glob.glob(os.path.join(app_path, rule.file_name))
        sdkconfig_paths = sorted(sdkconfig_paths)
        for sdkconfig_path in sdkconfig_paths:

            # Check if the sdkconfig file specifies IDF_TARGET, and if it is matches the --target argument.
            sdkconfig_dict = dict_from_sdkconfig(sdkconfig_path)
            target_from_config = sdkconfig_dict.get('CONFIG_IDF_TARGET')
            if target_from_config is not None and target_from_config != target_arg:
                logging.debug('Skipping sdkconfig {} which requires target {}'.format(
                    sdkconfig_path, target_from_config))
                continue

            # Figure out the config name
            config_name = rule.config_name or ''
            if '*' in rule.file_name:
                # convert glob pattern into a regex
                regex_str = r'.*' + rule.file_name.replace('.', r'\.').replace('*', r'(.*)')
                groups = re.match(regex_str, sdkconfig_path)
                assert groups
                config_name = groups.group(1)

            sdkconfig_path = os.path.relpath(sdkconfig_path, app_path)
            logging.debug('Adding build: app {}, sdkconfig {}, config name "{}"'.format(
                app_path, sdkconfig_path, config_name))
            build_items.append(
                BuildItem(
                    app_path,
                    work_dir,
                    build_dir,
                    build_log,
                    target_arg,
                    sdkconfig_path,
                    config_name,
                    build_system,
                    preserve_artifacts,
                ))

    if not build_items:
        logging.debug('\tAdding build: app {}, default sdkconfig, config name "{}"'.format(app_path, default_config_name))
        return [
            BuildItem(
                app_path,
                work_dir,
                build_dir,
                build_log,
                target_arg,
                None,
                default_config_name,
                build_system,
                preserve_artifacts,
            )
        ]

    return build_items


def find_apps(build_system_class, path, recursive, exclude_list, target):
    # type: (typing.Type[BuildSystem], str, bool, typing.List[str], str) -> typing.List[str]
    """
    Find app directories in path (possibly recursively), which contain apps for the given build system, compatible
    with the given target.
    :param build_system_class: class derived from BuildSystem, representing the build system in use
    :param path: path where to look for apps
    :param recursive: whether to recursively descend into nested directories if no app is found
    :param exclude_list: list of paths to be excluded from the recursive search
    :param target: desired value of IDF_TARGET; apps incompatible with the given target are skipped.
    :return: list of paths of the apps found
    """
    build_system_name = build_system_class.NAME
    logging.debug('Looking for {} apps in {}{}'.format(build_system_name, path, ' recursively' if recursive else ''))

    apps_found = []  # type: typing.List[str]
    for root, dirs, _ in os.walk(path, topdown=True):
        logging.debug('Entering {}'.format(root))
        if root in exclude_list:
            logging.debug('Skipping {} (excluded)'.format(root))
            del dirs[:]
            continue

        if build_system_class.is_app(root):
            logging.debug('Found {} app in {}'.format(build_system_name, root))
            # Don't recurse into app subdirectories
            del dirs[:]

            supported_targets = build_system_class.supported_targets(root)
            if supported_targets and (target in supported_targets):
                apps_found.append(root)
            else:
                if supported_targets:
                    logging.debug('Skipping, app only supports targets: ' + ', '.join(supported_targets))
                else:
                    logging.debug('Skipping, app has no supported targets')
                continue

        if not recursive:
            if not apps_found:
                logging.warning('Path {} specified without --recursive flag, but no {} app found there'.format(
                    path, build_system_name))
            break  # only check the top-most dir if "recursive" is unflagged

    return apps_found


def main():
    parser = argparse.ArgumentParser(description='Tool to generate build steps for IDF apps')
    parser.add_argument(
        '-v',
        '--verbose',
        action='count',
        help='Increase the logging level of the script. Can be specified multiple times.',
    )
    parser.add_argument(
        '--log-file',
        type=argparse.FileType('w'),
        help='Write the script log to the specified file, instead of stderr',
    )
    parser.add_argument(
        '--recursive',
        action='store_true',
        help='Look for apps in the specified directories recursively.',
    )
    parser.add_argument(
        '--build-system',
        choices=BUILD_SYSTEMS.keys()
    )
    parser.add_argument(
        '--work-dir',
        help='If set, the app is first copied into the specified directory, and then built.' +
             'If not set, the work directory is the directory of the app.',
    )
    parser.add_argument(
        '--config',
        action='append',
        help='Adds configurations (sdkconfig file names) to build. This can either be ' +
             'FILENAME[=NAME] or FILEPATTERN. FILENAME is the name of the sdkconfig file, ' +
             'relative to the project directory, to be used. Optional NAME can be specified, ' +
             'which can be used as a name of this configuration. FILEPATTERN is the name of ' +
             'the sdkconfig file, relative to the project directory, with at most one wildcard. ' +
             'The part captured by the wildcard is used as the name of the configuration.',
    )
    parser.add_argument(
        '--build-dir',
        help='If set, specifies the build directory name. Can expand placeholders. Can be either a ' +
             'name relative to the work directory, or an absolute path.',
    )
    parser.add_argument(
        '--build-log',
        help='If specified, the build log will be written to this file. Can expand placeholders.',
    )
    parser.add_argument('--target', help='Build apps for given target.')
    parser.add_argument(
        '--format',
        default='json',
        choices=['json'],
        help='Format to write the list of builds as',
    )
    parser.add_argument(
        '--exclude',
        action='append',
        help='Ignore specified directory (if --recursive is given). Can be used multiple times.',
    )
    parser.add_argument(
        '-o',
        '--output',
        type=argparse.FileType('w'),
        help='Output the list of builds to the specified file',
    )
    parser.add_argument(
        '--app-list',
        default=None,
        help='Scan tests results. Restrict the build/artifacts preservation behavior to apps need to be built. '
             'If the file does not exist, will build all apps and upload all artifacts.'
    )
    parser.add_argument(
        '-p', '--paths',
        nargs='+',
        help='One or more app paths.'
    )
    args = parser.parse_args()
    setup_logging(args)

    # Arguments Validation
    if args.app_list:
        conflict_args = [args.recursive, args.build_system, args.target, args.exclude, args.paths]
        if any(conflict_args):
            raise ValueError('Conflict settings. "recursive", "build_system", "target", "exclude", "paths" should not '
                             'be specified with "app_list"')
        if not os.path.exists(args.app_list):
            raise OSError('File not found {}'.format(args.app_list))
    else:
        # If the build target is not set explicitly, get it from the environment or use the default one (esp32)
        if not args.target:
            env_target = os.environ.get('IDF_TARGET')
            if env_target:
                logging.info('--target argument not set, using IDF_TARGET={} from the environment'.format(env_target))
                args.target = env_target
            else:
                logging.info('--target argument not set, using IDF_TARGET={} as the default'.format(DEFAULT_TARGET))
                args.target = DEFAULT_TARGET
        if not args.build_system:
            logging.info('--build-system argument not set, using {} as the default'.format(BUILD_SYSTEM_CMAKE))
            args.build_system = BUILD_SYSTEM_CMAKE
        required_args = [args.build_system, args.target, args.paths]
        if not all(required_args):
            raise ValueError('If app_list not set, arguments "build_system", "target", "paths" are required.')

    # Prepare the list of app paths, try to read from the scan_tests result.
    # If the file exists, then follow the file's app_dir and build/artifacts behavior, won't do find_apps() again.
    # If the file not exists, will do find_apps() first, then build all apps and upload all artifacts.
    if args.app_list:
        apps = [json.loads(line) for line in open(args.app_list)]
    else:
        app_dirs = []
        build_system_class = BUILD_SYSTEMS[args.build_system]
        for path in args.paths:
            app_dirs += find_apps(build_system_class, path, args.recursive, args.exclude or [], args.target)
        apps = [{'app_dir': app_dir, 'build': True, 'preserve': True} for app_dir in app_dirs]

    if not apps:
        logging.warning('No apps found')
        SystemExit(0)

    logging.info('Found {} apps'.format(len(apps)))
    apps.sort(key=lambda x: x['app_dir'])

    # Find compatible configurations of each app, collect them as BuildItems
    build_items = []  # type: typing.List[BuildItem]
    config_rules = config_rules_from_str(args.config or [])
    for app in apps:
        build_items += find_builds_for_app(
            app['app_dir'],
            args.work_dir,
            args.build_dir,
            args.build_log,
            args.target or app['target'],
            args.build_system or app['build_system'],
            config_rules,
            app['preserve'],
        )
    logging.info('Found {} builds'.format(len(build_items)))

    # Write out the BuildItems. Only JSON supported now (will add YAML later).
    if args.format != 'json':
        raise NotImplementedError()

    out = args.output or sys.stdout
    out.writelines([item.to_json() + '\n' for item in build_items])


if __name__ == '__main__':
    main()
