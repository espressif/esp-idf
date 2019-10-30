# Copyright 2019 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
from setuptools import setup

setup(name='esp-windows-curses',
      version='0.1',
      description='Wrapper for the windows-curses package',
      url='https://www.espressif.com',
      author='Espressif Systems',
      license='Apache License 2.0',
      author_email='donotreply@espressif.com',
      zip_safe=False,
      # This wrapper exists only because of the following install_requires statement which ensures that the package
      # dependency is not added for MSYS2 where it cannot be installed. There is no PEP 508 environment marker to
      # detect MSYS2.
      install_requires=('' if 'MSYSTEM' in os.environ else 'windows-curses; sys_platform == "win32"',)
      )
