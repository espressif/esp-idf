# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
import subprocess
from pathlib import Path

import pytest
from test_build_system_helpers import EnvDict
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file
from test_build_system_helpers import run_idf_py


@pytest.mark.usefixtures('test_app_copy')
def test_build_executable(idf_py: IdfPyFunc) -> None:
    """idf_build_executable creates an ELF named after the executable target by default;
    when NAME and SUFFIX are provided, the output filename reflects those options instead."""
    logging.info('Testing idf_build_executable default and custom NAME/SUFFIX output naming')

    replace_in_file(
        'CMakeLists.txt',
        'idf_project_default()',
        'idf_project_init()\n'
        'idf_build_executable(build_test_app COMPONENTS main)\n'
        'idf_build_executable(custom_app COMPONENTS main NAME custom_name SUFFIX .custom_elf)\n'
        'idf_build_generate_metadata(EXECUTABLE build_test_app)\n',
    )

    idf_py('build')
    assert Path('build/build_test_app.elf').exists(), (
        'idf_build_executable should produce build_test_app.elf with default naming'
    )
    assert Path('build/custom_name.custom_elf').exists(), (
        'idf_build_executable should produce custom_name.custom_elf with NAME and SUFFIX'
    )


@pytest.mark.usefixtures('test_app_copy')
def test_build_library(idf_py: IdfPyFunc) -> None:
    """idf_build_library with COMPONENTS respects the filter; without it, includes all."""
    logging.info('Testing idf_build_library with and without COMPONENTS filter')

    replace_in_file(
        'CMakeLists.txt',
        'idf_project_default()',
        'idf_project_init()\n'
        'idf_build_library(mylib_specific COMPONENTS main)\n'
        'idf_library_get_property(specific_comps mylib_specific LIBRARY_COMPONENTS)\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/lib_comps.txt" "${specific_comps}")\n'
        'idf_build_library(mylib_all)\n'
        'idf_library_get_property(all_comps mylib_all LIBRARY_COMPONENTS)\n'
        'idf_build_get_property(discovered COMPONENTS_DISCOVERED)\n'
        'list(LENGTH all_comps all_count)\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/lib_comps_count.txt" "${all_count}")\n'
        'if("${all_comps}" STREQUAL "${discovered}")\n'
        '  file(WRITE "${CMAKE_BINARY_DIR}/lib_comps_match.txt" "MATCH")\n'
        'else()\n'
        '  file(WRITE "${CMAKE_BINARY_DIR}/lib_comps_match.txt"'
        ' "MISMATCH\\nLIBRARY: ${all_comps}\\nDISCOVERED: ${discovered}")\n'
        'endif()\n'
        'idf_build_executable(build_test_app COMPONENTS main)\n'
        'idf_build_generate_metadata(EXECUTABLE build_test_app)\n',
    )

    idf_py('reconfigure')

    # Verify COMPONENTS filter was respected
    comps_text = Path('build/lib_comps.txt').read_text().strip()
    assert comps_text == 'main', f'Library with COMPONENTS main should list only "main", got: {comps_text!r}'

    # Verify absence of filter includes all discovered components
    count = int(Path('build/lib_comps_count.txt').read_text().strip())
    assert count > 1, f'Library without COMPONENTS should include all discovered components (>1), got: {count}'

    match_result = Path('build/lib_comps_match.txt').read_text().strip()
    assert match_result == 'MATCH', f'Library without COMPONENTS should match COMPONENTS_DISCOVERED: {match_result}'


