# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(COMPONENTS main espcoredump esp_gdbstub)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(test_panic)

if(CONFIG_TEST_MEMPROT)
    # TODO: IDF-6821 - Refactor this to make it easy to add any new targets
    if(CONFIG_SOC_MEMPROT_SUPPORTED)
        target_link_libraries(${project_elf} PRIVATE "-Wl,--wrap=esp_panic_handler")
        if(CONFIG_IDF_TARGET_ESP32C3)
            target_link_libraries(${project_elf} PRIVATE "-Wl,--wrap=panic_arch_fill_info")
        endif()
    endif()
endif()

if(NOT CONFIG_TEST_MEMPROT)
    # Enable UBSAN checks
    #
    # shift-base sanitizer is disabled due to the following pattern found in register header files:
    #   #define SOME_FIELD  0xFFFF
    #   #define SOME_FIELD_M  ((SOME_FIELD_V)<<(SOME_FIELD_S))
    #   #define SOME_FIELD_V  0xFFFF
    #   #define SOME_FIELD_S  16
    # here SOME_FIELD_V doesn't have an unsigned (U) prefix, so the compiler flags
    # SOME_FIELD_M expansion (0xFFFF << 16) as generating integer overflow.
    #
    set(ubsan_options "-fsanitize=undefined" "-fno-sanitize=shift-base")

    # Only enable UBSAN for a few components related to the panic test,
    # due to RAM size limitations.
    set(ubsan_components main espcoredump esp_system spi_flash
        esp_common esp_hw_support soc hal freertos)
    if(CONFIG_ESP_COREDUMP_CHECKSUM_SHA256)
        if(CONFIG_IDF_TARGET_ESP32S2)
            # due to the ram limitation, coredump is removed from esp32s2 built
            list(REMOVE_ITEM ubsan_components espcoredump)
        endif()
    endif()
    foreach(component  IN LISTS ubsan_components)
        idf_component_get_property(lib ${component} COMPONENT_LIB)
        target_compile_options(${lib} PRIVATE ${ubsan_options})
    endforeach()
endif()
