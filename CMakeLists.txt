cmake_minimum_required(VERSION 3.5)
project(esp-idf C CXX ASM)

#
# Add each component to the build as a library
#
foreach(COMPONENT_PATH ${BUILD_COMPONENT_PATHS})
    get_filename_component(COMPONENT_NAME ${COMPONENT_PATH} NAME)

    list(FIND BUILD_TEST_COMPONENT_PATHS ${COMPONENT_PATH} idx)

    if(NOT idx EQUAL -1)
        list(GET BUILD_TEST_COMPONENTS ${idx} test_component)
        set(COMPONENT_NAME ${test_component})
    endif()

    component_get_target(COMPONENT_TARGET ${COMPONENT_NAME})

    add_subdirectory(${COMPONENT_PATH} ${COMPONENT_NAME})
endforeach()
unset(COMPONENT_NAME)
unset(COMPONENT_PATH)

# each component should see the include directories of its requirements
#
# (we can't do this until all components are registered and targets exist in cmake, as we have
# a circular requirements graph...)
foreach(component ${BUILD_COMPONENTS})
    component_get_target(component_target ${component})
    if(TARGET ${component_target})
        get_component_requirements(${component} deps priv_deps)

        list(APPEND priv_deps ${IDF_COMPONENT_REQUIRES_COMMON})

        foreach(dep ${deps})
            component_get_target(dep_target ${dep})
            add_component_dependencies(${component_target} ${dep_target} PUBLIC)
        endforeach()

        foreach(dep ${priv_deps})
            component_get_target(dep_target ${dep})
            add_component_dependencies(${component_target} ${dep_target} PRIVATE)
        endforeach()
    endif()
endforeach()