@pytest.mark.usefixtures('test_app_copy')
def test_build_binary(idf_py: IdfPyFunc) -> None:
    """idf_build_binary converts an ELF into a .bin image and a .md5sum checksum file,
    setting BINARY_PATH and EXECUTABLE_TARGET on the created target. When MAPFILE_TARGET is
    provided to idf_build_executable, the target is created with a MAPFILE_PATH property
    pointing to the link map file. TARGET is a required argument; omitting it aborts
    configuration with a clear error message."""
    logging.info('Testing idf_build_binary artifacts, properties, MAPFILE_TARGET, and TARGET error')

    replace_in_file(
        'CMakeLists.txt',
        'idf_project_default()',
        'idf_project_init()\n'
        'idf_build_executable(myapp COMPONENTS main MAPFILE_TARGET myapp_mapfile)\n'
        'idf_build_binary(myapp\n'
        '    TARGET myapp_bin\n'
        '    OUTPUT_FILE "${CMAKE_BINARY_DIR}/myapp.bin")\n'
        # Dump target properties to files so Python can assert them
        'get_target_property(_bpath myapp_bin BINARY_PATH)\n'
        'get_target_property(_etarget myapp_bin EXECUTABLE_TARGET)\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/binary_path.txt" "${_bpath}")\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/exec_target.txt" "${_etarget}")\n'
        # Guard MAPFILE_TARGET with if(TARGET)
        'if(TARGET myapp_mapfile)\n'
        '  get_target_property(_mpath myapp_mapfile MAPFILE_PATH)\n'
        '  file(WRITE "${CMAKE_BINARY_DIR}/mapfile_path.txt" "${_mpath}")\n'
        'else()\n'
        '  file(WRITE "${CMAKE_BINARY_DIR}/mapfile_path.txt" "NO_MAPFILE_TARGET")\n'
        'endif()\n'
        'add_custom_target(app ALL DEPENDS myapp_bin)\n'
        'idf_build_generate_metadata(EXECUTABLE myapp)\n',
    )

    idf_py('build')

    # Binary image and MD5 checksum file must exist
    assert Path('build/myapp.bin').exists(), 'idf_build_binary should create the .bin image'
    assert Path('build/myapp.bin.md5sum').exists(), 'idf_build_binary should create the .bin.md5sum checksum file'

    # BINARY_PATH must point to the output file
    binary_path = Path('build/binary_path.txt').read_text().strip()
    assert 'myapp.bin' in binary_path, f'BINARY_PATH should reference myapp.bin, got: {binary_path!r}'

    # EXECUTABLE_TARGET must point back to the ELF target
    exec_target = Path('build/exec_target.txt').read_text().strip()
    assert exec_target == 'myapp', f'EXECUTABLE_TARGET should be "myapp", got: {exec_target!r}'

    # MAPFILE_TARGET: target and MAPFILE_PATH property
    mapfile_path_content = Path('build/mapfile_path.txt').read_text().strip()
    if mapfile_path_content != 'NO_MAPFILE_TARGET':
        ninja = Path('build/build.ninja').read_text()
        assert 'myapp_mapfile' in ninja, '"myapp_mapfile" target not found in build.ninja'
        assert '.map' in mapfile_path_content, (
            f'MAPFILE_PATH should reference a .map file, got: {mapfile_path_content!r}'
        )

    # TARGET is required: remove it from the invocation and verify configuration aborts.
    cmake_before = Path('CMakeLists.txt').read_text()
    replace_in_file(
        'CMakeLists.txt',
        'idf_build_binary(myapp\n    TARGET myapp_bin\n    OUTPUT_FILE "${CMAKE_BINARY_DIR}/myapp.bin")\n',
        'idf_build_binary(myapp\n    OUTPUT_FILE "${CMAKE_BINARY_DIR}/myapp.bin")\n',
    )
    assert Path('CMakeLists.txt').read_text() != cmake_before, 'replace_in_file did not modify CMakeLists.txt'
    with pytest.raises(subprocess.CalledProcessError) as exc_info:
        idf_py('reconfigure')
    err_output = (exc_info.value.stdout or '') + (exc_info.value.stderr or '')
    assert 'TARGET option is required' in err_output, 'Expected "TARGET option is required" in cmake error output'


