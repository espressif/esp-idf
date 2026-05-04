# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
import os
import re
import shutil
import stat
import subprocess
import sys
import textwrap
from pathlib import Path

import pytest
import yaml
from test_build_system_helpers import EXT_IDF_PATH
from test_build_system_helpers import EnvDict
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import append_to_file
from test_build_system_helpers import file_contains
from test_build_system_helpers import find_python
from test_build_system_helpers import get_snapshot
from test_build_system_helpers import replace_in_file
from test_build_system_helpers import run_idf_py

_tools_dir = str(Path(EXT_IDF_PATH) / 'tools')
if _tools_dir not in sys.path:
    sys.path.insert(0, _tools_dir)
from idf_py_actions.help_custom_targets_skip import CMAKE_CUSTOM_TARGETS_HELP_PANEL_TITLE  # noqa: E402


def _parse_idf_py_help_cmake_custom_target_names(stdout: str) -> list[str]:
    """Parse CMake Custom Targets listed in ``idf.py --help`` in rich-click format.
    Custom "plain-slim" rich-click theme is used to minimize the formatting impact.
    Parsing starts from the line containing ``CMAKE_CUSTOM_TARGETS_HELP_PANEL_TITLE``.

    - Piped outputs may use ``| … |`` table rows instead on some architectures (Unix/Windows).
    - CI may still wrap lines in CSI SGR sequences (``ESC [ … m``).
    """
    lines = stdout.splitlines()
    idx = next((i for i, line in enumerate(lines) if CMAKE_CUSTOM_TARGETS_HELP_PANEL_TITLE in line), None)
    if idx is None:
        return []

    _SGR_ESCAPE_RE = re.compile(r'\x1b\[[0-9;:]*m')
    names: list[str] = []
    for raw in lines[idx + 1 :]:
        line = _SGR_ESCAPE_RE.sub('', raw)
        stripped = line.strip()
        if not stripped:
            if names:
                break
            continue
        if stripped.startswith('|'):
            cells = [c.strip() for c in stripped.split('|') if c.strip()]
            if cells:
                names.append(cells[0].split(None, 1)[0])
            continue
        if line[:1].isspace():
            names.append(stripped.split(None, 1)[0])
            continue
        break
    return names


def get_subdirs_absolute_paths(path: Path) -> list[str]:
    """
    Get a list of files with absolute path in a given `path` folder
    """
    return [f'{dir_path}/{file_name}' for dir_path, _, file_names in os.walk(path) for file_name in file_names]


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
    replace_in_file(
        'main/build_test_app.c', '// placeholder_inside_main', 'esp_chip_info_t chip_info; esp_chip_info(&chip_info);'
    )

    with pytest.raises(subprocess.CalledProcessError) as exc_info:
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


def test_idf_build_with_env_var_sdkconfig_defaults(test_app_copy: Path, default_idf_env: EnvDict) -> None:
    with open(test_app_copy / 'sdkconfig.test', 'w') as fw:
        fw.write('CONFIG_BT_ENABLED=y')

    default_idf_env['SDKCONFIG_DEFAULTS'] = 'sdkconfig.test'
    # idf.py reconfigure would generate the sdkconfig file
    run_idf_py('reconfigure', env=default_idf_env)

    with open(test_app_copy / 'sdkconfig') as fr:
        assert 'CONFIG_BT_ENABLED=y' in fr.read()


@pytest.mark.usefixtures('test_app_copy')
@pytest.mark.test_app_copy('examples/system/efuse')
def test_efuse_summary_cmake_functions(default_idf_env: EnvDict) -> None:
    default_idf_env['IDF_CI_BUILD'] = '1'
    output = run_idf_py('efuse-filter', env=default_idf_env)
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
    (test_app_copy / 'python').write_text(
        textwrap.dedent(
            """
            #!/bin/sh
            echo "idf.py has executed '/usr/bin/env python' or similar"
            exit 1
            """
        )
    )
    st = os.stat(test_app_copy / 'python')
    # equivalent to 'chmod +x ./python'
    os.chmod((test_app_copy / 'python'), st.st_mode | stat.S_IEXEC)

    env_dict['PATH'] = str(test_app_copy) + os.pathsep + env_dict['PATH']
    # python is loaded from env:$PATH
    # but since false interpreter is provided there, python needs to be specified as argument
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
    # python is loaded from env:$PATH
    # but since false interpreter is provided there, python needs to be specified as argument
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
    data = json.load(open(test_app_copy / 'build' / 'project_description.json'))
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
    append_to_file(
        test_app_copy / 'CMakeLists.txt',
        f'add_custom_target(custom_target COMMAND ${{CMAKE_COMMAND}} -E echo "{msg}")',
    )
    ret = idf_py('custom_target')
    assert msg in ret.stdout, 'Custom target did not produce expected output'


