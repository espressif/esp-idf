# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
import os
import shutil
import stat
import subprocess
import sys
import textwrap
from pathlib import Path
from typing import List

import pytest
from test_build_system_helpers import append_to_file
from test_build_system_helpers import EnvDict
from test_build_system_helpers import file_contains
from test_build_system_helpers import find_python
from test_build_system_helpers import get_snapshot
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file
from test_build_system_helpers import run_idf_py


def get_subdirs_absolute_paths(path: Path) -> List[str]:
    """
    Get a list of files with absolute path in a given `path` folder
    """
    return [
        '{}/{}'.format(dir_path, file_name)
        for dir_path, _, file_names in os.walk(path)
        for file_name in file_names
    ]


@pytest.mark.usefixtures('test_app_copy')
@pytest.mark.test_app_copy('examples/get-started/blink')
def test_compile_commands_json_updated_by_reconfigure(idf_py: IdfPyFunc) -> None:
    output = idf_py('reconfigure')
    assert 'Building ESP-IDF components for target esp32' in output.stdout
    snapshot_1 = get_snapshot(['build/compile_commands.json'])
    snapshot_2 = get_snapshot(['build/compile_commands.json'])
    snapshot_2.assert_same(snapshot_1)
    idf_py('reconfigure')
    snapshot_3 = get_snapshot(['build/compile_commands.json'])
    snapshot_3.assert_different(snapshot_2)


@pytest.mark.usefixtures('test_app_copy')
def test_hints_no_color_output_when_noninteractive(idf_py: IdfPyFunc) -> None:
    """Check that idf.py hints don't include color escape codes in non-interactive builds"""

    # make the build fail in such a way that idf.py shows a hint
    replace_in_file('main/build_test_app.c', '// placeholder_inside_main',
                    'esp_chip_info_t chip_info; esp_chip_info(&chip_info);')

    with (pytest.raises(subprocess.CalledProcessError)) as exc_info:
        idf_py('build')

    # Should not actually include a color escape sequence!
    # Change the assert to the correct value once the bug is fixed.
    assert '\x1b[0;33mHINT: esp_chip_info.h' in exc_info.value.stderr


@pytest.mark.usefixtures('test_app_copy')
def test_idf_copy(idf_copy: Path, idf_py: IdfPyFunc) -> None:
    # idf_copy is the temporary IDF copy.
    # For example, we can check if idf.py build can work without the .git directory:
    shutil.rmtree(idf_copy / '.git', ignore_errors=True)
    idf_py('build')


def test_idf_build_with_env_var_sdkconfig_defaults(
    test_app_copy: Path,
    default_idf_env: EnvDict
) -> None:
    with open(test_app_copy / 'sdkconfig.test', 'w') as fw:
        fw.write('CONFIG_BT_ENABLED=y')

    default_idf_env['SDKCONFIG_DEFAULTS'] = 'sdkconfig.test'
    run_idf_py('build', env=default_idf_env)

    with open(test_app_copy / 'sdkconfig') as fr:
        assert 'CONFIG_BT_ENABLED=y' in fr.read()


@pytest.mark.usefixtures('test_app_copy')
@pytest.mark.test_app_copy('examples/system/efuse')
def test_efuse_summary_cmake_functions(
    default_idf_env: EnvDict
) -> None:
    default_idf_env['IDF_CI_BUILD'] = '1'
    output = run_idf_py('efuse-summary', env=default_idf_env)
    assert 'FROM_CMAKE: MAC: 00:00:00:00:00:00' in output.stdout
    assert 'FROM_CMAKE: WR_DIS: 0' in output.stdout


def test_custom_build_folder(test_app_copy: Path, idf_py: IdfPyFunc) -> None:
    idf_py('-BBuiLDdiR', 'build')
    assert (test_app_copy / 'BuiLDdiR').is_dir()


