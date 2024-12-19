idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

set(srcs)
set(include_dirs ".")
set(priv_requires esp_tee)

if(esp_tee_build)
    list(APPEND priv_requires attestation main)
endif()

if(CONFIG_SECURE_TEE_ATTESTATION)
    list(APPEND srcs "esp_tee_attestation.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include_dirs}
                       PRIV_REQUIRES ${priv_requires})