@pytest.mark.usefixtures('test_app_copy')
def test_idf_py_help_without_build_dir_has_no_cmake_custom_targets_section(idf_py: IdfPyFunc) -> None:
    """With no configured build directory, root help must not advertise CMake custom targets."""
    ret = idf_py('--help')
    assert CMAKE_CUSTOM_TARGETS_HELP_PANEL_TITLE not in ret.stdout


@pytest.mark.buildv2_skip(
    'cmakev2 generates per-project phony targets (${PROJECT_NAME}_binary, _check_size, _mapfile, .map, '
    'ldgen_libraries.in_library_${PROJECT_NAME}) that the static allowlist in '
    'tools/idf_py_actions/help_custom_targets_skip.py was designed around v1 hard-coded names.'
)
@pytest.mark.usefixtures('test_app_copy')
def test_idf_py_help_after_configure_with_no_custom_targets_has_no_section(idf_py: IdfPyFunc) -> None:
    """After configure, if the project defines no custom targets, `idf.py --help` must not show the section."""
    idf_py('reconfigure')
    ret = idf_py('--help')
    assert CMAKE_CUSTOM_TARGETS_HELP_PANEL_TITLE not in ret.stdout


@pytest.mark.buildv2_skip(
    'cmakev2 generates per-project phony targets (${PROJECT_NAME}_binary, _check_size, _mapfile, .map, '
    'ldgen_libraries.in_library_${PROJECT_NAME}) that the static allowlist in '
    'tools/idf_py_actions/help_custom_targets_skip.py was designed around v1 hard-coded names.'
)
@pytest.mark.usefixtures('test_app_copy')
def test_idf_py_help_lists_cmake_custom_targets_after_configure(
    idf_py: IdfPyFunc, test_app_copy: Path, default_idf_env: EnvDict
) -> None:
    """After configure, project-only phony targets should appear under CMake Custom Targets in idf.py --help."""
    tgt = 'idf_py_help_visible_custom_tgt'
    append_to_file(
        test_app_copy / 'CMakeLists.txt',
        f'add_custom_target({tgt} COMMAND ${{CMAKE_COMMAND}} -E echo "ok")\n',
    )
    idf_py('reconfigure')
    env = {**default_idf_env, 'RICH_CLICK_THEME': 'plain-slim'}
    ret = run_idf_py('--help', env=env)
    assert CMAKE_CUSTOM_TARGETS_HELP_PANEL_TITLE in ret.stdout
    assert tgt in ret.stdout

    tools_dir = str(Path(EXT_IDF_PATH) / 'tools')
    if tools_dir not in sys.path:
        sys.path.insert(0, tools_dir)
    from idf_py_actions.help_custom_targets_skip import IDF_PY_HELP_SKIP_TARGETS  # noqa: E402
    from idf_py_actions.help_custom_targets_skip import help_phony_name_passes_shape_policy  # noqa: E402

    names = _parse_idf_py_help_cmake_custom_target_names(ret.stdout)
    assert tgt in names, f'{tgt} not found in the CMake custom target printed in stdout'
    for n in names:
        assert help_phony_name_passes_shape_policy(n), (
            f'Target {n!r} in CMake Custom Targets violates shape policy (prefix/suffix/substring/path). '
            'Fix filtering in tools/idf.py or adjust HELP_PHONY_NAME_* / help_phony_name_passes_shape_policy in '
            'tools/idf_py_actions/help_custom_targets_skip.py.'
        )
    unexpected = sorted({n for n in names if n not in IDF_PY_HELP_SKIP_TARGETS and n != tgt})
    assert not unexpected, (
        f'Unexpected CMake custom target(s) in idf.py --help: {unexpected!r}.\n'
        'These are usually internal phony targets. Add each name to IDF_PY_HELP_SKIP_TARGETS in '
        'tools/idf_py_actions/help_custom_targets_skip.py (keep the set sorted), then re-run this test.\n'
        'If the leak matches a pattern (prefix, suffix, or substring), adjust HELP_PHONY_NAME_* in that module '
        'instead of the frozenset.\n'
        'If a name is intentionally user-visible for this test app only, extend the allowlist in '
        'test_idf_py_help_lists_cmake_custom_targets_after_configure in tools/test_build_system/test_common.py '
        f'(expected project target name: {tgt!r}).'
    )


