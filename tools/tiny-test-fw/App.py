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

"""
class for handling Test Apps. Currently it provides the following features:

1. get SDK path
2. get SDK tools
3. parse application info from its path. for example:
    * provide download info
    * provide partition table info

Test Apps should inherent from BaseApp class and overwrite the methods.
"""
import os
import sys
import time

# timestamp used for calculate log folder name
LOG_FOLDER_TIMESTAMP = time.time()


class BaseApp(object):
    """
    Base Class for App.
    Defines the mandatory methods that App need to implement.
    Also implements some common methods.

    :param app_path: the path for app.
    """

    def __init__(self, app_path):
        pass

    @classmethod
    def get_sdk_path(cls):
        """
        get sdk path.

        subclass must overwrite this method.

        :return: abs sdk path
        """
        pass

    @classmethod
    def get_tools(cls):
        """
        get SDK related tools for applications

        subclass must overwrite this method.

        :return: tuple, abs path of each tool
        """
        pass

    @classmethod
    def get_log_folder(cls, test_suite_name):
        """
        By default log folder is ``${SDK_PATH}/TEST_LOGS/${test_suite_name}_${timestamp}``.

        The log folder name is consist once start running, ensure all logs of will be put into the same folder.

        :param test_suite_name: the test suite name, by default it's the base file name for main module
        :return: the log folder path
        """
        if not test_suite_name:
            test_suite_name = os.path.splitext(os.path.basename(sys.modules['__main__'].__file__))[0]
        sdk_path = cls.get_sdk_path()
        return os.path.join(sdk_path, "TEST_LOGS",
                            test_suite_name +
                            time.strftime("_%m%d_%H_%M_%S", time.localtime(LOG_FOLDER_TIMESTAMP)))

    def process_app_info(self):
        """
        parse built app info for DUTTool

        subclass must overwrite this method.

        :return: required info for specific DUTTool
        """
        pass
