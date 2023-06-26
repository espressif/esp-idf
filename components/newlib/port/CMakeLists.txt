target_sources(${COMPONENT_LIB} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/esp_time_impl.c")

if(CONFIG_IDF_TARGET_ARCH_RISCV)
    target_sources(${COMPONENT_LIB} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/riscv/port_stdatomic.S")
endif()
