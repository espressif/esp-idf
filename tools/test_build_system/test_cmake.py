# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
import os
import re
import shutil
import sys
from pathlib import Path

import pytest
from test_build_system_helpers import EnvDict
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import append_to_file
from test_build_system_helpers import file_contains
from test_build_system_helpers import replace_in_file
from test_build_system_helpers import run_cmake
from test_build_system_helpers import run_cmake_and_build
from test_build_system_helpers import run_idf_py


# This test checks multiple targets in one test function. It would be better to have each target
# tested in a isolated test case, but that would mean doing idf_copy each time, and copying takes most of the time
@pytest.mark.usefixtures('idf_copy')
def test_build_custom_cmake_project(test_app_copy: Path) -> None:
    # Test is compatible with any target. Random targets in the list are selected for performance reasons
    idf_path = Path(os.environ['IDF_PATH'])
    for target in ['esp32', 'esp32c2', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32p4', 'esp32s2', 'esp32s3']:
        logging.info(f'Test build ESP-IDF as a library to a custom CMake projects for {target}')
        run_cmake_and_build(
            str(idf_path / 'examples' / 'build_system' / 'cmake' / 'idf_as_lib'),
            '-G',
            'Ninja',
            '-DCMAKE_TOOLCHAIN_FILE={}'.format(idf_path / 'tools' / 'cmake' / f'toolchain-{target}.cmake'),
            f'-DTARGET={target}',
        )
        assert file_contains((test_app_copy / 'build' / 'compile_commands.json'), '"command"')
        shutil.rmtree(test_app_copy / 'build')
        os.remove(idf_path / 'examples' / 'build_system' / 'cmake' / 'idf_as_lib' / 'sdkconfig')


@pytest.mark.skipif(
    sys.platform == 'win32',
    reason='On Win project is not buildable without system compiler on the host machine. (Win CI runners)',
)
@pytest.mark.usefixtures('idf_copy')
@pytest.mark.usefixtures('test_app_copy')
def test_build_custom_cmake_project_host() -> None:
    logging.info('Test build ESP-IDF as a library to a custom CMake projects for host')
    idf_path = Path(os.environ['IDF_PATH'])
    run_cmake_and_build(str(idf_path / 'examples' / 'build_system' / 'cmake' / 'idf_as_lib'), '-G', 'Ninja')


@pytest.mark.buildv2_skip('import_lib example uses cmakev1, not yet updated for buildv2 (IDF-14185)')
def test_build_cmake_library_with_toolchain_flags(test_app_copy: Path) -> None:
    logging.info('Building a project with CMake library imported with modified toolchain flags')
    idf_path = Path(os.environ['IDF_PATH'])
    # Enable Picolibc to verify that all flags are passed correctly to the external project.
    # In case something is missing, the build will fail on linking stage.
    # Note: To enable Picolibc, IDF_EXPERIMENTAL_FEATURES must also be set for now.
    (test_app_copy / 'sdkconfig.defaults').write_text(
        '\n'.join(['CONFIG_IDF_EXPERIMENTAL_FEATURES=y', 'CONFIG_LIBC_PICOLIBC=y'])
    )

    import_lib_path = idf_path / 'examples' / 'build_system' / 'cmake' / 'import_lib'
    run_cmake_and_build(
        str(import_lib_path),
        '-G',
        'Ninja',
        f'-DSDKCONFIG_DEFAULTS={import_lib_path / "sdkconfig.defaults"};{test_app_copy / "sdkconfig.defaults"}',
    )


def check_flag_in_compile_commands(build_dir: Path, flag_to_find: str) -> None:
    with open(build_dir / 'build' / 'compile_commands.json', encoding='utf-8') as f:
        compile_commands = json.load(f)
        # check if compile_commands is an array
        if not isinstance(compile_commands, list):
            assert False, f'compile_commands is not a list: {compile_commands}'
        assert len(compile_commands) != 0, 'compile_commands is empty'
        for entry in compile_commands:
            command = entry['command']
            assert isinstance(command, str), f'command is not a string: {command}'
            flag_is_found = flag_to_find in command
            if flag_is_found:
                continue  # Flag found in command, no need to check response files
            # check if command contains response file paths starts with @
            response_file_paths = re.findall(r'@([^\s]+)', command)
            for response_file_path in response_file_paths:
                # check if the flag file contains flag_to_find
                try:
                    # Strip surrounding quotes and normalize the path
                    response_file_path = response_file_path.strip('"\'\\')
                    response_file_path = Path(response_file_path).resolve()
                    with open(response_file_path, encoding='utf-8') as f:
                        flags = f.read()
                        if flag_to_find in flags:
                            flag_is_found = True
                            break
                except FileNotFoundError:
                    assert False, f'{response_file_path} does not exist'
            if not flag_is_found:
                assert False, f'{flag_to_find} not found in {command}'


@pytest.mark.buildv2_skip('import_lib example uses cmakev1, not yet updated for buildv2 (IDF-14185)')
def test_build_cmake_library_psram_workaround(test_app_copy: Path) -> None:
    logging.info(
        'Building a project with CMake library imported and PSRAM workaround, all files compile with workaround'
    )
    idf_path = Path(os.environ['IDF_PATH'])
    (test_app_copy / 'sdkconfig.defaults').write_text(
        '\n'.join(['CONFIG_SPIRAM=y', 'CONFIG_SPIRAM_CACHE_WORKAROUND=y'])
    )
    run_cmake(
        '-G',
        'Ninja',
        '-DCOMPONENTS=main;esp_psram',
        '-DSDKCONFIG_DEFAULTS={}'.format(test_app_copy / 'sdkconfig.defaults'),
        str(idf_path / 'examples' / 'build_system' / 'cmake' / 'import_lib'),
    )
    check_flag_in_compile_commands(test_app_copy, '-mfix-esp32-psram-cache-issue')


def test_build_cmake_library_psram_strategies(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    # Add explicit REQUIRES for cmakev2 compatibility
    # Adding a public dependency on esp_psram should propagate the compile options publicly.
    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES esp_psram',
    )

    for strategy in ['DUPLDST', 'NOPS', 'MEMW']:
        logging.info(f'Test for external libraries in custom CMake projects with PSRAM strategy {strategy}')
        (test_app_copy / 'sdkconfig.defaults').write_text(
            '\n'.join(
                [
                    'CONFIG_SPIRAM=y',
                    f'CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_{strategy}=y',
                    'CONFIG_SPIRAM_CACHE_WORKAROUND=y',
                ]
            )
        )
        idf_py('reconfigure')
        check_flag_in_compile_commands(test_app_copy, f'-mfix-esp32-psram-cache-strategy={strategy.lower()}')
        (test_app_copy / 'sdkconfig').unlink()


def test_defaults_unspecified_build_args(idf_copy: Path) -> None:
    logging.info('Defaults set properly for unspecified idf_build_process args')
    idf_as_lib_path = idf_copy / 'examples' / 'build_system' / 'cmake' / 'idf_as_lib'
    append_to_file(
        idf_as_lib_path / 'CMakeLists.txt',
        '\n'.join(['idf_build_get_property(project_dir PROJECT_DIR)', 'message("Project directory: ${project_dir}")']),
    )
    ret = run_cmake(
        '..',
        '-G',
        'Ninja',
        '-DCMAKE_TOOLCHAIN_FILE={}'.format(str(idf_copy / 'tools' / 'cmake' / 'toolchain-esp32.cmake')),
        '-DTARGET=esp32',
        workdir=idf_as_lib_path,
    )
    assert f'Project directory: {str(idf_as_lib_path.as_posix())}' in ret.stderr


def test_build_example_on_host(default_idf_env: EnvDict) -> None:
    logging.info('Test if it can build the example to run on host')
    idf_path = Path(default_idf_env.get('IDF_PATH'))
    idf_as_lib_path = Path(idf_path, 'examples', 'build_system', 'cmake', 'idf_as_lib')
    try:
        target = 'esp32'
        run_cmake(
            '..',
            f'-DCMAKE_TOOLCHAIN_FILE={idf_path}/tools/cmake/toolchain-{target}.cmake',
            f'-DTARGET={target}',
            '-GNinja',
            workdir=idf_as_lib_path,
        )

        run_cmake('--build', '.', workdir=idf_as_lib_path)
    finally:
        shutil.rmtree(idf_as_lib_path / 'build', ignore_errors=True)


def test_cmake_preset_basic_functionality(test_app_copy: Path, default_idf_env: EnvDict) -> None:
    logging.info('Test basic CMake preset functionality with multiple presets')

    # Create a CMakePresets.json file with multiple configurations
    presets_content = {
        'version': 3,
        'configurePresets': [
            {
                'name': 'default',
                'binaryDir': 'build/default',
                'displayName': 'Default Configuration',
                'cacheVariables': {'SDKCONFIG': './build/default/sdkconfig'},
            },
            {
                'name': 'prod1',
                'binaryDir': 'build/prod1',
                'displayName': 'Production 1',
                'cacheVariables': {
                    'SDKCONFIG_DEFAULTS': 'sdkconfig.prod_common;sdkconfig.prod1',
                    'SDKCONFIG': './build/prod1/sdkconfig',
                },
            },
            {
                'name': 'prod2',
                'binaryDir': 'build/prod2',
                'displayName': 'Production 2',
                'cacheVariables': {
                    'SDKCONFIG_DEFAULTS': 'sdkconfig.prod_common;sdkconfig.prod2',
                    'SDKCONFIG': './build/prod2/sdkconfig',
                },
            },
        ],
    }

    # Write the presets file
    presets_file = test_app_copy / 'CMakePresets.json'
    with open(presets_file, 'w') as f:
        json.dump(presets_content, f, indent=4)

    (test_app_copy / 'sdkconfig.prod_common').write_text('CONFIG_LWIP_IPV6=y\n')
    (test_app_copy / 'sdkconfig.prod1').write_text('CONFIG_ESP_TASK_WDT_TIMEOUT_S=10\n')
    (test_app_copy / 'sdkconfig.prod2').write_text('CONFIG_ESP_TASK_WDT_TIMEOUT_S=20\n')

    # Test default preset auto-selection
    ret = run_idf_py('reconfigure')
    assert "CMake presets file found but no preset name given; using 'default' preset" in ret.stderr
    assert (test_app_copy / 'build' / 'default').is_dir()
    assert (test_app_copy / 'build' / 'default' / 'sdkconfig').is_file()
    # Verify that sdkconfig is NOT in the project root, even when no preset is specified but auto-selected
    assert not (test_app_copy / 'sdkconfig').is_file(), (
        'sdkconfig should not be in project root when preset specifies custom location'
    )

    # Test explicit preset selection
    ret = run_idf_py('--preset', 'prod1', 'reconfigure')
    assert (test_app_copy / 'build' / 'prod1').is_dir()
    assert (test_app_copy / 'build' / 'prod1' / 'sdkconfig').is_file()

    # Test env variable preset selection
    env_with_preset = default_idf_env.copy()
    env_with_preset['IDF_PRESET'] = 'prod2'
    ret = run_idf_py('reconfigure', env=env_with_preset)
    assert (test_app_copy / 'build' / 'prod2').is_dir()
    assert (test_app_copy / 'build' / 'prod2' / 'sdkconfig').is_file()


def test_cmake_preset_build_directory_precedence(test_app_copy: Path) -> None:
    logging.info('Test CMake preset build directory precedence - command line overrides preset')

    presets_content = {
        'version': 3,
        'configurePresets': [{'name': 'test_preset', 'binaryDir': 'build/preset_dir', 'displayName': 'Test Preset'}],
    }

    presets_file = test_app_copy / 'CMakePresets.json'
    with open(presets_file, 'w') as f:
        json.dump(presets_content, f, indent=4)

    ret = run_idf_py('--preset', 'test_preset', '-B', 'custom_build', 'reconfigure')

    assert 'custom_build' in ret.stdout
    assert 'build/preset_dir' not in ret.stdout


def test_cmake_preset_without_binary_dir(test_app_copy: Path) -> None:
    logging.info('Test CMake preset without binaryDir specification')

    presets_content = {
        'version': 3,
        'configurePresets': [{'name': 'no_binary_dir', 'displayName': 'Preset without binaryDir'}],
    }

    presets_file = test_app_copy / 'CMakePresets.json'
    with open(presets_file, 'w') as f:
        json.dump(presets_content, f, indent=4)

    ret = run_idf_py('--preset', 'no_binary_dir', 'reconfigure', check=False)
    assert 'does not specify the build directory ("binaryDir")' in ret.stderr


def test_cmake_preset_error_cases(test_app_copy: Path) -> None:
    logging.info('Test CMake preset error cases and invalid inputs')

    # Test with invalid JSON
    presets_file = test_app_copy / 'CMakePresets.json'
    presets_file.write_text('{ invalid json content')

    ret = run_idf_py('--preset', 'nonexistent', 'reconfigure', check=False)
    assert ret.returncode != 0

    # Clean up and test with valid JSON but nonexistent preset
    presets_content = {
        'version': 3,
        'configurePresets': [
            {'name': 'existing_preset', 'binaryDir': 'build/existing', 'displayName': 'Existing Preset'}
        ],
    }

    with open(presets_file, 'w') as f:
        json.dump(presets_content, f, indent=4)

    ret = run_idf_py('--preset', 'nonexistent_preset', 'reconfigure', check=False)
    assert ret.returncode != 0
    assert "No preset 'nonexistent_preset' found" in ret.stderr

    presets_content_no_name = {
        'version': 3,
        'configurePresets': [
            {'binaryDir': 'build/no_name', 'displayName': 'Preset without name'},
            {'name': 'valid_preset', 'binaryDir': 'build/valid', 'displayName': 'Valid Preset'},
        ],
    }

    with open(presets_file, 'w') as f:
        json.dump(presets_content_no_name, f, indent=4)

    ret = run_idf_py('reconfigure', check=False)
    assert 'Found CMake preset without a name' in ret.stderr
    assert ret.returncode != 0

    presets_content_no_version = {
        'configurePresets': [
            {'name': 'no_version', 'binaryDir': 'build/no_version', 'displayName': 'Preset without version'},
        ],
    }

    with open(presets_file, 'w') as f:
        json.dump(presets_content_no_version, f, indent=4)

    ret = run_idf_py('reconfigure', check=False)
    assert 'Found CMake preset file without a version' in ret.stderr
    assert ret.returncode != 0


def test_cmake_preset_sdkconfig_defaults_integration(test_app_copy: Path) -> None:
    logging.info('Test CMake preset integration with SDKCONFIG_DEFAULTS')

    presets_content = {
        'version': 3,
        'configurePresets': [
            {
                'name': 'with_defaults',
                'binaryDir': 'build/with_defaults',
                'displayName': 'Preset with SDKCONFIG_DEFAULTS',
                'cacheVariables': {
                    'SDKCONFIG_DEFAULTS': 'sdkconfig.preset1;sdkconfig.preset2',
                    'SDKCONFIG': './build/with_defaults/sdkconfig',
                },
            },
            {
                'name': 'prod1',
                'binaryDir': 'build/prod1',
                'displayName': 'Production 1',
                'cacheVariables': {
                    'SDKCONFIG_DEFAULTS': 'sdkconfig.prod_common;sdkconfig.prod1',
                    'SDKCONFIG': './build/prod1/sdkconfig',
                },
            },
        ],
    }

    presets_file = test_app_copy / 'CMakePresets.json'
    with open(presets_file, 'w') as f:
        json.dump(presets_content, f, indent=4)

    (test_app_copy / 'sdkconfig.preset1').write_text('CONFIG_LWIP_IPV6=y\n')
    (test_app_copy / 'sdkconfig.preset2').write_text('CONFIG_ESP_TASK_WDT_TIMEOUT_S=15\n')

    run_idf_py('--preset', 'with_defaults', 'reconfigure')
    assert (test_app_copy / 'build' / 'with_defaults').is_dir()

    sdkconfig_path = test_app_copy / 'build' / 'with_defaults' / 'sdkconfig'
    assert sdkconfig_path.is_file()

    sdkconfig_content = sdkconfig_path.read_text()
    assert 'CONFIG_LWIP_IPV6=y' in sdkconfig_content
    assert 'CONFIG_ESP_TASK_WDT_TIMEOUT_S=15' in sdkconfig_content
