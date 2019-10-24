import os
import re
import subprocess
import sys

from .constants import GENERATORS
from .errors import FatalError


def executable_exists(args):
    try:
        subprocess.check_output(args)
        return True

    except Exception:
        return False


def realpath(path):
    """
    Return the cannonical path with normalized case.

    It is useful on Windows to comparision paths in case-insensitive manner.
    On Unix and Mac OS X it works as `os.path.realpath()` only.
    """
    return os.path.normcase(os.path.realpath(path))


def _idf_version_from_cmake():
    version_path = os.path.join(os.environ["IDF_PATH"], "tools/cmake/version.cmake")
    regex = re.compile(r"^\s*set\s*\(\s*IDF_VERSION_([A-Z]{5})\s+(\d+)")
    ver = {}
    try:
        with open(version_path) as f:
            for line in f:
                m = regex.match(line)

                if m:
                    ver[m.group(1)] = m.group(2)

        return "v%s.%s.%s" % (ver["MAJOR"], ver["MINOR"], ver["PATCH"])
    except (KeyError, OSError):
        sys.stderr.write("WARNING: Cannot find ESP-IDF version in version.cmake\n")
        return None


def idf_version():
    """Print version of ESP-IDF"""

    #  Try to get version from git:
    try:
        version = subprocess.check_output([
            "git",
            "--git-dir=%s" % os.path.join(os.environ["IDF_PATH"], '.git'),
            "--work-tree=%s" % os.environ["IDF_PATH"], "describe", "--tags", "--dirty"
        ]).decode('utf-8', 'ignore').strip()
    except (subprocess.CalledProcessError, UnicodeError):
        # if failed, then try to parse cmake.version file
        sys.stderr.write("WARNING: Git version unavailable, reading from source\n")
        version = _idf_version_from_cmake()

    return version


def run_tool(tool_name, args, cwd):
    def quote_arg(arg):
        " Quote 'arg' if necessary "
        if " " in arg and not (arg.startswith('"') or arg.startswith("'")):
            return "'" + arg + "'"
        return arg

    display_args = " ".join(quote_arg(arg) for arg in args)
    print("Running %s in directory %s" % (tool_name, quote_arg(cwd)))
    print('Executing "%s"...' % str(display_args))
    try:
        # Note: we explicitly pass in os.environ here, as we may have set IDF_PATH there during startup
        subprocess.check_call(args, env=os.environ, cwd=cwd)
    except subprocess.CalledProcessError as e:
        raise FatalError("%s failed with exit code %d" % (tool_name, e.returncode))


def _strip_quotes(value, regexp=re.compile(r"^\"(.*)\"$|^'(.*)'$|^(.*)$")):
    """
    Strip quotes like CMake does during parsing cache entries
    """

    return [x for x in regexp.match(value).groups() if x is not None][0].rstrip()


def _parse_cmakecache(path):
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
            m = re.match(r"^([^#/:=]+):([^:=]+)=(.*)\n$", line)
            if m:
                result[m.group(1)] = m.group(3)
    return result


def _new_cmakecache_entries(cache_path, new_cache_entries):
    if not os.path.exists(cache_path):
        return True

    if new_cache_entries:
        current_cache = _parse_cmakecache(cache_path)

        for entry in new_cache_entries:
            key, value = entry.split("=", 1)
            current_value = current_cache.get(key, None)
            if current_value is None or _strip_quotes(value) != current_value:
                return True

    return False


def _detect_cmake_generator(prog_name):
    """
    Find the default cmake generator, if none was specified. Raises an exception if no valid generator is found.
    """
    for (generator, _, version_check, _) in GENERATORS:
        if executable_exists(version_check):
            return generator
    raise FatalError("To use %s, either the 'ninja' or 'GNU make' build tool must be available in the PATH" % prog_name)


def ensure_build_directory(args, prog_name, always_run_cmake=False):
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
            raise FatalError("Project directory %s does not exist" % project_dir)
        else:
            raise FatalError("%s must be a project directory" % project_dir)
    if not os.path.exists(os.path.join(project_dir, "CMakeLists.txt")):
        raise FatalError("CMakeLists.txt not found in project directory %s" % project_dir)

    # Verify/create the build directory
    build_dir = args.build_dir
    if not os.path.isdir(build_dir):
        os.makedirs(build_dir)
    cache_path = os.path.join(build_dir, "CMakeCache.txt")

    args.define_cache_entry.append("CCACHE_ENABLE=%d" % args.ccache)

    if always_run_cmake or _new_cmakecache_entries(cache_path, args.define_cache_entry):
        if args.generator is None:
            args.generator = _detect_cmake_generator(prog_name)
        try:
            cmake_args = [
                "cmake",
                "-G",
                args.generator,
                "-DPYTHON_DEPS_CHECKED=1",
                "-DESP_PLATFORM=1",
            ]
            if not args.no_warnings:
                cmake_args += ["--warn-uninitialized"]

            if args.define_cache_entry:
                cmake_args += ["-D" + d for d in args.define_cache_entry]
            cmake_args += [project_dir]

            run_tool("cmake", cmake_args, cwd=args.build_dir)
        except Exception:
            # don't allow partially valid CMakeCache.txt files,
            # to keep the "should I run cmake?" logic simple
            if os.path.exists(cache_path):
                os.remove(cache_path)
            raise

    # Learn some things from the CMakeCache.txt file in the build directory
    cache = _parse_cmakecache(cache_path)
    try:
        generator = cache["CMAKE_GENERATOR"]
    except KeyError:
        generator = _detect_cmake_generator(prog_name)
    if args.generator is None:
        args.generator = (generator)  # reuse the previously configured generator, if none was given
    if generator != args.generator:
        raise FatalError("Build is configured for generator '%s' not '%s'. Run '%s fullclean' to start again." %
                         (generator, args.generator, prog_name))

    try:
        home_dir = cache["CMAKE_HOME_DIRECTORY"]
        if realpath(home_dir) != realpath(project_dir):
            raise FatalError(
                "Build directory '%s' configured for project '%s' not '%s'. Run '%s fullclean' to start again." %
                (build_dir, realpath(home_dir), realpath(project_dir), prog_name))
    except KeyError:
        pass  # if cmake failed part way, CMAKE_HOME_DIRECTORY may not be set yet


def merge_action_lists(*action_lists):
    merged_actions = {
        "global_options": [],
        "actions": {},
        "global_action_callbacks": [],
    }
    for action_list in action_lists:
        merged_actions["global_options"].extend(action_list.get("global_options", []))
        merged_actions["actions"].update(action_list.get("actions", {}))
        merged_actions["global_action_callbacks"].extend(action_list.get("global_action_callbacks", []))
    return merged_actions
