# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os.path
from pathlib import Path

from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file


def test_dependency_lock(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    replace_in_file(
        test_app_copy / 'CMakeLists.txt',
        search='# placeholder_after_include_project_cmake',
        replace='idf_build_set_property(DEPENDENCIES_LOCK dependencies.lock.${IDF_TARGET})',
    )

    idf_py('add-dependency', 'example/cmp')
    idf_py('fullclean')
    idf_py('reconfigure')
    assert os.path.isfile(test_app_copy / 'dependencies.lock.esp32')
    assert not os.path.isfile(test_app_copy / 'dependencies.lock')


def test_trimmed_components_still_passed_to_cmake(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    replace_in_file(
        test_app_copy / 'CMakeLists.txt',
        search='# placeholder_after_include_project_cmake',
        replace='set(COMPONENTS main)',
    )

    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        search='# placeholder_inside_idf_component_register',
        replace='REQUIRES foo',
    )

    os.makedirs(test_app_copy / 'components')
    idf_py('create-component', '-C', 'components', 'foo')
    idf_py('add-dependency', '--component', 'foo', 'example/cmp')

    idf_py('reconfigure')

    with open('dependencies.lock', 'r') as f:
        fs = f.read()

    assert '  example/cmp:' in fs


class TestOptionalDependencyWithKconfig:
    def test_direct_kconfig(self, idf_py: IdfPyFunc, test_app_copy: Path) -> None:
        (test_app_copy / 'main' / 'idf_component.yml').write_text("""
        dependencies:
            example/cmp:
              version: "*"
              rules:
                - if: $CONFIG{BOOTLOADER_PROJECT_VER} == 1
            """)

        idf_py('reconfigure')

        with open('dependencies.lock', 'r') as f:
            fs = f.read()
        assert '  example/cmp:' in fs

        (test_app_copy / 'main' / 'idf_component.yml').write_text("""
        dependencies:
            example/cmp:
              version: "*"
              rules:
                - if: $CONFIG{BOOTLOADER_PROJECT_VER} != 1
            """)

        idf_py('reconfigure')

        with open('dependencies.lock', 'r') as f:
            fs = f.read()
        assert '  example/cmp:' not in fs

    def test_missing_kconfig_first_round(self, idf_py: IdfPyFunc, test_app_copy: Path) -> None:
        (test_app_copy / 'main' / 'idf_component.yml').write_text("""
        dependencies:
          espressif/mdns:
            version: "*"

          example/cmp:
            version: "*"
            rules:
              - if: $CONFIG{MDNS_MAX_SERVICES} == 10 # mdns kconfig CONFIG_MDNS_MAX_SERVICES default to 10
            """)

        idf_py('reconfigure')

        with open('dependencies.lock', 'r') as f:
            fs = f.read()
        assert '  example/cmp:' in fs

    def test_kconfig_load_existing_sdkconfig_defaults(self, idf_py: IdfPyFunc, test_app_copy: Path) -> None:
        (test_app_copy / 'main' / 'idf_component.yml').write_text("""
        dependencies:
          example/cmp:
            version: "*"
            rules:
              - if: $CONFIG{BOOTLOADER_LOG_LEVEL_WARN} == True
            """)

        (test_app_copy / 'sdkconfig.defaults').write_text("""
            CONFIG_BOOTLOADER_LOG_LEVEL_WARN=y
            CONFIG_LOG_DEFAULT_LEVEL_WARN=y
        """)

        idf_py('reconfigure')

        with open('dependencies.lock', 'r') as f:
            fs = f.read()
        assert '  example/cmp:' in fs

    def test_kconfig_load_missing_sdkconfig_defaults(self, idf_py: IdfPyFunc, test_app_copy: Path) -> None:
        (test_app_copy / 'main' / 'idf_component.yml').write_text("""
        dependencies:
          espressif/mdns:
            version: "*"

          example/cmp:
            version: "*"
            rules:
              - if: $CONFIG{MDNS_MAX_SERVICES} == 30 # mdns kconfig CONFIG_MDNS_MAX_SERVICES default to 10
            """)

        (test_app_copy / 'sdkconfig.defaults').write_text("""
            CONFIG_MDNS_MAX_SERVICES=30
        """)

        idf_py('reconfigure')

        with open('dependencies.lock', 'r') as f:
            fs = f.read()
        assert '  example/cmp:' in fs

    def test_missing_kconfig_second_round(self, idf_py: IdfPyFunc, test_app_copy: Path) -> None:
        (test_app_copy / 'main' / 'idf_component.yml').write_text("""
        dependencies:
          espressif/mdns:
            version: "*"

          example/cmp:
            version: "*"
            rules:
              - if: $CONFIG{OF_COURSE_NO_ONE_USE_FOO} == "bar"
            """)

        res = idf_py('reconfigure', check=False)

        assert res.returncode != 0
        assert (
            f'OF_COURSE_NO_ONE_USE_FOO, introduced by example/cmp, '
            f'defined in {str(test_app_copy / "main" / "idf_component.yml")}' in res.stderr
        )
        assert 'Missing required kconfig option after retry.' in res.stderr
