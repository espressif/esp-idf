# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mqtt_tcp_custom_outbox)

# Add custom outbox implementation to mqtt component
idf_component_get_property(mqtt mqtt COMPONENT_LIB)
target_sources(${mqtt} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main/custom_outbox.cpp)

# Our C++ needs an extra dependency to mqtt component, so we add it to mqtt component.
# This is needed because we are adding another source to the mqtt component and the build
# system needs to be aware of it to be able to compile and link the mqtt component.
# First we get our dependency
idf_component_get_property(pthread pthread COMPONENT_LIB)
# And them we link the components
target_link_libraries(${mqtt} ${pthread})
