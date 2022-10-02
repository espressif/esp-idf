# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building HTTP PARSER MOCKS")

idf_component_get_property(original_http_parser_dir http_parser COMPONENT_OVERRIDEN_DIR)

idf_component_mock(INCLUDE_DIRS "${original_http_parser_dir}"
    MOCK_HEADER_FILES ${original_http_parser_dir}/http_parser.h)
