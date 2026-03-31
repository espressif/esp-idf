cmake_minimum_required(VERSION 3.22)

# Append ULP-specific defaults after the parent's sdkconfig (passed via
# -DSDKCONFIG_DEFAULTS). Later entries override earlier ones.
list(APPEND SDKCONFIG_DEFAULTS "${CMAKE_CURRENT_LIST_DIR}/sdkconfig.defaults")

include(${IDF_PATH}/tools/cmakev2/idf.cmake)

project(${ULP_APP_NAME} C CXX ASM)

idf_project_init()

include(IDFULPProject)

# Build the ULP executable from the main component and its transitive
# dependencies (ulp_sum, ulp_math, ulp_clamp).
idf_build_executable(${ULP_APP_NAME}
                     COMPONENTS main
                     MAPFILE_TARGET ${ULP_APP_NAME}_mapfile)

# Apply ULP-specific link options and runtime sources
ulp_apply_default_options(${ULP_APP_NAME})
ulp_apply_default_sources(${ULP_APP_NAME})

# Generate the binary, symbol table, linker script and header for the
# main application to embed
ulp_add_build_binary_targets(${ULP_APP_NAME})

idf_build_generate_metadata(EXECUTABLE ${ULP_APP_NAME})
idf_create_menuconfig(${ULP_APP_NAME} TARGET menuconfig)
