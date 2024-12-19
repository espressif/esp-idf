# This file must be manually included in the project's top level CMakeLists.txt before project()
# This ensures that the variables are set before TEE starts building

# Append secure service table consisting of secure services
idf_build_set_property(CUSTOM_SECURE_SERVICE_TBL ${CMAKE_CURRENT_LIST_DIR}/esp_tee_att.tbl APPEND)
