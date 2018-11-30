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
Internal use only.

This file provide method to control programmable attenuator.
"""

import time
import serial
import codecs


def set_att(port, att, att_fix=False):
    """
    set attenuation value on the attenuator

    :param port: serial port for attenuator
    :param att: attenuation value we want to set
    :param att_fix: fix the deviation with experience value
    :return: True or False
    """

    assert 0 <= att <= 62
    # fix att
    if att_fix:
        if att >= 33 and (att - 30 + 1) % 4 == 0:
            att_t = att - 1
        elif att >= 33 and (att - 30) % 4 == 0:
            att_t = att + 1
        else:
            att_t = att
    else:
        att_t = att

    serial_port = serial.Serial(port, baudrate=9600, rtscts=False, timeout=0.1)
    if serial_port.isOpen() is False:
        raise IOError("attenuator control, failed to open att port")

    cmd_hex = "7e7e10{:02x}{:x}".format(att_t, 0x10 + att_t)
    exp_res_hex = "7e7e20{:02x}00{:x}".format(att_t, 0x20 + att_t)

    cmd = codecs.decode(cmd_hex, "hex")
    exp_res = codecs.decode(exp_res_hex, "hex")

    serial_port.write(cmd)
    res = b""

    for i in range(5):
        res += serial_port.read(20)
        if res == exp_res:
            result = True
            break
        time.sleep(0.1)
    else:
        result = False

    serial_port.close()
    return result
