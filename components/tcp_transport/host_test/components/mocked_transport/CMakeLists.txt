# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.

idf_component_mock(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    REQUIRES tcp_transport
    MOCK_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/mock_transport.h)
