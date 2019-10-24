import os
import shutil
import sys

import click

from idf_py_actions.constants import GENERATOR_CMDS, GENERATOR_VERBOSE, SUPPORTED_TARGETS
from idf_py_actions.errors import FatalError
from idf_py_actions.global_options import global_options
from idf_py_actions.tools import ensure_build_directory, idf_version, merge_action_lists, realpath, run_tool


def action_extensions(base_actions, project_path):
    def build_target(target_name, ctx, args):
        """
        Execute the target build system to build target 'target_name'

        Calls ensure_build_directory() which will run cmake to generate a build
        directory (with the specified generator) as needed.
        """
        ensure_build_directory(args, ctx.info_name)
        generator_cmd = GENERATOR_CMDS[args.generator]

        if args.verbose:
            generator_cmd += [GENERATOR_VERBOSE[args.generator]]

        run_tool(generator_cmd[0], generator_cmd + [target_name], args.build_dir)

    def verbose_callback(ctx, param, value):
        if not value or ctx.resilient_parsing:
            return

        for line in ctx.command.verbose_output:
            print(line)

    def clean(action, ctx, args):
        if not os.path.isdir(args.build_dir):
            print("Build directory '%s' not found. Nothing to clean." % args.build_dir)
            return
        build_target("clean", ctx, args)

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
            raise FatalError("Directory '%s' doesn't seem to be a CMake build directory. Refusing to automatically "
                             "delete files in this directory. Delete the directory manually to 'clean' it." % build_dir)
        red_flags = ["CMakeLists.txt", ".git", ".svn"]
        for red in red_flags:
            red = os.path.join(build_dir, red)
            if os.path.exists(red):
                raise FatalError(
                    "Refusing to automatically delete files in directory containing '%s'. Delete files manually if you're sure."
                    % red)
        # OK, delete everything in the build directory...
        # Note: Python 2.7 doesn't detect symlinks on Windows (it is supported form 3.2). Tools promising to not
        # follow symlinks will actually follow them. Deleting the build directory with symlinks deletes also items
        # outside of this directory.
        deleted_symlinks = _delete_windows_symlinks(build_dir)
        if args.verbose and len(deleted_symlinks) > 1:
            print("The following symlinks were identified and removed:\n%s" % "\n".join(deleted_symlinks))
        for f in os.listdir(build_dir):  # TODO: once we are Python 3 only, this can be os.scandir()
            f = os.path.join(build_dir, f)
            if args.verbose:
                print("Removing: %s" % f)
            if os.path.isdir(f):
                shutil.rmtree(f)
            else:
                os.remove(f)

    def set_target(action, ctx, args, idf_target):
        args.define_cache_entry.append("IDF_TARGET=" + idf_target)
        sdkconfig_path = os.path.join(args.project_dir, 'sdkconfig')
        sdkconfig_old = sdkconfig_path + ".old"
        if os.path.exists(sdkconfig_old):
            os.remove(sdkconfig_old)
        if os.path.exists(sdkconfig_path):
            os.rename(sdkconfig_path, sdkconfig_old)
        print("Set Target to: %s, new sdkconfig created. Existing sdkconfig renamed to sdkconfig.old." % idf_target)
        ensure_build_directory(args, ctx.info_name, True)

    def reconfigure(action, ctx, args):
        ensure_build_directory(args, ctx.info_name, True)

    def validate_root_options(ctx, args, tasks):
        args.project_dir = realpath(args.project_dir)
        if args.build_dir is not None and args.project_dir == realpath(args.build_dir):
            raise FatalError("Setting the build directory to the project directory is not supported. Suggest dropping "
                             "--build-dir option, the default is a 'build' subdirectory inside the project directory.")
        if args.build_dir is None:
            args.build_dir = os.path.join(args.project_dir, "build")
        args.build_dir = realpath(args.build_dir)

    def idf_version_callback(ctx, param, value):
        if not value or ctx.resilient_parsing:
            return

        version = idf_version()

        if not version:
            raise FatalError("ESP-IDF version cannot be determined")

        print("ESP-IDF %s" % version)
        sys.exit(0)

    root_options = {
        "global_options": [
            {
                "names": ["--version"],
                "help": "Show IDF version and exit.",
                "is_flag": True,
                "callback": idf_version_callback
            },
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
                "is_eager": True,
                "default": False,
                "callback": verbose_callback
            },
            {
                "names": ["--ccache/--no-ccache"],
                "help": ("Use ccache in build. Disabled by default, unless "
                         "IDF_CCACHE_ENABLE environment variable is set to a non-zero value."),
                "is_flag": True,
                "default": os.getenv("IDF_CCACHE_ENABLE") not in [None, "", "0"],
            },
            {
                "names": ["-G", "--generator"],
                "help": "CMake generator.",
                "type": click.Choice(GENERATOR_CMDS.keys()),
            },
            {
                "names": ["--dry-run"],
                "help": "Only process arguments, but don't execute actions.",
                "is_flag": True,
                "hidden": True,
                "default": False
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
                "help": ("Build the project. This can involve multiple steps:\n\n"
                         "1. Create the build directory if needed. "
                         "The sub-directory 'build' is used to hold build output, "
                         "although this can be changed with the -B option.\n\n"
                         "2. Run CMake as necessary to configure the project "
                         "and generate build files for the main build tool.\n\n"
                         "3. Run the main build tool (Ninja or GNU Make). "
                         "By default, the build tool is automatically detected "
                         "but it can be explicitly set by passing the -G option to idf.py.\n\n"),
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
                "help": ("Re-run CMake even if it doesn't seem to need re-running. "
                         "This isn't necessary during normal usage, "
                         "but can be useful after adding/removing files from the source tree, "
                         "or when modifying CMake cache variables. "
                         "For example, \"idf.py -DNAME='VALUE' reconfigure\" "
                         'can be used to set variable "NAME" in CMake cache to value "VALUE".'),
                "options": global_options,
                "order_dependencies": ["menuconfig", "fullclean"],
            },
            "set-target": {
                "callback": set_target,
                "short_help": "Set the chip target to build.",
                "help": ("Set the chip target to build. This will remove the "
                         "existing sdkconfig file and corresponding CMakeCache and "
                         "create new ones according to the new target.\nFor example, "
                         "\"idf.py set-target esp32\" will select esp32 as the new chip "
                         "target."),
                "arguments": [
                    {
                        "names": ["idf-target"],
                        "nargs": 1,
                        "type": click.Choice(SUPPORTED_TARGETS),
                    },
                ],
                "dependencies": ["fullclean"],
            },
            "clean": {
                "callback": clean,
                "short_help": "Delete build output files from the build directory.",
                "help": ("Delete build output files from the build directory, "
                         "forcing a 'full rebuild' the next time "
                         "the project is built. Cleaning doesn't delete "
                         "CMake configuration output and some other files"),
                "order_dependencies": ["fullclean"],
            },
            "fullclean": {
                "callback": fullclean,
                "short_help": "Delete the entire build directory contents.",
                "help": ("Delete the entire build directory contents. "
                         "This includes all CMake configuration output."
                         "The next time the project is built, "
                         "CMake will configure it from scratch. "
                         "Note that this option recursively deletes all files "
                         "in the build directory, so use with care."
                         "Project configuration is not deleted.")
            },
        }
    }

    return merge_action_lists(root_options, build_actions, clean_actions)
