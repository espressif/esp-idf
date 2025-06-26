# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(utilities)

#[[api
.. cmakev2:function:: idf_build_set_property

   .. code-block:: cmake

      idf_build_set_property(<property> <value> [APPEND])

   :property[in]: Property name.
   :value[in]: Property value.
   :APPEND: Append the value to the property's current value instead of
            replacing it.

   Set the value of the specified property related to the ESP-IDF build. The
   property is also added to the internal list of build properties if it isn't
   already there.
#]]
function(idf_build_set_property property value)
    set(options APPEND)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    get_property(properties TARGET idf_build_properties PROPERTY BUILD_PROPERTIES)
    if(NOT property IN_LIST properties)
        list(APPEND properties "${property}")
        set_property(TARGET idf_build_properties PROPERTY BUILD_PROPERTIES "${properties}")
    endif()
    if(ARG_APPEND)
        set_property(TARGET idf_build_properties APPEND PROPERTY "${property}" "${value}")
    else()
        set_property(TARGET idf_build_properties PROPERTY "${property}" "${value}")
    endif()
endfunction()

#[[api
.. cmakev2:function:: idf_build_get_property

   .. code-block:: cmake

      idf_build_get_property(<var> <property> [GENERATOR_EXPRESSION])

   :variable[out]: Variable to store the value in.
   :property[in]: Property name to get the value of.
   :GENERATOR_EXPRESSION: Obtain the generator expression for the property
                          rather than the actual value.

   Get the value of the specified property related to the ESP-IDF build.
#]]
function(idf_build_get_property variable property)
    set(options GENERATOR_EXPRESSION)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if("${property}" STREQUAL BUILD_COMPONENTS)
        idf_die("Build property 'BUILD_COMPONENTS' is not supported")
    endif()
    if(ARG_GENERATOR_EXPRESSION)
        set(value "$<TARGET_PROPERTY:idf_build_properties,${property}>")
    else()
        get_property(value TARGET idf_build_properties PROPERTY ${property})
    endif()
    set(${variable} ${value} PARENT_SCOPE)
endfunction()

#[[
   __dump_build_properties()

   Dump all build properties.
#]]
function(__dump_build_properties)
    idf_build_get_property(properties BUILD_PROPERTIES)
    idf_msg("build properties: ${properties}")
    foreach(property IN LISTS properties)
        idf_build_get_property(value ${property})
        idf_msg("   ${property}: ${value}")
    endforeach()
endfunction()
