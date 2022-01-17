#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import re
import sys

PYTHON_PACKAGE_RE = re.compile(r'[^<>=~]+')

try:
    import pkg_resources
except ImportError:
    print('pkg_resources cannot be imported probably because the pip package is not installed and/or using a '
          'legacy Python interpreter. Please refer to the Get Started section of the ESP-IDF Programming Guide for '
          'setting up the required packages.')
    sys.exit(1)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='ESP-IDF Python package dependency checker')
    parser.add_argument('--requirements', '-r',
                        help='Path to a requirements file (can be used multiple times)',
                        action='append', default=[])
    parser.add_argument('--constraints', '-c', default=[],
                        help='Path to a constraints file (can be used multiple times)',
                        action='append')
    args = parser.parse_args()

    required_set = set()
    for req_path in args.requirements:
        with open(req_path) as f:
            required_set |= set(i for i in map(str.strip, f.readlines()) if len(i) > 0 and not i.startswith('#'))

    constr_dict = {}  # for example package_name -> package_name==1.0
    for const_path in args.constraints:
        with open(const_path) as f:
            for con in [i for i in map(str.strip, f.readlines()) if len(i) > 0 and not i.startswith('#')]:
                if con.startswith('file://'):
                    con = os.path.basename(con)
                elif con.startswith('--only-binary'):
                    continue
                elif con.startswith('-e') and '#egg=' in con:  # version control URLs, take the egg= part at the end only
                    con_m = re.search(r'#egg=([^\s]+)', con)
                    if not con_m:
                        print('Malformed input. Cannot find name in {}'.format(con))
                        sys.exit(1)
                    con = con_m[1]

                name_m = PYTHON_PACKAGE_RE.search(con)
                if not name_m:
                    print('Malformed input. Cannot find name in {}'.format(con))
                    sys.exit(1)
                constr_dict[name_m[0]] = con

    # We need to constrain package dependencies as well. So all installed packages need to be checked.
    # For example package A requires package B. We have only A in our requirements. But the newest version of B could
    # broke at some time and in that case we add a constraint for B (on the server) but don't have to update the
    # requirement file (in the ESP-IDF repo).
    required_set |= set(i.key for i in pkg_resources.working_set)

    not_satisfied = []
    for requirement in required_set:
        # If there is a version-specific constraint for the requirement then use it. Otherwise, just use the
        # requirement as is.
        to_require = constr_dict.get(requirement, requirement)
        try:
            pkg_resources.require(to_require)
        except pkg_resources.ResolutionError:
            not_satisfied.append(to_require)

    if len(not_satisfied) > 0:
        print('The following Python requirements are not satisfied:')
        print(os.linesep.join(not_satisfied))
        if 'IDF_PYTHON_ENV_PATH' in os.environ:
            # We are running inside a private virtual environment under IDF_TOOLS_PATH,
            # ask the user to run install.bat again.
            install_script = 'install.bat' if sys.platform == 'win32' else 'install.sh'
            print('To install the missing packages, please run "{}"'.format(install_script))
        else:
            print('Please follow the instructions found in the "Set up the tools" section of '
                  'ESP-IDF Getting Started Guide.')

        print('Diagnostic information:')
        idf_python_env_path = os.environ.get('IDF_PYTHON_ENV_PATH')
        print('    IDF_PYTHON_ENV_PATH: {}'.format(idf_python_env_path or '(not set)'))
        print('    Python interpreter used: {}'.format(sys.executable))
        if not idf_python_env_path or idf_python_env_path not in sys.executable:
            print('    Warning: python interpreter not running from IDF_PYTHON_ENV_PATH')
            print('    PATH: {}'.format(os.getenv('PATH')))
        sys.exit(1)

    print('Python requirements are satisfied.')
