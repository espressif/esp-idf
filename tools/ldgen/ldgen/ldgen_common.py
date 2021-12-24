#
# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#


class LdGenFailure(RuntimeError):
    """
    Parent class for any ldgen runtime failure which is due to input data
    """

    def __init__(self, message):
        super(LdGenFailure, self).__init__(message)
