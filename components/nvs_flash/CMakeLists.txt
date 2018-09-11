set(COMPONENT_SRCS "src/nvs_api.cpp"
                   "src/nvs_item_hash_list.cpp"
                   "src/nvs_page.cpp"
                   "src/nvs_pagemanager.cpp"
                   "src/nvs_storage.cpp"
                   "src/nvs_types.cpp")
set(COMPONENT_ADD_INCLUDEDIRS include)

set(COMPONENT_REQUIRES spi_flash)

register_component()
