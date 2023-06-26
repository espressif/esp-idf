# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# As this G0 example is not runnable as-is, the map file generated won't some important
# sections. Thus, in order to prevent IDF from generating a map file at all, specify
# null device as the destination file.
if(WIN32)
    idf_build_set_property(LINK_OPTIONS "-Wl,--Map=NUL" APPEND)
else()
    idf_build_set_property(LINK_OPTIONS "-Wl,--Map=/dev/null" APPEND)
endif()

# Force this project to use only G0 components
set(all_g0_components esp_rom soc hal esp_common main)  # also <arch>, i.e. xtensa or riscv, will be added below
set(COMPONENTS ${all_g0_components})

# By default, common components include some G1+ components. Override common components to only have G0 ones
idf_build_set_property(__COMPONENT_REQUIRES_COMMON "${all_g0_components}")
# Generate a graph to visually see the dependencies between G0 and G1+ (if any)
idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH_ENABLED 1)

project(g0_components)

if(CONFIG_IDF_TARGET_ESP32C2)
    # clk_tree hal-driver needs CONFIG_XTAL_FREQ
    idf_build_set_property(C_COMPILE_OPTIONS "-DCONFIG_XTAL_FREQ=26" APPEND)
endif()

# Currently, only support a single core on Xtensa targets.
if(CONFIG_IDF_TARGET_ARCH_XTENSA)
    idf_build_set_property(C_COMPILE_OPTIONS "-DportNUM_PROCESSORS=1" APPEND)
    idf_build_set_property(ASM_COMPILE_OPTIONS "-DportNUM_PROCESSORS=1" APPEND)
endif()

# Now that the project has been initialized, let's check which components it is using
# The following variable lists all the components that shall be used by this project
set(expected_components
    ${COMPONENTS}
    ${CONFIG_IDF_TARGET_ARCH}   # xtensa or riscv
    )

# Do not include libc into the build, we don't have any libC in G0
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib")

# Get all the components that were required to initialize this project
idf_build_get_property(build_components BUILD_COMPONENTS)

# Sort lists to be able to compare them literally
list(SORT expected_components)
list(SORT build_components)
if(NOT "${expected_components}" STREQUAL "${build_components}")
    message(FATAL_ERROR "Unexpected components list in G0 build\n"
                        "Expected: ${expected_components}\n"
                        "Actual: ${build_components}")
endif()
