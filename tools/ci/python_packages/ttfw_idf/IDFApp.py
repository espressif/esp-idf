# Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

""" IDF Test Applications """
import subprocess

import os
import json

from tiny_test_fw import App
from . import CIAssignExampleTest

try:
    import gitlab_api
except ImportError:
    gitlab_api = None


def parse_flash_settings(path):
    file_name = os.path.basename(path)
    if file_name == "flasher_args.json":
        # CMake version using build metadata file
        with open(path, "r") as f:
            args = json.load(f)
        flash_files = [(offs, binary) for (offs, binary) in args["flash_files"].items() if offs != ""]
        flash_settings = args["flash_settings"]
        app_name = os.path.splitext(args["app"]["file"])[0]
    else:
        # GNU Make version uses download.config arguments file
        with open(path, "r") as f:
            args = f.readlines()[-1].split(" ")
            flash_files = []
            flash_settings = {}
            for idx in range(0, len(args), 2):  # process arguments in pairs
                if args[idx].startswith("--"):
                    # strip the -- from the command line argument
                    flash_settings[args[idx][2:]] = args[idx + 1]
                else:
                    # offs, filename
                    flash_files.append((args[idx], args[idx + 1]))
            # we can only guess app name in download.config.
            for p in flash_files:
                if not os.path.dirname(p[1]) and "partition" not in p[1]:
                    # app bin usually in the same dir with download.config and it's not partition table
                    app_name = os.path.splitext(p[1])[0]
                    break
            else:
                app_name = None
    return flash_files, flash_settings, app_name


class Artifacts(object):
    def __init__(self, dest_root_path, artifact_index_file, app_path, config_name, target):
        assert gitlab_api
        # at least one of app_path or config_name is not None. otherwise we can't match artifact
        assert app_path or config_name
        assert os.path.exists(artifact_index_file)
        self.gitlab_inst = gitlab_api.Gitlab(os.getenv("CI_PROJECT_ID"))
        self.dest_root_path = dest_root_path
        with open(artifact_index_file, "r") as f:
            artifact_index = json.load(f)
        self.artifact_info = self._find_artifact(artifact_index, app_path, config_name, target)

    @staticmethod
    def _find_artifact(artifact_index, app_path, config_name, target):
        for artifact_info in artifact_index:
            match_result = True
            if app_path:
                match_result = app_path in artifact_info["app_dir"]
            if config_name:
                match_result = match_result and config_name == artifact_info["config"]
            if target:
                match_result = match_result and target == artifact_info["target"]
            if match_result:
                ret = artifact_info
                break
        else:
            ret = None
        return ret

    def download_artifacts(self):
        if self.artifact_info:
            base_path = os.path.join(self.artifact_info["work_dir"], self.artifact_info["build_dir"])
            job_id = self.artifact_info["ci_job_id"]

            # 1. download flash args file
            if self.artifact_info["build_system"] == "cmake":
                flash_arg_file = os.path.join(base_path, "flasher_args.json")
            else:
                flash_arg_file = os.path.join(base_path, "download.config")

            self.gitlab_inst.download_artifact(job_id, [flash_arg_file], self.dest_root_path)

            # 2. download all binary files
            flash_files, flash_settings, app_name = parse_flash_settings(os.path.join(self.dest_root_path,
                                                                                      flash_arg_file))
            artifact_files = [os.path.join(base_path, p[1]) for p in flash_files]
            artifact_files.append(os.path.join(base_path, app_name + ".elf"))

            self.gitlab_inst.download_artifact(job_id, artifact_files, self.dest_root_path)

            # 3. download sdkconfig file
            self.gitlab_inst.download_artifact(job_id, [os.path.join(os.path.dirname(base_path), "sdkconfig")],
                                               self.dest_root_path)
        else:
            base_path = None
        return base_path

    def download_artifact_files(self, file_names):
        if self.artifact_info:
            base_path = os.path.join(self.artifact_info["work_dir"], self.artifact_info["build_dir"])
            job_id = self.artifact_info["ci_job_id"]

            # download all binary files
            artifact_files = [os.path.join(base_path, fn) for fn in file_names]
            self.gitlab_inst.download_artifact(job_id, artifact_files, self.dest_root_path)

            # download sdkconfig file
            self.gitlab_inst.download_artifact(job_id, [os.path.join(os.path.dirname(base_path), "sdkconfig")],
                                               self.dest_root_path)
        else:
            base_path = None
        return base_path


