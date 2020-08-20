import logging
import os
import re
import shutil
import subprocess
import sys

from .common import BuildSystem, BuildItem, BuildError

BUILD_SYSTEM_CMAKE = "cmake"
IDF_PY = os.path.join(os.environ["IDF_PATH"], "tools", "idf.py")

# While ESP-IDF component CMakeLists files can be identified by the presence of 'idf_component_register' string,
# there is no equivalent for the project CMakeLists files. This seems to be the best option...
CMAKE_PROJECT_LINE = r"include($ENV{IDF_PATH}/tools/cmake/project.cmake)"

SUPPORTED_TARGETS_REGEX = re.compile(r'Supported [Tt]argets((?:[\s|]+(?:ESP[0-9A-Z\-]+))+)')

FORMAL_TO_USUAL = {
    'ESP32': 'esp32',
    'ESP32-S2': 'esp32s2',
    'ESP32-S3': 'esp32s3',
}


class CMakeBuildSystem(BuildSystem):
    NAME = BUILD_SYSTEM_CMAKE

    @classmethod
    def build(cls, build_item):  # type: (BuildItem) -> None
        build_path, work_path, extra_cmakecache_items = cls.build_prepare(build_item)
        # Prepare the build arguments
        args = [
            sys.executable,
            IDF_PY,
            "-B",
            build_path,
            "-C",
            work_path,
            "-DIDF_TARGET=" + build_item.target,
        ]
        if extra_cmakecache_items:
            for key, val in extra_cmakecache_items.items():
                args.append("-D{}={}".format(key, val))
            if "TEST_EXCLUDE_COMPONENTS" in extra_cmakecache_items \
                    and "TEST_COMPONENTS" not in extra_cmakecache_items:
                args.append("-DTESTS_ALL=1")
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
        else:
            # Also save the sdkconfig file in the build directory
            shutil.copyfile(
                os.path.join(work_path, "sdkconfig"),
                os.path.join(build_path, "sdkconfig"),
            )
            build_item.size_json_fp = build_item.get_size_json_fp()
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
        formal_to_usual = {
            'ESP32': 'esp32',
            'ESP32-S2': 'esp32s2',
            'ESP32-S3': 'esp32s3',
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
