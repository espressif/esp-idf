#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import sys
import tempfile
import unittest

import yaml

CWD = os.path.join(os.path.dirname(__file__))
ERR_OUT_YML = os.path.join(CWD, 'error_output.yml')

try:
    from idf_py_actions.tools import generate_hints
except ImportError:
    sys.path.append(os.path.join(CWD, '..'))
    from idf_py_actions.tools import generate_hints


class TestHintsMassages(unittest.TestCase):
    def test_output(self) -> None:
        with open(ERR_OUT_YML) as f:
            error_output = yaml.safe_load(f)
        for error, hint in error_output.items():
            with tempfile.NamedTemporaryFile(mode='w') as f:
                f.write(error)
                f.flush()
                for generated_hint in generate_hints(f.name):
                    self.assertEqual(generated_hint, hint)


if __name__ == '__main__':
    unittest.main()
