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

    def __init__(self, app_path):
        super(IDFApp, self).__init__(app_path)
        self.idf_path = self.get_sdk_path()
        self.binary_path = self.get_binary_path(app_path)
        assert os.path.exists(self.binary_path)
        assert self.IDF_DOWNLOAD_CONFIG_FILE in os.listdir(self.binary_path)
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

    def get_binary_path(self, app_path):
        """
        get binary path according to input app_path.

        subclass must overwrite this method.

        :param app_path: path of application
        :return: abs app binary path
        """
        pass

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
        if app_path:
            # specified path, join it and the idf path
            path = os.path.join(self.idf_path, app_path)
        else:
            path = os.path.join(self.idf_path, "tools", "unit-test-app", "build")
        return path


class SSC(IDFApp):
    def get_binary_path(self, app_path):
        # TODO: to implement SSC get binary path
        return app_path


class AT(IDFApp):
    def get_binary_path(self, app_path):
        # TODO: to implement AT get binary path
        return app_path
