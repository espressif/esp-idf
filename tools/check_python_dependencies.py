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
import argparse
try:
    import pkg_resources
except:
    print('pkg_resources cannot be imported probably because the pip package is not installed and/or using a '
          'legacy Python interpreter. Please refer to the Get Started section of the ESP-IDF Programming Guide for '
           'setting up the required packages.')
    sys.exit(1)

if __name__ == "__main__":
    idf_path = os.getenv("IDF_PATH")

    parser = argparse.ArgumentParser(description='ESP32 Python package dependency checker')
    parser.add_argument('--requirements', '-r',
            help='Path to the requrements file',
            default=idf_path + '/requirements.txt')
    args = parser.parse_args()

    # Special case for MINGW32 Python, needs some packages
    # via MSYS2 not via pip or system breaks...
    if sys.platform == "win32" and \
       os.environ.get("MSYSTEM", None) == "MINGW32" and \
       "/mingw32/bin/python" in sys.executable:
        failed = False
        try:
            import cryptography
        except ImportError:
            print("Please run the following command to install MSYS2's MINGW Python cryptography package:")
            print("pacman -S mingw-w64-i686-python%d-cryptography" % (sys.version_info[0],))
            failed = True
        try:
            import setuptools
        except ImportError:
            print("Please run the following command to install MSYS2's MINGW Python setuptools package:")
            print("pacman -S mingw-w64-i686-python%d-setuptools" % (sys.version_info[0],))
            failed = True
        if failed:
            sys.exit(1)

    not_satisfied = []
    with open(args.requirements) as f:
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
        print('Please refer to the Get Started section of the ESP-IDF Programming Guide for setting up the required '
              'packages. Alternatively, you can run "{} -m pip install --user -r {}" for resolving the issue.'
              ''.format(sys.executable, args.requirements))
        sys.exit(1)

    print('Python requirements from {} are satisfied.'.format(args.requirements))
