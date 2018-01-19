set(COMPONENT_SRCDIRS src/crypto port src/fast_crypto)
set(COMPONENT_ADD_INCLUDEDIRS include port/include)

register_component()

target_compile_definitions(wpa_supplicant PRIVATE __ets__)
target_compile_options(wpa_supplicant PRIVATE -Wno-strict-aliasing)
