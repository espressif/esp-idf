import copy
import glob
import os
import os.path
import re
import shutil
import tempfile


def action_extensions(base_actions, project_path=os.getcwd()):
    """ Describes extensions for unit tests. This function expects that actions "all" and "reconfigure" """

    PROJECT_NAME = "unit-test-app"

    # List of unit-test-app configurations.
    # Each file in configs/ directory defines a configuration. The format is the
    # same as sdkconfig file. Configuration is applied on top of sdkconfig.defaults
    # file from the project directory
    CONFIG_NAMES = os.listdir(os.path.join(project_path, "configs"))

    # Build (intermediate) and output (artifact) directories
    BUILDS_DIR = os.path.join(project_path, "builds")
    BINARIES_DIR = os.path.join(project_path, "output")

    def ut_apply_config(ut_apply_config_name, ctx, args):
        config_name = re.match(r"ut-apply-config-(.*)", ut_apply_config_name).group(1)

        def set_config_build_variables(prop, defval=None):
            property_value = re.findall(r"^%s=(.+)" % prop, config_file_content, re.MULTILINE)
            if property_value:
                property_value = property_value[0]
            else:
                property_value = defval

            if property_value:
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
            config = os.path.join(project_path, "configs", config_name)

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

                set_config_build_variables("TEST_EXCLUDE_COMPONENTS", "''")

            with tempfile.NamedTemporaryFile() as sdkconfig_temp:
                # Use values from the combined defaults and the values from
                # config folder to build config
                sdkconfig_default = os.path.join(project_path, "sdkconfig.defaults")

                with open(sdkconfig_default, "rb") as sdkconfig_default_file:
                    sdkconfig_temp.write(sdkconfig_default_file.read())

                sdkconfig_config = os.path.join(project_path, "configs", config_name)
                with open(sdkconfig_config, "rb") as sdkconfig_config_file:
                    sdkconfig_temp.write(b"\n")
                    sdkconfig_temp.write(sdkconfig_config_file.read())

                sdkconfig_temp.flush()

                try:
                    args.define_cache_entry.append("SDKCONFIG_DEFAULTS=" + sdkconfig_temp.name)
                except AttributeError:
                    args.define_cache_entry = ["SDKCONFIG_DEFAULTS=" + sdkconfig_temp.name]

                reconfigure = base_actions["actions"]["reconfigure"]["callback"]
                reconfigure(None, ctx, args)
        else:
            if not config_name == "all-configs":
                print("unknown unit test app config for action '%s'" % ut_apply_config_name)

    # This target builds the configuration. It does not currently track dependencies,
    # but is good enough for CI builds if used together with clean-all-configs.
    # For local builds, use 'apply-config-NAME' target and then use normal 'all'
    # and 'flash' targets.
    def ut_build(ut_build_name, ctx, args):
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
            ut_apply_config("ut-apply-config-" + config_name, ctx, build_args)

            build_target = base_actions["actions"]["all"]["callback"]

            build_target("all", ctx, build_args)

            # Copy artifacts to the output directory
            shutil.copyfile(
                os.path.join(build_args.project_dir, "sdkconfig"),
                os.path.join(dest, "sdkconfig"),
            )

            binaries = [PROJECT_NAME + x for x in [".elf", ".bin", ".map"]]

            for binary in binaries:
                shutil.copyfile(os.path.join(src, binary), os.path.join(dest, binary))

            try:
                os.mkdir(os.path.join(dest, "bootloader"))
            except OSError:
                pass

            shutil.copyfile(
                os.path.join(src, "bootloader", "bootloader.bin"),
                os.path.join(dest, "bootloader", "bootloader.bin"),
            )

            for partition_table in glob.glob(os.path.join(src, "partition_table", "partition-table*.bin")):
                try:
                    os.mkdir(os.path.join(dest, "partition_table"))
                except OSError:
                    pass
                shutil.copyfile(
                    partition_table,
                    os.path.join(dest, "partition_table", os.path.basename(partition_table)),
                )

            shutil.copyfile(
                os.path.join(src, "flasher_args.json"),
                os.path.join(dest, "flasher_args.json"),
            )

            binaries = glob.glob(os.path.join(src, "*.bin"))
            binaries = [os.path.basename(s) for s in binaries]

            for binary in binaries:
                shutil.copyfile(os.path.join(src, binary), os.path.join(dest, binary))

        else:
            if not config_name == "all-configs":
                print("unknown unit test app config for action '%s'" % ut_build_name)

    def ut_clean(ut_clean_name, ctx, args):
        config_name = re.match(r"ut-clean-(.*)", ut_clean_name).group(1)
        if config_name in CONFIG_NAMES:
            shutil.rmtree(os.path.join(BUILDS_DIR, config_name), ignore_errors=True)
            shutil.rmtree(os.path.join(BINARIES_DIR, config_name), ignore_errors=True)
        else:
            if not config_name == "all-configs":
                print("unknown unit test app config for action '%s'" % ut_clean_name)

    def test_component_callback(ctx, global_args, tasks):
        """ Convert the values passed to the -T and -E parameter to corresponding cache entry definitions TESTS_ALL and TEST_COMPONENTS """
        test_components = global_args.test_components
        test_exclude_components = global_args.test_exclude_components

        cache_entries = []

        if test_components:
            if "all" in test_components:
                cache_entries.append("TESTS_ALL=1")
                cache_entries.append("TEST_COMPONENTS=''")
            else:
                cache_entries.append("TESTS_ALL=0")
                cache_entries.append("TEST_COMPONENTS='%s'" % " ".join(test_components))

        if test_exclude_components:
            cache_entries.append("TEST_EXCLUDE_COMPONENTS='%s'" % " ".join(test_exclude_components))

        if cache_entries:
            global_args.define_cache_entry = list(global_args.define_cache_entry)
            global_args.define_cache_entry.extend(cache_entries)

            # Brute force add reconfigure at the very beginning
            reconfigure_task = ctx.invoke(ctx.command.get_command(ctx, "reconfigure"))
            # Strip arguments from the task
            reconfigure_task.action_args = {}
            tasks.insert(0, reconfigure_task)

    # Add global options
    extensions = {
        "global_options": [{
            "names": ["-T", "--test-components"],
            "help": "Specify the components to test.",
            "scope": "shared",
            "multiple": True,
        }, {
            "names": ["-E", "--test-exclude-components"],
            "help": "Specify the components to exclude from testing.",
            "scope": "shared",
            "multiple": True,
        }],
        "global_action_callbacks": [test_component_callback],
        "actions": {},
    }

    # This generates per-config targets (clean, build, apply-config).
    build_all_config_deps = []
    clean_all_config_deps = []

    for config in CONFIG_NAMES:
        config_build_action_name = "ut-build-" + config
        config_clean_action_name = "ut-clean-" + config
        config_apply_config_action_name = "ut-apply-config-" + config

        extensions["actions"][config_build_action_name] = {
            "callback":
            ut_build,
            "help":
            ("Build unit-test-app with configuration provided in configs/%s. "
             "Build directory will be builds/%s/, output binaries will be under output/%s/" % (config, config, config)),
        }

        extensions["actions"][config_clean_action_name] = {
            "callback": ut_clean,
            "help": "Remove build and output directories for configuration %s." % config_clean_action_name,
        }

        extensions["actions"][config_apply_config_action_name] = {
            "callback":
            ut_apply_config,
            "help":
            "Generates configuration based on configs/%s in sdkconfig file. " % config_apply_config_action_name +
            "After this, normal all/flash targets can be used. Useful for development/debugging.",
        }

        build_all_config_deps.append(config_build_action_name)
        clean_all_config_deps.append(config_clean_action_name)

    extensions["actions"]["ut-build-all-configs"] = {
        "callback": ut_build,
        "help": "Build all configurations defined in configs/ directory.",
        "dependencies": build_all_config_deps,
    }

    extensions["actions"]["ut-clean-all-configs"] = {
        "callback": ut_clean,
        "help": "Remove build and output directories for all configurations defined in configs/ directory.",
        "dependencies": clean_all_config_deps,
    }

    return extensions
