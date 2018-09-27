#!/usr/bin/env python
#
# 'idf.py' is a top-level config/build command line tool for ESP-IDF
#
# You don't have to use idf.py, you can use cmake directly
# (or use cmake in an IDE)
#
#
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

# Note: we don't check for Python build-time dependencies until
# check_environment() function below. If possible, avoid importing
# any external libraries here - put in external script, or import in
# their specific function instead.
import sys
import argparse
import os
import os.path
import subprocess
import multiprocessing
import re
import shutil
import json
import serial.tools.list_ports

class FatalError(RuntimeError):
    """
    Wrapper class for runtime errors that aren't caused by bugs in idf.py or the build proces.s
    """
    pass

# Use this Python interpreter for any subprocesses we launch
PYTHON=sys.executable

# note: os.environ changes don't automatically propagate to child processes,
# you have to pass env=os.environ explicitly anywhere that we create a process
os.environ["PYTHON"]=sys.executable

# Make flavors, across the various kinds of Windows environments & POSIX...
if "MSYSTEM" in os.environ:  # MSYS
    MAKE_CMD = "make"
    MAKE_GENERATOR = "MSYS Makefiles"
elif os.name == 'nt':  # other Windows
    MAKE_CMD = "mingw32-make"
    MAKE_GENERATOR = "MinGW Makefiles"
else:
    MAKE_CMD = "make"
    MAKE_GENERATOR = "Unix Makefiles"

GENERATORS = [
    # ('generator name', 'build command line', 'version command line', 'verbose flag')
    ("Ninja", [ "ninja" ], [ "ninja", "--version" ], "-v"),
    (MAKE_GENERATOR, [ MAKE_CMD, "-j", str(multiprocessing.cpu_count()+2) ], [ "make", "--version" ], "VERBOSE=1"),
    ]
GENERATOR_CMDS = dict( (a[0], a[1]) for a in GENERATORS )
GENERATOR_VERBOSE = dict( (a[0], a[3]) for a in GENERATORS )

def _run_tool(tool_name, args, cwd):
    def quote_arg(arg):
        " Quote 'arg' if necessary "
        if " " in arg and not (arg.startswith('"') or arg.startswith("'")):
            return "'" + arg + "'"
        return arg
    display_args = " ".join(quote_arg(arg) for arg in args)
    print("Running %s in directory %s" % (tool_name, quote_arg(cwd)))
    print('Executing "%s"...' % display_args)
    try:
        # Note: we explicitly pass in os.environ here, as we may have set IDF_PATH there during startup
        subprocess.check_call(args, env=os.environ, cwd=cwd)
    except subprocess.CalledProcessError as e:
        raise FatalError("%s failed with exit code %d" % (tool_name, e.returncode))


def check_environment():
    """
    Verify the environment contains the top-level tools we need to operate

    (cmake will check a lot of other things)
    """
    if not executable_exists(["cmake", "--version"]):
        raise FatalError("'cmake' must be available on the PATH to use idf.py")
    # find the directory idf.py is in, then the parent directory of this, and assume this is IDF_PATH
    detected_idf_path = os.path.realpath(os.path.join(os.path.dirname(__file__), ".."))
    if "IDF_PATH" in os.environ:
        set_idf_path = os.path.realpath(os.environ["IDF_PATH"])
        if set_idf_path != detected_idf_path:
            print("WARNING: IDF_PATH environment variable is set to %s but idf.py path indicates IDF directory %s. Using the environment variable directory, but results may be unexpected..."
                  % (set_idf_path, detected_idf_path))
    else:
        print("Setting IDF_PATH environment variable: %s" % detected_idf_path)
        os.environ["IDF_PATH"] = detected_idf_path

    # check Python dependencies
    print("Checking Python dependencies...")
    try:
        subprocess.check_call([ os.environ["PYTHON"],
                                os.path.join(os.environ["IDF_PATH"], "tools", "check_python_dependencies.py")],
                              env=os.environ)
    except subprocess.CalledProcessError:
        raise SystemExit(1)

def executable_exists(args):
    try:
        subprocess.check_output(args)
        return True
    except:
        return False

