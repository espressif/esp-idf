# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file


@pytest.mark.usefixtures('test_app_copy')
def test_native_cmake_component_builds(idf_py: IdfPyFunc) -> None:
    """add_library(${COMPONENT_TARGET} ...) without idf_component_register().

    COMPONENT_TARGET is pre-set before the component's CMakeLists.txt runs,
    so components can create their library target directly with add_library().
    """
    logging.info('Testing native CMake component with add_library')

    comp_dir = Path('components/native_lib_comp')
    comp_dir.mkdir(parents=True, exist_ok=True)
    (comp_dir / 'CMakeLists.txt').write_text(
        'add_library(${COMPONENT_TARGET} STATIC "native_lib.c")\n'
        'target_include_directories(${COMPONENT_TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")\n'
    )
    (comp_dir / 'native_lib.c').write_text('void native_lib_func(void) {}')

    replace_in_file(
        'main/CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES native_lib_comp\n    # placeholder_inside_idf_component_register',
    )

    idf_py('build')
    assert Path('build/build_test_app.elf').exists()


@pytest.mark.usefixtures('test_app_copy')
def test_native_cmake_depends_on_idf_component(idf_py: IdfPyFunc) -> None:
    """idf_component_include() lets native components depend on IDF components.

    The INTERFACE output variable provides the target name for linking.
    """
    logging.info('Testing native CMake component with IDF dependency via idf_component_include')

    comp_dir = Path('components/native_idf_dep')
    comp_dir.mkdir(parents=True, exist_ok=True)
    (comp_dir / 'CMakeLists.txt').write_text(
        'idf_component_include(log INTERFACE log_iface)\n'
        'add_library(${COMPONENT_TARGET} STATIC "native_idf.c")\n'
        'target_include_directories(${COMPONENT_TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")\n'
        'target_link_libraries(${COMPONENT_TARGET} PRIVATE ${log_iface})\n'
    )
    (comp_dir / 'native_idf.c').write_text(
        '#include "esp_log.h"\nvoid native_idf_func(void) { ESP_LOGI("test", "hello"); }\n'
    )

    replace_in_file(
        'main/CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES native_idf_dep\n    # placeholder_inside_idf_component_register',
    )

    idf_py('build')
    assert Path('build/build_test_app.elf').exists()


@pytest.mark.usefixtures('test_app_copy')
def test_native_cmake_transitive_deps(idf_py: IdfPyFunc) -> None:
    """Transitive deps between native CMake components via idf_component_include()."""
    logging.info('Testing native CMake transitive dependencies')

    comp_a = Path('components/native_comp_a')
    comp_a.mkdir(parents=True, exist_ok=True)
    (comp_a / 'CMakeLists.txt').write_text(
        'add_library(${COMPONENT_TARGET} STATIC "comp_a.c")\n'
        'target_include_directories(${COMPONENT_TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")\n'
    )
    (comp_a / 'comp_a.c').write_text('void func_native_a(void) {}')

    comp_b = Path('components/native_comp_b')
    comp_b.mkdir(parents=True, exist_ok=True)
    (comp_b / 'CMakeLists.txt').write_text(
        'idf_component_include(native_comp_a INTERFACE native_a_iface)\n'
        'add_library(${COMPONENT_TARGET} STATIC "comp_b.c")\n'
        'target_include_directories(${COMPONENT_TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")\n'
        'target_link_libraries(${COMPONENT_TARGET} PRIVATE ${native_a_iface})\n'
    )
    (comp_b / 'comp_b.c').write_text('void func_native_b(void) {}')

    replace_in_file(
        'main/CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES native_comp_b\n    # placeholder_inside_idf_component_register',
    )

    idf_py('build')
    assert Path('build/build_test_app.elf').exists()


@pytest.mark.usefixtures('test_app_copy')
def test_mixed_native_and_register_components(idf_py: IdfPyFunc) -> None:
    """Projects mixing add_library() and idf_component_register() components."""
    logging.info('Testing mixed native and v1-style components')

    native_comp = Path('components/native_mixed')
    native_comp.mkdir(parents=True, exist_ok=True)
    (native_comp / 'CMakeLists.txt').write_text(
        'add_library(${COMPONENT_TARGET} STATIC "native_m.c")\n'
        'target_include_directories(${COMPONENT_TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")\n'
    )
    (native_comp / 'native_m.c').write_text('void native_m_func(void) {}')

    v1_comp = Path('components/v1_mixed')
    v1_comp.mkdir(parents=True, exist_ok=True)
    (v1_comp / 'CMakeLists.txt').write_text('idf_component_register(SRCS "v1_m.c")\n')
    (v1_comp / 'v1_m.c').write_text('void v1_m_func(void) {}')

    replace_in_file(
        'main/CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES native_mixed v1_mixed\n    # placeholder_inside_idf_component_register',
    )

    idf_py('build')
    assert Path('build/build_test_app.elf').exists()