def test_python_clean(idf_py: IdfPyFunc) -> None:
    logging.info('Cleaning Python bytecode')
    idf_path = Path(os.environ['IDF_PATH'])
    abs_paths = get_subdirs_absolute_paths(idf_path)
    abs_paths_suffix = [path for path in abs_paths if path.endswith(('.pyc', '.pyo'))]
    assert len(abs_paths_suffix) != 0
    idf_py('python-clean')
    abs_paths = get_subdirs_absolute_paths(idf_path)
    abs_paths_suffix = [path for path in abs_paths if path.endswith(('.pyc', '.pyo'))]
    assert len(abs_paths_suffix) == 0


@pytest.mark.skipif(sys.platform == 'win32', reason='Windows does not support executing bash script')
def test_python_interpreter_unix(test_app_copy: Path) -> None:
    logging.info("Make sure idf.py never runs '/usr/bin/env python' or similar")
    env_dict = dict(**os.environ)
    python = find_python(env_dict['PATH'])
    (test_app_copy / 'python').write_text(textwrap.dedent("""#!/bin/sh
                                                          echo "idf.py has executed '/usr/bin/env python' or similar"
                                                          exit 1
                                                          """))
    st = os.stat(test_app_copy / 'python')
    # equivalent to 'chmod +x ./python'
    os.chmod((test_app_copy / 'python'), st.st_mode | stat.S_IEXEC)

    env_dict['PATH'] = str(test_app_copy) + os.pathsep + env_dict['PATH']
    # python is loaded from env:$PATH, but since false interpreter is provided there, python needs to be specified as argument
    # if idf.py is reconfigured during it's execution, it would load a false interpreter
    run_idf_py('reconfigure', env=env_dict, python=python)


@pytest.mark.skipif(sys.platform != 'win32', reason='Linux does not support executing .exe files')
def test_python_interpreter_win(test_app_copy: Path) -> None:
    logging.info('Make sure idf.py never runs false python interpreter')
    env_dict = dict(**os.environ)
    python = find_python(env_dict['PATH'])

    # on windows python interpreter has compiled code '.exe' format, so this false file can be empty
    (test_app_copy / 'python.exe').write_text('')
    env_dict['PATH'] = str(test_app_copy) + os.pathsep + env_dict['PATH']
    # python is loaded from env:$PATH, but since false interpreter is provided there, python needs to be specified as argument
    # if idf.py is reconfigured during it's execution, it would load a false interpreter
    run_idf_py('reconfigure', env=env_dict, python=python)


@pytest.mark.usefixtures('test_app_copy')
def test_invoke_confserver(idf_py: IdfPyFunc) -> None:
    logging.info('Confserver can be invoked by idf.py')
    idf_py('confserver', input_str='{"version": 1}')


def test_ccache_used_to_build(test_app_copy: Path) -> None:
    logging.info('Check ccache is used to build')
    (test_app_copy / 'ccache').touch(mode=0o755)
    env_dict = dict(**os.environ)
    env_dict['PATH'] = str(test_app_copy) + os.pathsep + env_dict['PATH']
    # Disable using ccache automatically
    if 'IDF_CCACHE_ENABLE' in env_dict:
        env_dict.pop('IDF_CCACHE_ENABLE')

    ret = run_idf_py('--ccache', 'reconfigure', env=env_dict)
    assert 'ccache will be used' in ret.stdout
    run_idf_py('fullclean', env=env_dict)
    ret = run_idf_py('reconfigure', env=env_dict)
    assert 'ccache will be used' not in ret.stdout
    ret = run_idf_py('--no-ccache', 'reconfigure', env=env_dict)
    assert 'ccache will be used' not in ret.stdout


