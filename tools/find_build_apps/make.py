import logging
import os
import subprocess
import sys
import shlex

from .common import BuildSystem, BuildError

# Same for the Makefile projects:
MAKE_PROJECT_LINE = r"include $(IDF_PATH)/make/project.mk"

BUILD_SYSTEM_MAKE = "make"


class MakeBuildSystem(BuildSystem):
    NAME = BUILD_SYSTEM_MAKE

    @staticmethod
    def build(build_item):
        build_path, work_path = BuildSystem.build_prepare(build_item)
        commands = [
            'make clean',
            'make defconfig',
            'make all',
            'make print_flash_cmd',
        ]

        log_file = None
        build_stdout = sys.stdout
        build_stderr = sys.stderr
        if build_item.build_log_path:
            logging.info("Writing build log to {}".format(build_item.build_log_path))
            log_file = open(build_item.build_log_path, "w")
            build_stdout = log_file
            build_stderr = log_file

        for cmd in commands:
            py3 = sys.version_info[0] == 3
            if py3:
                string_type = str
            else:
                string_type = basestring
            cmd = shlex.split(cmd) if isinstance(cmd, string_type) else cmd
            try:
                subprocess.check_call(cmd, stdout=build_stdout, stderr=build_stderr, cwd=work_path)
            except subprocess.CalledProcessError as e:
                if log_file:
                    log_file.close()
                raise BuildError("Build failed with exit code {}".format(e.returncode))

    @staticmethod
    def is_app(path):
        makefile_path = os.path.join(path, "Makefile")
        if not os.path.exists(makefile_path):
            return False
        with open(makefile_path, "r") as makefile:
            makefile_content = makefile.read()
        if MAKE_PROJECT_LINE not in makefile_content:
            return False
        return True
