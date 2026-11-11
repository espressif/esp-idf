# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Subprojects added with idf_subproject_add().

The subproject added to the test app is shaped like the ULP program: it is built
from components, but it is not an application, so it builds no bootloader and
links no esp_system. A component that both projects discover declares the
config options that the subproject imports from the application.
"""

import os
import subprocess
from pathlib import Path

import pytest
from test_build_system_helpers import EnvDict
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import append_to_file
from test_build_system_helpers import replace_in_file

SENSOR_FILES = {
    'components/shared_cfg/CMakeLists.txt': 'idf_component_register()\n',
    'components/shared_cfg/Kconfig': """
menu "Shared"
    config SHARED_SENSOR_COUNT
        int "How many sensors there are"
        default 4
endmenu
""",
    'main/sensor/CMakeLists.txt': """
cmake_minimum_required(VERSION 3.22)
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../components/shared_cfg")
include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
idf_build_set_property(EXCLUDE_COMPONENTS bootloader APPEND)
project(sensor C ASM)
idf_project_init()
message(STATUS "Sensor CMake: SHARED_SENSOR_COUNT=${CONFIG_SHARED_SENSOR_COUNT}")
idf_build_executable(sensor COMPONENTS main)
idf_create_menuconfig(sensor TARGET menuconfig)
idf_create_confserver(sensor TARGET confserver)
idf_create_save_defconfig()
idf_create_config_report(sensor TARGET config-report)
idf_build_generate_metadata(EXECUTABLE sensor)
""",
    'main/sensor/sdkconfig.defaults': 'CONFIG_SENSOR_SAMPLE_RATE_HZ=100\nCONFIG_SHARED_SENSOR_COUNT=2\n',
    'main/sensor/main/CMakeLists.txt': """
add_library(${COMPONENT_TARGET} STATIC sensor_main.c)
idf_component_include(sample_buffer)
target_link_libraries(${COMPONENT_TARGET} PRIVATE idf::sample_buffer)
""",
    'main/sensor/main/sensor_main.c': """
int sample_buffer_capacity(void);
int main(void) { return sample_buffer_capacity(); }
""",
    'main/sensor/components/sample_buffer/CMakeLists.txt': """
add_library(${COMPONENT_TARGET} STATIC sample_buffer.c)
""",
    'main/sensor/components/sample_buffer/Kconfig': """
menu "Sample buffer"
    config SENSOR_SAMPLE_RATE_HZ
        int "How often each sensor is sampled, in Hz"
        default 50
endmenu
""",
    'main/sensor/components/sample_buffer/sample_buffer.c': """
