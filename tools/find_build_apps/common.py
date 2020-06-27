# coding=utf-8
import re
import shutil
import sys
import os
from abc import abstractmethod
from collections import namedtuple
import logging
import json
import typing
from io import open

DEFAULT_TARGET = "esp32"

TARGET_PLACEHOLDER = "@t"
WILDCARD_PLACEHOLDER = "@w"
NAME_PLACEHOLDER = "@n"
FULL_NAME_PLACEHOLDER = "@f"
INDEX_PLACEHOLDER = "@i"

SDKCONFIG_LINE_REGEX = re.compile(r"^([^=]+)=\"?([^\"\n]*)\"?\n*$")

# If these keys are present in sdkconfig.defaults, they will be extracted and passed to CMake
SDKCONFIG_TEST_OPTS = [
    "EXCLUDE_COMPONENTS",
    "TEST_EXCLUDE_COMPONENTS",
    "TEST_COMPONENTS",
    "TEST_GROUPS"
]

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
    SUPPORTED_TARGETS_REGEX = re.compile(r'Supported [Tt]argets((?:[\s|]+(?:ESP[0-9A-Z\-]+))+)')

    @classmethod
    def build_prepare(cls, build_item):
        app_path = build_item.app_dir
        work_path = build_item.work_dir or app_path
        if not build_item.build_dir:
            build_path = os.path.join(work_path, "build")
        elif os.path.isabs(build_item.build_dir):
            build_path = build_item.build_dir
        else:
            build_path = os.path.join(work_path, build_item.build_dir)

        if work_path != app_path:
            if os.path.exists(work_path):
                logging.debug("Work directory {} exists, removing".format(work_path))
                if not build_item.dry_run:
                    shutil.rmtree(work_path)
            logging.debug("Copying app from {} to {}".format(app_path, work_path))
            if not build_item.dry_run:
                shutil.copytree(app_path, work_path)

        if os.path.exists(build_path):
            logging.debug("Build directory {} exists, removing".format(build_path))
            if not build_item.dry_run:
                shutil.rmtree(build_path)

        if not build_item.dry_run:
            os.makedirs(build_path)

        # Prepare the sdkconfig file, from the contents of sdkconfig.defaults (if exists) and the contents of
        # build_info.sdkconfig_path, i.e. the config-specific sdkconfig file.
        #
        # Note: the build system supports taking multiple sdkconfig.defaults files via SDKCONFIG_DEFAULTS
        # CMake variable. However here we do this manually to perform environment variable expansion in the
        # sdkconfig files.
        sdkconfig_defaults_list = ["sdkconfig.defaults", "sdkconfig.defaults." + build_item.target]
        if build_item.sdkconfig_path:
            sdkconfig_defaults_list.append(build_item.sdkconfig_path)

        sdkconfig_file = os.path.join(work_path, "sdkconfig")
        if os.path.exists(sdkconfig_file):
            logging.debug("Removing sdkconfig file: {}".format(sdkconfig_file))
            if not build_item.dry_run:
                os.unlink(sdkconfig_file)

        logging.debug("Creating sdkconfig file: {}".format(sdkconfig_file))
        extra_cmakecache_items = {}
        if not build_item.dry_run:
            with open(sdkconfig_file, "w") as f_out:
                for sdkconfig_name in sdkconfig_defaults_list:
                    sdkconfig_path = os.path.join(work_path, sdkconfig_name)
                    if not sdkconfig_path or not os.path.exists(sdkconfig_path):
                        continue
                    logging.debug("Appending {} to sdkconfig".format(sdkconfig_name))
                    with open(sdkconfig_path, "r") as f_in:
                        for line in f_in:
                            if not line.endswith("\n"):
                                line += "\n"
                            if cls.NAME == 'cmake':
                                m = SDKCONFIG_LINE_REGEX.match(line)
                                if m and m.group(1) in SDKCONFIG_TEST_OPTS:
                                    extra_cmakecache_items[m.group(1)] = m.group(2)
                                    continue
                            f_out.write(os.path.expandvars(line))
        else:
            for sdkconfig_name in sdkconfig_defaults_list:
                sdkconfig_path = os.path.join(app_path, sdkconfig_name)
                if not sdkconfig_path:
                    continue
                logging.debug("Considering sdkconfig {}".format(sdkconfig_path))
                if not os.path.exists(sdkconfig_path):
                    continue
                logging.debug("Appending {} to sdkconfig".format(sdkconfig_name))

        # The preparation of build is finished. Implement the build part in sub classes.
        if cls.NAME == 'cmake':
            return build_path, work_path, extra_cmakecache_items
        else:
            return build_path, work_path

    @staticmethod
    @abstractmethod
    def build(build_item):
        pass

    @staticmethod
    @abstractmethod
    def is_app(path):
        pass

    @staticmethod
    def _read_readme(app_path):
        # Markdown supported targets should be:
        # e.g. | Supported Targets | ESP32 |
        #      | ----------------- | ----- |
        # reStructuredText supported targets should be:
        # e.g. ================= =====
        #      Supported Targets ESP32
        #      ================= =====
        def get_md_or_rst(app_path):
            readme_path = os.path.join(app_path, 'README.md')
            if not os.path.exists(readme_path):
                readme_path = os.path.join(app_path, 'README.rst')
                if not os.path.exists(readme_path):
                    return None
            return readme_path

        readme_path = get_md_or_rst(app_path)
        # Handle sub apps situation, e.g. master-slave
        if not readme_path:
            readme_path = get_md_or_rst(os.path.dirname(app_path))
        if not readme_path:
            return None
        with open(readme_path, "r", encoding='utf8') as readme_file:
            return readme_file.read()

    @staticmethod
    def supported_targets(app_path):
        formal_to_usual = {
            'ESP32': 'esp32',
            'ESP32-S2': 'esp32s2',
        }

        readme_file_content = BuildSystem._read_readme(app_path)
        if not readme_file_content:
            return None
        match = re.findall(BuildSystem.SUPPORTED_TARGETS_REGEX, readme_file_content)
        if not match:
            return None
        if len(match) > 1:
            raise NotImplementedError("Can't determine the value of SUPPORTED_TARGETS in {}".format(app_path))
        support_str = match[0].strip()

        targets = []
        for part in support_str.split('|'):
            for inner in part.split(' '):
                inner = inner.strip()
                if not inner:
                    continue
                elif inner in formal_to_usual:
                    targets.append(formal_to_usual[inner])
                else:
                    raise NotImplementedError("Can't recognize value of target {} in {}, now we only support '{}'"
                                              .format(inner, app_path, ', '.join(formal_to_usual.keys())))
        return targets


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
