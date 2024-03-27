idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not necessary on the POSIX/Linux simulator
endif()

idf_component_register(SRCS "cxx_exception_stubs.cpp"
                            "cxx_guards.cpp"
                            "cxx_init.cpp"
                        # Make sure that pthread is in component list
                        PRIV_REQUIRES pthread esp_system)

if(NOT CONFIG_CXX_EXCEPTIONS)
    set(WRAP_FUNCTIONS
        __register_frame_info_bases
        __register_frame_info
        __register_frame
        __register_frame_info_table_bases
        __register_frame_info_table
        __register_frame_table
        __deregister_frame_info_bases
        __deregister_frame_info
        _Unwind_Find_FDE
        _Unwind_GetGR
        _Unwind_GetCFA
        _Unwind_GetIP
        _Unwind_GetIPInfo
        _Unwind_GetRegionStart
        _Unwind_GetDataRelBase
        _Unwind_GetTextRelBase
        _Unwind_SetIP
        _Unwind_SetGR
        _Unwind_GetLanguageSpecificData
        _Unwind_FindEnclosingFunction
        _Unwind_Resume
        _Unwind_RaiseException
        _Unwind_DeleteException
        _Unwind_ForcedUnwind
        _Unwind_Resume_or_Rethrow
        _Unwind_Backtrace
        __cxa_call_unexpected
        __gxx_personality_v0)

    foreach(wrap ${WRAP_FUNCTIONS})
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=${wrap}")
    endforeach()
endif()

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    # libstdc++ depends on C library, so it should appear later in link order.
    # Otherwise we get undefined references for esp-clang toolchain.
    target_link_libraries(${COMPONENT_LIB} PUBLIC stdc++ c ${CONFIG_COMPILER_RT_LIB_NAME})
else()
    target_link_libraries(${COMPONENT_LIB} PUBLIC stdc++ gcc)
endif()
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __cxa_guard_dummy")
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __cxx_init_dummy")

# Force libpthread to appear later than libstdc++ in link line since libstdc++ depends on libpthread.
# Furthermore, force libcxx to appear later than libgcc because some libgcc unwind code is wrapped, if C++
# exceptions are disabled. libcxx (this component) provides the unwind code wrappers.
# This is to prevent linking of libgcc's unwind code which considerably increases the binary size.
# Also force libnewlib to appear later than libstdc++ in link line since libstdc++ depends on
# some functions in libnewlib, e.g. getentropy().
idf_component_get_property(pthread pthread COMPONENT_LIB)
idf_component_get_property(newlib newlib COMPONENT_LIB)
idf_component_get_property(cxx cxx COMPONENT_LIB)
add_library(stdcpp_deps INTERFACE)
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_link_libraries(stdcpp_deps INTERFACE stdc++ c $<TARGET_FILE:${pthread}> $<TARGET_FILE:${newlib}>)
else()
    target_link_libraries(stdcpp_deps INTERFACE stdc++ $<TARGET_FILE:${pthread}> $<TARGET_FILE:${newlib}>)
endif()
target_link_libraries(${COMPONENT_LIB} PUBLIC stdcpp_deps)
add_library(libgcc_cxx INTERFACE)
target_link_libraries(libgcc_cxx INTERFACE ${CONFIG_COMPILER_RT_LIB_NAME} $<TARGET_FILE:${cxx}>)
target_link_libraries(${COMPONENT_LIB} PUBLIC libgcc_cxx)

if(NOT CONFIG_COMPILER_CXX_EXCEPTIONS)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __cxx_fatal_exception")
endif()
