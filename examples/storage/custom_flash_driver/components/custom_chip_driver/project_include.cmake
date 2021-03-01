# Add custom dependency to the spi_flash component.
# This is a workaround, which will also add the dependency to all other components.
idf_build_set_property(___COMPONENT_REQUIRES_COMMON idf::custom_chip_driver APPEND)
