idf_build_get_property(target IDF_TARGET)
idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

if(${target} STREQUAL "linux")
    return() # This component is not necessary on the POSIX/Linux simulator
endif()

set(srcs "cxx_exception_stubs.cpp")
set(priv_requires)

if(NOT esp_tee_build)
    list(APPEND srcs "cxx_guards.cpp" "cxx_init.cpp")
    # Make sure that pthread is in component list
    list(APPEND priv_requires pthread)
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES ${priv_requires})

if(esp_tee_build OR NOT CONFIG_CXX_EXCEPTIONS)
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
        __gxx_personality_v0
        __cxa_throw
        __cxa_allocate_exception)

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

add_library(stdcpp_deps INTERFACE)
target_link_libraries(${COMPONENT_LIB} PUBLIC stdcpp_deps)

if(NOT esp_tee_build)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __cxa_guard_dummy")
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __cxx_init_dummy")

    # Force libpthread to appear later than libstdc++ in link line since libstdc++ depends on libpthread.
    # Furthermore, force libcxx to appear later than libgcc because some libgcc unwind code is wrapped, if C++
    # exceptions are disabled. libcxx (this component) provides the unwind code wrappers.
    # This is to prevent linking of libgcc's unwind code which considerably increases the binary size.
    # Also force esp_libc to appear later than libstdc++ in link line since libstdc++ depends on
    # some functions in esp_libc, e.g. getentropy().
    idf_component_get_property(pthread pthread COMPONENT_LIB)
    idf_component_get_property(esp_libc esp_libc COMPONENT_LIB)

    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        target_link_libraries(stdcpp_deps INTERFACE stdc++ c $<TARGET_FILE:${pthread}> $<TARGET_FILE:${esp_libc}>)
    else()
        target_link_libraries(stdcpp_deps INTERFACE stdc++ $<TARGET_FILE:${pthread}> $<TARGET_FILE:${esp_libc}>)
    endif()
endif()

idf_component_get_property(cxx cxx COMPONENT_LIB)
add_library(libgcc_cxx INTERFACE)
target_link_libraries(libgcc_cxx INTERFACE ${CONFIG_COMPILER_RT_LIB_NAME} $<TARGET_FILE:${cxx}>)
# Link as INTERFACE, not PUBLIC: libgcc_cxx references $<TARGET_FILE:${cxx}>, so adding it to the
# cxx archive's own LINK_LIBRARIES would make the target depend on its own TARGET_FILE. CMake then
# needs the link language (hence SOURCES) to resolve that TARGET_FILE, which is a self-reference and
# fails generation with "The SOURCES of '...' use a generator expression that depends on the SOURCES
# themselves" whenever the cxx target is not itself link-consumed (e.g. cmakev2, IDFGH-16748). A
# static archive isn't linked, so its private link libraries are inert; consumers still get
# libgcc_cxx through INTERFACE_LINK_LIBRARIES, preserving link order.
target_link_libraries(${COMPONENT_LIB} INTERFACE libgcc_cxx)

if(NOT CONFIG_COMPILER_CXX_EXCEPTIONS)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __cxx_fatal_exception")
endif()

# When the lock-free atomic policy is forced, verify the final executable does
# not end up with mixed __gnu_cxx::_Lock_policy instantiations (which can happen
# when linking prebuilt libraries built with a different setting).
if(CONFIG_COMPILER_CXX_ATOMIC_LOCK_POLICY)
    if(IDF_BUILD_V2)
        function(cxx_check_atomic_policy target)
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND}
                    -DELF_FILE=$<TARGET_FILE:${target}>
                    -DCMAKE_OBJDUMP=${CMAKE_OBJDUMP}
                    -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_cxx_atomic_policy.cmake"
                COMMENT "Checking C++ atomic lock policy in linked executable"
                VERBATIM)
        endfunction()
        idf_component_register_build_event_callback(EVENT POST_ELF CALLBACK cxx_check_atomic_policy)
    else()
        idf_build_get_property(build_dir BUILD_DIR)
        idf_build_get_property(elf_target EXECUTABLE GENERATOR_EXPRESSION)
        set(_cxx_atomic_policy_check_marker "${build_dir}/.cxx_atomic_policy_checked")
        add_custom_command(OUTPUT "${_cxx_atomic_policy_check_marker}"
            COMMAND ${CMAKE_COMMAND}
                -DELF_FILE=$<TARGET_FILE:$<GENEX_EVAL:${elf_target}>>
                -DCMAKE_OBJDUMP=${CMAKE_OBJDUMP}
                -P "${CMAKE_CURRENT_LIST_DIR}/check_cxx_atomic_policy.cmake"
            COMMAND ${CMAKE_COMMAND} -E touch "${_cxx_atomic_policy_check_marker}"
            DEPENDS "$<TARGET_FILE:$<GENEX_EVAL:${elf_target}>>"
            COMMENT "Checking C++ atomic lock policy in linked executable"
            VERBATIM)
        add_custom_target(cxx_check_atomic_policy DEPENDS "${_cxx_atomic_policy_check_marker}")
        idf_build_add_post_elf_dependency("${CMAKE_PROJECT_NAME}.elf" cxx_check_atomic_policy)
    endif()
endif()
