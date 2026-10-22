/**
 * @file     mesh_packet.h
 * @author Chen Yunge (chenyunge@roborock.com)
 * @brief Mesh Packet file
 * 
 * @version 1.0.0
 * @date 2026-10-22
 * 
 * 
 * @par 修改日志
 * <table>
 * <tr><th>Date    <th>Version <th>Author <th>description
 * <tr><td>2026-10-22 <td>{version} <td>Chen Yunge <td>{description}
 * </table>
*/
#ifndef __MESH_PACKET_H__
#define __MESH_PACKET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "mesh_core.h"

// Packet::header values
#define PH_ROUTE_MASK     0x03   // 2-bits
#define PH_TYPE_SHIFT        2
#define PH_TYPE_MASK      0x0F   // 4-bits
#define PH_VER_SHIFT         6
#define PH_VER_MASK       0x03   // 2-bits

#define ROUTE_TYPE_TRANSPORT_FLOOD   0x00    // flood mode + transport codes
#define ROUTE_TYPE_FLOOD             0x01    // flood mode, needs 'path' to be built up (max 64 bytes)
#define ROUTE_TYPE_DIRECT            0x02    // direct route, 'path' is supplied
#define ROUTE_TYPE_TRANSPORT_DIRECT  0x03    // direct route + transport codes

#define PAYLOAD_TYPE_REQ         0x00    // request (prefixed with dest/src hashes, MAC) (enc data: timestamp, blob)
#define PAYLOAD_TYPE_RESPONSE    0x01    // response to REQ or ANON_REQ (prefixed with dest/src hashes, MAC) (enc data: timestamp, blob)
#define PAYLOAD_TYPE_TXT_MSG     0x02    // a plain text message (prefixed with dest/src hashes, MAC) (enc data: timestamp, text)
#define PAYLOAD_TYPE_ACK         0x03    // a simple ack
#define PAYLOAD_TYPE_ADVERT      0x04    // a node advertising its Identity
#define PAYLOAD_TYPE_GRP_TXT     0x05    // an (unverified) group text message (prefixed with channel hash, MAC) (enc data: timestamp, "name: msg")
#define PAYLOAD_TYPE_GRP_DATA    0x06    // an (unverified) group datagram (prefixed with channel hash, MAC) (enc data: data_type(uint16), data_len, blob)
#define PAYLOAD_TYPE_ANON_REQ    0x07    // generic request (prefixed with dest_hash, ephemeral pub_key, MAC) (enc data: ...)
#define PAYLOAD_TYPE_PATH        0x08    // returned path (prefixed with dest/src hashes, MAC) (enc data: path, extra)
#define PAYLOAD_TYPE_TRACE       0x09    // trace a path, collecting SNR for each hop
#define PAYLOAD_TYPE_MULTIPART   0x0A    // packet is one of a set of packets
#define PAYLOAD_TYPE_CONTROL     0x0B    // a control/discovery packet
//...
#define PAYLOAD_TYPE_RAW_CUSTOM   0x0F    // custom packet as raw bytes, for applications with custom encryption, payloads, etc

#define PAYLOAD_VER_1       0x00   // 1-byte src/dest hashes, 2-byte MAC
#define PAYLOAD_VER_2       0x01   // FUTURE (eg. 2-byte hashes, 4-byte MAC ??)
#define PAYLOAD_VER_3       0x02   // FUTURE
#define PAYLOAD_VER_4       0x03   // FUTURE

typedef struct _T_MeshPacket
{
    uint8_t header;                         // header of the packet
    uint16_t payload_len;                   // payload length
    uint16_t path_len;                      // path length
    uint16_t transport_codes[2];            // transport code of the packet
    uint8_t path[MAX_PATH_SIZE];            // path of the packet
    uint8_t payload[MAX_PACKET_PAYLOAD];    // payload of the packet
    int8_t _snr;                            // SNR of the packet
}T_MeshPacket;

T_MeshPacket* MeshPacketNew(void);
void MeshPacketFree(T_MeshPacket* packet);

// function api

// calculate hash of  payload + type
void calculatePacketHash(T_MeshPacket *packet, uint8_t *dst_hash);

// 
uint8_t getRouteType(T_MeshPacket *packet);

bool isRouteDirect(T_MeshPacket *packet);
bool isRouteFlood(T_MeshPacket *packet);

bool hasTransportCodes(T_MeshPacket *packet);

//
uint8_t getPayloadType(T_MeshPacket *packet);

//
uint8_t getPayloadVer(T_MeshPacket *packet);

//
uint8_t getPathHashSize(T_MeshPacket *packet);
uint8_t getPathHashCount(T_MeshPacket *packet);
uint8_t getPathByteLen(T_MeshPacket *packet);
void setPathHashCount(T_MeshPacket *packet, uint8_t n);
void setPathHashSizeAndCount(T_MeshPacket *packet, uint8_t sz, uint8_t n);

uint8_t copyPath(uint8_t* dest, const uint8_t* src, uint8_t path_len);
size_t writePath(uint8_t* dest, const uint8_t* src, uint8_t path_len);
bool isValidPathLen(uint8_t path_len);

void markDoNotRetransmit(T_MeshPacket *packet);
bool isMarkedDoNotRetransmit(T_MeshPacket *packet);

float getSNR(T_MeshPacket *packet);

// 
int getRawLength(T_MeshPacket *packet);

uint8_t writeTo(T_MeshPacket *packet, uint8_t *dst);
bool readFrom(T_MeshPacket *packet, const uint8_t *src, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif // __MESH_PACKET_H__
