# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import unittest

try:
    from typing import Any
except ImportError:
    pass  # only needed to check type annotations


class Py23TestCase(unittest.TestCase):

    def __init__(self, *args, **kwargs):  # type: (Any, Any) -> None
        super(Py23TestCase, self).__init__(*args, **kwargs)
        try:
            self.assertRaisesRegex
        except AttributeError:
            # assertRaisesRegexp is deprecated in Python3 but assertRaisesRegex doesn't exist in Python2
            # This fix is used in order to avoid using the alias from the six library
            self.assertRaisesRegex = self.assertRaisesRegexp  # type: ignore
