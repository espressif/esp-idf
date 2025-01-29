idf_build_get_property(target IDF_TARGET)
idf_build_get_property(arch IDF_TARGET_ARCH)
idf_build_get_property(idf_path IDF_PATH)

idf_component_get_property(efuse_dir efuse COMPONENT_DIR)
idf_component_get_property(esp_hw_support_dir esp_hw_support COMPONENT_DIR)
idf_component_get_property(hal_dir hal COMPONENT_DIR)
idf_component_get_property(heap_dir heap COMPONENT_DIR)
idf_component_get_property(mbedtls_dir mbedtls COMPONENT_DIR)

set(srcs)
set(include)

# Common core implementation for TEE
set(srcs "core/esp_tee_init.c"
         "core/esp_tee_intr.c"
         "core/esp_secure_services.c"
         "core/esp_secure_dispatcher.c"
         "core/esp_secure_service_table.c")

# Arch specific implementation for TEE
list(APPEND srcs "arch/${arch}/esp_tee_vectors.S"
                 "arch/${arch}/esp_tee_vector_table.S"
                 "arch/${arch}/esp_tee_secure_entry.S")

# SoC specific implementation for TEE
list(APPEND srcs "soc/${target}/esp_tee_secure_sys_cfg.c"
                 "soc/${target}/esp_tee_pmp_pma_prot_cfg.c"
                 "soc/${target}/esp_tee_apm_prot_cfg.c"
                 "soc/${target}/esp_tee_apm_intr.c"
                 "soc/${target}/esp_tee_aes_intr.c")

# Common module implementation for TEE

# Panic handler implementation for TEE
list(APPEND srcs "common/panic/esp_tee_panic.c"
                 "common/panic/panic_helper_riscv.c")

# Brownout detector
list(APPEND srcs "common/brownout.c")

list(APPEND include "include"
                    "common"
                    "soc/${target}/include")

# Heap
list(APPEND srcs "common/multi_heap.c")

# Sources and headers shared with IDF
list(APPEND include "${efuse_dir}/private_include"
                    "${efuse_dir}/${target}/private_include")

list(APPEND srcs "${hal_dir}/apm_hal.c"
                 "${hal_dir}/brownout_hal.c"
                 "${hal_dir}/wdt_hal_iram.c")

if(CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1)
    list(APPEND srcs "${hal_dir}/spi_flash_hal.c")
endif()

# TLSF implementation for heap
list(APPEND include "${heap_dir}/include"
                    "${heap_dir}/tlsf"
                    "${heap_dir}/tlsf/include")
list(APPEND srcs "${heap_dir}/tlsf/tlsf.c")

# Crypto
# AES
list(APPEND include "${mbedtls_dir}/port/include"
                    "${mbedtls_dir}/port/aes/include"
                    "${mbedtls_dir}/port/aes/dma/include")
# SHA
list(APPEND include "${mbedtls_dir}/port/sha/core/include")

# esp_app_desc_t configuration structure for TEE
list(APPEND srcs "common/esp_app_desc_tee.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include})

set_source_files_properties("core/esp_secure_services.c" PROPERTIES COMPILE_FLAGS -Wno-deprecated)
include(${CMAKE_CURRENT_LIST_DIR}/ld/esp_tee_ld.cmake)

# esp_app_desc_t configuration structure for TEE: Linking symbol and trimming project version and name
target_link_libraries(${COMPONENT_LIB} PRIVATE "-u esp_app_desc_tee_include_impl")

# cut PROJECT_VER and PROJECT_NAME to required 32 characters.
idf_build_get_property(project_ver PROJECT_VER)
idf_build_get_property(project_name PROJECT_NAME)
string(SUBSTRING "${project_ver}" 0 31 PROJECT_VER_CUT)
string(SUBSTRING "${project_name}" 0 31 PROJECT_NAME_CUT)
message(STATUS "App \"${PROJECT_NAME_CUT}\" version: ${PROJECT_VER_CUT}")

set_source_files_properties(
    SOURCE "common/esp_app_desc_tee.c"
    PROPERTIES COMPILE_DEFINITIONS
    "PROJECT_VER=\"${PROJECT_VER_CUT}\"; PROJECT_NAME=\"${PROJECT_NAME_CUT}\"")
