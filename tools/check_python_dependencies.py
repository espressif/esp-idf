#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import re
import sys
from typing import Optional

try:
    from packaging.requirements import Requirement
    from packaging.version import Version
except ImportError:
    print(
        'packaging cannot be imported. '
        "If you've installed a custom Python then this package is provided separately and have to be installed as "
        'well. Please refer to the Get Started section of the ESP-IDF Programming Guide for setting up the required '
        'packages.'
    )
    sys.exit(1)

try:
    from importlib.metadata import PackageNotFoundError
    from importlib.metadata import requires as _requires
    from importlib.metadata import version as _version
except ImportError:
    # compatibility for python <=3.7
    from importlib_metadata import PackageNotFoundError  # type: ignore
    from importlib_metadata import requires as _requires  # type: ignore
    from importlib_metadata import version as _version  # type: ignore

try:
    from typing import Set
except ImportError:
    # This is a script run during the early phase of setting up the environment. So try to avoid failure caused by
    # Python version incompatibility. The supported Python version is checked elsewhere.
    pass

PYTHON_PACKAGE_RE = re.compile(r'[^<>=~]+')


def validate_requirement_list(file_path: str) -> str:
    """Validate that a requirement file exists and is readable."""
    if not os.path.isfile(file_path):
        raise argparse.ArgumentTypeError(
            f'Requirement file {file_path} not found\n'
            'Please make sure the file path is correct.\n'
            'In case the file was removed, please run the export script again, to update the environment.'
        )
    try:
        open(file_path, encoding='utf-8').close()
    except IOError as e:
        raise argparse.ArgumentTypeError(f'Cannot read requirement file {file_path}: {e}')
    return file_path


# The version and requires function from importlib.metadata in python prior
# 3.10 does perform distribution name normalization before searching for
# package distribution. This might cause problems for package with dot in its
# name as the wheel build backend(e.g. setuptools >= 75.8.1), may perform
# distribution name normalization. If the package name is not found, try again
# with normalized name.
# https://packaging.python.org/en/latest/specifications/binary-distribution-format/#escaping-and-unicode
def normalize_name(name: str) -> str:
    return re.sub(r'[-_.]+', '-', name).lower().replace('-', '_')


def get_version(name: str) -> str:
    try:
        version = _version(name)
    except PackageNotFoundError:
        version = _version(normalize_name(name))
    return version


def get_requires(name: str) -> Optional[list]:
    try:
        requires = _requires(name)
    except PackageNotFoundError:
        requires = _requires(normalize_name(name))
    return requires


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='ESP-IDF Python package dependency checker')
    parser.add_argument(
        '--requirements',
        '-r',
        help='Path to a requirements file (can be used multiple times)',
        action='append',
        default=[],
        type=validate_requirement_list,
    )
    parser.add_argument(
        '--constraints',
        '-c',
        default=[],
        help='Path to a constraints file (can be used multiple times)',
        action='append',
    )
    args = parser.parse_args()

    required_set = set()
    for req_path in args.requirements:
        with open(req_path, encoding='utf-8') as f:
            required_set |= set(i for i in map(str.strip, f.readlines()) if len(i) > 0 and not i.startswith('#'))

    constr_dict = {}  # for example package_name -> package_name==1.0
    for const_path in args.constraints:
        with open(const_path, encoding='utf-8') as f:
            for con in [i for i in map(str.strip, f.readlines()) if len(i) > 0 and not i.startswith('#')]:
                if con.startswith('file://'):
                    con = os.path.basename(con)
                elif con.startswith('--only-binary'):
                    continue
                # version control URLs, take the egg= part at the end only
                elif con.startswith('-e') and '#egg=' in con:
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
    already_checked: Set[Requirement] = set()

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
                not_satisfied.append(
                    f"Error while checking requirement '{req}'. Package was not found and is required by the "
                    f'application: {e}'
                )
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
                # `requires` returns all sub-requirements including all extras; we need to filter out just required ones
                for name in get_requires(requirement.name) or []:
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
                not_satisfied.append(
                    f"Error while checking requirement '{req}'. Package was not found and is required by the "
                    f'application: {e}'
                )

    if len(not_satisfied) > 0:
        print('The following Python requirements are not satisfied:')
        print(os.linesep.join(not_satisfied))
        if 'IDF_PYTHON_ENV_PATH' in os.environ:
            # We are running inside a private virtual environment under IDF_TOOLS_PATH,
            # ask the user to run install.bat again.
            install_script = 'install.bat' if sys.platform == 'win32' else 'install.sh'
            print('To install the missing packages, please run "{}"'.format(install_script))
        else:
            print(
                'Please follow the instructions found in the "Set up the tools" section of '
                'ESP-IDF Getting Started Guide.'
            )

        print('Diagnostic information:')
        idf_python_env_path = os.environ.get('IDF_PYTHON_ENV_PATH')
        print('    IDF_PYTHON_ENV_PATH: {}'.format(idf_python_env_path or '(not set)'))
        print('    Python interpreter used: {}'.format(sys.executable))
        if not idf_python_env_path or idf_python_env_path not in sys.executable:
            print('    Warning: python interpreter not running from IDF_PYTHON_ENV_PATH')
            print('    PATH: {}'.format(os.getenv('PATH')))
        sys.exit(1)

    print('Python requirements are satisfied.')
