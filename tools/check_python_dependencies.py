#!/usr/bin/env python
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import sys
try:
    import pkg_resources
except:
    print('pkg_resources cannot be imported probably because the pip package is not installed and/or using a '
          'legacy Python interpreter. Please refer to the Get Started section of the ESP-IDF Programming Guide for '
           'setting up the required packages.')
    sys.exit(1)

req_file = '{}/requirements.txt'.format(os.getenv("IDF_PATH"))

if __name__ == "__main__":
    not_satisfied = []
    with open(req_file) as f:
        for line in f:
            line = line.strip()
            try:
                pkg_resources.require(line)
            except:
                not_satisfied.append(line)

    if len(not_satisfied) > 0:
        print('The following Python requirements are not satisfied:')
        for requirement in not_satisfied:
            print(requirement)
        print('Please run "{} -m pip install -r {}" for resolving the issue.'.format(sys.executable, req_file))
        sys.exit(1)

    print('Python requirements are satisfied.')
