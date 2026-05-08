set(srcs "")

if(CONFIG_OPENTHREAD_BORDER_ROUTER AND CONFIG_OPENTHREAD_NETWORK_AUTO_START)
    list(APPEND srcs "ot_examples_br.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES openthread protocol_examples_common)
