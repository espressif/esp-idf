idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "esp32c3")
    # TODO ESP32-C3 IDF-2107 - include the headers to avoid compile errors, no functions available to link...
    idf_component_register(SRCS "pm_impl_riscv_temp.c" INCLUDE_DIRS include)
    return()
endif()


idf_component_register(SRCS "pm_locks.c" "pm_trace.c" "pm_impl.c"
                       INCLUDE_DIRS include
                       LDFRAGMENTS linker.lf)
