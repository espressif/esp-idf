# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os.path
import sys
import unittest

from idf_build_apps.constants import PREVIEW_TARGETS as IBA_PREVIEW_TARGETS
from idf_build_apps.constants import SUPPORTED_TARGETS as IBA_SUPPORTED_TARGETS
from idf_build_apps.manifest.soc_header import SOC_HEADERS


class TestLoadSocHeaders(unittest.TestCase):
    def test_targets(self) -> None:
        sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

        from idf_py_actions.constants import PREVIEW_TARGETS, SUPPORTED_TARGETS

        self.assertEqual(IBA_PREVIEW_TARGETS, PREVIEW_TARGETS)
        self.assertEqual(IBA_SUPPORTED_TARGETS, SUPPORTED_TARGETS)

    def test_loaded(self) -> None:
        # supported_targets and preview_targets are dynamically loaded from idf
        # no need to change them while supporting new targets
        for target in [*IBA_SUPPORTED_TARGETS, *IBA_PREVIEW_TARGETS]:
            # make at least loaded something
            self.assertIn(target, SOC_HEADERS)
            self.assertIsNotNone(SOC_HEADERS[target])


if __name__ == '__main__':
    unittest.main()
