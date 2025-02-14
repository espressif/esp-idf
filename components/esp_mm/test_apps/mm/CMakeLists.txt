# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/components/esp_mm/test_apps/components")

# "Trim" the build. Include the minimal set of components, main, and anything it depends on. We also depend on esp_psram
# as we set CONFIG_SPIRAM_... options.
set(COMPONENTS main esp_psram)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mm_test)

string(JOIN "," ignore_refs)

if(CONFIG_SOC_MMU_PER_EXT_MEM_TARGET AND CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM)
    # On SOC_MMU_PER_EXT_MEM_TARGET chips, when xip_psram, we need
    #   - _instruction_reserved_start, _instruction_reserved_end
    #   - _rodata_reserved_start, _rodata_reserved_end
    # to do some calculation. As we don't access the addresses, so we disable this check
    list(APPEND ignore_refs esp_mmu_map_init/*)
endif()

if(CONFIG_COMPILER_DUMP_RTL_FILES)
    add_custom_target(check_test_app_sections ALL
                      COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
                      --rtl-dirs ${CMAKE_BINARY_DIR}/esp-idf/esp_mm/,${CMAKE_BINARY_DIR}/esp-idf/hal/
                      --elf-file ${CMAKE_BINARY_DIR}/mm_test.elf
                      find-refs
                      --from-sections=.iram0.text
                      --to-sections=.flash.text,.flash.rodata
                      --ignore-refs=${ignore_refs}
                      --exit-code
                      DEPENDS ${elf}
                      )
endif()