@pytest.mark.usefixtures('test_app_copy')
def test_idf_py_help_splits_multi_output_ninja_phony_targets(
    idf_py: IdfPyFunc, test_app_copy: Path, default_idf_env: EnvDict
) -> None:
    """Multi-output Ninja `build ...: phony` lines must yield separate target names (not a single whitespace string)."""
    a = 'idf_py_help_multi_out_a'
    b = 'idf_py_help_multi_out_b'
    c = 'idf_py_help_multi_out_c'
    idf_py('reconfigure')

    # Inject a multi-output phony line directly into build.ninja and verify help parsing splits it.
    append_to_file(test_app_copy / 'build' / 'build.ninja', f'\nbuild {a} {b} {c}: phony\n')
    env = {**default_idf_env, 'RICH_CLICK_THEME': 'plain-slim'}
    ret = run_idf_py('--help', env=env)

    names = _parse_idf_py_help_cmake_custom_target_names(ret.stdout)
    assert a in names
    assert b in names
    assert c in names
    assert f'{a} {b} {c}' not in names


def test_create_component_project(idf_copy: Path) -> None:
    logging.info('Create project and component using idf.py and build it')
    run_idf_py('-C', 'projects', 'create-project', 'temp_test_project', workdir=idf_copy)
    run_idf_py('-C', 'components', 'create-component', 'temp_test_component', workdir=idf_copy)
    replace_in_file(
        idf_copy / 'projects' / 'temp_test_project' / 'main' / 'temp_test_project.c',
        '{\n\n}',
        '\n'.join(['{', '\tfunc();', '}']),
    )
    replace_in_file(
        idf_copy / 'projects' / 'temp_test_project' / 'main' / 'temp_test_project.c',
        '#include <stdio.h>',
        '\n'.join(['#include <stdio.h>', '#include "temp_test_component.h"']),
    )
    run_idf_py('build', workdir=(idf_copy / 'projects' / 'temp_test_project'))


