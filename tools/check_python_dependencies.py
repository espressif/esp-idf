#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import re
import sys

try:
    import pkg_resources
except Exception:
    print('pkg_resources cannot be imported probably because the pip package is not installed and/or using a '
          'legacy Python interpreter. Please refer to the Get Started section of the ESP-IDF Programming Guide for '
          'setting up the required packages.')
    sys.exit(1)


def escape_backslash(path):
    if sys.platform == 'win32':
        # escaped backslashes are necessary in order to be able to copy-paste the printed path
        return path.replace('\\', '\\\\')
    else:
        return path


if __name__ == '__main__':
    idf_path = os.getenv('IDF_PATH')

    default_requirements_path = os.path.join(idf_path, 'requirements.txt')  # type: ignore

    parser = argparse.ArgumentParser(description='ESP-IDF Python package dependency checker')
    parser.add_argument('--requirements', '-r',
                        help='Path to the requirements file',
                        default=default_requirements_path)
    args = parser.parse_args()

    not_satisfied = []
    with open(args.requirements) as f:
        for line in f:
            line = line.strip()
            # pkg_resources.require() cannot handle the full requirements file syntax so we need to make
            # adjustments for options which we use.
            if line.startswith('file://'):
                line = os.path.basename(line)
            if line.startswith('--only-binary'):
                continue
            if line.startswith('-e') and '#egg=' in line:  # version control URLs, take the egg= part at the end only
                line = re.search(r'#egg=([^\s]+)', line).group(1)  # type: ignore
            try:
                pkg_resources.require(line)
            except Exception:
                not_satisfied.append(line)

    if len(not_satisfied) > 0:
        print('The following Python requirements are not satisfied:')
        for requirement in not_satisfied:
            print(requirement)
        if os.path.realpath(args.requirements) != os.path.realpath(default_requirements_path):
            # we're using this script to check non-default requirements.txt, so tell the user to run pip
            print('Please check the documentation for the feature you are using, or run "%s -m pip install -r %s"' % (sys.executable, args.requirements))
        elif os.environ.get('IDF_PYTHON_ENV_PATH'):
            # We are running inside a private virtual environment under IDF_TOOLS_PATH,
            # ask the user to run install.bat again.
            if sys.platform == 'win32' and not os.environ.get('MSYSTEM'):
                install_script = 'install.bat'
            else:
                install_script = 'install.sh'
            print('To install the missing packages, please run "%s"' % os.path.join(idf_path, install_script))  # type: ignore
        elif sys.platform == 'win32' and os.environ.get('MSYSTEM', None) == 'MINGW32' and '/mingw32/bin/python' in sys.executable:
            print("The recommended way to install a packages is via \"pacman\". Please run \"pacman -Ss <package_name>\" for"
                  ' searching the package database and if found then '
                  "\"pacman -S mingw-w64-i686-python-<package_name>\" for installing it.")
            print("NOTE: You may need to run \"pacman -Syu\" if your package database is older and run twice if the "
                  "previous run updated \"pacman\" itself.")
            print('Please read https://github.com/msys2/msys2/wiki/Using-packages for further information about using '
                  "\"pacman\"")
            # Special case for MINGW32 Python, needs some packages
            # via MSYS2 not via pip or system breaks...
            for requirement in not_satisfied:
                if requirement.startswith('cryptography'):
                    print('WARNING: The cryptography package have dependencies on system packages so please make sure '
                          "you run \"pacman -Syu\" followed by \"pacman -S mingw-w64-i686-python{}-cryptography\"."
                          ''.format(sys.version_info[0],))
                    continue
                elif requirement.startswith('setuptools'):
                    print("Please run the following command to install MSYS2's MINGW Python setuptools package:")
                    print('pacman -S mingw-w64-i686-python-setuptools')
                    continue
        else:
            print('Please follow the instructions found in the "Set up the tools" section of '
                  'ESP-IDF Getting Started Guide')

        print('Diagnostic information:')
        idf_python_env_path = os.environ.get('IDF_PYTHON_ENV_PATH')
        print('    IDF_PYTHON_ENV_PATH: {}'.format(idf_python_env_path or '(not set)'))
        print('    Python interpreter used: {}'.format(sys.executable))
        if not idf_python_env_path or idf_python_env_path not in sys.executable:
            print('    Warning: python interpreter not running from IDF_PYTHON_ENV_PATH')
            print('    PATH: {}'.format(os.getenv('PATH')))
        sys.exit(1)

    print('Python requirements from {} are satisfied.'.format(args.requirements))
