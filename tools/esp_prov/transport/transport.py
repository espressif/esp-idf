# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

# Base class for protocomm transport

import abc


class Transport():

    @abc.abstractmethod
    def send_session_data(self, data):
        pass

    @abc.abstractmethod
    def send_config_data(self, data):
        pass

    async def disconnect(self):
        pass
