# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import re
import sys

import yaml


if __name__ == '__main__':
    description = os.getenv('CI_MERGE_REQUEST_DESCRIPTION', '')
    if not description:
        sys.exit(0)
    pattern = r'^## Dynamic Pipeline Configuration(?:[^`]*?)```(?:\w+)(.*?)```'
    result = re.search(pattern, description, re.DOTALL | re.MULTILINE)
    if not result:
        sys.exit(0)

    data = yaml.safe_load(result.group(1))
    raise_report_exit_code = False
    with open('pipeline.env', 'a+') as f:
        if 'Test Case Filters' in data:
            raise_report_exit_code = True
            test_case_filters = ' or '.join(data.get('Test Case Filters'))
            f.write(f'TEST_CASE_FILTERS={test_case_filters}\n')

        if raise_report_exit_code:
            f.write('REPORT_EXIT_CODE=30\n')
