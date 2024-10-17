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

# Everything below this line is optional and can be used to customize the build process

# Create a custom library
set(lib_path "${CMAKE_CURRENT_LIST_DIR}/lib")
add_library(custom_lib STATIC "${lib_path}/lib_src.c")
target_include_directories(custom_lib PUBLIC "${lib_path}/")

# Link the library
target_link_libraries(${ULP_APP_NAME} PRIVATE custom_lib)

# Set custom compile flags
target_compile_options(${ULP_APP_NAME} PRIVATE -msave-restore)
