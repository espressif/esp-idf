cmake_minimum_required(VERSION 3.5)
project(idfexe)

set(VERSION 1.0.1)
set(ARCHIVE_NAME idf-exe-v${VERSION}.zip)

add_executable(idf idf_main.c)
target_compile_definitions(idf PRIVATE -DVERSION=\"${VERSION}\")
set_target_properties(idf PROPERTIES C_STANDARD 99)
target_link_libraries(idf "-lshlwapi")

if(CMAKE_BUILD_TYPE STREQUAL Release)
    add_custom_command(TARGET idf
                        POST_BUILD
                        COMMAND ${CMAKE_STRIP} idf.exe)
endif()

add_custom_target(dist ALL DEPENDS idf)

add_custom_command(
    TARGET dist
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} ARGS -E copy "${CMAKE_CURRENT_BINARY_DIR}/idf.exe" "${CMAKE_CURRENT_BINARY_DIR}/idf.py.exe"
    COMMAND ${CMAKE_COMMAND} ARGS -E tar cfv ${ARCHIVE_NAME} --format=zip
       "${CMAKE_CURRENT_BINARY_DIR}/idf.py.exe"
    )