def test_create_project_cpp(idf_copy: Path) -> None:
    logging.info('Create C++ project with idf.py create-project --cpp')
    proj_dir = idf_copy / 'projects' / 'cpp_test_project'
    run_idf_py('-C', 'projects', 'create-project', '--cpp', 'cpp_test_project', workdir=idf_copy)
    main_cpp = proj_dir / 'main' / 'cpp_test_project.cpp'
    assert main_cpp.is_file()
    text = main_cpp.read_text(encoding='utf-8')
    assert 'extern "C" void app_main(void)' in text
    cmake = (proj_dir / 'main' / 'CMakeLists.txt').read_text(encoding='utf-8')
    assert 'cpp_test_project.cpp' in cmake
    # Avoid `assert 'cpp_test_project.c' not in cmake`: that string is a substring of `cpp_test_project.cpp`.
    assert 'SRCS "cpp_test_project.c"' not in cmake
    run_idf_py('build', workdir=str(proj_dir))


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
    def change_file_permissions(src: Path, write_permission: bool) -> None:
        dir_permission = 0o777 if write_permission else 0o555  # all or read & execute
        file_permission = 0o777 if write_permission else 0o444  # all or readonly or all
        for root, dirs, files in os.walk(src):
            for name in dirs:
                os.chmod(os.path.join(root, name), dir_permission)
            for name in files:
                path = os.path.join(root, name)
                if '/bin/' in path:
                    continue  # skip executables
                os.chmod(os.path.join(root, name), file_permission)

    logging.info('Check that command for creating new project will success if the IDF itself is readonly.')
    change_file_permissions(idf_copy, write_permission=False)
    try:
        run_idf_py('create-project', '--path', str(idf_copy / 'example_proj'), 'temp_test_project')
    except Exception:
        raise
    else:
        change_file_permissions(idf_copy, write_permission=True)
        shutil.rmtree(idf_copy)


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
    ret = idf_py(
        'docs',
        '--no-browser',
        '--language',
        'en',
        '--version',
        'v4.2.1',
        '--target',
        'esp32',
        '--starting-page',
        'get-started',
    )
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
    (test_app_copy / 'sdkconfig').write_text(
        '\n'.join(['CONFIG_COMPILER_OPTIMIZATION_SIZE=y', 'CONFIG_ESPTOOLPY_FLASHFREQ_80M=y'])
    )
    idf_py('save-defconfig')
    assert not file_contains(test_app_copy / 'sdkconfig.defaults', 'CONFIG_IDF_TARGET'), (
        'CONFIG_IDF_TARGET should not be in sdkconfig.defaults'
    )
    assert file_contains(test_app_copy / 'sdkconfig.defaults', 'CONFIG_COMPILER_OPTIMIZATION_SIZE=y'), (
        'Missing CONFIG_COMPILER_OPTIMIZATION_SIZE=y in sdkconfig.defaults'
    )
    assert file_contains(test_app_copy / 'sdkconfig.defaults', 'CONFIG_ESPTOOLPY_FLASHFREQ_80M=y'), (
        'Missing CONFIG_ESPTOOLPY_FLASHFREQ_80M=y in sdkconfig.defaults'
    )
    idf_py('fullclean')
    (test_app_copy / 'sdkconfig').unlink()
    (test_app_copy / 'sdkconfig.defaults').unlink()
    idf_py('set-target', 'esp32c3')
    (test_app_copy / 'sdkconfig').write_text('CONFIG_PARTITION_TABLE_OFFSET=0x8001')
    idf_py('save-defconfig')
    assert file_contains(test_app_copy / 'sdkconfig.defaults', 'CONFIG_IDF_TARGET="esp32c3"'), (
        'Missing CONFIG_IDF_TARGET="esp32c3" in sdkconfig.defaults'
    )
    assert file_contains(test_app_copy / 'sdkconfig.defaults', 'CONFIG_PARTITION_TABLE_OFFSET=0x8001'), (
        'Missing CONFIG_PARTITION_TABLE_OFFSET=0x8001 in sdkconfig.defaults'
    )


