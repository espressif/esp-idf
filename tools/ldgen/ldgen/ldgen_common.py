#
# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

from esp_pylib.errors import FatalError


class LdGenFailure(FatalError):
    """
    Parent class for any ldgen runtime failure which is due to input data
    """
