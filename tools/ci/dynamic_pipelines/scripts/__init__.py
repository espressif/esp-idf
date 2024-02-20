# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import sys

IDF_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))

# run this scripts only in idf path, to ensure the relative path is the same
os.chdir(IDF_PATH)

if 'IDF_PATH' not in os.environ:
    os.environ['IDF_PATH'] = IDF_PATH

tools_path = os.path.join(os.path.dirname(__file__), '..', '..', '..')
if tools_path not in sys.path:
    sys.path.append(tools_path)

tools_ci_path = os.path.join(os.path.dirname(__file__), '..', '..')
if tools_ci_path not in sys.path:
    sys.path.append(tools_ci_path)

tools_ci_python_packages_path = os.path.join(os.path.dirname(__file__), '..', '..', 'python_packages')
if tools_ci_python_packages_path not in sys.path:
    sys.path.append(tools_ci_python_packages_path)
