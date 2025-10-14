# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os

from idf_ci_utils import IDF_PATH

COMMENT_START_MARKER = '### Dynamic Pipeline Report'

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

KNOWN_GENERATE_TEST_CHILD_PIPELINE_WARNINGS_FILEPATH = os.path.join(
    IDF_PATH, 'tools', 'ci', 'dynamic_pipelines', 'templates', 'known_generate_test_child_pipeline_warnings.yml'
)

CI_JOB_TOKEN = os.getenv('CI_JOB_TOKEN', '')
CI_DASHBOARD_API = os.getenv('CI_DASHBOARD_API', '')
CI_PAGES_URL = os.getenv('CI_PAGES_URL', '')
CI_PROJECT_URL = os.getenv('CI_PROJECT_URL', '')
CI_MERGE_REQUEST_SOURCE_BRANCH_SHA = os.getenv('CI_MERGE_REQUEST_SOURCE_BRANCH_SHA', '')
