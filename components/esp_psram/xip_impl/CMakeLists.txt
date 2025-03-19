set(srcs)

if(${target} STREQUAL "esp32s2")
    list(APPEND srcs "mmu_psram_flash.c")
endif()

if(${target} STREQUAL "esp32s3")
    list(APPEND srcs "mmu_psram_flash.c")
endif()

if(CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM)
    list(APPEND srcs "mmu_psram_flash_v2.c")
endif()

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