@pytest.mark.usefixtures('test_app_copy')
def test_legacy_component_compile_functions(idf_py: IdfPyFunc) -> None:
    """component_compile_options() and component_compile_definitions() apply to COMPONENT_LIB."""
    logging.info('Testing legacy component_compile_options and component_compile_definitions')

    comp_dir = Path('components/legacy_flags_comp')
    comp_dir.mkdir(parents=True, exist_ok=True)
    (comp_dir / 'CMakeLists.txt').write_text(
        'idf_component_register(SRCS "legacy_flags.c")\n'
        'component_compile_options(-DLEGACY_OPTION_FLAG=1)\n'
        'component_compile_definitions(LEGACY_DEFINITION_FLAG=1)\n'
    )
    (comp_dir / 'legacy_flags.c').write_text(
        '#if !defined(LEGACY_OPTION_FLAG)\n'
        '#error "component_compile_options() did not reach the component"\n'
        '#endif\n'
        '#if !defined(LEGACY_DEFINITION_FLAG)\n'
        '#error "component_compile_definitions() did not reach the component"\n'
        '#endif\n'
        'void legacy_flags_func(void) {}\n'
    )

    replace_in_file(
        'main/CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES legacy_flags_comp\n    # placeholder_inside_idf_component_register',
    )

    idf_py('build')
    assert Path('build/build_test_app.elf').exists()


@pytest.mark.usefixtures('test_app_copy')
def test_fetchcontent_available_in_component(idf_py: IdfPyFunc) -> None:
    """FetchContent is available in a v2 component: declare a dependency, make it available, and link it."""
    logging.info('Testing FetchContent available in v2 component with real usage')

    comp_dir = Path('components/fc_comp')
    comp_dir.mkdir(parents=True, exist_ok=True)
    fetched_dep = comp_dir / 'fetched_dep'
    fetched_dep.mkdir(parents=True, exist_ok=True)

    (fetched_dep / 'fetched.c').write_text('void fetched_dep_func(void) {}')
    (fetched_dep / 'fetched.h').write_text('void fetched_dep_func(void);\n')
    (fetched_dep / 'CMakeLists.txt').write_text(
        'add_library(fetched_dep_lib STATIC fetched.c)\n'
        'target_include_directories(fetched_dep_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")\n'
    )

    (comp_dir / 'fc.c').write_text('#include "fetched.h"\nvoid fc_func(void) { fetched_dep_func(); }\n')
    (comp_dir / 'CMakeLists.txt').write_text(
        'include(FetchContent)\n'
        'FetchContent_Declare(fetched_dep SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fetched_dep")\n'
        'FetchContent_MakeAvailable(fetched_dep)\n'
        'idf_component_register(SRCS "fc.c")\n'
        'target_link_libraries(${COMPONENT_TARGET} PRIVATE fetched_dep_lib)\n'
        'target_include_directories(${COMPONENT_TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/fetched_dep")\n'
    )

    replace_in_file(
        'main/CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES fc_comp\n    # placeholder_inside_idf_component_register',
    )

    idf_py('build')
    assert Path('build/build_test_app.elf').exists()


@pytest.mark.usefixtures('test_app_copy')
def test_idf_component_include_returns_interface(idf_py: IdfPyFunc) -> None:
    """idf_component_include() INTERFACE output variable returns the interface target name."""
    logging.info('Testing idf_component_include INTERFACE output variable')

    comp = Path('components/iface_test_comp')
    comp.mkdir(parents=True, exist_ok=True)
    (comp / 'CMakeLists.txt').write_text('idf_component_register(SRCS "iface.c")\n')
    (comp / 'iface.c').write_text('void iface_func(void) {}')

    replace_in_file(
        'main/CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES iface_test_comp\n    # placeholder_inside_idf_component_register',
    )

    replace_in_file(
        'CMakeLists.txt',
        '# placeholder_after_project_default',
        'idf_component_include(iface_test_comp INTERFACE iface_var)\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/iface_output.txt" "${iface_var}")\n',
    )

    idf_py('reconfigure')

    iface_val = Path('build/iface_output.txt').read_text().strip()
    assert len(iface_val) > 0, 'idf_component_include INTERFACE should return a non-empty target name'


