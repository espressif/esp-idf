# Check for configuration options and set the EMBED_TXTFILES accordingly
if(DEFINED CONFIG_HTTPS_DNS_CERT_GOOGLE_HIDDEN)
    set(cert_file "${CONFIG_HTTPS_DNS_CERT_GOOGLE_HIDDEN}")
elseif(DEFINED CONFIG_HTTPS_DNS_CERT_CUSTOM_HIDDEN)
    set(cert_file "${CONFIG_HTTPS_DNS_CERT_CUSTOM_HIDDEN}")
endif()

idf_component_register(SRCS "example_dns_over_https.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES ${cert_file})