def detect_cmake_generator():
    """
    Find the default cmake generator, if none was specified. Raises an exception if no valid generator is found.
    """
    for (generator, _, version_check, _) in GENERATORS:
        if executable_exists(version_check):
            return generator
    raise FatalError("To use idf.py, either the 'ninja' or 'GNU make' build tool must be available in the PATH")

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
            raise FatalError("Project directory %s does not exist")
        else:
            raise FatalError("%s must be a project directory")
    if not os.path.exists(os.path.join(project_dir, "CMakeLists.txt")):
        raise FatalError("CMakeLists.txt not found in project directory %s" % project_dir)

    # Verify/create the build directory
    build_dir = args.build_dir
    if not os.path.isdir(build_dir):
        os.mkdir(build_dir)
    cache_path = os.path.join(build_dir, "CMakeCache.txt")
    if not os.path.exists(cache_path) or always_run_cmake:
        if args.generator is None:
            args.generator = detect_cmake_generator()
        try:
            cmake_args = ["cmake", "-G", args.generator, "-DPYTHON_DEPS_CHECKED=1"]
            if not args.no_warnings:
                cmake_args += [ "--warn-uninitialized" ]
            if args.no_ccache:
                cmake_args += [ "-DCCACHE_DISABLE=1" ]
            cmake_args += [ project_dir]
            _run_tool("cmake", cmake_args, cwd=args.build_dir)
        except:
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
        args.generator = generator  # reuse the previously configured generator, if none was given
    if generator != args.generator:
        raise FatalError("Build is configured for generator '%s' not '%s'. Run 'idf.py fullclean' to start again."
                           % (generator, args.generator))

    try:
        home_dir = cache["CMAKE_HOME_DIRECTORY"]
        if os.path.normcase(os.path.realpath(home_dir)) != os.path.normcase(os.path.realpath(project_dir)):
            raise FatalError("Build directory '%s' configured for project '%s' not '%s'. Run 'idf.py fullclean' to start again."
                            % (build_dir, os.path.realpath(home_dir), os.path.realpath(project_dir)))
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
            m = re.match(r"^([^#/:=]+):([^:=]+)=(.+)\n$", line)
            if m:
               result[m.group(1)] = m.group(3)
    return result

def build_target(target_name, args):
    """
    Execute the target build system to build target 'target_name'

    Calls _ensure_build_directory() which will run cmake to generate a build
    directory (with the specified generator) as needed.
    """
    _ensure_build_directory(args)
    generator_cmd = GENERATOR_CMDS[args.generator]
    if not args.no_ccache:
        # Setting CCACHE_BASEDIR & CCACHE_NO_HASHDIR ensures that project paths aren't stored in the ccache entries
        # (this means ccache hits can be shared between different projects. It may mean that some debug information
        # will point to files in another project, if these files are perfect duplicates of each other.)
        #
        # It would be nicer to set these from cmake, but there's no cross-platform way to set build-time environment
        #os.environ["CCACHE_BASEDIR"] = args.build_dir
        #os.environ["CCACHE_NO_HASHDIR"] = "1"
        pass
    if args.verbose:
        generator_cmd += [ GENERATOR_VERBOSE[args.generator] ]

    _run_tool(generator_cmd[0], generator_cmd + [target_name], args.build_dir)


def _get_esptool_args(args):
    esptool_path = os.path.join(os.environ["IDF_PATH"], "components/esptool_py/esptool/esptool.py")
    if args.port is None:
        args.port = get_default_serial_port()
    result = [ PYTHON, esptool_path ]
    result += [ "-p", args.port ]
    result += [ "-b", str(args.baud) ]
    return result

def flash(action, args):
    """
    Run esptool to flash the entire project, from an argfile generated by the build system
    """
    flasher_args_path = {  # action -> name of flasher args file generated by build system
        "bootloader-flash":      "flash_bootloader_args",
        "partition_table-flash": "flash_partition_table_args",
        "app-flash":             "flash_app_args",
        "flash":                 "flash_project_args",
    }[action]
    esptool_args = _get_esptool_args(args)
    esptool_args += [ "write_flash", "@"+flasher_args_path ]
    _run_tool("esptool.py", esptool_args, args.build_dir)


def erase_flash(action, args):
    esptool_args = _get_esptool_args(args)
    esptool_args += [ "erase_flash" ]
    _run_tool("esptool.py", esptool_args, args.build_dir)


def monitor(action, args):
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
        raise FatalError("ELF file '%s' not found. You need to build & flash the project before running 'monitor', and the binary on the device must match the one in the build directory exactly. Try 'idf.py flash monitor'." % elf_file)
    idf_monitor = os.path.join(os.environ["IDF_PATH"], "tools/idf_monitor.py")
    monitor_args = [PYTHON, idf_monitor ]
    if args.port is not None:
        monitor_args += [ "-p", args.port ]
    monitor_args += [ "-b", project_desc["monitor_baud"] ]
    monitor_args += [ elf_file ]

    idf_py = [ PYTHON ] + get_commandline_options()  # commands to re-run idf.py
    monitor_args += [ "-m", " ".join("'%s'" % a for a in idf_py) ]

    if "MSYSTEM" is os.environ:
        monitor_args = [ "winpty" ] + monitor_args
    _run_tool("idf_monitor", monitor_args, args.project_dir)


