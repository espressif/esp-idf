# SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

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
from yaml import Loader as Loader


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
                configs = yaml.load(f, Loader=Loader)[env_name]
        except (OSError, TypeError, IOError, KeyError):
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
            # TODO: to support auto get variable here
            value = None
        if value is None:
            raise ValueError('Failed to get variable')
        return value
