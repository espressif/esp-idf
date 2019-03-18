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
import App


class IDFApp(App.BaseApp):
    """
    Implements common esp-idf application behavior.
    idf applications should inherent from this class and overwrite method get_binary_path.
    """

    IDF_DOWNLOAD_CONFIG_FILE = "download.config"
    IDF_FLASH_ARGS_FILE = "flash_project_args"

    def __init__(self, app_path):
        super(IDFApp, self).__init__(app_path)
        self.idf_path = self.get_sdk_path()
        self.binary_path = self.get_binary_path(app_path)
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

        self.esptool, self.partition_tool = self.get_tools()

    @classmethod
    def get_sdk_path(cls):
        idf_path = os.getenv("IDF_PATH")
        assert idf_path
        assert os.path.exists(idf_path)
        return idf_path

    @classmethod
    def get_tools(cls):
        idf_path = cls.get_sdk_path()
        # get esptool and partition tool for esp-idf
        esptool = os.path.join(idf_path, "components",
                               "esptool_py", "esptool", "esptool.py")
        partition_tool = os.path.join(idf_path, "components",
                                      "partition_table", "gen_esp32part.py")
        assert os.path.exists(esptool) and os.path.exists(partition_tool)
        return esptool, partition_tool
    def _get_sdkconfig_paths(self):
        """
        returns list of possible paths where sdkconfig could be found

        Note: could be overwritten by a derived class to provide other locations or order
        """
        return [os.path.join(self.binary_path, "sdkconfig"), os.path.join(self.binary_path, "..", "sdkconfig")]

    def get_sdkconfig(self):
        """
        reads sdkconfig and returns a dictionary with all configuredvariables

        :param sdkconfig_file: location of sdkconfig
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
                    d[configs[0]] = configs[1]
        return d

    def get_binary_path(self, app_path):
        """
        get binary path according to input app_path.

        subclass must overwrite this method.

        :param app_path: path of application
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

    def process_arg(self, arg):
        """
        process args in download.config. convert to abs path for .bin args. strip spaces and CRLFs.
        """
        if ".bin" in arg:
            ret = os.path.join(self.binary_path, arg)
        else:
            ret = arg
        return ret.strip("\r\n ")

    def process_app_info(self):
        """
        get app download config and partition info from a specific app path

        :return: download config, partition info
        """

        if self.IDF_FLASH_ARGS_FILE in os.listdir(self.binary_path):
            with open(os.path.join(self.binary_path, self.IDF_FLASH_ARGS_FILE), "r") as f:
                configs = []
                for line in f:
                    line = line.strip()
                    if len(line) > 0:
                        configs += line.split()
        else:
            with open(os.path.join(self.binary_path, self.IDF_DOWNLOAD_CONFIG_FILE), "r") as f:
                configs = f.read().split(" ")

        download_configs = ["--chip", "auto", "--before", "default_reset",
                            "--after", "hard_reset", "write_flash", "-z"]
        download_configs += [self.process_arg(x) for x in configs]

        # handle partition table
        for partition_file in download_configs:
            if "partition" in partition_file:
                partition_file = os.path.join(self.binary_path, partition_file)
                break
        else:
            raise ValueError("No partition table found for IDF binary path: {}".format(self.binary_path))

        process = subprocess.Popen(["python", self.partition_tool, partition_file],
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
        return download_configs, partition_table


class Example(IDFApp):
    def _get_sdkconfig_paths(self):
        """
        overrides the parent method to provide exact path of sdkconfig for example tests
        """
        return [os.path.join(self.binary_path, "..", "sdkconfig")]

    def get_binary_path(self, app_path):
        # build folder of example path
        path = os.path.join(self.idf_path, app_path, "build")
        if not os.path.exists(path):
            # search for CI build folders
            app = os.path.basename(app_path)
            example_path = os.path.join(self.idf_path, "build_examples", "example_builds")
            for dirpath, dirnames, files in os.walk(example_path):
                if dirnames:
                    if dirnames[0] == app:
                        path = os.path.join(example_path, dirpath, dirnames[0], "build")
                        break
            else:
                raise OSError("Failed to find example binary")
        return path


class UT(IDFApp):
    def get_binary_path(self, app_path):
        """
        :param app_path: app path or app config
        :return: binary path
        """
        if not app_path:
            app_path = "default"

        path = os.path.join(self.idf_path, app_path)
        if not os.path.exists(path):
            while True:
                # try to get by config
                if app_path == "default":
                    # it's default config, we first try to get form build folder of unit-test-app
                    path = os.path.join(self.idf_path, "tools", "unit-test-app", "build")
                    if os.path.exists(path):
                        # found, use bin in build path
                        break
                # ``make ut-build-all-configs`` or ``make ut-build-CONFIG`` will copy binary to output folder
                path = os.path.join(self.idf_path, "tools", "unit-test-app", "output", app_path)
                if os.path.exists(path):
                    break
                raise OSError("Failed to get unit-test-app binary path")
        return path


class SSC(IDFApp):
    def get_binary_path(self, app_path):
        # TODO: to implement SSC get binary path
        return app_path


class AT(IDFApp):
    def get_binary_path(self, app_path):
        # TODO: to implement AT get binary path
        return app_path
