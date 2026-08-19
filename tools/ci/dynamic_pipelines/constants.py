# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os

from idf_ci_utils import IDF_PATH

KNOWN_GENERATE_TEST_CHILD_PIPELINE_WARNINGS_FILEPATH = os.path.join(
    IDF_PATH, 'tools', 'ci', 'dynamic_pipelines', 'templates', 'known_generate_test_child_pipeline_warnings.yml'
)
