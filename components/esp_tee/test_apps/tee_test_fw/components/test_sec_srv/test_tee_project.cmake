# This file must be manually included in the project's top level CMakeLists.txt before project()
# This ensures that the variables are set before TEE starts building

get_filename_component(directory "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE DIRECTORY)
get_filename_component(name ${CMAKE_CURRENT_LIST_DIR} NAME)

# Append secure service table consisting of secure services
idf_build_set_property(CUSTOM_SECURE_SERVICE_YAML ${CMAKE_CURRENT_LIST_DIR}/sec_srv_tbl_test.yml APPEND)

# Append the directory of this component which is used by esp_tee component as
# EXTRA_COMPONENT_DIRS
idf_build_set_property(CUSTOM_SECURE_SERVICE_COMPONENT_DIR ${directory} APPEND)

# Append the name of the component so that esp_tee can include it in its COMPONENTS list
idf_build_set_property(CUSTOM_SECURE_SERVICE_COMPONENT ${name} APPEND)
