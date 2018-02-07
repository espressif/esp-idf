#
# Component Makefile
#
 
# Component configuration in preprocessor defines
CFLAGS += -DUSE_LWIP_SOCKET_FOR_AZURE_IOT
 
 
COMPONENT_ADD_INCLUDEDIRS :=  \
pal  \
pal/inc  \
sdk/c-utility/inc  \
sdk/c-utility/inc/azure_c_shared_utility \
sdk/c-utility/pal/inc \
sdk/c-utility/pal/freertos \
sdk/c-utility/pal/generic \
sdk/iothub_client/inc \
sdk/serializer/inc \
sdk/umqtt/inc  \
sdk/umqtt/inc/azure_umqtt_c                 \
sdk/deps/parson
 
COMPONENT_OBJS =  \
sdk/c-utility/pal/freertos/lock.o                \
sdk/c-utility/pal/dns_async.o     \
sdk/c-utility/pal/socket_async.o               \
sdk/c-utility/pal/freertos/threadapi.o    \
sdk/c-utility/pal/freertos/tickcounter.o                \
sdk/c-utility/pal/lwip/sntp_lwip.o            \
sdk/c-utility/pal/tlsio_options.o            \
sdk/c-utility/adapters/agenttime.o         \
\
pal/src/platform_openssl_compact.o    \
pal/src/tlsio_openssl_compact.o     \
\
sdk/c-utility/src/xlogging.o          \
sdk/c-utility/src/singlylinkedlist.o             \
sdk/c-utility/src/buffer.o             \
sdk/c-utility/src/consolelogger.o              \
sdk/c-utility/src/constbuffer.o  \
sdk/c-utility/src/constmap.o      \
sdk/c-utility/src/crt_abstractions.o          \
sdk/c-utility/src/doublylinkedlist.o          \
sdk/c-utility/src/gballoc.o            \
sdk/c-utility/src/gb_stdio.o         \
sdk/c-utility/src/gb_time.o         \
sdk/c-utility/src/hmac.o               \
sdk/c-utility/src/hmacsha256.o \
sdk/c-utility/src/httpapiex.o       \
sdk/c-utility/src/httpapiexsas.o                \
sdk/c-utility/src/httpheaders.o \
sdk/c-utility/src/map.o \
sdk/c-utility/src/optionhandler.o             \
sdk/c-utility/src/sastoken.o        \
sdk/c-utility/src/sha1.o \
sdk/c-utility/src/sha224.o            \
sdk/c-utility/src/sha384-512.o    \
sdk/c-utility/src/strings.o             \
sdk/c-utility/src/string_tokenizer.o         \
sdk/c-utility/src/urlencode.o      \
sdk/c-utility/src/usha.o \
sdk/c-utility/src/vector.o             \
sdk/c-utility/src/xio.o    \
sdk/c-utility/src/base64.o \
\
\
sdk/iothub_client/src/iothub_client_ll.o               \
sdk/iothub_client/src/iothub_client_ll_uploadtoblob.o \
sdk/iothub_client/src/iothub_client_authorization.o      \
sdk/iothub_client/src/iothub_client_retry_control.o      \
sdk/iothub_client/src/iothub_client_diagnostic.o            \
sdk/iothub_client/src/iothub_message.o            \
sdk/iothub_client/src/iothubtransport.o              \
sdk/iothub_client/src/iothubtransportmqtt.o    \
sdk/iothub_client/src/iothubtransport_mqtt_common.o             \
sdk/iothub_client/src/version.o               \
\
\
sdk/umqtt/src/mqtt_client.o     \
sdk/umqtt/src/mqtt_codec.o    \
sdk/umqtt/src/mqtt_message.o              \
\
\
sdk/deps/parson/parson.o         \
\
sdk/serializer/src/codefirst.o   \
sdk/serializer/src/agenttypesystem.o   \
sdk/serializer/src/commanddecoder.o   \
sdk/serializer/src/datamarshaller.o   \
sdk/serializer/src/datapublisher.o   \
sdk/serializer/src/dataserializer.o   \
sdk/serializer/src/iotdevice.o   \
sdk/serializer/src/jsondecoder.o   \
sdk/serializer/src/jsonencoder.o   \
sdk/serializer/src/methodreturn.o   \
sdk/serializer/src/multitree.o   \
sdk/serializer/src/schema.o   \
sdk/serializer/src/schemalib.o   \
sdk/serializer/src/schemaserializer.o   \
\
\
 
COMPONENT_SRCDIRS :=  \
pal/src  \
sdk/c-utility/pal  \
sdk/c-utility/pal/freertos  \
sdk/c-utility/pal/lwip  \
sdk/c-utility/src \
sdk/c-utility/adapters  \
sdk/umqtt/src   \
sdk/iothub_client/src  \
sdk/serializer/src  \
sdk/deps/parson
