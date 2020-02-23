# coding=utf-8

import sys
import os
from collections import namedtuple
import logging
import json
import typing

DEFAULT_TARGET = "esp32"

TARGET_PLACEHOLDER = "@t"
WILDCARD_PLACEHOLDER = "@w"
NAME_PLACEHOLDER = "@n"
FULL_NAME_PLACEHOLDER = "@f"
INDEX_PLACEHOLDER = "@i"

# ConfigRule represents one --config argument of find_apps.py.
# file_name is the name of the sdkconfig file fragment, optionally with a single wildcard ('*' character).
# file_name can also be empty to indicate that the default configuration of the app should be used.
# config_name is the name of the corresponding build configuration, or None if the value of wildcard is to be used.
# For example:
#   filename='', config_name='default' — represents the default app configuration, and gives it a name 'default'
#   filename='sdkconfig.*', config_name=None - represents the set of configurations, names match the wildcard value
ConfigRule = namedtuple("ConfigRule", ["file_name", "config_name"])


def config_rules_from_str(rule_strings):  # type: (typing.List[str]) -> typing.List[ConfigRule]
    """
    Helper function to convert strings like 'file_name=config_name' into ConfigRule objects
    :param rule_strings: list of rules as strings
    :return: list of ConfigRules
    """
    rules = []  # type: typing.List[ConfigRule]
    for rule_str in rule_strings:
        items = rule_str.split("=", 2)
        rules.append(ConfigRule(items[0], items[1] if len(items) == 2 else None))
    return rules


class BuildItem(object):
    """
    Instance of this class represents one build of an application.
    The parameters which distinguish the build are passed to the constructor.
    """
    def __init__(
            self,
            app_path,
            work_dir,
            build_path,
            build_log_path,
            target,
            sdkconfig_path,
            config_name,
            build_system,
    ):
        # These internal variables store the paths with environment variables and placeholders;
        # Public properties with similar names use the _expand method to get the actual paths.
        self._app_dir = app_path
        self._work_dir = work_dir
        self._build_dir = build_path
        self._build_log_path = build_log_path

        self.sdkconfig_path = sdkconfig_path
        self.config_name = config_name
        self.target = target
        self.build_system = build_system

        self._app_name = os.path.basename(os.path.normpath(app_path))

        # Some miscellaneous build properties which are set later, at the build stage
        self.index = None
        self.verbose = False
        self.dry_run = False
        self.keep_going = False

    @property
    def app_dir(self):
        """
        :return: directory of the app
        """
        return self._expand(self._app_dir)

    @property
    def work_dir(self):
        """
        :return: directory where the app should be copied to, prior to the build. Can be None, which means that the app
                 directory should be used.
        """
        return self._expand(self._work_dir)

    @property
    def build_dir(self):
        """
        :return: build directory, either relative to the work directory (if relative path is used) or absolute path.
        """
        return self._expand(self._build_dir)

    @property
    def build_log_path(self):
        """
        :return: path of the build log file
        """
        return self._expand(self._build_log_path)

    def __repr__(self):
        return "Build app {} for target {}, sdkconfig {} in {}".format(
            self.app_dir,
            self.target,
            self.sdkconfig_path or "(default)",
            self.build_dir,
        )

    def to_json(self):  # type: () -> str
        """
        :return: JSON string representing this object
        """
        return self._to_json(self._app_dir, self._work_dir, self._build_dir, self._build_log_path)

    def to_json_expanded(self):  # type: () -> str
        """
        :return: JSON string representing this object, with all placeholders in paths expanded
        """
        return self._to_json(self.app_dir, self.work_dir, self.build_dir, self.build_log_path)

    def _to_json(self, app_dir, work_dir, build_dir, build_log_path):  # type: (str, str, str, str) -> str
        """
        Internal function, called by to_json and to_json_expanded
        """
        return json.dumps({
            "build_system": self.build_system,
            "app_dir": app_dir,
            "work_dir": work_dir,
            "build_dir": build_dir,
            "build_log_path": build_log_path,
            "sdkconfig": self.sdkconfig_path,
            "config": self.config_name,
            "target": self.target,
            "verbose": self.verbose,
        })

    @staticmethod
    def from_json(json_str):  # type: (typing.Text) -> BuildItem
        """
        :return: Get the BuildItem from a JSON string
        """
        d = json.loads(str(json_str))
        result = BuildItem(
            app_path=d["app_dir"],
            work_dir=d["work_dir"],
            build_path=d["build_dir"],
            build_log_path=d["build_log_path"],
            sdkconfig_path=d["sdkconfig"],
            config_name=d["config"],
            target=d["target"],
            build_system=d["build_system"],
        )
        result.verbose = d["verbose"]
        return result

    def _expand(self, path):  # type: (str) -> str
        """
        Internal method, expands any of the placeholders in {app,work,build} paths.
        """
        if not path:
            return path

        if self.index is not None:
            path = path.replace(INDEX_PLACEHOLDER, str(self.index))
        path = path.replace(TARGET_PLACEHOLDER, self.target)
        path = path.replace(NAME_PLACEHOLDER, self._app_name)
        if (FULL_NAME_PLACEHOLDER in path):  # to avoid recursion to the call to app_dir in the next line:
            path = path.replace(FULL_NAME_PLACEHOLDER, self.app_dir.replace(os.path.sep, "_"))
        wildcard_pos = path.find(WILDCARD_PLACEHOLDER)
        if wildcard_pos != -1:
            if self.config_name:
                # if config name is defined, put it in place of the placeholder
                path = path.replace(WILDCARD_PLACEHOLDER, self.config_name)
            else:
                # otherwise, remove the placeholder and one character on the left
                # (which is usually an underscore, dash, or other delimiter)
                left_of_wildcard = max(0, wildcard_pos - 1)
                right_of_wildcard = wildcard_pos + len(WILDCARD_PLACEHOLDER)
                path = path[0:left_of_wildcard] + path[right_of_wildcard:]
        path = os.path.expandvars(path)
        return path


class BuildSystem(object):
    """
    Class representing a build system.
    Derived classes implement the methods below.
    Objects of these classes aren't instantiated, instead the class (type object) is used.
    """

    NAME = "undefined"

    @staticmethod
    def build(self):
        raise NotImplementedError()

    @staticmethod
    def is_app(path):
        raise NotImplementedError()

    @staticmethod
    def supported_targets(app_path):
        raise NotImplementedError()


class BuildError(RuntimeError):
    pass


def setup_logging(args):
    """
    Configure logging module according to the number of '--verbose'/'-v' arguments and the --log-file argument.
    :param args: namespace obtained from argparse
    """
    if not args.verbose:
        log_level = logging.WARNING
    elif args.verbose == 1:
        log_level = logging.INFO
    else:
        log_level = logging.DEBUG

    logging.basicConfig(
        format="%(levelname)s: %(message)s",
        stream=args.log_file or sys.stderr,
        level=log_level,
    )
