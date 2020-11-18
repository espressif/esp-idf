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

""" Test Env, manages DUT, App and EnvConfig, interface for test cases to access these components """
import os
import threading
import functools

import netifaces
import traceback

from . import EnvConfig


def _synced(func):
    @functools.wraps(func)
    def decorator(self, *args, **kwargs):
        with self.lock:
            ret = func(self, *args, **kwargs)
        return ret

    decorator.__doc__ = func.__doc__
    return decorator


class Env(object):
    """
    test env, manages DUTs and env configs.

    :keyword app: class for default application
    :keyword dut: class for default DUT
    :keyword env_tag: test env tag, used to select configs from env config file
    :keyword env_config_file: test env config file path
    :keyword test_name: test suite name, used when generate log folder name
    """

    def __init__(self,
                 app=None,
                 dut=None,
                 env_tag=None,
                 env_config_file=None,
                 test_suite_name=None,
                 **kwargs):
        self.app_cls = app
        self.default_dut_cls = dut
        self.config = EnvConfig.Config(env_config_file, env_tag)
        self.log_path = self.app_cls.get_log_folder(test_suite_name)
        if not os.path.exists(self.log_path):
            os.makedirs(self.log_path)

        self.allocated_duts = dict()
        self.lock = threading.RLock()

    @_synced
    def get_dut(self, dut_name, app_path, dut_class=None, app_class=None, app_config_name=None, **dut_init_args):
        """
        get_dut(dut_name, app_path, dut_class=None, app_class=None)

        :param dut_name: user defined name for DUT
        :param app_path: application path, app instance will use this path to process application info
        :param dut_class: dut class, if not specified will use default dut class of env
        :param app_class: app class, if not specified will use default app of env
        :param app_config_name: app build config
        :keyword dut_init_args: extra kwargs used when creating DUT instance
        :return: dut instance
        """
        if dut_name in self.allocated_duts:
            dut = self.allocated_duts[dut_name]["dut"]
        else:
            if dut_class is None:
                dut_class = self.default_dut_cls
            if app_class is None:
                app_class = self.app_cls
            detected_target = None
            try:
                port = self.config.get_variable(dut_name)
            except ValueError:
                # try to auto detect ports
                allocated_ports = [self.allocated_duts[x]["port"] for x in self.allocated_duts]
                available_ports = dut_class.list_available_ports()
                for port in available_ports:
                    if port not in allocated_ports:
                        result, detected_target = dut_class.confirm_dut(port)
                        if result:
                            break
                else:
                    port = None

            app_target = dut_class.TARGET
            if not app_target:
                app_target = detected_target
            if not app_target:
                raise ValueError("DUT class doesn't specify the target, and autodetection failed")
            app_inst = app_class(app_path, app_config_name, app_target)

            if port:
                try:
                    dut_config = self.get_variable(dut_name + "_port_config")
                except ValueError:
                    dut_config = dict()
                dut_config.update(dut_init_args)
                dut = dut_class(dut_name, port,
                                os.path.join(self.log_path, dut_name + ".log"),
                                app_inst,
                                **dut_config)
                self.allocated_duts[dut_name] = {"port": port, "dut": dut}
            else:
                raise ValueError("Failed to get DUT")
        return dut

    @_synced
    def close_dut(self, dut_name):
        """
        close_dut(dut_name)
        close one DUT by name if DUT name is valid (the name used by ``get_dut``). otherwise will do nothing.

        :param dut_name: user defined name for DUT
        :return: None
        """
        try:
            dut = self.allocated_duts.pop(dut_name)["dut"]
            dut.close()
        except KeyError:
            pass

    @_synced
    def get_variable(self, variable_name):
        """
        get_variable(variable_name)
        get variable from config file. If failed then try to auto-detected it.

        :param variable_name: name of the variable
        :return: value of variable if successfully found. otherwise None.
        """
        return self.config.get_variable(variable_name)

    PROTO_MAP = {
        "ipv4": netifaces.AF_INET,
        "ipv6": netifaces.AF_INET6,
        "mac": netifaces.AF_LINK,
    }

    @_synced
    def get_pc_nic_info(self, nic_name="pc_nic", proto="ipv4"):
        """
        get_pc_nic_info(nic_name="pc_nic")
        try to get info of a specified NIC and protocol.

        :param nic_name: pc nic name. allows passing variable name, nic name value.
        :param proto: "ipv4", "ipv6" or "mac"
        :return: a dict of nic info if successfully found. otherwise None.
                 nic info keys could be different for different protocols.
                 key "addr" is available for both mac, ipv4 and ipv6 pic info.
        """
        interfaces = netifaces.interfaces()
        if nic_name in interfaces:
            # the name is in the interface list, we regard it as NIC name
            if_addr = netifaces.ifaddresses(nic_name)
        else:
            # it's not in interface name list, we assume it's variable name
            _nic_name = self.get_variable(nic_name)
            if_addr = netifaces.ifaddresses(_nic_name)

        return if_addr[self.PROTO_MAP[proto]][0]

    @_synced
    def close(self, dut_debug=False):
        """
        close()
        close all DUTs of the Env.

        :param dut_debug: if dut_debug is True, then print all dut expect failures before close it
        :return: exceptions during close DUT
        """
        dut_close_errors = []
        for dut_name in self.allocated_duts:
            dut = self.allocated_duts[dut_name]["dut"]
            try:
                if dut_debug:
                    dut.print_debug_info()
                dut.close()
            except Exception as e:
                traceback.print_exc()
                dut_close_errors.append(e)
        self.allocated_duts = dict()
        return dut_close_errors
