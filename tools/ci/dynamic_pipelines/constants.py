# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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

DEFAULT_BUILD_CHILD_PIPELINE_NAME = 'Build Child Pipeline'
DEFAULT_TARGET_TEST_CHILD_PIPELINE_NAME = 'Target Test Child Pipeline'

DEFAULT_TARGET_TEST_JOB_TEMPLATE_NAME = '.dynamic_target_test_template'
TIMEOUT_4H_TEMPLATE_NAME = '.timeout_4h_template'

TEST_RELATED_BUILD_JOB_NAME = 'build_test_related_apps'
NON_TEST_RELATED_BUILD_JOB_NAME = 'build_non_test_related_apps'

COMMENT_START_MARKER = '### Dynamic Pipeline Report'
TEST_RELATED_APPS_FILENAME = 'test_related_apps.txt'
NON_TEST_RELATED_APPS_FILENAME = 'non_test_related_apps.txt'

TEST_RELATED_APPS_DOWNLOAD_URLS_FILENAME = 'test_related_apps_download_urls.yml'
REPORT_TEMPLATE_FILEPATH = os.path.join(
    IDF_PATH, 'tools', 'ci', 'dynamic_pipelines', 'templates', 'report.template.html'
)
CSS_STYLES_FILEPATH = os.path.join(IDF_PATH, 'tools', 'ci', 'dynamic_pipelines', 'templates', 'styles.css')
JS_SCRIPTS_FILEPATH = os.path.join(IDF_PATH, 'tools', 'ci', 'dynamic_pipelines', 'templates', 'scripts.js')
TOP_N_APPS_BY_SIZE_DIFF = 10
SIZE_DIFFERENCE_BYTES_THRESHOLD = 500
BINARY_SIZE_METRIC_NAME = 'binary_size'

RETRY_JOB_PICTURE_PATH = 'tools/ci/dynamic_pipelines/templates/retry-jobs.png'
RETRY_JOB_TITLE = '\n\nRetry failed jobs with with help of "retry_failed_jobs" stage of the pipeline:'
RETRY_JOB_PICTURE_LINK = '![Retry Jobs Image]({pic_url})'

BUILD_ONLY_LABEL = 'For Maintainers: Only Build Tests'

KNOWN_GENERATE_TEST_CHILD_PIPELINE_WARNINGS_FILEPATH = os.path.join(
    IDF_PATH, 'tools', 'ci', 'dynamic_pipelines', 'templates', 'known_generate_test_child_pipeline_warnings.yml'
)

CI_JOB_TOKEN = os.getenv('CI_JOB_TOKEN', '')
CI_DASHBOARD_API = os.getenv('CI_DASHBOARD_API', '')
CI_PAGES_URL = os.getenv('CI_PAGES_URL', '')
CI_PROJECT_URL = os.getenv('CI_PROJECT_URL', '')
CI_MERGE_REQUEST_SOURCE_BRANCH_SHA = os.getenv('CI_MERGE_REQUEST_SOURCE_BRANCH_SHA', '')