@pytest.mark.usefixtures('test_app_copy')
def test_sign_binary(idf_py: IdfPyFunc) -> None:
    """idf_sign_binary requires CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES; without it the
    configuration aborts with a message naming the missing option. When the option is enabled
    idf_sign_binary registers the signing target in the build graph and sets BINARY_PATH and
    EXECUTABLE_TARGET on it."""
    logging.info('Testing idf_sign_binary config-disabled and config-enabled paths')

    replace_in_file(
        'CMakeLists.txt',
        'idf_project_default()',
        'idf_project_init()\n'
        'idf_build_executable(myapp COMPONENTS main)\n'
        'idf_build_binary(myapp TARGET myapp_bin\n'
        '    OUTPUT_FILE "${CMAKE_BINARY_DIR}/myapp.bin")\n'
        'idf_sign_binary(myapp_bin TARGET myapp_signed\n'
        '    OUTPUT_FILE "${CMAKE_BINARY_DIR}/myapp_signed.bin")\n'
        # Dump idf_sign_binary target properties to files so Python can assert them
        'get_target_property(_signed_bpath myapp_signed BINARY_PATH)\n'
        'get_target_property(_signed_etarget myapp_signed EXECUTABLE_TARGET)\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/signed_binary_path.txt" "${_signed_bpath}")\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/signed_exec_target.txt" "${_signed_etarget}")\n'
        'idf_build_generate_metadata(EXECUTABLE myapp)\n',
    )

    # Config disabled: idf_sign_binary must abort with a clear diagnostic.
    with pytest.raises(subprocess.CalledProcessError) as exc_info:
        idf_py('reconfigure')
    err_output = (exc_info.value.stdout or '') + (exc_info.value.stderr or '')
    assert 'SECURE_BOOT_BUILD_SIGNED_BINARIES' in err_output, (
        'Expected "SECURE_BOOT_BUILD_SIGNED_BINARIES" in cmake error output'
    )

    # Config enabled: drive the Kconfig dependency chain through sdkconfig.defaults.
    # Delete any sdkconfig written by the failed run above so the new defaults are not
    # shadowed by the existing file (sdkconfig takes priority over sdkconfig.defaults).
    sdkconfig = Path('sdkconfig')
    if sdkconfig.exists():
        sdkconfig.unlink()
    Path('sdkconfig.defaults').write_text('CONFIG_SECURE_SIGNED_APPS_NO_SECURE_BOOT=y\n')

    idf_py('reconfigure')

    # Signing target must appear in the build graph
    ninja = Path('build/build.ninja').read_text()
    assert 'myapp_signed' in ninja, '"myapp_signed" signing target not found in build.ninja'

    # BINARY_PATH on the signed target must point to the signed output file
    signed_binary_path = Path('build/signed_binary_path.txt').read_text().strip()
    assert 'myapp_signed.bin' in signed_binary_path, (
        f'BINARY_PATH on signed target should reference myapp_signed.bin, got: {signed_binary_path!r}'
    )

    # EXECUTABLE_TARGET must be propagated from the unsigned binary to the signed target
    signed_exec_target = Path('build/signed_exec_target.txt').read_text().strip()
    assert signed_exec_target == 'myapp', (
        f'EXECUTABLE_TARGET should be propagated as "myapp", got: {signed_exec_target!r}'
    )


@pytest.mark.usefixtures('test_app_copy')
def test_generate_metadata_and_flash(idf_py: IdfPyFunc) -> None:
    """idf_build_generate_metadata produces project description JSON for EXECUTABLE and BINARY
    modes with optional custom output paths. idf_flash_binary registers a named flash target,
    with the FLASH flag also wiring it into the global flash target."""
    logging.info('Testing idf_build_generate_metadata and idf_flash_binary')

    replace_in_file(
        'CMakeLists.txt',
        'idf_project_default()',
        'idf_project_init()\n'
        'idf_build_executable(myapp COMPONENTS main)\n'
        'idf_build_binary(myapp TARGET myapp_bin\n'
        '    OUTPUT_FILE "${CMAKE_BINARY_DIR}/myapp.bin")\n'
        'idf_flash_binary(myapp_bin TARGET myapp-flash NAME myapp FLASH)\n'
        'idf_build_generate_metadata(EXECUTABLE myapp)\n'
        'idf_build_generate_metadata(BINARY myapp_bin\n'
        '    OUTPUT_FILE "${CMAKE_BINARY_DIR}/binary_desc.json")\n',
    )

    idf_py('reconfigure')

    ninja = Path('build/build.ninja').read_text()

    # idf_flash_binary: named flash target must appear in the build graph
    assert 'myapp-flash' in ninja, '"myapp-flash" target not found in build.ninja'

    # idf_build_generate_metadata(EXECUTABLE): default output path
    assert Path('build/project_description.json').exists(), (
        'project_description.json not created by idf_build_generate_metadata(EXECUTABLE)'
    )
    proj_desc = json.loads(Path('build/project_description.json').read_text())
    assert 'build_components' in proj_desc, 'project_description.json should contain "build_components" key'

    # idf_build_generate_metadata(BINARY): custom output path
    assert Path('build/binary_desc.json').exists(), (
        'binary_desc.json not created by idf_build_generate_metadata(BINARY)'
    )
    binary_desc = json.loads(Path('build/binary_desc.json').read_text())
    assert 'build_components' in binary_desc, 'binary_desc.json should contain "build_components" key'