class IDFApp(App.BaseApp):
    """
    Implements common esp-idf application behavior.
    idf applications should inherent from this class and overwrite method get_binary_path.
    """

    IDF_DOWNLOAD_CONFIG_FILE = "download.config"
    IDF_FLASH_ARGS_FILE = "flasher_args.json"

    def __init__(self, app_path, config_name=None, target=None):
        super(IDFApp, self).__init__(app_path)
        self.config_name = config_name
        self.target = target
        self.idf_path = self.get_sdk_path()
        self.binary_path = self.get_binary_path(app_path, config_name, target)
        self.elf_file = self._get_elf_file_path(self.binary_path)
        assert os.path.exists(self.binary_path)
        if self.IDF_DOWNLOAD_CONFIG_FILE not in os.listdir(self.binary_path):
            if self.IDF_FLASH_ARGS_FILE not in os.listdir(self.binary_path):
                msg = ("Neither {} nor {} exists. "
                       "Try to run 'make print_flash_cmd | tail -n 1 > {}/{}' "
                       "or 'idf.py build' "
                       "for resolving the issue."
                       "").format(self.IDF_DOWNLOAD_CONFIG_FILE, self.IDF_FLASH_ARGS_FILE,
                                  self.binary_path, self.IDF_DOWNLOAD_CONFIG_FILE)
                raise AssertionError(msg)

        self.flash_files, self.flash_settings = self._parse_flash_download_config()
        self.partition_table = self._parse_partition_table()

    @classmethod
    def get_sdk_path(cls):
        # type: () -> str
        idf_path = os.getenv("IDF_PATH")
        assert idf_path
        assert os.path.exists(idf_path)
        return idf_path

    def _get_sdkconfig_paths(self):
        """
        returns list of possible paths where sdkconfig could be found

        Note: could be overwritten by a derived class to provide other locations or order
        """
        return [os.path.join(self.binary_path, "sdkconfig"), os.path.join(self.binary_path, "..", "sdkconfig")]

    def get_sdkconfig(self):
        """
        reads sdkconfig and returns a dictionary with all configuredvariables

        :raise: AssertionError: if sdkconfig file does not exist in defined paths
        """
        d = {}
        sdkconfig_file = None
        for i in self._get_sdkconfig_paths():
            if os.path.exists(i):
                sdkconfig_file = i
                break
        assert sdkconfig_file is not None
        with open(sdkconfig_file) as f:
            for line in f:
                configs = line.split('=')
                if len(configs) == 2:
                    d[configs[0]] = configs[1].rstrip()
        return d

    def get_binary_path(self, app_path, config_name=None, target=None):
        # type: (str, str, str) -> str
        """
        get binary path according to input app_path.

        subclass must overwrite this method.

        :param app_path: path of application
        :param config_name: name of the application build config. Will match any config if None
        :param target: target name. Will match for target if None
        :return: abs app binary path
        """
        pass

    @staticmethod
    def _get_elf_file_path(binary_path):
        ret = ""
        file_names = os.listdir(binary_path)
        for fn in file_names:
            if os.path.splitext(fn)[1] == ".elf":
                ret = os.path.join(binary_path, fn)
        return ret

    def _parse_flash_download_config(self):
        """
        Parse flash download config from build metadata files

        Sets self.flash_files, self.flash_settings

        (Called from constructor)

        Returns (flash_files, flash_settings)
        """

        if self.IDF_FLASH_ARGS_FILE in os.listdir(self.binary_path):
            # CMake version using build metadata file
            path = os.path.join(self.binary_path, self.IDF_FLASH_ARGS_FILE)
        else:
            # GNU Make version uses download.config arguments file
            path = os.path.join(self.binary_path, self.IDF_DOWNLOAD_CONFIG_FILE)

        flash_files, flash_settings, app_name = parse_flash_settings(path)
        # The build metadata file does not currently have details, which files should be encrypted and which not.
        # Assume that all files should be encrypted if flash encryption is enabled in development mode.
        sdkconfig_dict = self.get_sdkconfig()
        flash_settings["encrypt"] = "CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT" in sdkconfig_dict

        # make file offsets into integers, make paths absolute
        flash_files = [(int(offs, 0), os.path.join(self.binary_path, path.strip())) for (offs, path) in flash_files]

        return flash_files, flash_settings

    def _parse_partition_table(self):
        """
        Parse partition table contents based on app binaries

        Returns partition_table data

        (Called from constructor)
        """
        partition_tool = os.path.join(self.idf_path,
                                      "components",
                                      "partition_table",
                                      "gen_esp32part.py")
        assert os.path.exists(partition_tool)

        for (_, path) in self.flash_files:
            if "partition" in path:
                partition_file = os.path.join(self.binary_path, path)
                break
        else:
            raise ValueError("No partition table found for IDF binary path: {}".format(self.binary_path))

        process = subprocess.Popen(["python", partition_tool, partition_file],
                                   stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        raw_data = process.stdout.read()
        if isinstance(raw_data, bytes):
            raw_data = raw_data.decode()
        partition_table = dict()

        for line in raw_data.splitlines():
            if line[0] != "#":
                try:
                    _name, _type, _subtype, _offset, _size, _flags = line.split(",")
                    if _size[-1] == "K":
                        _size = int(_size[:-1]) * 1024
                    elif _size[-1] == "M":
                        _size = int(_size[:-1]) * 1024 * 1024
                    else:
                        _size = int(_size)
                except ValueError:
                    continue
                partition_table[_name] = {
                    "type": _type,
                    "subtype": _subtype,
                    "offset": _offset,
                    "size": _size,
                    "flags": _flags
                }

        return partition_table


class Example(IDFApp):
    def _get_sdkconfig_paths(self):
        """
        overrides the parent method to provide exact path of sdkconfig for example tests
        """
        return [os.path.join(self.binary_path, "..", "sdkconfig")]

    def _try_get_binary_from_local_fs(self, app_path, config_name=None, target=None):
        # build folder of example path
        path = os.path.join(self.idf_path, app_path, "build")
        if os.path.exists(path):
            return path

        if not config_name:
            config_name = "default"

        if not target:
            target = "esp32"

        # Search for CI build folders.
        # Path format: $IDF_PATH/build_examples/app_path_with_underscores/config/target
        # (see tools/ci/build_examples_cmake.sh)
        # For example: $IDF_PATH/build_examples/examples_get-started_blink/default/esp32
        app_path_underscored = app_path.replace(os.path.sep, "_")
        example_path = os.path.join(self.idf_path, "build_examples")
        for dirpath in os.listdir(example_path):
            if os.path.basename(dirpath) == app_path_underscored:
                path = os.path.join(example_path, dirpath, config_name, target, "build")
                if os.path.exists(path):
                    return path
                else:
                    return None

    def get_binary_path(self, app_path, config_name=None, target=None):
        path = self._try_get_binary_from_local_fs(app_path, config_name, target)
        if path:
            return path
        else:
            artifacts = Artifacts(self.idf_path, CIAssignExampleTest.ARTIFACT_INDEX_FILE,
                                  app_path, config_name, target)
            path = artifacts.download_artifacts()
            if path:
                return os.path.join(self.idf_path, path)
            else:
                raise OSError("Failed to find example binary")


class UT(IDFApp):
    def get_binary_path(self, app_path, config_name=None, target=None):
        if not config_name:
            config_name = "default"

        path = os.path.join(self.idf_path, app_path)
        default_build_path = os.path.join(path, "build")
        if os.path.exists(default_build_path):
            return path

        # first try to get from build folder of unit-test-app
        path = os.path.join(self.idf_path, "tools", "unit-test-app", "build")
        if os.path.exists(path):
            # found, use bin in build path
            return path

        # ``make ut-build-all-configs`` or ``make ut-build-CONFIG`` will copy binary to output folder
        path = os.path.join(self.idf_path, "tools", "unit-test-app", "output", config_name)
        if os.path.exists(path):
            return path

        raise OSError("Failed to get unit-test-app binary path")


class SSC(IDFApp):
    def get_binary_path(self, app_path, config_name=None, target=None):
        # TODO: to implement SSC get binary path
        return app_path


class AT(IDFApp):
    def get_binary_path(self, app_path, config_name=None, target=None):
        # TODO: to implement AT get binary path
        return app_path
