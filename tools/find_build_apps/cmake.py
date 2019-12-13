import os
import sys
import subprocess
import logging
import shutil
import re
from .common import BuildSystem, BuildItem, BuildError

BUILD_SYSTEM_CMAKE = "cmake"
IDF_PY = "idf.py"

# While ESP-IDF component CMakeLists files can be identified by the presence of 'idf_component_register' string,
# there is no equivalent for the project CMakeLists files. This seems to be the best option...
CMAKE_PROJECT_LINE = r"include($ENV{IDF_PATH}/tools/cmake/project.cmake)"

SUPPORTED_TARGETS_REGEX = re.compile(r"set\(\s*SUPPORTED_TARGETS\s+([a-z_0-9\- ]+)\s*\)")


class CMakeBuildSystem(BuildSystem):
    NAME = BUILD_SYSTEM_CMAKE

    @staticmethod
    def build(build_item):  # type: (BuildItem) -> None
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
        sdkconfig_defaults_list = ["sdkconfig.defaults"]
        if build_item.sdkconfig_path:
            sdkconfig_defaults_list.append(build_item.sdkconfig_path)

        sdkconfig_file = os.path.join(work_path, "sdkconfig")
        if os.path.exists(sdkconfig_file):
            logging.debug("Removing sdkconfig file: {}".format(sdkconfig_file))
            if not build_item.dry_run:
                os.unlink(sdkconfig_file)

        logging.debug("Creating sdkconfig file: {}".format(sdkconfig_file))
        if not build_item.dry_run:
            with open(sdkconfig_file, "w") as f_out:
                for sdkconfig_name in sdkconfig_defaults_list:
                    sdkconfig_path = os.path.join(work_path, sdkconfig_name)
                    if not sdkconfig_path or not os.path.exists(sdkconfig_path):
                        continue
                    logging.debug("Appending {} to sdkconfig".format(sdkconfig_name))
                    with open(sdkconfig_path, "r") as f_in:
                        for line in f_in:
                            f_out.write(os.path.expandvars(line))
            # Also save the sdkconfig file in the build directory
            shutil.copyfile(
                os.path.join(work_path, "sdkconfig"),
                os.path.join(build_path, "sdkconfig"),
            )

        else:
            for sdkconfig_name in sdkconfig_defaults_list:
                sdkconfig_path = os.path.join(app_path, sdkconfig_name)
                if not sdkconfig_path:
                    continue
                logging.debug("Considering sdkconfig {}".format(sdkconfig_path))
                if not os.path.exists(sdkconfig_path):
                    continue
                logging.debug("Appending {} to sdkconfig".format(sdkconfig_name))

        # Prepare the build arguments
        args = [
            # Assume it is the responsibility of the caller to
            # set up the environment (run . ./export.sh)
            IDF_PY,
            "-B",
            build_path,
            "-C",
            work_path,
            "-DIDF_TARGET=" + build_item.target,
        ]
        if build_item.verbose:
            args.append("-v")
        args.append("build")
        cmdline = format(" ".join(args))
        logging.info("Running {}".format(cmdline))

        if build_item.dry_run:
            return

        log_file = None
        build_stdout = sys.stdout
        build_stderr = sys.stderr
        if build_item.build_log_path:
            logging.info("Writing build log to {}".format(build_item.build_log_path))
            log_file = open(build_item.build_log_path, "w")
            build_stdout = log_file
            build_stderr = log_file

        try:
            subprocess.check_call(args, stdout=build_stdout, stderr=build_stderr)
        except subprocess.CalledProcessError as e:
            raise BuildError("Build failed with exit code {}".format(e.returncode))
        finally:
            if log_file:
                log_file.close()

    @staticmethod
    def _read_cmakelists(app_path):
        cmakelists_path = os.path.join(app_path, "CMakeLists.txt")
        if not os.path.exists(cmakelists_path):
            return None
        with open(cmakelists_path, "r") as cmakelists_file:
            return cmakelists_file.read()

    @staticmethod
    def is_app(path):
        cmakelists_file_content = CMakeBuildSystem._read_cmakelists(path)
        if not cmakelists_file_content:
            return False
        if CMAKE_PROJECT_LINE not in cmakelists_file_content:
            return False
        return True

    @staticmethod
    def supported_targets(app_path):
        cmakelists_file_content = CMakeBuildSystem._read_cmakelists(app_path)
        if not cmakelists_file_content:
            return None
        match = re.findall(SUPPORTED_TARGETS_REGEX, cmakelists_file_content)
        if not match:
            return None
        if len(match) > 1:
            raise NotImplementedError("Can't determine the value of SUPPORTED_TARGETS in {}".format(app_path))
        targets = match[0].split(" ")
        return targets
