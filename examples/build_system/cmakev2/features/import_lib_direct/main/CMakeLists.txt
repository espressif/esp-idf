# Fetch the external lwjson library at configure time.
include(FetchContent)
fetchcontent_declare(
    lwjson
    GIT_REPOSITORY https://github.com/MaJerle/lwjson.git
    GIT_TAG        v1.8.1
    GIT_SHALLOW    TRUE
)
fetchcontent_makeavailable(lwjson)

idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS ".")

# Link the fetched lwjson library to this component
target_link_libraries(${COMPONENT_LIB} PUBLIC lwjson)