def test_toolchain_prefix_in_description_file(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Toolchain prefix is set in project description file')
    idf_py('reconfigure')
    data = json.load(open(test_app_copy / 'build' / 'project_description.json', 'r'))
    assert 'monitor_toolprefix' in data


@pytest.mark.usefixtures('test_app_copy')
def test_subcommands_with_options(idf_py: IdfPyFunc, default_idf_env: EnvDict) -> None:
    logging.info('Can set options to subcommands: print_filter for monitor')
    idf_path = Path(default_idf_env.get('IDF_PATH'))
    # try - finally block is here used to backup and restore idf_monitor.py
    # since we need to handle only one file, this souluton is much faster than using idf_copy fixture
    monitor_backup = (idf_path / 'tools' / 'idf_monitor.py').read_text()
    try:
        (idf_path / 'tools' / 'idf_monitor.py').write_text('import sys;print(sys.argv[1:])')
        idf_py('build')
        ret = idf_py('monitor', '--print-filter=*:I', '-p', 'tty.fake')
        assert "'--print_filter', '*:I'" in ret.stdout
    finally:
        (idf_path / 'tools' / 'idf_monitor.py').write_text(monitor_backup)


def test_fallback_to_build_system_target(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('idf.py fallback to build system target')
    msg = 'Custom target is running'
    append_to_file(test_app_copy / 'CMakeLists.txt',
                   'add_custom_target(custom_target COMMAND ${{CMAKE_COMMAND}} -E echo "{}")'.format(msg))
    ret = idf_py('custom_target')
    assert msg in ret.stdout, 'Custom target did not produce expected output'


def test_create_component_project(idf_copy: Path) -> None:
    logging.info('Create project and component using idf.py and build it')
    run_idf_py('-C', 'projects', 'create-project', 'temp_test_project', workdir=idf_copy)
    run_idf_py('-C', 'components', 'create-component', 'temp_test_component', workdir=idf_copy)
    replace_in_file(idf_copy / 'projects' / 'temp_test_project' / 'main' / 'temp_test_project.c', '{\n\n}',
                    '\n'.join(['{', '\tfunc();', '}']))
    replace_in_file(idf_copy / 'projects' / 'temp_test_project' / 'main' / 'temp_test_project.c', '#include <stdio.h>',
                    '\n'.join(['#include <stdio.h>', '#include "temp_test_component.h"']))
    run_idf_py('build', workdir=(idf_copy / 'projects' / 'temp_test_project'))


# In this test function, there are actually two logical tests in one test function.
# It would be better to have every check in a separate
# test case, but that would mean doing idf_copy each time, and copying takes most of the time
def test_create_project(idf_py: IdfPyFunc, idf_copy: Path) -> None:
    logging.info('Check that command for creating new project will fail if the target folder is not empty.')
    (idf_copy / 'example_proj').mkdir()
    (idf_copy / 'example_proj' / 'tmp_1').touch()
    ret = idf_py('create-project', '--path', str(idf_copy / 'example_proj'), 'temp_test_project', check=False)
    assert ret.returncode == 3, 'Command create-project exit value is wrong.'

    # cleanup for the following test
    shutil.rmtree(idf_copy / 'example_proj')

    logging.info('Check that command for creating new project will fail if the target path is file.')
    (idf_copy / 'example_proj_file').touch()
    ret = idf_py('create-project', '--path', str(idf_copy / 'example_proj_file'), 'temp_test_project', check=False)
    assert ret.returncode == 4, 'Command create-project exit value is wrong.'


@pytest.mark.skipif(sys.platform == 'darwin', reason='macos runner is a shell executor, it would break the file system')
def test_create_project_with_idf_readonly(idf_copy: Path) -> None:
    def change_to_readonly(src: Path) -> None:
        for root, dirs, files in os.walk(src):
            for name in dirs:
                os.chmod(os.path.join(root, name), 0o555)  # read & execute
            for name in files:
                path = os.path.join(root, name)
                if '/bin/' in path:
                    continue  # skip executables
                os.chmod(os.path.join(root, name), 0o444)  # readonly
    logging.info('Check that command for creating new project will success if the IDF itself is readonly.')
    change_to_readonly(idf_copy)
    try:
        run_idf_py('create-project', '--path', str(idf_copy / 'example_proj'), 'temp_test_project')
    except Exception:
        raise
    else:
        def del_rw(function, path, excinfo):  # type: ignore
            os.chmod(path, stat.S_IWRITE)
            os.remove(path)

        shutil.rmtree(idf_copy, onerror=del_rw)


@pytest.mark.usefixtures('test_app_copy')
def test_docs_command(idf_py: IdfPyFunc) -> None:
    logging.info('Check docs command')
    idf_py('set-target', 'esp32')
    ret = idf_py('docs', '--no-browser')
    assert 'https://docs.espressif.com/projects/esp-idf/en' in ret.stdout
    ret = idf_py('docs', '--no-browser', '--language', 'en')
    assert 'https://docs.espressif.com/projects/esp-idf/en' in ret.stdout
    ret = idf_py('docs', '--no-browser', '--language', 'en', '--version', 'v4.2.1')
    assert 'https://docs.espressif.com/projects/esp-idf/en/v4.2.1' in ret.stdout
    ret = idf_py('docs', '--no-browser', '--language', 'en', '--version', 'v4.2.1', '--target', 'esp32')
    assert 'https://docs.espressif.com/projects/esp-idf/en/v4.2.1/esp32' in ret.stdout
    ret = idf_py('docs', '--no-browser', '--language', 'en', '--version', 'v4.2.1', '--target', 'esp32', '--starting-page', 'get-started')
    assert 'https://docs.espressif.com/projects/esp-idf/en/v4.2.1/esp32/get-started' in ret.stdout


@pytest.mark.usefixtures('test_app_copy')
def test_deprecation_warning(idf_py: IdfPyFunc) -> None:
    logging.info('Deprecation warning check')
    ret = idf_py('post_debug', check=False)
    # click warning
    assert 'Error: Command "post_debug" is deprecated since v4.4 and was removed in v5.0.' in ret.stderr

    ret = idf_py('efuse_common_table', check=False)
    # cmake warning
    assert 'Have you wanted to run "efuse-common-table" instead?' in ret.stdout


def test_save_defconfig_check(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Save-defconfig checks')
    (test_app_copy / 'sdkconfig').write_text('\n'.join(['CONFIG_COMPILER_OPTIMIZATION_SIZE=y',
                                                        'CONFIG_ESPTOOLPY_FLASHFREQ_80M=y']))
    idf_py('save-defconfig')
    assert not file_contains(test_app_copy / 'sdkconfig.defaults', 'CONFIG_IDF_TARGET'), \
        'CONFIG_IDF_TARGET should not be in sdkconfig.defaults'
    assert file_contains(test_app_copy / 'sdkconfig.defaults', 'CONFIG_COMPILER_OPTIMIZATION_SIZE=y'), \
        'Missing CONFIG_COMPILER_OPTIMIZATION_SIZE=y in sdkconfig.defaults'
    assert file_contains(test_app_copy / 'sdkconfig.defaults', 'CONFIG_ESPTOOLPY_FLASHFREQ_80M=y'), \
        'Missing CONFIG_ESPTOOLPY_FLASHFREQ_80M=y in sdkconfig.defaults'
    idf_py('fullclean')
    (test_app_copy / 'sdkconfig').unlink()
    (test_app_copy / 'sdkconfig.defaults').unlink()
    idf_py('set-target', 'esp32c3')
    (test_app_copy / 'sdkconfig').write_text('CONFIG_PARTITION_TABLE_OFFSET=0x8001')
    idf_py('save-defconfig')
    assert file_contains(test_app_copy / 'sdkconfig.defaults', 'CONFIG_IDF_TARGET="esp32c3"'), \
        'Missing CONFIG_IDF_TARGET="esp32c3" in sdkconfig.defaults'
    assert file_contains(test_app_copy / 'sdkconfig.defaults', 'CONFIG_PARTITION_TABLE_OFFSET=0x8001'), \
        'Missing CONFIG_PARTITION_TABLE_OFFSET=0x8001 in sdkconfig.defaults'


def test_merge_bin_cmd(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Test if merge-bin command works correctly')
    idf_py('merge-bin')
    assert (test_app_copy / 'build' / 'merged-binary.bin').is_file()
    idf_py('merge-bin', '--output', 'merged-binary-2.bin')
    assert (test_app_copy / 'build' / 'merged-binary-2.bin').is_file()
    idf_py('merge-bin', '--format', 'hex')
    assert (test_app_copy / 'build' / 'merged-binary.hex').is_file()
