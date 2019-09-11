#!/usr/bin/env python
# coding=utf-8
#
# ESP-IDF helper script to enumerate the builds of multiple configurations of multiple apps.
# Produces the list of builds. The list can be consumed by build_apps.py, which performs the actual builds.

import argparse
import os
import sys
import re
import glob
import logging
import typing
from find_build_apps import (
    BUILD_SYSTEMS,
    BUILD_SYSTEM_CMAKE,
    BuildSystem,
    BuildItem,
    setup_logging,
    ConfigRule,
    config_rules_from_str,
    DEFAULT_TARGET,
)

# Helper functions


def dict_from_sdkconfig(path):
    """
    Parse the sdkconfig file at 'path', return name:value pairs as a dict
    """
    regex = re.compile(r"^([^#=]+)=(.+)$")
    result = {}
    with open(path) as f:
        for line in f:
            m = regex.match(line)
            if m:
                result[m.group(1)] = m.group(2)
    return result


# Main logic: enumerating apps and builds


def find_builds_for_app(
        app_path, work_dir, build_dir, build_log, target_arg, build_system,
        config_rules):  # type: (str, str, str, str, str, str, typing.List[ConfigRule]) -> typing.List[BuildItem]
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
    :return: list of BuildItems representing build configuration of the app
    """
    build_items = []  # type: typing.List[BuildItem]
    default_config_name = ""

    for rule in config_rules:
        if not rule.file_name:
            default_config_name = rule.config_name
            continue

        sdkconfig_paths = glob.glob(os.path.join(app_path, rule.file_name))
        sdkconfig_paths = sorted(sdkconfig_paths)
        for sdkconfig_path in sdkconfig_paths:

            # Check if the sdkconfig file specifies IDF_TARGET, and if it is matches the --target argument.
            sdkconfig_dict = dict_from_sdkconfig(sdkconfig_path)
            target_from_config = sdkconfig_dict.get("CONFIG_IDF_TARGET")
            if target_from_config is not None and target_from_config != target_arg:
                logging.debug("Skipping sdkconfig {} which requires target {}".format(
                    sdkconfig_path, target_from_config))
                continue

            # Figure out the config name
            config_name = rule.config_name or ""
            if "*" in rule.file_name:
                # convert glob pattern into a regex
                regex_str = r".*" + rule.file_name.replace(".", r"\.").replace("*", r"(.*)")
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
                ))

    if not build_items:
        logging.debug('Adding build: app {}, default sdkconfig, config name "{}"'.format(app_path, default_config_name))
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
            )
        ]

    return build_items


def find_apps(build_system_class, path, recursive, exclude_list,
              target):  # type: (typing.Type[BuildSystem], str, bool, typing.List[str], str) -> typing.List[str]
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
    logging.debug("Looking for {} apps in {}{}".format(build_system_name, path, " recursively" if recursive else ""))
    if not recursive:
        if exclude_list:
            logging.warn("--exclude option is ignored when used without --recursive")
        if not build_system_class.is_app(path):
            logging.warn("Path {} specified without --recursive flag, but no {} app found there".format(
                path, build_system_name))
            return []
        return [path]

    # The remaining part is for recursive == True
    apps_found = []  # type: typing.List[str]
    for root, dirs, _ in os.walk(path, topdown=True):
        logging.debug("Entering {}".format(root))
        if root in exclude_list:
            logging.debug("Skipping {} (excluded)".format(root))
            del dirs[:]
            continue

        if build_system_class.is_app(root):
            logging.debug("Found {} app in {}".format(build_system_name, root))
            # Don't recurse into app subdirectories
            del dirs[:]

            supported_targets = build_system_class.supported_targets(root)
            if supported_targets and target not in supported_targets:
                logging.debug("Skipping, app only supports targets: " + ", ".join(supported_targets))
                continue

            apps_found.append(root)

    return apps_found


def main():
    parser = argparse.ArgumentParser(description="Tool to generate build steps for IDF apps")
    parser.add_argument(
        "-v",
        "--verbose",
        action="count",
        help="Increase the logging level of the script. Can be specified multiple times.",
    )
    parser.add_argument(
        "--log-file",
        type=argparse.FileType("w"),
        help="Write the script log to the specified file, instead of stderr",
    )
    parser.add_argument(
        "--recursive",
        action="store_true",
        help="Look for apps in the specified directories recursively.",
    )
    parser.add_argument("--build-system", choices=BUILD_SYSTEMS.keys(), default=BUILD_SYSTEM_CMAKE)
    parser.add_argument(
        "--work-dir",
        help="If set, the app is first copied into the specified directory, and then built." +
        "If not set, the work directory is the directory of the app.",
    )
    parser.add_argument(
        "--config",
        action="append",
        help="Adds configurations (sdkconfig file names) to build. This can either be " +
        "FILENAME[=NAME] or FILEPATTERN. FILENAME is the name of the sdkconfig file, " +
        "relative to the project directory, to be used. Optional NAME can be specified, " +
        "which can be used as a name of this configuration. FILEPATTERN is the name of " +
        "the sdkconfig file, relative to the project directory, with at most one wildcard. " +
        "The part captured by the wildcard is used as the name of the configuration.",
    )
    parser.add_argument(
        "--build-dir",
        help="If set, specifies the build directory name. Can expand placeholders. Can be either a " +
        "name relative to the work directory, or an absolute path.",
    )
    parser.add_argument(
        "--build-log",
        help="If specified, the build log will be written to this file. Can expand placeholders.",
    )
    parser.add_argument("--target", help="Build apps for given target.")
    parser.add_argument(
        "--format",
        default="json",
        choices=["json"],
        help="Format to write the list of builds as",
    )
    parser.add_argument(
        "--exclude",
        action="append",
        help="Ignore specified directory (if --recursive is given). Can be used multiple times.",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=argparse.FileType("w"),
        help="Output the list of builds to the specified file",
    )
    parser.add_argument("paths", nargs="+", help="One or more app paths.")
    args = parser.parse_args()
    setup_logging(args)

    build_system_class = BUILD_SYSTEMS[args.build_system]

    # If the build target is not set explicitly, get it from the environment or use the default one (esp32)
    if not args.target:
        env_target = os.environ.get("IDF_TARGET")
        if env_target:
            logging.info("--target argument not set, using IDF_TARGET={} from the environment".format(env_target))
            args.target = env_target
        else:
            logging.info("--target argument not set, using IDF_TARGET={} as the default".format(DEFAULT_TARGET))
            args.target = DEFAULT_TARGET

    # Prepare the list of app paths
    app_paths = []  # type: typing.List[str]
    for path in args.paths:
        app_paths += find_apps(build_system_class, path, args.recursive, args.exclude or [], args.target)

    if not app_paths:
        logging.critical("No {} apps found".format(build_system_class.NAME))
        raise SystemExit(1)
    logging.info("Found {} apps".format(len(app_paths)))

    app_paths = sorted(app_paths)

    # Find compatible configurations of each app, collect them as BuildItems
    build_items = []  # type: typing.List[BuildItem]
    config_rules = config_rules_from_str(args.config or [])
    for app_path in app_paths:
        build_items += find_builds_for_app(
            app_path,
            args.work_dir,
            args.build_dir,
            args.build_log,
            args.target,
            args.build_system,
            config_rules,
        )
    logging.info("Found {} builds".format(len(build_items)))

    # Write out the BuildItems. Only JSON supported now (will add YAML later).
    if args.format != "json":
        raise NotImplementedError()
    out = args.output or sys.stdout
    out.writelines([item.to_json() + "\n" for item in build_items])


if __name__ == "__main__":
    main()
