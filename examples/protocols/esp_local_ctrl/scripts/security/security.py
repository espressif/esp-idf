# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

# Base class for protocomm security


class Security:
    def __init__(self, security_session):
        self.security_session = security_session
