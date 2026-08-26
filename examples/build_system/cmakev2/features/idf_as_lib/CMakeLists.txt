# A standard CMake project that uses ESP-IDF as a library.
# When ESP_PLATFORM is set (idf.py invocation), IDF components are bundled
# into a single library and linked into the executable.  Without it the
# project builds a plain host executable with no IDF dependency.

cmake_minimum_required(VERSION 3.22)

if(ESP_PLATFORM)
    include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
endif()

project(idf_as_lib C CXX ASM)

# Single source file for both builds. IDF code paths inside main.c are
# guarded by #ifdef ESP_PLATFORM.
add_executable(${CMAKE_PROJECT_NAME}.elf main.c)

if(ESP_PLATFORM)
    # Initialise the IDF build system and bundle the required components
    # into a single linkable library.
    idf_project_init()

    # Create ESP-IDF library with specified components.
    idf_build_library(idf_components COMPONENTS spi_flash esp_system)

    # Apply IDF build properties to the custom executable so it receives
    # include paths, compile definitions, and compile options that IDF normally adds.
    idf_build_get_property(include_directories INCLUDE_DIRECTORIES GENERATOR_EXPRESSION)
    target_include_directories(${CMAKE_PROJECT_NAME}.elf PRIVATE "${include_directories}")

    idf_build_get_property(compile_definitions COMPILE_DEFINITIONS GENERATOR_EXPRESSION)
    target_compile_definitions(${CMAKE_PROJECT_NAME}.elf PRIVATE "${compile_definitions}")

    idf_build_get_compile_options(compile_options)
    target_compile_options(${CMAKE_PROJECT_NAME}.elf PRIVATE "${compile_options}")

    # Link the bundled IDF library.  This brings in component headers,
    # sdkconfig includes, and all necessary linker options.
    target_link_libraries(${CMAKE_PROJECT_NAME}.elf PRIVATE idf_components)

    # Binary and flash targets
    idf_build_binary(${CMAKE_PROJECT_NAME}.elf
                     OUTPUT_FILE "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin"
                     TARGET ${CMAKE_PROJECT_NAME}_binary)

    idf_flash_binary(${CMAKE_PROJECT_NAME}_binary
                     TARGET app-flash
                     NAME "app"
                     FLASH)

    idf_check_binary_size(${CMAKE_PROJECT_NAME}_binary)
    idf_build_generate_metadata(BINARY ${CMAKE_PROJECT_NAME}_binary)
    idf_build_generate_flasher_args()
    add_custom_target(app ALL DEPENDS ${CMAKE_PROJECT_NAME}_binary)
endif()
