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
The test env could change when we running test from different computers.
Test env config provide ``get_variable`` method to allow user get test environment related variables.
It will first try to get variable from config file.
If failed, then it will try to auto detect (Not supported yet).

Config file format is yaml. it's a set of key-value pair. The following is an example of config file::

    Example_WIFI:
      ap_ssid: "myssid"
      ap_password: "mypassword"
    Example_ShieldBox:
      attenuator_port: "/dev/ttyUSB2"
      ap_ssid: "myssid"
      ap_password: "mypassword"

It will first define the env tag for each environment, then add its key-value pairs.
This will prevent test cases from getting configs from other env when there're configs for multiple env in one file.
"""

import yaml


class Config(object):
    """ Test Env Config """

    def __init__(self, config_file, env_tag):
        self.configs = self.load_config_file(config_file, env_tag)

    @staticmethod
    def load_config_file(config_file, env_name):
        """
        load configs from config file.

        :param config_file: config file path
        :param env_name: env tag name
        :return: configs for the test env
        """
        try:
            with open(config_file) as f:
                configs = yaml.load(f)[env_name]
        except (OSError, TypeError):
            configs = dict()
        return configs

    def get_variable(self, variable_name):
        """
        first try to get from config file. if not found, try to auto detect the variable.

        :param variable_name: name of variable
        :return: value or None
        """
        try:
            value = self.configs[variable_name]
        except KeyError:
            #TODO: to support auto get variable here
            value = None
        if value is None:
            raise ValueError("Failed to get variable")
        return value
