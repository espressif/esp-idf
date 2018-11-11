set(COMPONENT_SRCS "main.cpp")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_EMBED_TXTFILES "sample.xml")

register_component()

# Build static library, do not build test executables
option(BUILD_SHARED_LIBS OFF)
option(BUILD_TESTING OFF)

# Import tinyxml2 targets 
add_subdirectory(lib/tinyxml2)

# Propagate compile settings to tinyxml2
target_include_directories(tinyxml2 PRIVATE ${IDF_INCLUDE_DIRECTORIES})
target_compile_options(tinyxml2 PRIVATE "${IDF_COMPILE_OPTIONS}")
target_compile_options(tinyxml2 PRIVATE "${IDF_CXX_COMPILE_OPTIONS}")

# Link tinyxml2 to main component
target_link_libraries(${COMPONENT_TARGET} tinyxml2)
