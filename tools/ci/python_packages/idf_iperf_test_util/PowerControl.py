# SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""
Internal use only.

This file implements controlling  APC PDU via telnet.
"""

import telnetlib


class Control(object):
    """ control APC via telnet """

    @classmethod
    def apc_telnet_make_choice(cls, telnet, choice):
        """ select a choice """
        telnet.read_until(b'Event Log')
        telnet.read_until(b'>')
        telnet.write(choice.encode() + b'\r\n')

    @classmethod
    def apc_telnet_common_action(cls, telnet, check_str, action):
        """ wait until a pattern and then write a line """
        telnet.read_until(check_str.encode())
        telnet.write(action.encode() + b'\r\n')

    @classmethod
    def control(cls, apc_ip, control_dict):
        """
        control APC

        :param apc_ip: IP of APC
        :param control_dict: dict with outlet ID and "ON" or "OFF"
        """

        for _outlet in control_dict:
            assert 0 < _outlet < 9
            assert control_dict[_outlet] in ['ON', 'OFF']

        # telnet
        # set timeout as 2s so that it won't waste time even can't access APC
        tn = telnetlib.Telnet(host=apc_ip, timeout=5)
        # log on
        cls.apc_telnet_common_action(tn, 'User Name :', 'apc')
        cls.apc_telnet_common_action(tn, 'Password  :', 'apc')
        # go to Device Manager
        cls.apc_telnet_make_choice(tn, '1')
        # go to Outlet Management
        cls.apc_telnet_make_choice(tn, '2')
        # go to Outlet Control/Configuration
        cls.apc_telnet_make_choice(tn, '1')

        # do select Outlet and control
        for _outlet in control_dict:
            # choose Outlet
            cls.apc_telnet_make_choice(tn, str(_outlet))
            # choose Control Outlet
            cls.apc_telnet_make_choice(tn, '1')
            # choose action
            _action = control_dict[_outlet]
            if 'ON' in _action:
                cls.apc_telnet_make_choice(tn, '1')
            else:
                cls.apc_telnet_make_choice(tn, '2')
            # do confirm
            cls.apc_telnet_common_action(tn, 'cancel :', 'YES')
            cls.apc_telnet_common_action(tn, 'continue...', '')
            # return to Outlet Control/Configuration
            cls.apc_telnet_make_choice(tn, '\033')
            cls.apc_telnet_make_choice(tn, '\033')

        # exit to main menu and logout
        tn.write(b'\033\r\n')
        tn.write(b'\033\r\n')
        tn.write(b'\033\r\n')
        tn.write(b'4\r\n')

    @classmethod
    def control_rest(cls, apc_ip, outlet, action):
        outlet_list = list(range(1, 9))  # has to be a list if we want to remove from it under Python 3
        outlet_list.remove(outlet)
        cls.control(apc_ip, dict.fromkeys(outlet_list, action))
