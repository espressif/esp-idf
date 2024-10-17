
# This CMakelists.txt is included in the ULP project
# so we have access to the ULP target: ULP_APP_NAME



cmake_minimum_required(VERSION 3.16)

# Project/target name is passed from the main project to allow IDF to have a dependency on this target
# as well as embed the binary into the main app
project(${ULP_APP_NAME})
add_executable(${ULP_APP_NAME} main.c)

# Import the ULP project helper functions
include(IDFULPProject)

# Apply default compile options
ulp_apply_default_options(${ULP_APP_NAME})

# Apply default sources provided by the IDF ULP component
ulp_apply_default_sources(${ULP_APP_NAME})

# Add targets for building the binary, as well as the linkerscript which exports ULP shared variables to the main app
ulp_add_build_binary_targets(${ULP_APP_NAME})


# Set custom compile flags
# By default ULP sources are compiled with -Os which is set in toolchain file in IDF build system.
# These options will appear on command line after default ones effectively overriding them.
# Therefore '-Os' will be overridden with '-O0' for this example for convenient debugging.
target_compile_options(${ULP_APP_NAME} PRIVATE -O0 -fsanitize=undefined -fno-sanitize=shift-base)
