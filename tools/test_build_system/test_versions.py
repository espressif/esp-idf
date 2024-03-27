# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import subprocess
import typing
from pathlib import Path

import pytest
from test_build_system_helpers import EnvDict, IdfPyFunc, append_to_file, replace_in_file


#############################################################################################
# Test Case: Test that the build-system can set the default version for an IDF app
#
# Test Steps:
#   1. Copy the base build_test_app
#   2. Run idf.py reconfigure
#   3. Verify that the app version takes the default value of 1
#
# Note: This test must run outside a git repository for it to pass. Hence we force the test
#       to use a temporary work directory.
#############################################################################################
@pytest.mark.force_temp_work_dir
def test_versions_get_default_version(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Verify the default version of an app')
    ret = idf_py('reconfigure')
    assert 'App "build_test_app" version: 1' in ret.stdout


#############################################################################################
# Test Case: Test that the build-system can set the version of an IDF app from git describe
#
# Test Steps:
#   1. Clone the idf template app from https://github.com/espressif/esp-idf-template.git
#   2. Run idf.py reconfigure
#   3. Run git describe in the cloned app git repository
#   4. Verify that the app version is picked up from the git describe command
#
#############################################################################################
def test_versions_get_version_from_git_describe(idf_py: IdfPyFunc,
                                                test_git_template_app: Path,
                                                env: typing.Optional[EnvDict] = None) -> None:
    logging.info('Verify that the version of app can be set from git describe')
    idf_ret = idf_py('reconfigure')
    env_dict = dict(**os.environ)
    if env:
        env_dict.update(env)
    git_ret = subprocess.run(['git', 'describe', '--always', '--tags', '--dirty'],
                             cwd=test_git_template_app, env=env_dict, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    assert f'App "app-template" version: {git_ret.stdout.decode("utf-8")}' in idf_ret.stdout


#############################################################################################
# Test Case: Test that the build-system can set the version for an IDF app from the VERSION argument
#
# Test Steps:
#   1. Clone the idf template app from https://github.com/espressif/esp-idf-template.git
#   2. Replace the default project() command in the top level CMakeLists.txt file to call the version parsing
#      function __parse_and_store_version_arg()
#   3. Append several calls to __parse_and_store_version_arg() with different inputs for the VERSION argument
#   4. Append a project() call with valid arguments at the end of the CMakeLists.txt file
#   5. Run idf.py reconfigure
#   6. Verify that cmake correctly flags invalid inputs for the VERSION argument and accepts valid inputs for the same
#
#############################################################################################
def test_versions_get_version_from_version_arg(idf_py: IdfPyFunc, test_git_template_app: Path) -> None:
    logging.info('Verify that the VERSION argument in project() is correctly parsed by cmake')

    # empty VERSION argument
    replace_in_file((test_git_template_app / 'CMakeLists.txt'), 'project(app-template)',
                    '__parse_and_store_version_arg(app-template VERSION)')
    # Invalid VERSION argument format
    append_to_file((test_git_template_app / 'CMakeLists.txt'),
                   '\n__parse_and_store_version_arg(app-tempplate VERSION 1..2)')
    # Invalid VERSION argument format
    append_to_file((test_git_template_app / 'CMakeLists.txt'),
                   '\n__parse_and_store_version_arg(app-template VERSION version_text)')
    # Invalid VERSION argument format
    append_to_file((test_git_template_app / 'CMakeLists.txt'),
                   '\n__parse_and_store_version_arg(app-template VERSION 1.2.3.4.5)')
    append_to_file((test_git_template_app / 'CMakeLists.txt'),
                   '\n__parse_and_store_version_arg(app-template VERSION 0)')
    # Valid VERSION argument format
    append_to_file((test_git_template_app / 'CMakeLists.txt'),
                   '\n__parse_and_store_version_arg(app-template VERSION 0.1)')
    # Valid VERSION argument format
    append_to_file((test_git_template_app / 'CMakeLists.txt'),
                   '\n__parse_and_store_version_arg(app-template VERSION 0.1.2)')
    # Valid VERSION argument format
    append_to_file((test_git_template_app / 'CMakeLists.txt'),
                   '\n__parse_and_store_version_arg(app-template VERSION 0.1.2.3)')
    # project() call with valid VERSION argument format
    append_to_file((test_git_template_app / 'CMakeLists.txt'),
                   '\nproject(app-template VERSION 0.1.2.3)')

    with pytest.raises(subprocess.CalledProcessError) as e:
        idf_py('reconfigure')

        assert 'VERSION keyword not followed by a value or was followed by a value that expanded to nothing.' in e.stdout
        assert 'Version "1..2" format invalid' in e.stderr
        assert 'Version "version_text" format invalid' in e.stderr
        assert 'Version "1.2.3.4.5" format invalid' in e.stderr
        assert 'Version "1.2.3.4.5" format invalid' in e.stderr
        assert 'Version "0" format invalid' not in e.stderr
        assert 'Version "0.1" format invalid' not in e.stderr
        assert 'Version "0.1.2" format invalid' not in e.stderr
        assert 'Version "0.1.2.3" format invalid' not in e.stderr
        assert 'App "app-template" version: 0.1.2.3' in e.stdout


#############################################################################################
# Test Case: Test that the build-system can set the version of an IDF app from version.txt file
#
# Test Steps:
#   1. Clone the idf template app from https://github.com/espressif/esp-idf-template.git
#   2. Replace the default project() command in the top level CMakeLists.txt file to include VERSION argument
#   3. Copy version.txt file into the cloned app repository
#   4. Updated the version in version.txt file to a known value
#   5. Run idf.py reconfigure
#   6. Verify that the app version is picked up from the version.txt file
#
#############################################################################################
def test_versions_get_version_from_version_file(idf_py: IdfPyFunc, test_git_template_app: Path) -> None:
    logging.info('Verify that the version of app can be set from version.txt file')
    replace_in_file((test_git_template_app / 'CMakeLists.txt'), 'project(app-template)',
                    'project(app-template VERSION 0.1.2.3)')
    (test_git_template_app / 'version.txt').write_text('project_version_from_txt')
    idf_ret = idf_py('reconfigure')

    assert f'App "app-template" version: project_version_from_txt' in idf_ret.stdout


#############################################################################################
# Test Case: Test that the build-system can set the version of an IDF app if PROJECT_VER is set in the CMakeLists.txt
#
# Test Steps:
#   1. Clone the idf template app from https://github.com/espressif/esp-idf-template.git
#   2. Update CMakeLists.txt file to set PROJECT_VER before calling project()
#   3. Replace the default project() command in the top level CMakeLists.txt file to include VERSION argument
#   4. Copy version.txt file into the cloned app repository
#   5. Updated the version in version.txt file to a known value
#   6. Run idf.py reconfigure
#   7. Verify that the app version is picked up from the CMakeLists.txt file
#
#############################################################################################
def test_versions_get_version_from_top_level_cmake(idf_py: IdfPyFunc, test_git_template_app: Path) -> None:
    logging.info('Verify that the version of app can be set from PROJECT_VER in CMakeLists.txt')
    replace_in_file((test_git_template_app / 'CMakeLists.txt'), 'project(app-template)',
                    'set(PROJECT_VER project_version_from_CMakeLists)')
    append_to_file((test_git_template_app / 'CMakeLists.txt'), 'project(app-template VERSION 0.1.2.3)')
    (test_git_template_app / 'version.txt').write_text('project_version_from_txt')
    idf_ret = idf_py('reconfigure')

    assert f'App "app-template" version: project_version_from_CMakeLists' in idf_ret.stdout


#############################################################################################
# Test Case: Test that the build-system can set the version of an IDF app from Kconfig option
#
# Test Steps:
#   1. Clone the idf template app from https://github.com/espressif/esp-idf-template.git
#   2. Update CMakeLists.txt file to set PROJECT_VER before calling project()
#   3. Replace the default project() command in the top level CMakeLists.txt file to include VERSION argument
#   4. Copy version.txt file into the cloned app repository
#   5. Updated the version in version.txt file to a known value
#   6. Run idf.py reconfigure
#   7. Updated sdkconfig.defaults to configure CONFIG_APP_PROJECT_VER_FROM_CONFIG and CONFIG_APP_PROJECT_VER
#   8. Run idf.py reconfigure
#   9. Verify that the app version is picked up from the Kconfig option
#
#############################################################################################
def test_versions_get_version_from_kconfig_option(idf_py: IdfPyFunc, test_git_template_app: Path) -> None:
    logging.info('Verify that the version of app can be set from Kconfig option')
    replace_in_file((test_git_template_app / 'CMakeLists.txt'), 'project(app-template)',
                    'set(PROJECT_VER project_version_from_CMakeLists)')
    append_to_file((test_git_template_app / 'CMakeLists.txt'), 'project(app-template VERSION 0.1.2.3)')
    (test_git_template_app / 'sdkconfig.defaults').write_text('\n'.join(['CONFIG_APP_PROJECT_VER_FROM_CONFIG=y',
                                                              'CONFIG_APP_PROJECT_VER="project_version_from_Kconfig"']))
    idf_ret = idf_py('reconfigure')

    assert f'App "app-template" version: project_version_from_Kconfig' in idf_ret.stdout
