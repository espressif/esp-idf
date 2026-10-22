#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "psa/crypto.h"
#include "mesh_packet.h"

/**
 * @brief Create a new object of T_MeshPacket
 * 
 * @author Chen Yunge (chenyunge@roborock.com)
 * @return T_MeshPacket* The new object of T_MeshPacket
 * 
 * @warning 
 * @note 
 * @attention 
*/
T_MeshPacket* MeshPacketNew(void)
{
    T_MeshPacket* packet = (T_MeshPacket*)calloc(1, sizeof(T_MeshPacket));
    if(packet == NULL)
    {
        MESH_ERROR_PRINT("MeshPacketNew: calloc failed");
        return NULL;
    }
    return packet;
}

/**
 * @brief Free the object of T_MeshPacket
 * 
 * @author Chen Yunge (chenyunge@roborock.com)
 * @param[in] packet The object of T_MeshPacket to be freed
 * 
 * @warning 
 * @note 
 * @attention 
*/
void MeshPacketFree(T_MeshPacket* packet)
{
    if (packet != NULL) 
    {
        free(packet);
    }
}

// function api

/**
 * @brief calculate hash of payload + type
 * 
 * @author Chen Yunge (chenyunge@roborock.com)
 * @param[in] packet The packet to calculate hash
 * @param[out] dst_hash The destination hash 8 bytes long
 * 
 * @warning 
 * @note 
 * @attention ESP-IDF mbedtls 4.x: use PSA Crypto (no public mbedtls/sha256.h)
 * if use other platform, use other hash algorithm
*/
void calculatePacketHash(T_MeshPacket *packet, uint8_t *dst_hash)
{
    if(packet == NULL || dst_hash == NULL)
    {
        MESH_ERROR_PRINT("packet or dst_hash is NULL");
        return;
    }

    uint8_t full[32];
    uint8_t t;
    size_t hash_len = 0;
    psa_status_t status;
    psa_hash_operation_t op = PSA_HASH_OPERATION_INIT;

    // get payload type
    t = getPayloadType(packet);

    // start up hash operation
    status = psa_hash_setup(&op, PSA_ALG_SHA_256);
    if (status != PSA_SUCCESS)
    {
        MESH_ERROR_PRINT("psa_hash_setup failed: %d", (int)status);
        return;
    }

    // use type to update hash
    status = psa_hash_update(&op, &t, 1);
    if (status != PSA_SUCCESS) 
    {
        goto fail;
    }

    if (t == PAYLOAD_TYPE_TRACE) 
    {
        status = psa_hash_update(&op, (const uint8_t *)&packet->path_len,
                                 sizeof(packet->path_len));
        if (status != PSA_SUCCESS) 
        {
            goto fail;
        }
    }

    status = psa_hash_update(&op, packet->payload, packet->payload_len);
    if (status != PSA_SUCCESS)
    {
        goto fail;
    }

    status = psa_hash_finish(&op, full, sizeof(full), &hash_len);
    if (status != PSA_SUCCESS || hash_len < MAX_HASH_SIZE)
    {
        MESH_ERROR_PRINT("psa_hash_finish failed: %d", (int)status);
        psa_hash_abort(&op);
        return;
    }

    memcpy(dst_hash, full, MAX_HASH_SIZE);
    return;

fail:
    MESH_ERROR_PRINT("psa_hash_update failed: %d", (int)status);
    psa_hash_abort(&op);
}

/**
 * @brief Get the Route Type object
 * 
 * @author Chen Yunge (chenyunge@roborock.com)
 * @param[in] packet 
 * @return uint8_t 0xFF if packet is NULL, otherwise the route type
 * 
 * @warning 
 * @note 
 * @attention 
*/
uint8_t getRouteType(T_MeshPacket *packet)
{
    if (packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return 0xFF;
    }
    return (packet->header & PH_ROUTE_MASK);
}