def test_merge_bin_cmd(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Test if merge-bin command works correctly')
    idf_py('merge-bin')
    assert (test_app_copy / 'build' / 'merged-binary.bin').is_file()
    idf_py('merge-bin', '--output', 'merged-binary-2.bin')
    assert (test_app_copy / 'build' / 'merged-binary-2.bin').is_file()
    idf_py('merge-bin', '--format', 'hex')
    assert (test_app_copy / 'build' / 'merged-binary.hex').is_file()


def test_hints_components_loading(
    idf_copy: Path, test_app_copy: Path, idf_py: IdfPyFunc, request: pytest.FixtureRequest
) -> None:
    logging.info('Testing component hint loading mechanism')
    logging.debug('Creating test IDF component')

    # Create a test IDF component with hints
    idf_test_component_dir = idf_copy / 'components' / 'test_idf_comp'
    idf_test_component_dir.mkdir(parents=True, exist_ok=True)
    idf_component_hints = textwrap.dedent("""
        -
            re: "test_idf_component_error"
            hint: "HINT FROM IDF COMPONENT: This is a test hint from ESP-IDF component"
        """)
    (idf_test_component_dir / 'hints.yml').write_text(idf_component_hints)
    (idf_test_component_dir / 'CMakeLists.txt').write_text(
        'idf_component_register(SRCS "test_comp.c" INCLUDE_DIRS ".")'
    )
    (idf_test_component_dir / 'test_comp.c').touch()

    logging.debug('Creating test project component')
    # Create a test project component with hints
    project_component_dir = test_app_copy / 'components' / 'test_project_comp'
    project_component_dir.mkdir(parents=True, exist_ok=True)
    project_component_hints = textwrap.dedent("""
        -
            re: "test_project_component_error"
            hint: "HINT FROM PROJECT COMPONENT: This is a test hint from project component"
        """)
    (project_component_dir / 'hints.yml').write_text(project_component_hints)
    (project_component_dir / 'CMakeLists.txt').write_text('idf_component_register(SRCS "test_comp.c" INCLUDE_DIRS ".")')
    (project_component_dir / 'test_comp.c').touch()

    error_code = """
    test_idf_component_error();
    test_project_component_error();
    """
    replace_in_file(test_app_copy / 'main' / 'build_test_app.c', '// placeholder_inside_main', error_code)

    # The default test app in buildv2 only includes the 'main' component. Hence, the IDF component
    # and project components must be explicitly added as dependencies for them to be included in the build.
    # Consequently, the hints from the IDF and project components will not be displayed in the build output
    # unless they are explicitly required. In contrast, buildv1 automatically includes all discovered components
    # unless MINIMAL_BUILD is set. Hence, add the components to the main component's REQUIRES list.
    if request.config.getoption('buildv2', False):
        replace_in_file(
            test_app_copy / 'main' / 'CMakeLists.txt',
            '# placeholder_inside_idf_component_register',
            'REQUIRES test_idf_comp test_project_comp',
        )

    ret = idf_py('build', check=False)
    assert 'HINT FROM IDF COMPONENT' in ret.stderr, 'Hint from IDF component should be displayed in build output'
    assert 'HINT FROM PROJECT COMPONENT' in ret.stderr, (
        'Hint from project component should be displayed in build output'
    )


def test_merged_hints_artifact_in_build_dir(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    """Check that hints.yml is generated in the build directory and that hints from all components are merged"""
    # Create a local component with a uniquely identifiable hint entry so we
    # can verify it ends up in the merged output.
    test_comp_dir = test_app_copy / 'components' / 'test_hint_comp'
    test_comp_dir.mkdir(parents=True, exist_ok=True)
    (test_comp_dir / 'CMakeLists.txt').write_text('idf_component_register()\n')
    (test_comp_dir / 'hints.yml').write_text(
        '- re: "UNIQUE_TEST_HINT_MARKER_12345"\n  hint: "This is a test hint for merge verification"\n'
    )
    # In buildv2, only components in the REQUIRES chain are included in
    # build_component_paths. Add test_hint_comp so its hints are merged.
    # This call is harmless in v1 (all components are auto-discovered).
    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES test_hint_comp',
    )

    idf_py('reconfigure')
    hints_file = test_app_copy / 'build' / 'hints.yml'
    assert hints_file.is_file(), 'hints.yml should exist in the build directory after reconfigure'
    content = hints_file.read_text(encoding='utf-8')
    parsed = yaml.safe_load(content)
    assert isinstance(parsed, list), 'hints.yml should be a valid YAML list'
    assert len(parsed) > 0, 'hints.yml should be non-empty'

    # Verify hints from the custom component are actually merged in
    hint_patterns = [entry.get('re', '') for entry in parsed if isinstance(entry, dict)]
    assert any('UNIQUE_TEST_HINT_MARKER_12345' in p for p in hint_patterns), (
        'Custom component hint should be present in merged hints.yml'
    )


@pytest.mark.buildv2_skip('hello_world uses cmake/project.cmake (v1 only)')
def test_merged_hints_artifact_real_project(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    """Check that hints.yml is generated in a custom build directory (-B flag)"""
    # Verify the build dir is dynamic, not hardcoded
    idf_py('-B', 'custom_build', 'reconfigure')
    custom_hints_file = test_app_copy / 'custom_build' / 'hints.yml'
    assert custom_hints_file.is_file(), 'hints.yml should exist in a custom build directory'


def test_sbom_create_cmd(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Test if sbom-create command works correctly')
    idf_py('sbom-create', '--spdx-file', 'test_app.spdx')
    assert (test_app_copy / 'test_app.spdx').is_file()
