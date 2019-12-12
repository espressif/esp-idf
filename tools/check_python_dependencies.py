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

import argparse
import os
import sys

try:
    import pkg_resources
except Exception:
    print('pkg_resources cannot be imported probably because the pip package is not installed and/or using a '
          'legacy Python interpreter. Please refer to the Get Started section of the ESP-IDF Programming Guide for '
          'setting up the required packages.')
    sys.exit(1)


def escape_backslash(path):
    if sys.platform == "win32":
        # escaped backslashes are necessary in order to be able to copy-paste the printed path
        return path.replace("\\", "\\\\")
    else:
        return path


if __name__ == "__main__":
    idf_path = os.getenv("IDF_PATH")

    parser = argparse.ArgumentParser(description='ESP32 Python package dependency checker')
    parser.add_argument('--requirements', '-r',
                        help='Path to the requrements file',
                        default=os.path.join(idf_path, 'requirements.txt'))
    args = parser.parse_args()

    not_satisfied = []
    with open(args.requirements) as f:
        for line in f:
            line = line.strip()
            # pkg_resources.require() cannot handle the full requirements file syntax so we need to make
            # adjustments for options which we use.
            if line.startswith('file://'):
                line = os.path.basename(line)
            try:
                pkg_resources.require(line)
            except Exception:
                not_satisfied.append(line)

    if len(not_satisfied) > 0:
        print('The following Python requirements are not satisfied:')
        for requirement in not_satisfied:
            print(requirement)
        if os.environ.get('IDF_PYTHON_ENV_PATH'):
            # We are running inside a private virtual environment under IDF_TOOLS_PATH,
            # ask the user to run install.bat again.
            if sys.platform == "win32" and not os.environ.get("MSYSTEM"):
                install_script = 'install.bat'
            else:
                install_script = 'install.sh'
            print('To install the missing packages, please run "%s"' % os.path.join(idf_path, install_script))
        elif sys.platform == "win32" and os.environ.get("MSYSTEM", None) == "MINGW32" and "/mingw32/bin/python" in sys.executable:
            print("The recommended way to install a packages is via \"pacman\". Please run \"pacman -Ss <package_name>\" for"
                  " searching the package database and if found then "
                  "\"pacman -S mingw-w64-i686-python{}-<package_name>\" for installing it.".format(sys.version_info[0],))
            print("NOTE: You may need to run \"pacman -Syu\" if your package database is older and run twice if the "
                  "previous run updated \"pacman\" itself.")
            print("Please read https://github.com/msys2/msys2/wiki/Using-packages for further information about using "
                  "\"pacman\"")
            # Special case for MINGW32 Python, needs some packages
            # via MSYS2 not via pip or system breaks...
            for requirement in not_satisfied:
                if requirement.startswith('cryptography'):
                    print("WARNING: The cryptography package have dependencies on system packages so please make sure "
                          "you run \"pacman -Syu\" followed by \"pacman -S mingw-w64-i686-python{}-cryptography\"."
                          "".format(sys.version_info[0],))
                    continue
                elif requirement.startswith('setuptools'):
                    print("Please run the following command to install MSYS2's MINGW Python setuptools package:")
                    print("pacman -S mingw-w64-i686-python{}-setuptools".format(sys.version_info[0],))
                    continue
        else:
            print('Please follow the instructions found in the "Set up the tools" section of '
                  'ESP-IDF Getting Started Guide')
        sys.exit(1)

    print('Python requirements from {} are satisfied.'.format(args.requirements))
