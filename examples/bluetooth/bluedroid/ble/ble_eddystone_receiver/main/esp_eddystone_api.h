/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */


#ifndef __ESP_EDDYSTONE_API_H__
#define __ESP_EDDYSTONE_API_H__

typedef struct {
    struct {
        uint8_t   flags;          /*<! AD flags data */
        uint16_t  srv_uuid;       /*<! complete list of 16-bit service uuid*/
        uint16_t  srv_data_type;  /*<! service data type */
        uint8_t   frame_type;     /*<! Eddystone UID, URL or TLM */
    } common;
    union {
        struct {
            /*<! Eddystone-UID */
            int8_t  ranging_data;     /*<! calibrated Tx power at 0m */
            uint8_t namespace_id[10];
            uint8_t instance_id[6];
        } uid;
        struct {
            /*<! Eddystone-URL */
            int8_t  tx_power;                    /*<! calibrated Tx power at 0m */
            char    url[EDDYSTONE_URL_MAX_LEN];  /*<! the decoded URL */
        } url;
        struct {
            /*<! Eddystone-TLM */
            uint8_t   version;           /*<! TLM version,0x00 for now */
            uint16_t  battery_voltage;   /*<! battery voltage in mV */
            float     temperature;       /*<! beacon temperature in degrees Celsius */
            uint32_t  adv_count;         /*<! adv pdu count since power-up */
            uint32_t  time;              /*<! time since power-up, a 0.1 second resolution counter */
        } tlm;
    } inform;
} esp_eddystone_result_t;

/* utils */
static inline uint16_t little_endian_read_16(const uint8_t *buffer, uint8_t pos)
{
    return ((uint16_t)buffer[pos]) | (((uint16_t)buffer[(pos)+1]) << 8);
}

static inline uint16_t big_endian_read_16(const uint8_t *buffer, uint8_t pos)
{
    return (((uint16_t)buffer[pos]) << 8) | ((uint16_t)buffer[(pos)+1]);
}

static inline uint32_t big_endian_read_32(const uint8_t *buffer, uint8_t pos)
{
    return (((uint32_t)buffer[pos]) << 24) | (((uint32_t)buffer[(pos)+1]) << 16) | (((uint32_t)buffer[(pos)+2]) << 8) | ((uint32_t)buffer[(pos)+3]);
}

/*
 * The esp eddystone API.
 * This function is called to decode eddystone information from adv_data.
 * The res points to the result struct.
 *
 */
esp_err_t esp_eddystone_decode(const uint8_t* buf, uint8_t len, esp_eddystone_result_t* res);

//bool esp_eddystone_is_eddystone_packet(.....);

#endif /* __ESP_EDDYSTONE_API_H__ */