def clean(action, args):
    if not os.path.isdir(args.build_dir):
        print("Build directory '%s' not found. Nothing to clean." % args.build_dir)
        return
    build_target("clean", args)

def reconfigure(action, args):
    _ensure_build_directory(args, True)

def fullclean(action, args):
    build_dir = args.build_dir
    if not os.path.isdir(build_dir):
        print("Build directory '%s' not found. Nothing to clean." % build_dir)
        return
    if len(os.listdir(build_dir)) == 0:
        print("Build directory '%s' is empty. Nothing to clean." % build_dir)
        return

    if not os.path.exists(os.path.join(build_dir, "CMakeCache.txt")):
        raise FatalError("Directory '%s' doesn't seem to be a CMake build directory. Refusing to automatically delete files in this directory. Delete the directory manually to 'clean' it." % build_dir)
    red_flags = [ "CMakeLists.txt", ".git", ".svn" ]
    for red in red_flags:
        red = os.path.join(build_dir, red)
        if os.path.exists(red):
            raise FatalError("Refusing to automatically delete files in directory containing '%s'. Delete files manually if you're sure." % red)
    # OK, delete everything in the build directory...
    for f in os.listdir(build_dir):  # TODO: once we are Python 3 only, this can be os.scandir()
        f = os.path.join(build_dir, f)
        if os.path.isdir(f):
            shutil.rmtree(f)
        else:
            os.remove(f)

def print_closing_message(args):
    # print a closing message of some kind
    #

    if "flash" in str(args.actions):
        print("Done")
        return

    # Otherwise, if we built any binaries print a message about
    # how to flash them
    def print_flashing_message(title, key):
        print("\n%s build complete. To flash, run this command:" % title)

        with open(os.path.join(args.build_dir, "flasher_args.json")) as f:
            flasher_args = json.load(f)

        def flasher_path(f):
            return os.path.relpath(os.path.join(args.build_dir, f))

        if key != "project":  # flashing a single item
            cmd = ""
            if key == "bootloader":  # bootloader needs --flash-mode, etc to be passed in
                cmd = " ".join(flasher_args["write_flash_args"]) + " "

            cmd += flasher_args[key]["offset"] + " "
            cmd += flasher_path(flasher_args[key]["file"])
        else:  # flashing the whole project
            cmd = " ".join(flasher_args["write_flash_args"]) + " "
            flash_items = sorted(((o,f) for (o,f) in flasher_args["flash_files"].items() if len(o) > 0),
                                 key = lambda x: int(x[0], 0))
            for o,f in flash_items:
                cmd += o + " " + flasher_path(f) + " "

        print("%s -p %s -b %s write_flash %s" % (
            os.path.relpath("%s/components/esptool_py/esptool/esptool.py" % os.environ["IDF_PATH"]),
            args.port or "(PORT)",
            args.baud,
            cmd.strip()))
        print("or run 'idf.py -p %s %s'" % (args.port or "(PORT)", key + "-flash" if key != "project" else "flash",))

    if "all" in args.actions or "build" in args.actions:
        print_flashing_message("Project", "project")
    else:
        if "app" in args.actions:
            print_flashing_message("App", "app")
        if "partition_table" in args.actions:
            print_flashing_message("Partition Table", "partition_table")
        if "bootloader" in args.actions:
            print_flashing_message("Bootloader", "bootloader")

