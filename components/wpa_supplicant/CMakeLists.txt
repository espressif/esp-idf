set(COMPONENT_SRCDIRS src/crypto port src/fast_crypto)
set(COMPONENT_ADD_INCLUDEDIRS include port/include)

register_component()

component_compile_definitions(__ets__)
component_compile_options(-Wno-strict-aliasing)
