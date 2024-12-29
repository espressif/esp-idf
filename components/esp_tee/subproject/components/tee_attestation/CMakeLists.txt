idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

if(esp_tee_build)
    return()
endif()

set(srcs "esp_tee_attestation.c")
set(include_dirs ".")
set(priv_requires esp_tee)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include_dirs}
                       PRIV_REQUIRES ${priv_requires})
