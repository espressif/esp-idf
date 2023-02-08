# Migration from test_build_system_cmake.sh to pytest

This table tracks migration of tests from [test_build_system_cmake.sh](../ci/test_build_system_cmake.sh) and [test_build_system_spaces.py](../ci/test_build_system_spaces.py) to pytest.

When all tests are migrated to pytest, remove the original tests, corresponding CI jobs, and this file.

Legacy test name | New test name | Comments
-----------------|---------------|---------
Initial clean build | test_rebuild::test_rebuild_no_changes |
Updating component source file rebuilds component | test_rebuild::test_rebuild_source_files |
Bootloader source file rebuilds bootloader | test_rebuild::test_rebuild_source_files |
Partition CSV file rebuilds partitions | test_rebuild::test_rebuild_source_files |
Partial build doesn't compile anything by default | test_rebuild::test_rebuild_no_changes |
Rebuild when app version was changed | test_rebuild.py::test_rebuild_version_change |
Change app version | test_rebuild.py::test_rebuild_version_change |
Re-building does not change app.bin | test_rebuild.py::test_rebuild_version_change |
Get the version of app from git describe. Project is not inside IDF and do not have a tag only a hash commit. | |
Get the version of app from Kconfig option | |
Use IDF version variables in component CMakeLists.txt file | |
Project is in ESP-IDF which has a custom tag | |
Moving BUILD_DIR_BASE out of tree | test_build.py::test_build_alternative_directories |
BUILD_DIR_BASE inside default build directory | test_build.py::test_build_alternative_directories |
Can still clean build if all text files are CRLFs | |
Updating rom ld file should re-link app and bootloader | test_rebuild::test_rebuild_linker |
Updating app-only ld file should only re-link app | test_rebuild::test_rebuild_linker |
Updating ld file should only re-link app | test_rebuild::test_rebuild_linker |
Updating fragment file should only re-link app | test_rebuild::test_rebuild_linker |
sdkconfig update triggers full recompile | test_rebuild::test_rebuild_source_files |
Updating project CMakeLists.txt triggers full recompile | test_rebuild::test_rebuild_source_files |
Can build with Ninja (no idf.py) | test_build.py::test_build_cmake_ninja |
Can build with GNU Make (no idf.py) | test_build.py::test_build_cmake_makefile |
idf.py can build with Ninja | test_build.py::test_build_with_generator_ninja |
idf.py can build with Unix Makefiles | test_build.py::test_build_with_generator_makefile |
Can build with IDF_PATH set via cmake cache not environment | test_build.py::test_build_with_cmake_and_idf_path_unset |
Can build with IDF_PATH unset and inferred by build system | test_build.py::test_build_with_cmake_and_idf_path_unset |
Can build with IDF_PATH unset and inferred by cmake when Kconfig needs it to be set | test_build.py::test_build_with_cmake_and_idf_path_unset |
can build with phy_init_data | test_build.py::test_build_skdconfig_phy_init_data |
can build with ethernet component disabled | | moved to test_apps/system/build_test/sdkconfig.ci.ethernet_disabled
Compiler flags on build command line are taken into account | test_build.py::test_build_compiler_flag_in_source_file |
Compiler flags cannot be overwritten | test_build.py::test_build_compiler_flags_no_overwriting |
Can override IDF_TARGET from environment | test_non_default_target.py::test_target_from_environment_cmake |
Can set target using idf.py -D | test_non_default_target.py::test_target_using_D_parameter |
Can set target using -D as subcommand parameter for idf.py | test_non_default_target.py::test_target_using_D_parameter |
Can set target using idf.py set-target | test_non_default_target.py::test_target_using_settarget_parameter |
idf.py understands alternative target names | test_non_default_target.py::test_target_using_settarget_parameter_alternative_name |
Can guess target from sdkconfig, if CMakeCache does not exist | test_non_default_target.py::test_target_using_settarget_parameter |
Can set the default target using sdkconfig.defaults | test_non_default_target.py::test_target_using_sdkconfig |
IDF_TARGET takes precedence over the value of CONFIG_IDF_TARGET in sdkconfig.defaults | test_non_default_target.py::test_target_precedence |
idf.py fails if IDF_TARGET settings don't match in sdkconfig, CMakeCache.txt, and the environment | test_non_default_target.py::test_target_from_environment_idf_py |
Setting EXTRA_COMPONENT_DIRS works | test_components.py::test_component_extra_dirs |
Non-existent paths in EXTRA_COMPONENT_DIRS are not allowed | test_components.py::test_component_nonexistent_extra_dirs_not_allowed |
Component names may contain spaces | test_components.py::test_component_names_contain_spaces |
sdkconfig should have contents of all files: sdkconfig, sdkconfig.defaults, sdkconfig.defaults.IDF_TARGET | |
Test if it can build the example to run on host | |
Test build ESP-IDF as a library to a custom CMake projects for all targets | test_cmake.py::test_build_custom_cmake_project |
Building a project with CMake library imported and PSRAM workaround, all files compile with workaround | test_cmake.py::test_build_cmake_library_psram_workaround |
Test for external libraries in custom CMake projects with ESP-IDF components linked | test_cmake.py::test_build_custom_cmake_project |
Test for external libraries in custom CMake projects with PSRAM strategy $strat | test_cmake.py::test_build_cmake_library_psram_strategies |
Cleaning Python bytecode | test_common.py::test_python_clean |
Displays partition table when executing target partition_table | test_common.py::test_partition_table |
Make sure a full build never runs '/usr/bin/env python' or similar | test_common.py::test_python_interpreter_unix, test_common.py::test_python_interpreter_win |
Handling deprecated Kconfig options | test_kconfig.py::test_kconfig_deprecated_options |
Handling deprecated Kconfig options in sdkconfig.defaults | test_kconfig.py::test_kconfig_deprecated_options |
Can have multiple deprecated Kconfig options map to a single new option | test_kconfig.py::test_kconfig_multiple_and_target_specific_options |
Can have target specific deprecated Kconfig options | test_kconfig.py::test_kconfig_multiple_and_target_specific_options |
Confserver can be invoked by idf.py | |
Check ccache is used to build | |
Custom bootloader overrides original | |
Empty directory not treated as a component | |
If a component directory is added to COMPONENT_DIRS, its subdirectories are not added | |
If a component directory is added to COMPONENT_DIRS, its sibling directories are not added | |
toolchain prefix is set in project description file | |
Can set options to subcommands: print_filter for monitor | |
Fail on build time works | |
Component properties are set | |
should be able to specify multiple sdkconfig default files | |
Supports git worktree | |
idf.py fallback to build system target | |
Build fails if partitions don't fit in flash | |
Warning is given if smallest partition is nearly full | |
Flash size is correctly set in the bootloader image header | |
DFU build works | |
UF2 build works | |
Loadable ELF build works | |
Defaults set properly for unspecified idf_build_process args | |
Getting component overriden dir | |
Overriding Kconfig | |
Project components prioritized over EXTRA_COMPONENT_DIRS | |
Components in EXCLUDE_COMPONENTS not passed to idf_component_manager | |
Create project using idf.py and build it | |
Create component using idf.py, create project using idf.py. | |
Add the component to the created project and build the project. | |
Check that command for creating new project will fail if the target folder is not empty. | |
Check that command for creating new project will fail if the target path is file. | |
Check docs command | |
Deprecation warning check | |
Save-defconfig checks | |
test_build | |
test_build_ulp_fsm | |
test_build_ulp_riscv | |
test_spiffsgen | |
test_flash_encryption | |
test_secure_boot_v1 | |
test_secure_boot_v2 | |
test_app_signing | |
test_secure_boot_release_mode | |
test_x509_cert_bundle | |
test_dfu | |
test_uf2 | |