@pytest.mark.usefixtures('test_app_copy')
def test_component_format_cmakev1_vs_cmakev2(idf_py: IdfPyFunc) -> None:
    """COMPONENT_FORMAT should be CMAKEV1 for idf_component_register, CMAKEV2 for native."""
    logging.info('Testing COMPONENT_FORMAT detection')

    v1_comp = Path('components/format_v1')
    v1_comp.mkdir(parents=True, exist_ok=True)
    (v1_comp / 'CMakeLists.txt').write_text('idf_component_register(SRCS "v1.c")\n')
    (v1_comp / 'v1.c').write_text('void v1_func(void) {}')

    v2_comp = Path('components/format_v2')
    v2_comp.mkdir(parents=True, exist_ok=True)
    (v2_comp / 'CMakeLists.txt').write_text(
        'add_library(${COMPONENT_TARGET} STATIC "v2.c")\n'
        'target_include_directories(${COMPONENT_TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")\n'
    )
    (v2_comp / 'v2.c').write_text('void v2_func(void) {}')

    replace_in_file(
        'main/CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES format_v1 format_v2\n    # placeholder_inside_idf_component_register',
    )

    replace_in_file(
        'CMakeLists.txt',
        '# placeholder_after_project_default',
        'idf_component_get_property(v1_fmt format_v1 COMPONENT_FORMAT)\n'
        'idf_component_get_property(v2_fmt format_v2 COMPONENT_FORMAT)\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/format_check.txt" "${v1_fmt}:${v2_fmt}")\n',
    )

    idf_py('build')
    result = Path('build/format_check.txt').read_text().strip()
    assert result == 'CMAKEV1:CMAKEV2', f'Expected CMAKEV1:CMAKEV2, got {result!r}'


@pytest.mark.usefixtures('test_app_copy')
def test_idf_component_optional_requires(idf_py: IdfPyFunc) -> None:
    """idf_component_optional_requires silently skips unknown components."""
    logging.info('Testing idf_component_optional_requires with unknown component')

    comp = Path('components/opt_req_comp')
    comp.mkdir(parents=True, exist_ok=True)
    (comp / 'CMakeLists.txt').write_text(
        'idf_component_register(SRCS "opt.c")\nidf_component_optional_requires(PRIVATE nonexistent_comp_xyz)\n'
    )
    (comp / 'opt.c').write_text('void opt_func(void) {}')

    replace_in_file(
        'main/CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES opt_req_comp\n    # placeholder_inside_idf_component_register',
    )

    idf_py('build')
    assert Path('build/build_test_app.elf').exists(), 'Build should succeed even with nonexistent optional dependency'


@pytest.mark.usefixtures('test_app_copy')
def test_idf_component_set_get_property_apis(idf_py: IdfPyFunc) -> None:
    """Test idf_component_set_property and idf_component_get_property APIs work correctly."""
    logging.info('Testing idf_component_set_property and idf_component_get_property APIs')

    replace_in_file(
        'CMakeLists.txt',
        '# placeholder_after_project_default',
        'idf_component_set_property(main CUSTOM_PROP "custom_value")\n'
        'idf_component_set_property(main TEST_LIST "item1")\n'
        'idf_component_set_property(main TEST_LIST "item2" APPEND)\n'
        'idf_component_get_property(custom main CUSTOM_PROP)\n'
        'idf_component_get_property(list_prop main TEST_LIST)\n'
        'idf_component_get_property(name main COMPONENT_NAME)\n'
        'idf_component_get_property(dir main COMPONENT_DIR)\n'
        'idf_component_get_property(lib main COMPONENT_LIB)\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/properties_test.txt"\n'
        '  "CUSTOM=${custom}\\n"\n'
        '  "LIST=${list_prop}\\n"\n'
        '  "NAME=${name}\\n"\n'
        '  "DIR=${dir}\\n"\n'
        '  "LIB=${lib}\\n"\n'
        ')\n',
    )

    idf_py('reconfigure')
    result = Path('build/properties_test.txt').read_text().strip()

    # Verify set/get works for custom property
    assert 'CUSTOM=custom_value' in result, (
        'idf_component_set_property and idf_component_get_property should work for custom properties'
    )

    # Verify append works
    assert 'LIST=item1;item2' in result or ('item1' in result and 'item2' in result), (
        'idf_component_set_property APPEND should work correctly'
    )

    # Verify standard properties are retrievable
    assert 'NAME=main' in result, 'idf_component_get_property should retrieve COMPONENT_NAME'
    assert 'DIR=' in result and len(result.split('DIR=')[1].split('\n')[0]) > 0, (
        'idf_component_get_property should retrieve COMPONENT_DIR'
    )
    assert 'LIB=' in result and len(result.split('LIB=')[1].split('\n')[0]) > 0, (
        'idf_component_get_property should retrieve COMPONENT_LIB'
    )


