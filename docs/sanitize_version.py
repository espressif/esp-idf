# Tiny Python module to sanitize a Git version into something that can be used in a URL
#
# (this is used in multiple places: conf_common.py and in tools/ci/docs_deploy
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
import os


def sanitize_version(original_version):
    """ Given a version (probably output from 'git describe --always' or similar), return
    a URL-safe sanitized version. (this is used as 'release' config variable when building
    the docs.)

    Will override the original version with the Gitlab CI CI_COMMIT_REF_NAME environment variable if
    this is present.

    Also follows the RTD-ism that master branch is named 'latest'

    """

    try:
        version = os.environ['CI_COMMIT_REF_NAME']
    except KeyError:
        version = original_version

    if version == 'master':
        return 'latest'

    version = version.replace('/', '-')

    return version