#include "sdkconfig.h"
int sample_buffer_capacity(void) { return CONFIG_SHARED_SENSOR_COUNT * CONFIG_SENSOR_SAMPLE_RATE_HZ; }
""",
}

ADD_SENSOR = """
idf_subproject_add(sensor
    CONFIG_OPTIONS CONFIG_SHARED_SENSOR_COUNT
    EXTERNALPROJECT_OPTIONS
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/sensor")
"""

REPLACED = 'imported from the main project, so the values set in this'
DIFFER = 'but this subproject resolves them to other values'
UNKNOWN = 'Check that the names are right'

# Stands in for the menuconfig UI, which kconfgen imports from esp_menuconfig.
# It sets a config option and saves, as a user would.
MENUCONFIG_SAVES = """
import os


def menuconfig(kconf):
    kconf.syms['SENSOR_SAMPLE_RATE_HZ'].set_value('200')
    kconf.write_config(os.environ['KCONFIG_CONFIG'])
    return True
"""


def _resolved() -> str:
    return Path('build/subprojects/sensor/config/sdkconfig.h').read_text()


def _log(result) -> str:  # type: ignore
    return ' '.join((result.stdout + result.stderr).split())


@pytest.fixture
def sensor(test_app_copy: Path) -> Path:
    for name, content in SENSOR_FILES.items():
        path = test_app_copy / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content)
    append_to_file(test_app_copy / 'main' / 'CMakeLists.txt', ADD_SENSOR)
    return test_app_copy


@pytest.mark.usefixtures('sensor')
def test_subproject_keeps_a_configuration_of_its_own(idf_py: IdfPyFunc) -> None:
    # A component of the subproject declares a config option. The subproject
    # sets it and the application never sees it.
    idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')

    own = Path('main/sensor/sdkconfig')
    assert own.exists(), 'the subproject kept no configuration of its own'
    assert 'CONFIG_SENSOR_SAMPLE_RATE_HZ=100' in own.read_text(), 'its own sdkconfig.defaults was not read'
    assert 'CONFIG_SENSOR_SAMPLE_RATE_HZ' not in Path('build/sdkconfig').read_text()

    # Changing the config option in the subproject's sdkconfig changes the subproject's build.
    replace_in_file(own, 'CONFIG_SENSOR_SAMPLE_RATE_HZ=100', 'CONFIG_SENSOR_SAMPLE_RATE_HZ=25')
    idf_py('build')
    assert '#define CONFIG_SENSOR_SAMPLE_RATE_HZ 25' in _resolved()


@pytest.mark.usefixtures('sensor')
def test_subproject_imports_what_the_application_sets(idf_py: IdfPyFunc) -> None:
    # The imported config option has the application's value in the subproject,
    # over the subproject's own sdkconfig.defaults, and follows a change in the
    # application. idf_subproject_add() builds the subproject on every build,
    # without BUILD_ALWAYS in the call. The first configure of the subproject
    # reports nothing.
    built = idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    assert REPLACED not in _log(built)
    assert 'Manually-specified variables were not used' not in _log(built), _log(built)
    assert '#define CONFIG_SHARED_SENSOR_COUNT 4' in _resolved()
    assert 'CONFIG_SHARED_SENSOR_COUNT=4' in Path('main/sensor/sdkconfig').read_text()
    assert 'Sensor CMake: SHARED_SENSOR_COUNT=4' in _log(built)

    replace_in_file(Path('build/sdkconfig'), 'CONFIG_SHARED_SENSOR_COUNT=4', 'CONFIG_SHARED_SENSOR_COUNT=9')
    built = idf_py('build')
    assert REPLACED not in _log(built), 'a change in the application was reported as a change in the subproject'
    assert '#define CONFIG_SHARED_SENSOR_COUNT 9' in _resolved()


def test_subproject_ignores_the_inherited_configuration_environment(
    idf_py: IdfPyFunc, default_idf_env: EnvDict, sensor: Path
) -> None:
    # SDKCONFIG and DEPENDENCIES_LOCK are read from the environment, and
    # externalproject_add hands the subproject the environment the main project
    # was configured in. A subproject that kept them would resolve into the
    # application's sdkconfig and write its dependencies into the application's
    # lock file.
    (sensor / 'main/sensor/main/idf_component.yml').write_text('dependencies:\n  idf: ">=5.0"\n')
    default_idf_env['SDKCONFIG'] = str(sensor / 'build' / 'sdkconfig')
    default_idf_env['DEPENDENCIES_LOCK'] = str(sensor / 'application.lock')

    idf_py('-DIDF_TARGET=esp32c6', 'build')

    own = Path('main/sensor/sdkconfig')
    assert own.exists(), 'the subproject resolved the inherited SDKCONFIG instead of its own'
    assert 'CONFIG_SENSOR_SAMPLE_RATE_HZ=100' in own.read_text()

    application = Path('build/sdkconfig').read_text()
    assert 'CONFIG_SHARED_SENSOR_COUNT=4' in application, "the subproject overwrote the application's sdkconfig"
    assert 'CONFIG_SENSOR_SAMPLE_RATE_HZ' not in application, (
        "the subproject's config option was written to the application's sdkconfig"
    )
    assert Path('main/sensor/dependencies.lock').exists(), 'the subproject used the inherited DEPENDENCIES_LOCK'


@pytest.mark.usefixtures('sensor')
def test_subproject_follows_the_application_to_a_new_target(idf_py: IdfPyFunc) -> None:
    # The subproject does not choose its target. After set-target in the main
    # project, the subproject's sdkconfig still has the old target, and the
    # next build rewrites it for the new one.
    idf_py('-DIDF_TARGET=esp32c6', 'build')
    assert 'CONFIG_IDF_TARGET="esp32c6"' in Path('main/sensor/sdkconfig').read_text()

    idf_py('set-target', 'esp32h2')
    idf_py('build')

    assert 'CONFIG_IDF_TARGET="esp32h2"' in Path('main/sensor/sdkconfig').read_text()
    assert '#define CONFIG_IDF_TARGET "esp32h2"' in _resolved()
    assert 'CONFIG_SENSOR_SAMPLE_RATE_HZ=100' in Path('main/sensor/sdkconfig').read_text()


def test_subproject_builds_no_bootloader(idf_py: IdfPyFunc, sensor: Path) -> None:
    # The subproject discovers the bootloader component, which declares config
    # options the subproject may import, but only the application builds a
    # bootloader.
    replace_in_file(
        sensor / 'main/sensor/CMakeLists.txt', 'idf_build_set_property(EXCLUDE_COMPONENTS bootloader APPEND)\n', ''
    )
    idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    assert Path('build/bootloader/bootloader.bin').exists()
    assert not Path('build/subprojects/sensor/bootloader').exists(), 'the subproject built a bootloader'
    assert 'CONFIG_BOOTLOADER_OFFSET_IN_FLASH' in Path('main/sensor/sdkconfig').read_text()


def test_subproject_shares_the_main_project_configuration(
    idf_py: IdfPyFunc, default_idf_env: EnvDict, sensor: Path
) -> None:
    # With SHARED_CONFIG the subproject reads the application's sdkconfig,
    # writes none, and builds no bootloader. The configuration variables and
    # DEPENDENCIES_LOCK it inherits from the application's environment are not
    # used.
    replace_in_file(
        sensor / 'main/CMakeLists.txt', '    CONFIG_OPTIONS CONFIG_SHARED_SENSOR_COUNT\n', '    SHARED_CONFIG\n'
    )
    replace_in_file(
        sensor / 'main/sensor/CMakeLists.txt', 'idf_build_set_property(EXCLUDE_COMPONENTS bootloader APPEND)\n', ''
    )
    (sensor / 'main/sensor/main/idf_component.yml').write_text('dependencies:\n  idf: ">=5.0"\n')
    (sensor / 'inherited.defaults').write_text('CONFIG_SENSOR_SAMPLE_RATE_HZ=7\n')
    default_idf_env['SDKCONFIG_DEFAULTS'] = str(sensor / 'inherited.defaults')
    default_idf_env['GENERATE_SDKCONFIG'] = '1'
    default_idf_env['DEPENDENCIES_LOCK'] = str(sensor / 'application.lock')

    idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')

    assert '#define CONFIG_SHARED_SENSOR_COUNT 4' in _resolved()
    # Only the subproject declares this config option. Neither the subproject's
    # sdkconfig.defaults nor the inherited SDKCONFIG_DEFAULTS is applied.
    assert '#define CONFIG_SENSOR_SAMPLE_RATE_HZ 50' in _resolved()
    assert not Path('main/sensor/sdkconfig').exists(), 'the subproject kept a configuration of its own'
    assert 'CONFIG_SENSOR_SAMPLE_RATE_HZ' not in Path('build/sdkconfig').read_text(), (
        "the subproject wrote the application's sdkconfig"
    )
    assert Path('build/bootloader/bootloader.bin').exists()
    assert not Path('build/subprojects/sensor/bootloader').exists(), 'the subproject built a bootloader'
    assert Path('main/sensor/dependencies.lock').exists(), 'the subproject used the inherited DEPENDENCIES_LOCK'

    # The subproject is built with the application's changed configuration.
    replace_in_file(Path('build/sdkconfig'), 'CONFIG_SHARED_SENSOR_COUNT=4', 'CONFIG_SHARED_SENSOR_COUNT=9')
    idf_py('build')
    assert '#define CONFIG_SHARED_SENSOR_COUNT 9' in _resolved()


def test_shared_config_with_config_options_stops_the_build(idf_py: IdfPyFunc, sensor: Path) -> None:
    replace_in_file(
        sensor / 'main/CMakeLists.txt',
        '    CONFIG_OPTIONS CONFIG_SHARED_SENSOR_COUNT\n',
        '    SHARED_CONFIG\n    CONFIG_OPTIONS CONFIG_SHARED_SENSOR_COUNT\n',
    )
    failed = idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'reconfigure', check=False)
    assert failed.returncode != 0
    assert 'CONFIG_OPTIONS cannot be given with SHARED_CONFIG' in _log(failed), _log(failed)


def test_subproject_switches_between_own_and_shared_configuration(idf_py: IdfPyFunc, sensor: Path) -> None:
    # The subproject's CMake cache keeps the values of its last configure. A
    # switch to the shared configuration and back leaves none of them behind.
    idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    assert '#define CONFIG_SENSOR_SAMPLE_RATE_HZ 100' in _resolved()

    replace_in_file(
        sensor / 'main/CMakeLists.txt', '    CONFIG_OPTIONS CONFIG_SHARED_SENSOR_COUNT\n', '    SHARED_CONFIG\n'
    )
    idf_py('build')
    assert '#define CONFIG_SENSOR_SAMPLE_RATE_HZ 50' in _resolved(), 'the subproject still read its own sdkconfig'
    assert 'CONFIG_SENSOR_SAMPLE_RATE_HZ' not in Path('build/sdkconfig').read_text(), (
        "the subproject wrote the application's sdkconfig"
    )

    replace_in_file(
        sensor / 'main/CMakeLists.txt', '    SHARED_CONFIG\n', '    CONFIG_OPTIONS CONFIG_SHARED_SENSOR_COUNT\n'
    )
    idf_py('build')
    assert '#define CONFIG_SENSOR_SAMPLE_RATE_HZ 100' in _resolved(), (
        "the subproject still read the application's sdkconfig"
    )
    assert 'CONFIG_SENSOR_SAMPLE_RATE_HZ' not in Path('build/sdkconfig').read_text(), (
        "the subproject wrote the application's sdkconfig"
    )


def test_subproject_sdkconfig_is_given_by_the_caller(idf_py: IdfPyFunc, sensor: Path) -> None:
    # SDKCONFIG of idf_subproject_add() moves the subproject's sdkconfig, as
    # SDKCONFIG does for an application. A relative path is relative to the
    # subproject's directory.
    replace_in_file(
        sensor / 'main/CMakeLists.txt',
        '    EXTERNALPROJECT_OPTIONS\n',
        '    SDKCONFIG sdkconfig.a\n    EXTERNALPROJECT_OPTIONS\n',
    )
    idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    assert 'CONFIG_SENSOR_SAMPLE_RATE_HZ=100' in Path('main/sensor/sdkconfig.a').read_text()
    assert not Path('main/sensor/sdkconfig').exists(), 'the subproject used its default sdkconfig'

    # An absolute path. The changed path configures the subproject again.
    (sensor / 'configs').mkdir()
    absolute = (sensor / 'configs' / 'sensor.cfg').as_posix()
    replace_in_file(sensor / 'main/CMakeLists.txt', 'SDKCONFIG sdkconfig.a', f'SDKCONFIG "{absolute}"')
    replace_in_file(
        Path('main/sensor/sdkconfig.a'), 'CONFIG_SENSOR_SAMPLE_RATE_HZ=100', 'CONFIG_SENSOR_SAMPLE_RATE_HZ=25'
    )
    idf_py('build')
    assert 'CONFIG_SENSOR_SAMPLE_RATE_HZ=100' in Path(absolute).read_text()
    assert '#define CONFIG_SENSOR_SAMPLE_RATE_HZ 100' in _resolved()


@pytest.mark.parametrize(
    'main_edit,sensor_edit,reported',
    (
        pytest.param(
            ('    EXTERNALPROJECT_OPTIONS\n', '    EXTERNALPROJECT_OPTIONS\n        CMAKE_ARGS -DSDKCONFIG=other\n'),
            None,
            "'-DSDKCONFIG=other' sets the subproject's sdkconfig in the options of externalproject_add",
            id='cmake-args',
        ),
        pytest.param(
            (
                '    EXTERNALPROJECT_OPTIONS\n',
                '    EXTERNALPROJECT_OPTIONS\n        CMAKE_CACHE_ARGS -DSDKCONFIG:STRING=other\n',
            ),
            None,
            "'-DSDKCONFIG:STRING=other' sets the subproject's sdkconfig in the options of externalproject_add",
            id='cmake-cache-args',
        ),
        pytest.param(
            None,
            ('include($ENV{IDF_PATH}', 'set(SDKCONFIG "${CMAKE_CURRENT_LIST_DIR}/mine")\ninclude($ENV{IDF_PATH}'),
            'Give the path with the SDKCONFIG option of idf_subproject_add() in that project',
            id='set-in-subproject',
        ),
        pytest.param(
            ('    CONFIG_OPTIONS CONFIG_SHARED_SENSOR_COUNT\n', '    SHARED_CONFIG\n    SDKCONFIG sdkconfig.a\n'),
            None,
            'SDKCONFIG cannot be given with SHARED_CONFIG',
            id='shared-config',
        ),
        pytest.param(
            (
                '    EXTERNALPROJECT_OPTIONS\n',
                '    SDKCONFIG ${CMAKE_SOURCE_DIR}/build/sdkconfig\n    EXTERNALPROJECT_OPTIONS\n',
            ),
            None,
            "would keep its configuration in the main project's sdkconfig",
            id='main-sdkconfig',
        ),
    ),
)
def test_subproject_sdkconfig_set_elsewhere_stops_the_build(
    idf_py: IdfPyFunc,
    sensor: Path,
    main_edit: tuple[str, str] | None,
    sensor_edit: tuple[str, str] | None,
    reported: str,
) -> None:
    # The main project decides where the subproject's sdkconfig is and lists the
    # path, so a path set anywhere else stops the build.
    if main_edit:
        replace_in_file(sensor / 'main/CMakeLists.txt', *main_edit)
    if sensor_edit:
        replace_in_file(sensor / 'main/sensor/CMakeLists.txt', *sensor_edit)
    failed = idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build', check=False)
    assert failed.returncode != 0
    assert reported in _log(failed), _log(failed)
    assert 'CONFIG_SENSOR_SAMPLE_RATE_HZ' not in Path('build/sdkconfig').read_text(), (
        "the subproject wrote the application's sdkconfig"
    )


def test_subprojects_from_one_directory_share_its_sdkconfig(idf_py: IdfPyFunc, sensor: Path) -> None:
    # Two subprojects built from one directory use one sdkconfig, as the
    # executables of one application do. The second one is configured after the
    # first.
    append_to_file(
        sensor / 'main/CMakeLists.txt', ADD_SENSOR.replace('idf_subproject_add(sensor', 'idf_subproject_add(sensor2')
    )
    idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    for subproject in ('sensor', 'sensor2'):
        header = Path(f'build/subprojects/{subproject}/config/sdkconfig.h').read_text()
        assert '#define CONFIG_SENSOR_SAMPLE_RATE_HZ 100' in header, subproject

    replace_in_file(
        Path('main/sensor/sdkconfig'), 'CONFIG_SENSOR_SAMPLE_RATE_HZ=100', 'CONFIG_SENSOR_SAMPLE_RATE_HZ=25'
    )
    idf_py('build')
    for subproject in ('sensor', 'sensor2'):
        header = Path(f'build/subprojects/{subproject}/config/sdkconfig.h').read_text()
        assert '#define CONFIG_SENSOR_SAMPLE_RATE_HZ 25' in header, subproject

    # The build step runs kconfgen as well when the sdkconfig has changed, so
    # it is ordered too, after the whole first subproject.
    for step, first in (('configure', 'esp-idf/main/sensor-configure'), ('build', 'esp-idf/main/sensor')):
        stamp = f'esp-idf/main/sensor2-prefix/src/sensor2-stamp/sensor2-{step}'
        query = subprocess.run(
            ['ninja', '-C', 'build', '-t', 'query', stamp], capture_output=True, text=True, check=True
        )
        inputs = [line.strip().lstrip('|').strip() for line in query.stdout.splitlines()]
        assert first in inputs, f'the second subproject does not {step} after the first'


SHARED_OPTIONS = """
menu "Shared options of every kind"
    config SHARED_MASK
        hex "Mask"
        default 0x040
    config SHARED_LABEL
        string "Label"
        default "abc"
    config SHARED_FILTER
        bool "Filter"
        default y
    config SHARED_SINGLE
        bool "Single"
        default n
    config SHARED_LANES
        int
        default 1 if SHARED_SINGLE
        default 2
    choice SHARED_CLOCK
        prompt "Clock"
        default SHARED_CLOCK_RC
        config SHARED_CLOCK_RC
            bool "RC"
        config SHARED_CLOCK_XTAL
            bool "XTAL"
    endchoice
endmenu
"""

SENSOR_FILTER_USER = """
config SENSOR_SMOOTHING
    bool "Smooth the samples, which needs the filter"
    depends on SHARED_FILTER
    default y
"""


def test_subproject_imports_options_of_every_kind(idf_py: IdfPyFunc, sensor: Path) -> None:
    # Hex, string, a bool that is on and one that is off, a choice, and an
    # option derived from an imported one, in every file kconfgen generates.
    append_to_file(sensor / 'components/shared_cfg/Kconfig', SHARED_OPTIONS)
    append_to_file(sensor / 'main/sensor/components/sample_buffer/Kconfig', SENSOR_FILTER_USER)
    append_to_file(
        sensor / 'sdkconfig.defaults',
        'CONFIG_SHARED_MASK=0x80\nCONFIG_SHARED_LABEL="front door"\n'
        '# CONFIG_SHARED_FILTER is not set\nCONFIG_SHARED_SINGLE=y\nCONFIG_SHARED_CLOCK_XTAL=y\n',
    )
    replace_in_file(
        sensor / 'main/CMakeLists.txt',
        'CONFIG_OPTIONS CONFIG_SHARED_SENSOR_COUNT',
        'CONFIG_OPTIONS CONFIG_SHARED_SENSOR_COUNT CONFIG_SHARED_MASK CONFIG_SHARED_LABEL CONFIG_SHARED_FILTER\n'
        '                   CONFIG_SHARED_SINGLE CONFIG_SHARED_CLOCK_RC CONFIG_SHARED_CLOCK_XTAL',
    )
    built = idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    assert REPLACED not in _log(built) and UNKNOWN not in _log(built), _log(built)

    resolved = _resolved()
    for line in (
        '#define CONFIG_SHARED_MASK 0x80',
        '#define CONFIG_SHARED_LABEL "front door"',
        '#define CONFIG_SHARED_SINGLE 1',
        '#define CONFIG_SHARED_LANES 1',
        '#define CONFIG_SHARED_CLOCK_XTAL 1',
    ):
        assert line in resolved, line
    for undefined in ('CONFIG_SHARED_FILTER', 'CONFIG_SHARED_CLOCK_RC', 'CONFIG_SENSOR_SMOOTHING'):
        assert f'#define {undefined} ' not in resolved, undefined

    cmake = Path('build/subprojects/sensor/config/sdkconfig.cmake').read_text()
    assert 'set(CONFIG_SHARED_LABEL "front door")' in cmake
    assert 'set(CONFIG_SHARED_FILTER "")' in cmake

    # A build without any change reports nothing.
    rebuilt = idf_py('build')
    assert REPLACED not in _log(rebuilt), _log(rebuilt)


def test_config_options_given_in_a_variable(idf_py: IdfPyFunc, sensor: Path) -> None:
    # CONFIG_OPTIONS ends at EXTERNALPROJECT_OPTIONS, not at a name without the
    # CONFIG_ prefix, so the names can come from a variable, expanded or as one
    # quoted list.
    append_to_file(sensor / 'components/shared_cfg/Kconfig', SHARED_OPTIONS)
    append_to_file(sensor / 'sdkconfig.defaults', 'CONFIG_SHARED_MASK=0x80\nCONFIG_SHARED_LABEL="front door"\n')
    replace_in_file(
        sensor / 'main/CMakeLists.txt',
        'idf_subproject_add(sensor\n    CONFIG_OPTIONS CONFIG_SHARED_SENSOR_COUNT',
        'set(shared_options CONFIG_SHARED_SENSOR_COUNT CONFIG_SHARED_LABEL)\n'
        'idf_subproject_add(sensor\n    CONFIG_OPTIONS "${shared_options}" ${more_options} CONFIG_SHARED_MASK',
    )
    built = idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    assert UNKNOWN not in _log(built), _log(built)
    resolved = _resolved()
    for line in (
        '#define CONFIG_SHARED_SENSOR_COUNT 4',
        '#define CONFIG_SHARED_LABEL "front door"',
        '#define CONFIG_SHARED_MASK 0x80',
    ):
        assert line in resolved, line

    # Without EXTERNALPROJECT_OPTIONS, the options of externalproject_add are
    # not passed on, and the build stops.
    replace_in_file(sensor / 'main/CMakeLists.txt', '    EXTERNALPROJECT_OPTIONS\n', '')
    failed = idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'reconfigure', check=False)
    assert failed.returncode != 0
    assert 'the options of externalproject_add are required, after EXTERNALPROJECT_OPTIONS' in _log(failed)

    # Without CONFIG_OPTIONS either, they are refused as unexpected arguments.
    replace_in_file(
        sensor / 'main/CMakeLists.txt',
        '    CONFIG_OPTIONS "${shared_options}" ${more_options} CONFIG_SHARED_MASK\n',
        '',
    )
    failed = idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'reconfigure', check=False)
    assert failed.returncode != 0
    assert "unexpected argument 'SOURCE_DIR'" in _log(failed), _log(failed)


def test_option_declared_by_a_managed_component_is_imported(idf_py: IdfPyFunc, sensor: Path) -> None:
    # The component manager adds the component that declares the imported config
    # option to the subproject. Its config options are known only in the last
    # kconfgen pass, which is where the import is checked.
    (sensor / 'components/shared_cfg/CMakeLists.txt').write_text('add_library(${COMPONENT_TARGET} INTERFACE)\n')
    replace_in_file(
        sensor / 'main/sensor/CMakeLists.txt',
        'set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../components/shared_cfg")\n',
        '',
    )
    (sensor / 'main/sensor/main/idf_component.yml').write_text(
        'dependencies:\n  shared_cfg:\n    path: ../../../components/shared_cfg\n'
    )
    built = idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    assert DIFFER not in _log(built), _log(built)
    assert '#define CONFIG_SHARED_SENSOR_COUNT 4' in _resolved()

    # A value set in the subproject is reported once, not once per pass.
    replace_in_file(Path('main/sensor/sdkconfig'), 'CONFIG_SHARED_SENSOR_COUNT=4', 'CONFIG_SHARED_SENSOR_COUNT=7')
    built = idf_py('build')
    assert _log(built).count(REPLACED) == 1, _log(built)
    assert '#define CONFIG_SHARED_SENSOR_COUNT 4' in _resolved()


def test_bool_that_is_off_matches_one_without_a_value(idf_py: IdfPyFunc, sensor: Path) -> None:
    # A bool option that is off in the application and hidden by its
    # dependencies in the subproject is off in both, so the check accepts it.
    append_to_file(
        sensor / 'components/shared_cfg/Kconfig',
        SHARED_OPTIONS + 'config SHARED_PROBE\n    bool "Probe"\n    depends on SHARED_SINGLE\n    default n\n',
    )
    append_to_file(sensor / 'sdkconfig.defaults', 'CONFIG_SHARED_SINGLE=y\n')
    replace_in_file(
        sensor / 'main/CMakeLists.txt',
        'CONFIG_OPTIONS CONFIG_SHARED_SENSOR_COUNT',
        'CONFIG_OPTIONS CONFIG_SHARED_PROBE',
    )
    built = idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    assert DIFFER not in _log(built) and UNKNOWN not in _log(built), _log(built)
    assert '# CONFIG_SHARED_PROBE is not set' in Path('build/sdkconfig').read_text()
    assert 'CONFIG_SHARED_PROBE' not in Path('main/sensor/sdkconfig').read_text()


def test_only_the_listed_options_are_imported(idf_py: IdfPyFunc, sensor: Path) -> None:
    # Config options that both projects declare, but that are not listed, keep
    # the subproject's own values, even if the application sets them
    # differently. A name that starts with the name of a listed option is not
    # imported either.
    append_to_file(
        sensor / 'components/shared_cfg/Kconfig',
        'menu "Not imported"\n'
        '    config SHARED_SENSOR_COUNT_MAX\n        int "Most sensors"\n        default 8\n'
        '    config SHARED_GAIN\n        int "Gain"\n        default 1\n'
        '    config SHARED_TRACE\n        bool "Trace"\n        default n\n'
        'endmenu\n',
    )
    append_to_file(
        sensor / 'sdkconfig.defaults',
        'CONFIG_SHARED_SENSOR_COUNT_MAX=64\nCONFIG_SHARED_GAIN=5\nCONFIG_SHARED_TRACE=y\n',
    )
    append_to_file(
        sensor / 'main/sensor/sdkconfig.defaults',
        'CONFIG_SHARED_SENSOR_COUNT_MAX=16\nCONFIG_SHARED_GAIN=2\n',
    )
    idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')

    application = Path('build/sdkconfig').read_text()
    assert 'CONFIG_SHARED_GAIN=5' in application and 'CONFIG_SHARED_TRACE=y' in application
    resolved = _resolved()
    assert '#define CONFIG_SHARED_SENSOR_COUNT 4' in resolved, 'the listed option was not imported'
    assert '#define CONFIG_SHARED_SENSOR_COUNT_MAX 16' in resolved, 'an option with a longer name was imported'
    assert '#define CONFIG_SHARED_GAIN 2' in resolved, 'an option that is not listed was imported'
    assert 'CONFIG_SHARED_TRACE' not in resolved, 'an option that is not listed was imported'

    # A change in the application to an option that is not listed changes
    # nothing in the subproject.
    replace_in_file(Path('build/sdkconfig'), 'CONFIG_SHARED_GAIN=5', 'CONFIG_SHARED_GAIN=6')
    idf_py('build')
    assert '#define CONFIG_SHARED_GAIN 2' in _resolved()


@pytest.mark.usefixtures('sensor')
def test_value_set_in_the_subproject_is_replaced(idf_py: IdfPyFunc) -> None:
    # A value set for an imported config option in the subproject's
    # configuration does not last. It is replaced by the application's value,
    # and a warning says so, once.
    idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    replace_in_file(Path('main/sensor/sdkconfig'), 'CONFIG_SHARED_SENSOR_COUNT=4', 'CONFIG_SHARED_SENSOR_COUNT=7')

    built = idf_py('build')
    assert REPLACED in _log(built), _log(built)
    assert 'CONFIG_SHARED_SENSOR_COUNT=7 is replaced by CONFIG_SHARED_SENSOR_COUNT=4' in _log(built)
    assert '#define CONFIG_SHARED_SENSOR_COUNT 4' in _resolved()

    rebuilt = idf_py('build')
    assert REPLACED not in _log(rebuilt)


def test_value_from_another_build_is_not_reported(idf_py: IdfPyFunc, sensor: Path) -> None:
    # A second build directory of the application uses the same sdkconfig of
    # the subproject. The value that the other build imported into it was not
    # set in the subproject, so the first build in the new directory reports
    # nothing.
    idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    other_defaults = sensor / 'other.defaults'
    other_defaults.write_text('CONFIG_SHARED_SENSOR_COUNT=9\n')

    built = idf_py(
        '-B',
        'build_other',
        '-DIDF_TARGET=esp32c6',
        '-DSDKCONFIG=build_other/sdkconfig',
        f'-DSDKCONFIG_DEFAULTS={other_defaults}',
        'build',
    )
    assert REPLACED not in _log(built), _log(built)
    assert (
        '#define CONFIG_SHARED_SENSOR_COUNT 9' in Path('build_other/subprojects/sensor/config/sdkconfig.h').read_text()
    )


def test_value_saved_in_menuconfig_is_kept(idf_py: IdfPyFunc, default_idf_env: EnvDict, sensor: Path) -> None:
    # The configuration targets of a subproject that imports config options
    # read its sdkconfig, not the copy with the imported values. menuconfig
    # reads its input again after saving and writes the sdkconfig from it, so
    # with the copy the value saved in menuconfig would be lost.
    idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    stand_in = sensor / 'stand_in' / 'esp_menuconfig'
    stand_in.mkdir(parents=True)
    (stand_in / '__init__.py').write_text(MENUCONFIG_SAVES)
    env = dict(default_idf_env)
    env['PYTHONPATH'] = os.pathsep.join(filter(None, [str(stand_in.parent), env.get('PYTHONPATH')]))

    for target in ('menuconfig', 'save-defconfig'):
        subprocess.run(['cmake', '--build', 'build/subprojects/sensor', '--target', target], env=env, check=True)

    assert 'CONFIG_SENSOR_SAMPLE_RATE_HZ=200' in Path('main/sensor/sdkconfig').read_text(), (
        'the value saved in menuconfig was lost'
    )
    assert 'CONFIG_SENSOR_SAMPLE_RATE_HZ=200' in Path('main/sensor/sdkconfig.defaults').read_text()


def test_option_the_subproject_does_not_declare_stops_the_build(idf_py: IdfPyFunc, sensor: Path) -> None:
    # A config option that only the application declares has no place in the
    # subproject's configuration, so it cannot be imported.
    append_to_file(
        sensor / 'main/Kconfig.projbuild',
        'menu "Test app"\n    config APP_ONLY\n        int "x"\n        default 3\nendmenu\n',
    )
    replace_in_file(
        sensor / 'main/CMakeLists.txt', 'CONFIG_SHARED_SENSOR_COUNT', 'CONFIG_SHARED_SENSOR_COUNT CONFIG_APP_ONLY'
    )
    failed = idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build', check=False)
    assert failed.returncode != 0
    assert DIFFER in _log(failed), _log(failed)
    assert 'CONFIG_APP_ONLY: CONFIG_APP_ONLY=3 in the main project, no value in the subproject' in _log(failed)


IMPORT_FAILS = """
menu "Options that are not imported"
    config SHARED_DEPTH
        int "Depth"
        depends on SHARED_SINGLE
        default 3
endmenu
"""


@pytest.mark.parametrize(
    'listed,application,subproject,reported',
    (
        pytest.param(
            'CONFIG_SHARED_LANES',
            'CONFIG_SHARED_SINGLE=y\n',
            '',
            'CONFIG_SHARED_LANES: CONFIG_SHARED_LANES=1 in the main project, CONFIG_SHARED_LANES=2 in the subproject',
            id='no-prompt',
        ),
        pytest.param(
            'CONFIG_SHARED_DEPTH',
            'CONFIG_SHARED_SINGLE=y\nCONFIG_SHARED_DEPTH=5\n',
            '',
            'CONFIG_SHARED_DEPTH: CONFIG_SHARED_DEPTH=5 in the main project, no value in the subproject',
            id='hidden-in-subproject',
        ),
        pytest.param(
            'CONFIG_SHARED_DEPTH',
            '',
            'CONFIG_SHARED_SINGLE=y\n',
            'CONFIG_SHARED_DEPTH: no value in the main project, CONFIG_SHARED_DEPTH=3 in the subproject',
            id='hidden-in-application',
        ),
    ),
)
def test_import_that_does_not_take_effect_stops_the_build(
    idf_py: IdfPyFunc, sensor: Path, listed: str, application: str, subproject: str, reported: str
) -> None:
    # kconfgen does not take the value of a config option without a prompt, or
    # of one whose dependencies are not met. The build checks the result and
    # stops, and stops again on the next build until it is fixed.
    append_to_file(sensor / 'components/shared_cfg/Kconfig', SHARED_OPTIONS + IMPORT_FAILS)
    append_to_file(sensor / 'sdkconfig.defaults', application)
    append_to_file(sensor / 'main/sensor/sdkconfig.defaults', subproject)
    replace_in_file(
        sensor / 'main/CMakeLists.txt', 'CONFIG_OPTIONS CONFIG_SHARED_SENSOR_COUNT', f'CONFIG_OPTIONS {listed}'
    )
    failed = idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build', check=False)
    assert failed.returncode != 0
    assert DIFFER in _log(failed) and reported in _log(failed), _log(failed)

    failed = idf_py('build', check=False)
    assert failed.returncode != 0 and reported in _log(failed), 'the build went on after the check failed'

    # Importing the config option it depends on too makes the values agree. The
    # option without a prompt is then checked, not imported.
    replace_in_file(
        sensor / 'main/CMakeLists.txt', f'CONFIG_OPTIONS {listed}', f'CONFIG_OPTIONS {listed} CONFIG_SHARED_SINGLE'
    )
    built = idf_py('build')
    assert DIFFER not in _log(built), _log(built)


def test_misspelled_option_is_reported(idf_py: IdfPyFunc, sensor: Path) -> None:
    replace_in_file(
        sensor / 'main/CMakeLists.txt',
        'CONFIG_SHARED_SENSOR_COUNT',
        'CONFIG_SHARED_SENSOR_COUNT CONFIG_SHARED_SENSOR_CUONT',
    )
    built = idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    assert UNKNOWN in _log(built) and 'CONFIG_SHARED_SENSOR_CUONT' in _log(built), _log(built)
    assert '#define CONFIG_SHARED_SENSOR_COUNT 4' in _resolved()
