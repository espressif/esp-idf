# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

from __future__ import print_function

from builtins import input

import utils

from .transport import Transport


class Transport_Console(Transport):

    async def send_data(self, path, data, session_id=0):
        print('Client->Device msg :', path, session_id, utils.str_to_hexstr(data))
        try:
            resp = input('Enter device->client msg : ')
        except Exception as err:
            print('error:', err)
            return None
        return utils.hexstr_to_str(resp)