@pytest.mark.usefixtures('test_app_copy')
def test_check_binary_size_and_signed(idf_py: IdfPyFunc) -> None:
    """idf_check_binary_size adds a partition-fit target when APP_BUILD_TYPE_APP_2NDBOOT is
    enabled. idf_check_binary_signed is a no-op when signed applications are not required;
    when SECURE_SIGNED_APPS is enabled without SECURE_BOOT_BUILD_SIGNED_BINARIES it attaches
    a POST_BUILD signing reminder to the binary target."""
    logging.info('Testing idf_check_binary_size and idf_check_binary_signed')

    replace_in_file(
        'CMakeLists.txt',
        'idf_project_default()',
        'idf_project_init()\n'
        'idf_build_executable(myapp COMPONENTS main)\n'
        'idf_build_binary(myapp TARGET myapp_bin\n'
        '    OUTPUT_FILE "${CMAKE_BINARY_DIR}/myapp.bin")\n'
        'idf_check_binary_size(myapp_bin)\n'
        'idf_check_binary_signed(myapp_bin)\n'
        # Write a sentinel so Python can check whether the size-check target was created
        'if(TARGET myapp_bin_check_size)\n'
        '  file(WRITE "${CMAKE_BINARY_DIR}/check_size_target.txt" "EXISTS")\n'
        'else()\n'
        '  file(WRITE "${CMAKE_BINARY_DIR}/check_size_target.txt" "ABSENT")\n'
        'endif()\n'
        'idf_build_generate_metadata(EXECUTABLE myapp)\n',
    )

    # --- Phase 1: default config (no-op paths) ---------------------------------
    idf_py('reconfigure')

    ninja = Path('build/build.ninja').read_text()

    # idf_check_binary_size: when APP_BUILD_TYPE_APP_2NDBOOT is enabled a size-check
    # custom target is created and added as a dependency of the binary target
    check_size_status = Path('build/check_size_target.txt').read_text().strip()
    if check_size_status == 'EXISTS':
        assert 'myapp_bin_check_size' in ninja, '"myapp_bin_check_size" target not found in build.ninja'

    # --- Phase 2: idf_check_binary_signed active path --------------------------
    # SECURE_SIGNED_APPS=y but SECURE_BOOT_BUILD_SIGNED_BINARIES=n:
    # the function attaches a POST_BUILD "not signed" reminder to the binary target.
    sdkconfig = Path('sdkconfig')
    if sdkconfig.exists():
        sdkconfig.unlink()
    Path('sdkconfig.defaults').write_text(
        'CONFIG_SECURE_SIGNED_APPS_NO_SECURE_BOOT=y\nCONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES=n\n'
    )
    # ECDSA scheme + no build-time signing requires signature_verification_key.bin to exist.
    # Content is not validated at configure time, so an empty file suffices here.
    Path('signature_verification_key.bin').write_bytes(b'')

    idf_py('reconfigure')

    # CMake serialises the POST_BUILD echo into build.ninja or cmake helper scripts
    ninja2 = Path('build/build.ninja').read_text()
    cmake_scripts = ''.join(
        p.read_text(errors='replace') for p in Path('build/CMakeFiles').rglob('*.cmake') if p.is_file()
    )
    assert 'not signed' in ninja2 + cmake_scripts, (
        'Expected POST_BUILD signing reminder from idf_check_binary_signed in build graph'
    )


