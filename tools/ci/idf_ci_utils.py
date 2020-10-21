# internal use only for CI
# some CI related util functions
#
# Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#

import logging
import os
import subprocess

IDF_PATH = os.getenv('IDF_PATH', os.getcwd())


def get_submodule_dirs():  # type: () -> list
    """
    To avoid issue could be introduced by multi-os or additional dependency,
    we use python and git to get this output
    :return: List of submodule dirs
    """
    dirs = []
    try:
        lines = subprocess.check_output(
            ['git', 'config', '--file', os.path.realpath(os.path.join(IDF_PATH, '.gitmodules')),
             '--get-regexp', 'path']).decode('utf8').strip().split('\n')
        for line in lines:
            _, path = line.split(' ')
            dirs.append(path)
    except Exception as e:
        logging.warning(str(e))

    return dirs
