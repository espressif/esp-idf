cmake_minimum_required(VERSION 3.5)
project(esp-idf C CXX ASM)

unset(compile_options)
unset(c_compile_options)
unset(cxx_compile_options)
unset(compile_definitions)

# Add the following build specifications here, since these seem to be dependent
# on config values on the root Kconfig.

if(CONFIG_COMPILER_OPTIMIZATION_LEVEL_RELEASE)
    list(APPEND compile_options "-Os")
else()
    list(APPEND compile_options "-Og")
endif()

if(CONFIG_COMPILER_CXX_EXCEPTIONS)
    list(APPEND cxx_compile_options "-fexceptions")
else()
    list(APPEND cxx_compile_options "-fno-exceptions")
endif()

if(CONFIG_COMPILER_DISABLE_GCC8_WARNINGS)
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

if(CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE)
    list(APPEND compile_definitions "-DNDEBUG")
endif()

if(CONFIG_COMPILER_STACK_CHECK_MODE_NORM)
    list(APPEND compile_options "-fstack-protector")
elseif(CONFIG_COMPILER_STACK_CHECK_MODE_STRONG)
    list(APPEND compile_options "-fstack-protector-strong")
elseif(CONFIG_COMPILER_STACK_CHECK_MODE_ALL)
    list(APPEND compile_options "-fstack-protector-all")
endif()


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
    set(COMPONENT_PATH ${dir}) # for backward compatibility only, COMPONENT_DIR is preferred
    idf_build_get_property(build_prefix __PREFIX)
    set(__idf_component_context 1)
    if(NOT prefix STREQUAL build_prefix)
        add_subdirectory(${dir} ${prefix}_${_name})
    else()
        add_subdirectory(${dir} ${_name})
    endif()
    set(__idf_component_context 0)
endforeach()