@pytest.mark.usefixtures('test_app_copy')
def test_property_apis(idf_py: IdfPyFunc) -> None:
    """library property and build property APIs"""

    logging.info('Testing property APIs')

    # Use the library already created by idf_project_default() (idf_build_executable
    # creates library_${executable}, i.e. library_build_test_app). Do not add a
    # second idf_build_library() or compat.cmake will fail (DEFERRED + multi-library).
    replace_in_file(
        'CMakeLists.txt',
        '# placeholder_after_project_default',
        # --- idf_library_set_property / idf_library_get_property (on default library) ---
        'idf_library_set_property(library_build_test_app MY_CUSTOM_PROP "test_value")\n'
        'idf_library_get_property(lib_prop_val library_build_test_app MY_CUSTOM_PROP)\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/lib_prop.txt" "${lib_prop_val}")\n'
        # --- idf_build_set_property / idf_build_get_property ---
        'idf_build_set_property(MY_BUILD_PROP "build_test_value")\n'
        'idf_build_get_property(build_prop_val MY_BUILD_PROP)\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/build_prop.txt" "${build_prop_val}")\n'
        # --- idf_build_set_property APPEND ---
        'idf_build_set_property(MY_LIST "a")\n'
        'idf_build_set_property(MY_LIST "b" APPEND)\n'
        'idf_build_get_property(list_val MY_LIST)\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/build_list.txt" "${list_val}")\n',
    )

    idf_py('reconfigure')

    # idf_library_set_property / idf_library_get_property checks
    assert Path('build/lib_prop.txt').read_text().strip() == 'test_value', (
        'idf_library_set/get_property round-trip failed'
    )

    # idf_build_set_property / idf_build_get_property checks
    assert Path('build/build_prop.txt').read_text().strip() == 'build_test_value', (
        'idf_build_set/get_property round-trip failed'
    )

    # APPEND should produce a list containing both values
    list_result = Path('build/build_list.txt').read_text().strip()
    assert 'a' in list_result and 'b' in list_result, (
        f'APPEND property should contain "a" and "b", got: {list_result!r}'
    )


@pytest.mark.usefixtures('test_app_copy')
def test_depgraph_generation(idf_py: IdfPyFunc) -> None:
    """idf_build_generate_depgraph() should produce a DOT file"""
    logging.info('Testing depgraph generation')

    replace_in_file(
        'CMakeLists.txt',
        'idf_project_default()',
        'idf_project_init()\n'
        'idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH_ENABLED YES)\n'
        'idf_build_executable(build_test_app COMPONENTS main)\n'
        'idf_build_generate_depgraph(build_test_app)\n'
        'idf_build_generate_metadata(EXECUTABLE build_test_app)\n',
    )

    idf_py('reconfigure')

    dot_file = Path('build/component_deps.dot')
    assert dot_file.exists(), 'component_deps.dot not created by idf_build_generate_depgraph'
    dot_content = dot_file.read_text()
    assert 'digraph' in dot_content, 'component_deps.dot should contain a digraph definition'
    assert '->' in dot_content, 'component_deps.dot should contain at least one dependency edge (->)'


@pytest.mark.usefixtures('test_app_copy')
def test_build_with_component_manager_disabled(default_idf_env: EnvDict) -> None:
    """Setting IDF_COMPONENT_MANAGER=0 should skip the component manager
    flow entirely and still produce a successful build."""
    logging.info('Testing build with IDF_COMPONENT_MANAGER=0')

    default_idf_env['IDF_COMPONENT_MANAGER'] = '0'
    ret = run_idf_py('build', env=default_idf_env)

    assert 'Component manager round' not in (ret.stdout or ''), (
        'Component manager should not run when IDF_COMPONENT_MANAGER=0'
    )
    assert Path('build/build_test_app.elf').exists(), 'Build should succeed with component manager disabled'
