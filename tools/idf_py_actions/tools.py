import click
import os
import re
import subprocess
import sys
from io import open

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


def run_tool(tool_name, args, cwd, env=dict()):
    def quote_arg(arg):
        " Quote 'arg' if necessary "
        if " " in arg and not (arg.startswith('"') or arg.startswith("'")):
            return "'" + arg + "'"
        return arg

    args = [str(arg) for arg in args]
    display_args = " ".join(quote_arg(arg) for arg in args)
    print("Running %s in directory %s" % (tool_name, quote_arg(cwd)))
    print('Executing "%s"...' % str(display_args))

    env_copy = dict(os.environ)
    env_copy.update(env)

    if sys.version_info[0] < 3:
        # The subprocess lib cannot accept environment variables as "unicode". Convert to str.
        # This encoding step is required only in Python 2.
        for (key, val) in env_copy.items():
            if not isinstance(val, str):
                env_copy[key] = val.encode(sys.getfilesystemencoding() or 'utf-8')

    try:
        # Note: we explicitly pass in os.environ here, as we may have set IDF_PATH there during startup
        subprocess.check_call(args, env=env_copy, cwd=cwd)
    except subprocess.CalledProcessError as e:
        raise FatalError("%s failed with exit code %d" % (tool_name, e.returncode))


def run_target(target_name, args, env=dict()):
    generator_cmd = GENERATORS[args.generator]["command"]

    if args.verbose:
        generator_cmd += [GENERATORS[args.generator]["verbose_flag"]]

    run_tool(generator_cmd[0], generator_cmd + [target_name], args.build_dir, env)


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
    with open(path, encoding='utf-8') as f:
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
    for (generator_name,  generator) in GENERATORS.items():
        if executable_exists(generator["version"]):
            return generator_name
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

    # Parse CMakeCache, if it exists
    cache_path = os.path.join(build_dir, "CMakeCache.txt")
    cache = _parse_cmakecache(cache_path) if os.path.exists(cache_path) else {}

    # Validate or set IDF_TARGET
    _guess_or_check_idf_target(args, prog_name, cache)

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
            if args.cmake_warn_uninitialized:
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

    # need to update cache so subsequent access in this method would reflect the result of the previous cmake run
    cache = _parse_cmakecache(cache_path) if os.path.exists(cache_path) else {}

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


def get_sdkconfig_value(sdkconfig_file, key):
    """
    Return the value of given key from sdkconfig_file.
    If sdkconfig_file does not exist or the option is not present, returns None.
    """
    assert key.startswith("CONFIG_")
    if not os.path.exists(sdkconfig_file):
        return None
    # keep track of the last seen value for the given key
    value = None
    # if the value is quoted, this excludes the quotes from the value
    pattern = re.compile(r"^{}=\"?([^\"]*)\"?$".format(key))
    with open(sdkconfig_file, "r") as f:
        for line in f:
            match = re.match(pattern, line)
            if match:
                value = match.group(1)
    return value


def is_target_supported(project_path, supported_targets):
    """
    Returns True if the active target is supported, or False otherwise.
    """
    return get_sdkconfig_value(os.path.join(project_path, "sdkconfig"), 'CONFIG_IDF_TARGET') in supported_targets


def _guess_or_check_idf_target(args, prog_name, cache):
    """
    If CMakeCache.txt doesn't exist, and IDF_TARGET is not set in the environment, guess the value from
    sdkconfig or sdkconfig.defaults, and pass it to CMake in IDF_TARGET variable.

    Otherwise, cross-check the three settings (sdkconfig, CMakeCache, environment) and if there is
    mismatch, fail with instructions on how to fix this.
    """
    # Default locations of sdkconfig files.
    # FIXME: they may be overridden in the project or by a CMake variable (IDF-1369).
    sdkconfig_path = os.path.join(args.project_dir, "sdkconfig")
    sdkconfig_defaults_path = os.path.join(args.project_dir, "sdkconfig.defaults")

    # These are used to guess the target from sdkconfig, or set the default target by sdkconfig.defaults.
    idf_target_from_sdkconfig = get_sdkconfig_value(sdkconfig_path, "CONFIG_IDF_TARGET")
    idf_target_from_sdkconfig_defaults = get_sdkconfig_value(sdkconfig_defaults_path, "CONFIG_IDF_TARGET")
    idf_target_from_env = os.environ.get("IDF_TARGET")
    idf_target_from_cache = cache.get("IDF_TARGET")

    if not cache and not idf_target_from_env:
        # CMakeCache.txt does not exist yet, and IDF_TARGET is not set in the environment.
        guessed_target = idf_target_from_sdkconfig or idf_target_from_sdkconfig_defaults
        if guessed_target:
            if args.verbose:
                print("IDF_TARGET is not set, guessed '%s' from sdkconfig" % (guessed_target))
            args.define_cache_entry.append("IDF_TARGET=" + guessed_target)

    elif idf_target_from_env:
        # Let's check that IDF_TARGET values are consistent
        if idf_target_from_sdkconfig and idf_target_from_sdkconfig != idf_target_from_env:
            raise FatalError("Project sdkconfig was generated for target '{t_conf}', but environment variable IDF_TARGET "
                             "is set to '{t_env}'. Run '{prog} set-target {t_env}' to generate new sdkconfig file for target {t_env}."
                             .format(t_conf=idf_target_from_sdkconfig, t_env=idf_target_from_env, prog=prog_name))

        if idf_target_from_cache and idf_target_from_cache != idf_target_from_env:
            raise FatalError("Target settings are not consistent: '{t_env}' in the environment, '{t_cache}' in CMakeCache.txt. "
                             "Run '{prog} fullclean' to start again."
                             .format(t_env=idf_target_from_env, t_cache=idf_target_from_cache, prog=prog_name))

    elif idf_target_from_cache and idf_target_from_sdkconfig and idf_target_from_cache != idf_target_from_sdkconfig:
        # This shouldn't happen, unless the user manually edits CMakeCache.txt or sdkconfig, but let's check anyway.
        raise FatalError("Project sdkconfig was generated for target '{t_conf}', but CMakeCache.txt contains '{t_cache}'. "
                         "To keep the setting in sdkconfig ({t_conf}) and re-generate CMakeCache.txt, run '{prog} fullclean'. "
                         "To re-generate sdkconfig for '{t_cache}' target, run '{prog} set-target {t_cache}'."
                         .format(t_conf=idf_target_from_sdkconfig, t_cache=idf_target_from_cache, prog=prog_name))


class TargetChoice(click.Choice):
    """
    A version of click.Choice with two special features:
    - ignores hyphens
    - not case sensitive
    """
    def __init__(self, choices):
        super(TargetChoice, self).__init__(choices, case_sensitive=False)

    def convert(self, value, param, ctx):
        def normalize(str):
            return str.lower().replace("-", "")

        saved_token_normalize_func = ctx.token_normalize_func
        ctx.token_normalize_func = normalize

        try:
            return super(TargetChoice, self).convert(value, param, ctx)
        finally:
            ctx.token_normalize_func = saved_token_normalize_func
