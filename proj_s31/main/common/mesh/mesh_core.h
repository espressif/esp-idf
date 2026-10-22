#ifndef __MESH_CORE_H__
#define __MESH_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MAX_HASH_SIZE        8
#define PUB_KEY_SIZE        32
#define PRV_KEY_SIZE        64
#define SEED_SIZE           32
#define SIGNATURE_SIZE      64
#define MAX_ADVERT_DATA_SIZE  32
#define CIPHER_KEY_SIZE     16
#define CIPHER_BLOCK_SIZE   16

// V1
#define CIPHER_MAC_SIZE      2
#define PATH_HASH_SIZE       1

#define MAX_PACKET_PAYLOAD  184
#define MAX_GROUP_DATA_LENGTH  (MAX_PACKET_PAYLOAD - CIPHER_BLOCK_SIZE - 3)
#define MAX_PATH_SIZE        64
#define MAX_TRANS_UNIT      255

/* Logging: ESP-IDF → esp_log; Arduino → Serial; else no-op */
#if defined(ESP_PLATFORM)
    #include "esp_log.h"
    #ifndef MESH_LOG_TAG
        #define MESH_LOG_TAG "mesh"
    #endif
        #define MESH_INFO_PRINT(F, ...)   ESP_LOGI(MESH_LOG_TAG, F, ##__VA_ARGS__)
        #define MESH_WARN_PRINT(F, ...)   ESP_LOGW(MESH_LOG_TAG, F, ##__VA_ARGS__)
        #define MESH_ERROR_PRINT(F, ...)   ESP_LOGE(MESH_LOG_TAG, F, ##__VA_ARGS__)
        #define MESH_DEBUG_PRINT(F, ...)   ESP_LOGD(MESH_LOG_TAG, F, ##__VA_ARGS__)
        #define MESH_DEBUG_PRINTLN(F, ...) ESP_LOGD(MESH_LOG_TAG, F, ##__VA_ARGS__)
#elif defined(ARDUINO)
    #include <Arduino.h>
        #define MESH_DEBUG_PRINT(F, ...)   Serial.printf("DEBUG: " F, ##__VA_ARGS__)
        #define MESH_DEBUG_PRINTLN(F, ...) Serial.printf("DEBUG: " F "\n", ##__VA_ARGS__)
        #define BRIDGE_DEBUG_PRINTLN(F, ...) Serial.printf("%s BRIDGE: " F, getLogDateTime(), ##__VA_ARGS__)
#else
    #define MESH_DEBUG_PRINT(...)     do {} while (0)
    #define MESH_DEBUG_PRINTLN(...)   do {} while (0)
    #define BRIDGE_DEBUG_PRINTLN(...) do {} while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MESH_CORE_H__ */