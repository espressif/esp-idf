import logging
import os
import shlex
import subprocess
import sys

from .common import BuildError, BuildSystem

# Same for the Makefile projects:
MAKE_PROJECT_LINE = r'include $(IDF_PATH)/make/project.mk'

BUILD_SYSTEM_MAKE = 'make'

try:
    string_type = basestring  # type: ignore
except NameError:
    string_type = str


class MakeBuildSystem(BuildSystem):
    NAME = BUILD_SYSTEM_MAKE

    @classmethod
    def build(cls, build_item):
        build_path, work_path = cls.build_prepare(build_item)
        commands = [
            'make clean',
            'make defconfig',
            'make all',
            # In case if secure_boot is enabled then for bootloader build need to add `bootloader` cmd
            'make bootloader',
            'make print_flash_cmd',
        ]

        log_file = None
        build_stdout = sys.stdout
        build_stderr = sys.stderr
        if build_item.build_log_path:
            logging.info('Writing build log to {}'.format(build_item.build_log_path))
            log_file = open(build_item.build_log_path, 'w')
            build_stdout = log_file
            build_stderr = log_file

        for cmd in commands:
            cmd = shlex.split(cmd) if isinstance(cmd, string_type) else cmd
            try:
                subprocess.check_call(cmd, stdout=build_stdout, stderr=build_stderr, cwd=work_path)
            except subprocess.CalledProcessError as e:
                if log_file:
                    log_file.close()
                raise BuildError('Build failed with exit code {}'.format(e.returncode))

        build_item.size_json_fp = build_item.get_size_json_fp()

    @staticmethod
    def is_app(path):
        makefile_path = os.path.join(path, 'Makefile')
        if not os.path.exists(makefile_path):
            return False
        with open(makefile_path, 'r') as makefile:
            makefile_content = makefile.read()
        if MAKE_PROJECT_LINE not in makefile_content:
            return False
        return True

    @classmethod
    def supported_targets(cls, app_path):
        readme_supported_targets = cls._supported_targets(app_path)
        if readme_supported_targets and 'esp32' in readme_supported_targets:
            return ['esp32']
        else:
            return []