ACTIONS = {
    # action name : ( function (or alias), dependencies, order-only dependencies )
    "all" :                  ( build_target, [], [ "reconfigure", "menuconfig", "clean", "fullclean" ] ),
    "build":                 ( "all",        [], [] ),  # build is same as 'all' target
    "clean":                 ( clean,        [], [ "fullclean" ] ),
    "fullclean":             ( fullclean,    [], [] ),
    "reconfigure":           ( reconfigure,  [], [ "menuconfig" ] ),
    "menuconfig":            ( build_target, [], [] ),
    "confserver":            ( build_target, [], [] ),
    "size":                  ( build_target, [ "app" ], [] ),
    "size-components":       ( build_target, [ "app" ], [] ),
    "size-files":            ( build_target, [ "app" ], [] ),
    "bootloader":            ( build_target, [], [] ),
    "bootloader-clean":      ( build_target, [], [] ),
    "bootloader-flash":      ( flash,        [ "bootloader" ], [ "erase_flash"] ),
    "app":                   ( build_target, [], [ "clean", "fullclean", "reconfigure" ] ),
    "app-flash":             ( flash,        [ "app" ], [ "erase_flash"]),
    "partition_table":       ( build_target, [], [ "reconfigure" ] ),
    "partition_table-flash": ( flash,        [ "partition_table" ], [ "erase_flash" ]),
    "flash":                 ( flash,        [ "all" ], [ "erase_flash" ] ),
    "erase_flash":           ( erase_flash,  [], []),
    "monitor":               ( monitor,      [], [ "flash", "partition_table-flash", "bootloader-flash", "app-flash" ]),
}


def get_commandline_options():
    """ Return all the command line options up to but not including the action """
    result = []
    for a in sys.argv:
        if a in ACTIONS.keys():
            break
        else:
            result.append(a)
    return result

def get_default_serial_port():
    """ Return a default serial port. esptool can do this (smarter), but it can create
    inconsistencies where esptool.py uses one port and idf_monitor uses another.

    Same logic as esptool.py search order, reverse sort by name and choose the first port.
    """
    ports = list(reversed(sorted(
        p.device for p in serial.tools.list_ports.comports() )))
    try:
        print ("Choosing default port %s (use '-p PORT' option to set a specific serial port)" % ports[0])
        return ports[0]
    except IndexError:
        raise RuntimeError("No serial ports found. Connect a device, or use '-p PORT' option to set a specific port.")


def main():
    if sys.version_info[0] != 2 or sys.version_info[1] != 7:
        print("Note: You are using Python %d.%d.%d. Python 3 support is new, please report any problems "
              "you encounter. Search for 'Setting the Python Interpreter' in the ESP-IDF docs if you want to use "
              "Python 2.7." % sys.version_info[:3])

    parser = argparse.ArgumentParser(description='ESP-IDF build management tool')
    parser.add_argument('-p', '--port', help="Serial port",
                        default=os.environ.get('ESPPORT', None))
    parser.add_argument('-b', '--baud', help="Baud rate",
                        default=os.environ.get('ESPBAUD', 460800))
    parser.add_argument('-C', '--project-dir', help="Project directory", default=os.getcwd())
    parser.add_argument('-B', '--build-dir', help="Build directory", default=None)
    parser.add_argument('-G', '--generator', help="Cmake generator", choices=GENERATOR_CMDS.keys())
    parser.add_argument('-n', '--no-warnings', help="Disable Cmake warnings", action="store_true")
    parser.add_argument('-v', '--verbose', help="Verbose build output", action="store_true")
    parser.add_argument('--no-ccache', help="Disable ccache. Otherwise, if ccache is available on the PATH then it will be used for faster builds.", action="store_true")
    parser.add_argument('actions', help="Actions (build targets or other operations)", nargs='+',
                        choices=ACTIONS.keys())

    args = parser.parse_args()

    check_environment()

    # Advanced parameter checks
    if args.build_dir is not None and os.path.realpath(args.project_dir) == os.path.realpath(args.build_dir):
        raise FatalError("Setting the build directory to the project directory is not supported. Suggest dropping --build-dir option, the default is a 'build' subdirectory inside the project directory.")
    if args.build_dir is None:
        args.build_dir = os.path.join(args.project_dir, "build")
    args.build_dir = os.path.realpath(args.build_dir)

    completed_actions = set()
    def execute_action(action, remaining_actions):
        ( function, dependencies, order_dependencies ) = ACTIONS[action]
        # very simple dependency management, build a set of completed actions and make sure
        # all dependencies are in it
        for dep in dependencies:
            if not dep in completed_actions:
                execute_action(dep, remaining_actions)
        for dep in order_dependencies:
            if dep in remaining_actions and not dep in completed_actions:
                execute_action(dep, remaining_actions)

        if action in completed_actions:
            pass  # we've already done this, don't do it twice...
        elif function in ACTIONS:  # alias of another action
            execute_action(function, remaining_actions)
        else:
            function(action, args)

        completed_actions.add(action)

    actions = list(args.actions)
    while len(actions) > 0:
        execute_action(actions[0], actions[1:])
        actions.pop(0)

    print_closing_message(args)

if __name__ == "__main__":
    try:
        main()
    except FatalError as e:
        print(e)
        sys.exit(2)