/**
 * @brief Check if the packet is a direct route
 * 
 * @author Chen Yunge (chenyunge@roborock.com)
 * @param[in] packet The packet to check
 * @return bool true if the packet is a direct route, false otherwise
 * 
 * @warning 
 * @note 
 * @attention 
*/
bool isRouteDirect(T_MeshPacket *packet)
{
    if (packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return false;
    }
    return (getRouteType(packet) == ROUTE_TYPE_DIRECT || getRouteType(packet) == ROUTE_TYPE_TRANSPORT_DIRECT);
}

/**
 * @brief Check if the packet is a flood route
 * 
 * @author Chen Yunge (chenyunge@roborock.com)
 * @param[in] packet The packet to check
 * @return bool true if the packet is a flood route, false otherwise
 * 
 * @warning 
 * @note 
 * @attention 
*/
bool isRouteFlood(T_MeshPacket *packet)
{
    if (packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return false;
    }
    return (getRouteType(packet) == ROUTE_TYPE_FLOOD || getRouteType(packet) == ROUTE_TYPE_TRANSPORT_FLOOD);
}

/**
 * @brief Check if the packet has a transport code
 * 
 * @author Chen Yunge (chenyunge@roborock.com)
 * @param[in] packet The packet to check
 * @return bool true if the packet has a transport code, false otherwise
 * 
 * @warning 
 * @note 
 * @attention 
*/
bool hasTransportCodes(T_MeshPacket *packet)
{
    if (packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return false;
    }
    return (getRouteType(packet) == ROUTE_TYPE_TRANSPORT_FLOOD || getRouteType(packet) == ROUTE_TYPE_TRANSPORT_DIRECT);
}

/**
 * @brief Get the payload type
 * 
 * @author Chen Yunge (chenyunge@roborock.com)
 * @param[in] packet The packet to get the payload type
 * @return uint8_t 0xFF if packet is NULL, otherwise the payload type
 * 
 * @warning 
 * @note 
 * @attention 
*/
uint8_t getPayloadType(T_MeshPacket *packet)
{
    if(packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return 0xFF;
    }

    return (packet->header >> PH_TYPE_SHIFT) & PH_TYPE_MASK;
}

/**
 * @brief Get the payload version
 * 
 * @author Chen Yunge (chenyunge@roborock.com)
 * @param[in] packet The packet to get the payload version
 * @return uint8_t 0xFF if packet is NULL, otherwise the payload version
 * 
 * @warning 
 * @note 
 * @attention 
*/
uint8_t getPayloadVer(T_MeshPacket *packet)
{
    if(packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return 0xFF;
    }

    return (packet->header >> PH_VER_SHIFT) & PH_VER_MASK;
}

/**
 * @brief Get the path hash size
 * 
 * @author Chen Yunge (chenyunge@roborock.com)
 * @param[in] packet The packet to get the path hash size
 * @return uint8_t 0 if packet is NULL, otherwise the path hash size
 * 
 * @warning 
 * @note 
 * @attention 
*/
uint8_t getPathHashSize(T_MeshPacket *packet)
{
    if(packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return 0;
    }

    return (packet->path_len >> 6) + 1;
}

uint8_t getPathHashCount(T_MeshPacket *packet)
{
    if(packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return 0;
    }

    return (packet->path_len & 63);
}

uint8_t getPathByteLen(T_MeshPacket *packet)
{
    if(packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return 0;
    }
    return getPathHashCount(packet) * getPathHashSize(packet);
}

void setPathHashCount(T_MeshPacket *packet, uint8_t n)
{
    if(packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return;
    }

    packet->path_len &= ~63;
    packet->path_len |= n;
}

void setPathHashSizeAndCount(T_MeshPacket *packet, uint8_t sz, uint8_t n)
{
    if(packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return;
    }

    packet->path_len = ((sz - 1) << 6) | (n & 63);
}

uint8_t copyPath(uint8_t* dest, const uint8_t* src, uint8_t path_len)
{
    writePath(dest, src, path_len);
    return path_len;
}

