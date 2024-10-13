idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs
    "app_trace.c"
    "app_trace_util.c"
    "host_file_io.c")

if(CONFIG_ESP_DEBUG_STUBS_ENABLE)
    list(APPEND srcs
        "debug_stubs.c")
endif()

if(CONFIG_APPTRACE_GCOV_ENABLE)
    if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
        list(APPEND srcs
            "gcov/gcov_rtio.c")
    else()
        fail_at_build_time(app_trace "Only GNU compiler can link with Gcov library")
    endif()
endif()

set(include_dirs "include")

set(priv_include_dirs "private_include" "port/include")

if(CONFIG_APPTRACE_MEMBUFS_APPTRACE_PROTO_ENABLE)
    list(APPEND srcs
        "app_trace_membufs_proto.c")

    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        list(APPEND srcs
            "port/xtensa/port.c")
    endif()
    if(CONFIG_IDF_TARGET_ARCH_RISCV)
        list(APPEND srcs
            "port/riscv/port.c")
    endif()
endif()
list(APPEND srcs
    "port/port_uart.c")

if(CONFIG_APPTRACE_SV_ENABLE)
    list(APPEND include_dirs
        sys_view/Config
        sys_view/SEGGER
        sys_view/Sample/FreeRTOSV10.4)

    list(APPEND srcs
        "sys_view/SEGGER/SEGGER_SYSVIEW.c"
        "sys_view/Sample/FreeRTOSV10.4/Config/esp/SEGGER_SYSVIEW_Config_FreeRTOS.c"
        "sys_view/Sample/FreeRTOSV10.4/SEGGER_SYSVIEW_FreeRTOS.c"
        "sys_view/esp/SEGGER_RTT_esp.c"
        "sys_view/ext/heap_trace_module.c"
        "sys_view/ext/logging.c")
endif()

if(CONFIG_HEAP_TRACING_TOHOST)
    list(APPEND srcs "heap_trace_tohost.c")
    set_source_files_properties(heap_trace_tohost.c
        PROPERTIES COMPILE_FLAGS
        -Wno-frame-address)
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                       PRIV_REQUIRES esp_driver_gptimer esp_driver_gpio esp_driver_uart
                       REQUIRES esp_timer
                       LDFRAGMENTS linker.lf)

# Force app_trace to also appear later than gcov in link line
idf_component_get_property(app_trace app_trace COMPONENT_LIB)

if(CONFIG_APPTRACE_GCOV_ENABLE)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        # Coverage info is not supported when clang is used
        # TODO: LLVM-214
        message(FATAL_ERROR "Coverage info is not supported when building with Clang!")
    endif()

    # The original Gcov library from toolchain will be objcopy with symbols redefinitions (see file gcov/io_sym.map).
    # This needs because ESP has no file-system onboard, and redefined functions solves this problem and transmits
    # output file to host PC.

    # Set a name for Gcov library
    set(GCOV_LIB libgcov_rtio)

    # Set include direcrory of Gcov internal headers
    execute_process(COMMAND ${CMAKE_C_COMPILER} -print-file-name=plugin
                    OUTPUT_VARIABLE gcc_plugin_dir
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET)
    set_source_files_properties(gcov/gcov_rtio.c
                                PROPERTIES COMPILE_FLAGS "-I${gcc_plugin_dir}/include")

    # Copy libgcov.a with symbols redefinition
    find_library(GCOV_LIBRARY_PATH gcov ${CMAKE_C_IMPLICIT_LINK_DIRECTORIES})
    add_custom_command(OUTPUT ${GCOV_LIB}.a
                       COMMAND ${_CMAKE_TOOLCHAIN_PREFIX}objcopy
                               --redefine-syms ${CMAKE_CURRENT_LIST_DIR}/gcov/io_sym.map
                               ${GCOV_LIBRARY_PATH} ${GCOV_LIB}.a
                       MAIN_DEPENDENCY ${GCOV_LIBRARY_PATH}
                       VERBATIM)
    add_custom_target(${GCOV_LIB}_target DEPENDS ${GCOV_LIB}.a)
    add_library(${GCOV_LIB} STATIC IMPORTED)
    set_target_properties(${GCOV_LIB}
                          PROPERTIES
                          IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/${GCOV_LIB}.a)
    add_dependencies(${GCOV_LIB} ${GCOV_LIB}_target)
    add_dependencies(${COMPONENT_LIB} ${GCOV_LIB})

    # disable --coverage for this component, as it is used as transport for gcov
    target_compile_options(${COMPONENT_LIB} PRIVATE "-fno-profile-arcs" "-fno-test-coverage")
    target_link_options(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=__gcov_init")

    target_link_libraries(${COMPONENT_LIB} INTERFACE ${GCOV_LIB} $<TARGET_FILE:${app_trace}> c)
else()
    target_link_libraries(${COMPONENT_LIB} INTERFACE $<TARGET_FILE:${app_trace}> c)
endif()

# This function adds a dependency on the given component if the component is included into the build.
function(maybe_add_component component_name)
    idf_build_get_property(components BUILD_COMPONENTS)
    if(${component_name} IN_LIST components)
        idf_component_get_property(lib_name ${component_name} COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} PUBLIC ${lib_name})
    endif()
endfunction()

if(CONFIG_APPTRACE_DEST_UART0 OR CONFIG_APPTRACE_DEST_UART1 OR CONFIG_APPTRACE_DEST_UART2)
    maybe_add_component(driver)
endif()
