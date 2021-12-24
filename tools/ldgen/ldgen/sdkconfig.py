#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import kconfiglib


class SDKConfig:
    """
    Evaluates conditional expressions based on the build's sdkconfig and Kconfig files.
    """
    def __init__(self, kconfig_file, sdkconfig_file):
        self.config = kconfiglib.Kconfig(kconfig_file)
        self.config.load_config(sdkconfig_file)
        self.config.warn = False  # eval_string may contain un-declared symbol

    def evaluate_expression(self, expression):
        result = self.config.eval_string(expression)

        if result == 0:  # n
            return False
        elif result == 2:  # y
            return True
        else:  # m
            raise Exception('unsupported config expression result')
