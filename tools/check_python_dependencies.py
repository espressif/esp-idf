#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import re
import sys

try:
    from packaging.requirements import Requirement
    from packaging.version import Version
except ImportError:
    print('packaging cannot be imported. '
          'If you\'ve installed a custom Python then this package is provided separately and have to be installed as well. '
          'Please refer to the Get Started section of the ESP-IDF Programming Guide for setting up the required packages.')
    sys.exit(1)

try:
    from importlib.metadata import requires
    from importlib.metadata import version as get_version
except ImportError:
    # compatibility for python <=3.7
    from importlib_metadata import requires  # type: ignore
    from importlib_metadata import version as get_version  # type: ignore

try:
    from typing import Set
except ImportError:
    # This is a script run during the early phase of setting up the environment. So try to avoid failure caused by
    # Python version incompatibility. The supported Python version is checked elsewhere.
    pass

PYTHON_PACKAGE_RE = re.compile(r'[^<>=~]+')

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
                constr_dict[name_m[0]] = con.partition(' #')[0]  # remove comments

    not_satisfied = []  # in string form which will be printed

    # already_checked set is used in order to avoid circular checks which would cause looping.
    already_checked = set()  # type: Set[Requirement]

    # required_set contains package names in string form without version constraints. If the package has a constraint
    # specification (package name + version requirement) then use that instead. new_req_list is used to store
    # requirements to be checked on each level of breath-first-search of the package dependency tree. The initial
    # version is the direct dependencies deduced from the requirements arguments of the script.
    new_req_list = [Requirement(constr_dict.get(i, i)) for i in required_set]

    def version_check(requirement: Requirement) -> None:
        # compare installed version with required
        version = Version(get_version(requirement.name))
        if not requirement.specifier.contains(version, prereleases=True):
            not_satisfied.append(f"Requirement '{requirement}' was not met. Installed version: {version}")

    # evaluate markers and check versions of direct requirements
    for req in new_req_list[:]:
        if not req.marker or req.marker.evaluate():
            try:
                version_check(req)
            except Exception as e:
                # Catch general exception, because get_version may return None (https://github.com/python/cpython/issues/91216)
                # log package name alongside the error message for easier debugging
                not_satisfied.append(f"Error while checking requirement '{req}'. Package was not found and is required by the application: {e}")
                new_req_list.remove(req)
        else:
            new_req_list.remove(req)

    while new_req_list:
        req_list = new_req_list
        new_req_list = []
        already_checked.update(req_list)
        for requirement in req_list:  # check one level of the dependency tree
            try:
                dependency_requirements = set()
                extras = list(requirement.extras) or ['']
                # `requires` returns all sub-requirements including all extras - we need to filter out just required ones
                for name in requires(requirement.name) or []:
                    sub_req = Requirement(name)
                    # check extras e.g. esptool[hsm]
                    for extra in extras:
                        # evaluate markers if present
                        if not sub_req.marker or sub_req.marker.evaluate(environment={'extra': extra}):
                            dependency_requirements.add(sub_req)
                            version_check(sub_req)
                # dependency_requirements are the direct dependencies of "requirement". They belong to the next level
                # of the dependency tree. They will be checked only if they haven't been already. Note that the
                # version is taken into account as well because packages can have different requirements for a given
                # Python package. The dependencies need to be checked for all of them because they can be different.
                new_req_list.extend(dependency_requirements - already_checked)
            except Exception as e:
                # Catch general exception, because get_version may return None (https://github.com/python/cpython/issues/91216)
                # log package name alongside the error message for easier debugging
                not_satisfied.append(f"Error while checking requirement '{req}'. Package was not found and is required by the application: {e}")

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
