# This component provides the main function and conditionally
# includes logging_util and math_util based on the Kconfig options.

# Provide the main component as a static library
add_library(${COMPONENT_TARGET} STATIC main.c)

if(CONFIG_EXAMPLE_ENABLE_LOGGING)
    idf_component_include(logging_util)
    target_link_libraries(${COMPONENT_TARGET} PRIVATE idf::logging_util)
    idf_msg("Conditional component 'logging_util' is ENABLED")
else()
    idf_msg("Conditional component 'logging_util' is DISABLED")
endif()

if(CONFIG_EXAMPLE_ENABLE_MATH)
    idf_component_include(math_util)
    target_link_libraries(${COMPONENT_TARGET} PRIVATE idf::math_util)
    idf_msg("Conditional component 'math_util' is ENABLED")
else()
    idf_msg("Conditional component 'math_util' is DISABLED")
endif()