@pytest.mark.usefixtures('test_app_copy')
def test_local_component_shadows_managed_dep_in_manifest(idf_py: IdfPyFunc) -> None:
    """A component's manifest declares `<ns>/<name>` while the project has a
    local `components/<name>` that shadows it. The dep must resolve to the
    local short-named component rather than failing with
    `Failed to resolve component '<ns>__<name>'`.

    Regression test for the cmakev2 per-component injection path that lost
    the project-wide context _choose_component needs to rewrite a manifest-
    declared namespaced dep to its locally-shadowing short name. The fix
    seeds the per-component requirements file with one entry per discovered
    component so known_components matches what cmakev1's project-wide
    injection produces.
    """
    logging.info('Testing local component shadows manifest-declared managed dep')

    # Local shadow named `lvgl`.
    (Path('components/lvgl')).mkdir(parents=True)
    (Path('components/lvgl/CMakeLists.txt')).write_text('idf_component_register(SRCS "lvgl_stub.c" INCLUDE_DIRS ".")\n')
    (Path('components/lvgl/lvgl_stub.c')).write_text('void lvgl_local_stub(void) {}\n')

    # A second local component whose manifest declares the namespaced dep.
    # override_path keeps the component manager offline by pointing the dep
    # at the local lvgl directory.
    (Path('components/consumer')).mkdir(parents=True)
    (Path('components/consumer/CMakeLists.txt')).write_text(
        'idf_component_register(SRCS "consumer.c" INCLUDE_DIRS ".")\n'
    )
    (Path('components/consumer/consumer.c')).write_text('void consumer_stub(void) {}\n')
    (Path('components/consumer/idf_component.yml')).write_text(
        'dependencies:\n  idf: ">=5.0"\n  lvgl/lvgl:\n    version: "*"\n    override_path: "../lvgl"\n'
    )

    # Force consumer into the build via main (cmakev2 only builds required components).
    replace_in_file(
        'main/CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'PRIV_REQUIRES consumer',
    )

    # Without the fix this fails with "Failed to resolve component 'lvgl__lvgl'".
    idf_py('reconfigure')

    with open('build/project_description.json') as f:
        data = json.load(f)
    paths = data.get('build_component_paths', [])
    assert any(p.endswith('/components/lvgl') for p in paths), f'local lvgl not in build_component_paths: {paths}'
    assert not any('lvgl__lvgl' in p for p in paths), f'managed lvgl__lvgl should not be in build: {paths}'


@pytest.mark.usefixtures('test_app_copy')
def test_component_scope_isolation(idf_py: IdfPyFunc) -> None:
    """A component does not see the variables and directory properties of the
    component that requires it.

    leak_src sets 'srcs', adds a compile definition and requires leak_dst.
    leak_dst appends to 'srcs' without initializing it, like many cmakev1
    components. Without the isolation leak_dst appends to the 'srcs' of
    leak_src and the configuration fails.
    """
    logging.info('Testing scope isolation of a recursively evaluated component')

    src_dir = Path('components/leak_src')
    src_dir.mkdir(parents=True)
    (src_dir / 'CMakeLists.txt').write_text(
        'set(srcs "leak_src.c")\n'
        'set(leak_marker "set by leak_src")\n'
        'add_compile_definitions(LEAK_SRC_DIRECTORY)\n'
        'idf_component_register(SRCS ${srcs} REQUIRES leak_dst)\n'
    )
    (src_dir / 'leak_src.c').write_text('void leak_src_func(void) {}\n')

    dst_dir = Path('components/leak_dst')
    dst_dir.mkdir(parents=True)
    (dst_dir / 'CMakeLists.txt').write_text(
        'list(APPEND srcs "leak_dst.c")\n'
        'idf_component_register(SRCS ${srcs})\n'
        # leak_marker is set by leak_src. ARG_REQUIRES and sources are local
        # variables of the idf_component_register() call of leak_src.
        'foreach(variable leak_marker ARG_REQUIRES sources)\n'
        '    if(DEFINED ${variable})\n'
        '        message(FATAL_ERROR "leak_dst sees ${variable}=${${variable}}")\n'
        '    endif()\n'
        'endforeach()\n'
    )
    (dst_dir / 'leak_dst.c').write_text('void leak_dst_func(void) {}\n')

    replace_in_file(
        'main/CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES leak_src',
    )

    idf_py('reconfigure')

    with open('build/compile_commands.json') as f:
        commands = json.load(f)
    defined = {Path(c['file']).name: '-DLEAK_SRC_DIRECTORY' in c['command'] for c in commands}
    assert defined['leak_src.c'], 'definition missing on the component target'
    assert not defined['leak_dst.c'], 'definition leaked into the required component'

    # With the isolation disabled leak_dst sees the variables of leak_src.
    ret = idf_py('-DIDF_COMPONENT_SCOPE_ISOLATION=NO', 'reconfigure', check=False)
    assert ret.returncode != 0
    assert 'leak_dst sees leak_marker=set by leak_src' in ret.stderr
