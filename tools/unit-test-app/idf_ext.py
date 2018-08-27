import sys
import glob
import tempfile
import os
import os.path
import re
import shutil
import argparse
import json
import copy

PROJECT_NAME = "unit-test-app"
PROJECT_PATH = os.getcwd()

# List of unit-test-app configurations.
# Each file in configs/ directory defines a configuration. The format is the
# same as sdkconfig file. Configuration is applied on top of sdkconfig.defaults
# file from the project directory
CONFIG_NAMES =  os.listdir(os.path.join(PROJECT_PATH, "configs"))

# Build (intermediate) and output (artifact) directories
BUILDS_DIR = os.path.join(PROJECT_PATH, "builds")
BINARIES_DIR = os.path.join(PROJECT_PATH, "output")

# Convert the values passed to the -T parameter to corresponding cache entry definitions
# TESTS_ALL and TEST_COMPONENTS
class TestComponentAction(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        # Create a new of cache definition entry, adding previous elements
        cache_entries = list()

        existing_entries = getattr(namespace, "define_cache_entry", [])

        if existing_entries:
            cache_entries.extend(existing_entries)

        # Form -D arguments
        if "all" in values:
            cache_entries.append("TESTS_ALL=1")
            cache_entries.append("TEST_COMPONENTS=''")
        else:
            cache_entries.append("TESTS_ALL=0")
            cache_entries.append("TEST_COMPONENTS='%s'" % " ".join(values))

        setattr(namespace, "define_cache_entry", cache_entries)

        # Brute force add reconfigure at the very beginning
        existing_actions = getattr(namespace, "actions", [])
        if not "reconfigure" in existing_actions:
            existing_actions = ["reconfigure"] + existing_actions
        setattr(namespace, "actions", existing_actions)

class TestExcludeComponentAction(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        # Create a new of cache definition entry, adding previous elements
        cache_entries = list()

        existing_entries = getattr(namespace, "define_cache_entry", [])

        if existing_entries:
            cache_entries.extend(existing_entries)

        cache_entries.append("TEST_EXCLUDE_COMPONENTS='%s'" % " ".join(values))

        setattr(namespace, "define_cache_entry", cache_entries)

        # Brute force add reconfigure at the very beginning
        existing_actions = getattr(namespace, "actions", [])
        if not "reconfigure" in existing_actions:
            existing_actions = ["reconfigure"] + existing_actions
        setattr(namespace, "actions", existing_actions)

def add_argument_extensions(parser):
    # For convenience, define a -T argument that gets converted to -D arguments
    parser.add_argument('-T', '--test-component', help="Specify the components to test", nargs='+', action=TestComponentAction)
    # For convenience, define a -T argument that gets converted to -D arguments
    parser.add_argument('-E', '--test-exclude-components', help="Specify the components to exclude from testing", nargs='+', action=TestExcludeComponentAction)

def add_action_extensions(base_functions, base_actions):

    def ut_apply_config(ut_apply_config_name, args):
        config_name = re.match(r"ut-apply-config-(.*)", ut_apply_config_name).group(1)

        def set_config_build_variables(prop, defval = None):
            property_value = re.match(r"^%s=(.*)" % prop, config_file_content)
            if (property_value):
                property_value = property_value.group(1)
            else:
                property_value = defval

            if (property_value):
                try:
                    args.define_cache_entry.append("%s=" % prop + property_value)
                except AttributeError:
                    args.define_cache_entry = ["%s=" % prop + property_value]

            return property_value

        sdkconfig_set = None

        if args.define_cache_entry:
            sdkconfig_set = filter(lambda s: "SDKCONFIG=" in s, args.define_cache_entry)

        sdkconfig_path = os.path.join(args.project_dir, "sdkconfig")

        if sdkconfig_set:
            sdkconfig_path = sdkconfig_set[-1].split("=")[1]
            sdkconfig_path = os.path.abspath(sdkconfig_path)

        try:
            os.remove(sdkconfig_path)
        except OSError:
            pass

        if config_name in CONFIG_NAMES:
            # Parse the sdkconfig for components to be included/excluded and tests to be run
            config = os.path.join(PROJECT_PATH, "configs", config_name)

            with open(config, "r") as config_file:
                config_file_content = config_file.read()

                set_config_build_variables("EXCLUDE_COMPONENTS", "''")

                test_components = set_config_build_variables("TEST_COMPONENTS", "''")

                tests_all = None
                if test_components == "''":
                    tests_all = "TESTS_ALL=1"
                else:
                    tests_all = "TESTS_ALL=0"

                try:
                    args.define_cache_entry.append(tests_all)
                except AttributeError:
                    args.define_cache_entry = [tests_all]

                set_config_build_variables("TEST_EXCLUDE_COMPONENTS","''")

            with tempfile.NamedTemporaryFile() as sdkconfig_temp:
                # Use values from the combined defaults and the values from
                # config folder to build config
                sdkconfig_default = os.path.join(PROJECT_PATH, "sdkconfig.defaults")

                with open(sdkconfig_default, "r") as sdkconfig_default_file:
                    sdkconfig_temp.write(sdkconfig_default_file.read())

                sdkconfig_config = os.path.join(PROJECT_PATH, "configs", config_name)
                with open(sdkconfig_config, "r") as sdkconfig_config_file:
                    sdkconfig_temp.write("\n")
                    sdkconfig_temp.write(sdkconfig_config_file.read())

                sdkconfig_temp.flush()

                try:
                    args.define_cache_entry.append("SDKCONFIG_DEFAULTS=" + sdkconfig_temp.name)
                except AttributeError:
                    args.define_cache_entry = ["SDKCONFIG_DEFAULTS=" + sdkconfig_temp.name]

                reconfigure = base_functions["reconfigure"]
                reconfigure(None, args)
        else:
            if not config_name == "all-configs":
                print("unknown unit test app config for action '%s'" % ut_apply_config_name)

    # This target builds the configuration. It does not currently track dependencies,
    # but is good enough for CI builds if used together with clean-all-configs.
    # For local builds, use 'apply-config-NAME' target and then use normal 'all'
    # and 'flash' targets.
    def ut_build(ut_build_name, args):
        # Create a copy of the passed arguments to prevent arg modifications to accrue if 
        # all configs are being built
        build_args = copy.copy(args)

        config_name = re.match(r"ut-build-(.*)", ut_build_name).group(1)

        if config_name in CONFIG_NAMES:
            build_args.build_dir = os.path.join(BUILDS_DIR, config_name)

            src = os.path.join(BUILDS_DIR, config_name)
            dest = os.path.join(BINARIES_DIR, config_name)

            try:
                os.makedirs(dest)
            except OSError:
                pass

            # Build, tweaking paths to sdkconfig and sdkconfig.defaults
            ut_apply_config("ut-apply-config-" + config_name, build_args)

            build_target = base_functions["build_target"]

            build_target("all", build_args)

            # Copy artifacts to the output directory
            shutil.copyfile(os.path.join(build_args.project_dir, "sdkconfig"), os.path.join(dest, "sdkconfig"))

            binaries = [PROJECT_NAME + x for x in [".elf", ".bin", ".map"]]

            for binary in binaries:
                shutil.copyfile(os.path.join(src, binary), os.path.join(dest, binary))

            try:
                os.mkdir(os.path.join(dest, "bootloader"))
            except OSError:
                pass

            shutil.copyfile(os.path.join(src, "bootloader", "bootloader.bin"), os.path.join(dest, "bootloader", "bootloader.bin"))

            for partition_table in glob.glob(os.path.join(src, "partition_table", "partition-table*.bin")):
                try:
                    os.mkdir(os.path.join(dest, "partition_table"))
                except OSError:
                    pass
                shutil.copyfile(partition_table, os.path.join(dest, "partition_table", os.path.basename(partition_table)))

            shutil.copyfile(os.path.join(src, "flash_project_args"), os.path.join(dest, "flash_project_args"))

            binaries = glob.glob(os.path.join(src, "*.bin"))
            binaries = [os.path.basename(s) for s in binaries]

            for binary in binaries:
                shutil.copyfile(os.path.join(src, binary), os.path.join(dest, binary))

        else:
            if not config_name == "all-configs":
                print("unknown unit test app config for action '%s'" % ut_build_name)

    def ut_clean(ut_clean_name, args):
        config_name = re.match(r"ut-clean-(.*)", ut_clean_name).group(1)
        if config_name in CONFIG_NAMES:
            shutil.rmtree(os.path.join(BUILDS_DIR, config_name), ignore_errors=True)
            shutil.rmtree(os.path.join(BINARIES_DIR, config_name), ignore_errors=True)
        else:
            if not config_name == "all-configs":
                print("unknown unit test app config for action '%s'" % ut_clean_name)

    def ut_help(action, args):
        HELP_STRING = """
Additional unit-test-app specific targets

idf.py ut-build-NAME - Build unit-test-app with configuration provided in configs/NAME.
                    Build directory will be builds/NAME/, output binaries will be
                    under output/NAME/

idf.py ut-clean-NAME - Remove build and output directories for configuration NAME.

idf.py ut-build-all-configs - Build all configurations defined in configs/ directory.

idf.py ut-apply-config-NAME - Generates configuration based on configs/NAME in sdkconfig
                    file. After this, normal all/flash targets can be used.
                    Useful for development/debugging.
"""
        print(HELP_STRING)

    # Build dictionary of action extensions
    extensions = dict()

    # This generates per-config targets (clean, build, apply-config).
    build_all_config_deps = []
    clean_all_config_deps = []

    for config in CONFIG_NAMES:
        config_build_action_name = "ut-build-" + config
        config_clean_action_name = "ut-clean-" + config
        config_apply_config_action_name = "ut-apply-config-" + config

        extensions[config_build_action_name] = (ut_build, [], [])
        extensions[config_clean_action_name] = (ut_clean, [], [])
        extensions[config_apply_config_action_name] = (ut_apply_config, [], [])

        build_all_config_deps.append(config_build_action_name)
        clean_all_config_deps.append(config_clean_action_name)

    extensions["ut-build-all-configs"] = (ut_build, build_all_config_deps, [])
    extensions["ut-clean-all-configs"] = (ut_clean, clean_all_config_deps, [])

    extensions["ut-help"] = (ut_help, [], [])

    base_actions.update(extensions)