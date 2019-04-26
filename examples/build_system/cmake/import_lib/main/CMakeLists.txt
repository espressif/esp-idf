set(COMPONENT_SRCS "main.cpp")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_EMBED_TXTFILES "sample.xml")

register_component()

# Build static library, do not build test executables
option(BUILD_SHARED_LIBS OFF)
option(BUILD_TESTING OFF)

# Unfortunately the library performs install and export. Would 
# have been nice if devs made that an option like BUILD_SHARED_LIBS
# and BUILD_TESTING. Override install() and export() to do nothing
# instead.
function(install)
endfunction()

function(export)
endfunction()

# Import tinyxml2 targets 
add_subdirectory(lib/tinyxml2)

# Link tinyxml2 to main component
target_link_libraries(${COMPONENT_LIB} tinyxml2)
