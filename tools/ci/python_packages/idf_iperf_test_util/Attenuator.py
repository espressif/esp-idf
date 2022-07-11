# SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""
Internal use only.

This file provide method to control programmable attenuator.
"""

import codecs
import time

import serial


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
        raise IOError('attenuator control, failed to open att port')

    cmd_hex = '7e7e10{:02x}{:x}'.format(att_t, 0x10 + att_t)
    exp_res_hex = '7e7e20{:02x}00{:x}'.format(att_t, 0x20 + att_t)

    cmd = codecs.decode(cmd_hex, 'hex')
    exp_res = codecs.decode(exp_res_hex, 'hex')

    serial_port.write(cmd)
    res = b''

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
