# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os

from idf_ci_utils import IDF_PATH

# use relative path to avoid absolute path in pipeline
DEFAULT_TEST_PATHS = [
    'examples',
    os.path.join('tools', 'test_apps'),
    'components',
]

DEFAULT_APPS_BUILD_PER_JOB = 60
DEFAULT_CASES_TEST_PER_JOB = 30

DEFAULT_BUILD_CHILD_PIPELINE_FILEPATH = os.path.join(IDF_PATH, 'build_child_pipeline.yml')
DEFAULT_TARGET_TEST_CHILD_PIPELINE_FILEPATH = os.path.join(IDF_PATH, 'target_test_child_pipeline.yml')

TEST_RELATED_BUILD_JOB_NAME = 'build_test_related_apps'
NON_TEST_RELATED_BUILD_JOB_NAME = 'build_non_test_related_apps'

COMMENT_START_MARKER = '### Dynamic Pipeline Report'
TEST_RELATED_APPS_FILENAME = 'test_related_apps.txt'
NON_TEST_RELATED_APPS_FILENAME = 'non_test_related_apps.txt'

TEST_RELATED_APPS_DOWNLOAD_URLS_FILENAME = 'test_related_apps_download_urls.yml'
REPORT_TEMPLATE_FILEPATH = os.path.join(
    IDF_PATH, 'tools', 'ci', 'dynamic_pipelines', 'templates', 'report.template.html'
)

BUILD_ONLY_LABEL = 'For Maintainers: Only Build Tests'

KNOWN_GENERATE_TEST_CHILD_PIPELINE_WARNINGS_FILEPATH = os.path.join(
    IDF_PATH, 'tools', 'ci', 'dynamic_pipelines', 'templates', 'known_generate_test_child_pipeline_warnings.yml'
)
