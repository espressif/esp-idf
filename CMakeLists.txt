cmake_minimum_required(VERSION 3.5)
project(esp-idf C CXX ASM)

unset(compile_options)
unset(c_compile_options)
unset(cxx_compile_options)
unset(compile_definitions)

# Add the following build specifications here, since these seem to be dependent
# on config values on the root Kconfig.

# Temporary trick to support both gcc5 and gcc8 builds
if(CMAKE_C_COMPILER_VERSION VERSION_EQUAL 5.2.0)
    set(GCC_NOT_5_2_0 0 CACHE STRING "GCC is 5.2.0 version")
else()
    set(GCC_NOT_5_2_0 1 CACHE STRING "GCC is not 5.2.0 version")
endif()

list(APPEND compile_definitions "-DGCC_NOT_5_2_0=${GCC_NOT_5_2_0}")

if(CONFIG_OPTIMIZATION_LEVEL_RELEASE)
    list(APPEND compile_options "-Os")
else()
    list(APPEND compile_options "-Og")
endif()

if(CONFIG_CXX_EXCEPTIONS)
    list(APPEND cxx_compile_options "-fexceptions")
else()
    list(APPEND cxx_compile_options "-fno-exceptions")
endif()

if(CONFIG_DISABLE_GCC8_WARNINGS)
    list(APPEND compile_options "-Wno-parentheses"
                                "-Wno-sizeof-pointer-memaccess"
                                "-Wno-clobbered")

    # doesn't use GCC_NOT_5_2_0 because idf_set_global_variables was not called before
    if(GCC_NOT_5_2_0)
        list(APPEND compile_options "-Wno-format-overflow"
                                    "-Wno-stringop-truncation"
                                    "-Wno-misleading-indentation"
                                    "-Wno-cast-function-type"
                                    "-Wno-implicit-fallthrough"
                                    "-Wno-unused-const-variable"
                                    "-Wno-switch-unreachable"
                                    "-Wno-format-truncation"
                                    "-Wno-memset-elt-size"
                                    "-Wno-int-in-bool-context")
    endif()
endif()

if(CONFIG_OPTIMIZATION_ASSERTIONS_DISABLED)
    list(APPEND compile_definitions "NDEBUG")
endif()

if(CONFIG_STACK_CHECK_NORM)
    list(APPEND compile_options "-fstack-protector")
elseif(CONFIG_STACK_CHECK_STRONG)
    list(APPEND compile_options "-fstack-protector-strong")
elseif(CONFIG_STACK_CHECK_ALL)
    list(APPEND compile_options "-fstack-protector-all")
endif()

# All targets built under this scope is with the ESP-IDF build system
set(ESP_PLATFORM 1)
list(APPEND compile_definitions "-DESP_PLATFORM")

idf_build_set_property(COMPILE_OPTIONS "${compile_options}" APPEND)
idf_build_set_property(C_COMPILE_OPTIONS "${c_compile_options}" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "${cxx_compile_options}" APPEND)
idf_build_set_property(COMPILE_DEFINITIONS "${compile_definitions}" APPEND)

idf_build_get_property(build_component_targets __BUILD_COMPONENT_TARGETS)

# Add each component as a subdirectory, processing each component's CMakeLists.txt
foreach(component_target ${build_component_targets})
    __component_get_property(dir ${component_target} COMPONENT_DIR)
    __component_get_property(_name ${component_target} COMPONENT_NAME)
    __component_get_property(prefix ${component_target} __PREFIX)
    __component_get_property(alias ${component_target} COMPONENT_ALIAS)
    set(COMPONENT_NAME ${_name})
    set(COMPONENT_DIR ${dir})
    set(COMPONENT_ALIAS ${alias})
    set(COMPONENT_PATH ${dir}) # also deprecated, see comment in previous loop
    idf_build_get_property(build_prefix __PREFIX)
    set(__idf_component_context 1)
    if(NOT prefix STREQUAL build_prefix)
        add_subdirectory(${dir} ${prefix}_${_name} EXCLUDE_FROM_ALL)
    else()
        add_subdirectory(${dir} ${_name} EXCLUDE_FROM_ALL)
    endif()
    set(__idf_component_context 0)
endforeach()

# Establish dependencies between components
idf_build_get_property(build_components BUILD_COMPONENTS)
foreach(build_component ${build_components})
    __component_get_target(component_target ${build_component})
    __component_get_property(component_lib ${component_target} COMPONENT_LIB)
    __component_get_property(reqs ${component_target} __REQUIRES)
    foreach(req ${reqs})
        __component_get_property(req_lib ${req} COMPONENT_LIB)
        if(TARGET ${req_lib})
            set_property(TARGET ${component_lib} APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${req_lib})
        endif()
    endforeach()

    get_property(type TARGET ${component_lib} PROPERTY TYPE)
    if(type STREQUAL STATIC_LIBRARY)
        __component_get_property(reqs ${component_target} __REQUIRES)
        __component_get_property(priv_reqs ${component_target} __PRIV_REQUIRES)
        foreach(req ${reqs} ${priv_reqs})
            __component_get_property(req_lib ${req} COMPONENT_LIB)
            if(TARGET ${req_lib})
                set_property(TARGET ${component_lib} APPEND PROPERTY LINK_LIBRARIES ${req_lib})
            endif()
        endforeach()
    endif()
endforeach()