size_t writePath(uint8_t* dest, const uint8_t* src, uint8_t path_len)
{
    uint8_t hash_count = path_len & 63;
    uint8_t hash_size = (path_len >> 6) + 1;
    size_t len = hash_count * hash_size;
    if (len > MAX_PATH_SIZE)
    {
        MESH_DEBUG_PRINTLN("Packet::copyPath, invalid path_len=%d", (uint32_t)path_len);
        return 0;   // Error
    }
    memcpy(dest, src, len);
    return len;
}

bool isValidPathLen(uint8_t path_len)
{
    uint8_t hash_count = path_len & 63;
    uint8_t hash_size = (path_len >> 6) + 1;
    if (hash_size == 4)
    {
        MESH_ERROR_PRINT("path_len is reserved for future");
        return false;  // Reserved for future
    }
    return hash_count*hash_size <= MAX_PATH_SIZE;
}

/**
 * @brief Mark the packet as do not retransmit
 * 
 * @author Chen Yunge (chenyunge@roborock.com)
 * @param[in/out] packet The packet to mark as do not retransmit
 * 
 * @warning 
 * @note 
 * @attention 
*/
void markDoNotRetransmit(T_MeshPacket *packet)
{
    if(packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return;
    }

    packet->header = 0xFF;
}

/**
 * @brief Check if the packet is marked as do not retransmit
 * 
 * @author Chen Yunge (chenyunge@roborock.com)
 * @param[in] packet The packet to check if it is marked as do not retransmit
 * @return bool true if the packet is marked as do not retransmit, false otherwise
 * 
 * @warning 
 * @note 
 * @attention 
*/
bool isMarkedDoNotRetransmit(T_MeshPacket *packet)
{
    if(packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return false;
    }

    return packet->header == 0xFF;
}

float getSNR(T_MeshPacket *packet)
{
    if(packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return 0.0;
    }

    return ((float)packet->_snr) / 4.0f;
}

// 
int getRawLength(T_MeshPacket *packet)
{
    if(packet == NULL)
    {
        MESH_ERROR_PRINT("packet is NULL");
        return 0;
    }

    return 2 + getPathByteLen(packet) + packet->payload_len + (hasTransportCodes(packet) ? 4 : 0);
}

uint8_t writeTo(T_MeshPacket *packet, uint8_t *dst)
{
    if(packet == NULL || dst == NULL)
    {
        MESH_ERROR_PRINT("packet or dst is NULL");
        return 0;
    }

    uint8_t i = 0;
    dst[i++] = packet->header;
    if (hasTransportCodes(packet))
    {
        memcpy(&dst[i], &packet->transport_codes[0], 2); i += 2;
        memcpy(&dst[i], &packet->transport_codes[1], 2); i += 2;
    }
    dst[i++] = packet->path_len;
    i += writePath(&dst[i], packet->path, packet->path_len);
    memcpy(&dst[i], packet->payload, packet->payload_len); i += packet->payload_len;
    return i;
}

bool readFrom(T_MeshPacket *packet, const uint8_t *src, uint8_t len)
{
    if(packet == NULL || src == NULL)
    {
        MESH_ERROR_PRINT("packet or src is NULL");
        return false;
    }

    uint8_t i = 0;
    packet->header = src[i++];
    if(hasTransportCodes(packet))
    {
        memcpy(&packet->transport_codes[0], &src[i], 2); i += 2;
        memcpy(&packet->transport_codes[1], &src[i], 2); i += 2;
    }
    else 
    {
        packet->transport_codes[0] = 0;
        packet->transport_codes[1] = 0;
    }

    packet->path_len = src[i++];

    if(!isValidPathLen(packet->path_len))
    {//bad encoding
        return false;
    }

    uint8_t bl = getPathByteLen(packet);
    memcpy(packet->path, &src[i], bl);
    i += bl;

    if(i >= len)
    {// bad encoding
        return false;
    }
    packet->payload_len = len - i;

    if(packet->payload_len > sizeof(packet->payload))
    {// bad encoding
        return false;
    }

    memcpy(packet->payload, &src[i], packet->payload_len); // i += packet->payload_len

    return true;
}