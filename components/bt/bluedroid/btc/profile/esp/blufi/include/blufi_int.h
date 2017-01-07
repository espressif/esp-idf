// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __BLUFI_INT_H__
#define __BLUFI_INT_H__

/* service engine control block */
typedef struct {
    /* Protocol reference */
    tGATT_IF                gatt_if;
    UINT8                   srvc_inst;
    UINT16                  handle_srvc;             
    UINT16                  handle_char_p2e;             
    UINT16                  handle_char_e2p;             
    UINT16                  handle_descr_e2p;             
    UINT16                  conn_id;
    BOOLEAN                 is_connected;
    BD_ADDR                 remote_bda;
    UINT32                  trans_id;
    UINT8                   congest;
#define BLUFI_PREPAIR_BUF_MAX_SIZE 1024
    uint8_t                 *prepare_buf;
    int                     prepare_len;
    /* Control reference */
    esp_blufi_callbacks_t   *cbs;
    BOOLEAN                 enabled;
    uint8_t                 send_seq;
    uint8_t                 recv_seq;
    uint8_t                 sec_mode;
    uint8_t                 *aggr_buf;
    uint16_t                 total_len;
    uint16_t                offset;
} tBLUFI_ENV;

/* BLUFI protocol */
struct blufi_hdr{
    uint8_t type;
    uint8_t fc;
    uint8_t seq;
    uint8_t data_len;
    uint8_t data[0];
};
typedef struct blufi_hdr blufi_hd_t;

struct blufi_frag_hdr {
    uint8_t type;
    uint8_t fc;
    uint8_t seq;
    uint8_t data_len;
    uint16_t total_len;
    uint8_t content[0];
};
typedef struct blufi_frag_hdr blufi_frag_hdr_t;

#define BLUFI_DATA_SEC_MODE_CHECK_MASK  0x01  
#define BLUFI_DATA_SEC_MODE_ENC_MASK    0x02  
#define BLUFI_CTRL_SEC_MODE_CHECK_MASK  0x10  
#define BLUFI_CTRL_SEC_MODE_ENC_MASK    0x20

// packet type
#define BLUFI_TYPE_MASK         0x03
#define BLUFI_TYPE_SHIFT        0 
#define BLUFI_SUBTYPE_MASK      0xFC
#define BLUFI_SUBTYPE_SHIFT     2 

#define BLUFI_GET_TYPE(type)    ((type) & BLUFI_TYPE_MASK)
#define BLUFI_GET_SUBTYPE(type) (((type) & BLUFI_SUBTYPE_MASK) >>BLUFI_SUBTYPE_SHIFT)
#define BLUFI_BUILD_TYPE(type, subtype) (((type) & BLUFI_TYPE_MASK) | ((subtype)<<BLUFI_SUBTYPE_SHIFT))

#define BLUFI_TYPE_CTRL                                 0x0
#define BLUFI_TYPE_CTRL_SUBTYPE_ACK                     0x00
#define BLUFI_TYPE_CTRL_SUBTYPE_SET_SEC_MODE            0x01
#define BLUFI_TYPE_CTRL_SUBTYPE_SET_WIFI_OPMODE         0x02
#define BLUFI_TYPE_CTRL_SUBTYPE_CONN_TO_AP              0x03
#define BLUFI_TYPE_CTRL_SUBTYPE_DISCONN_FROM_AP         0x04
#define BLUFI_TYPE_CTRL_SUBTYPE_GET_WIFI_STATUS         0x05
#define BLUFI_TYPE_CTRL_SUBTYPE_DEAUTHENTICATE_STA             0x06

#define BLUFI_TYPE_DATA                                 0x1
#define BLUFI_TYPE_DATA_SUBTYPE_NEG                     0x00
#define BLUFI_TYPE_DATA_SUBTYPE_STA_BSSID               0x01
#define BLUFI_TYPE_DATA_SUBTYPE_STA_SSID                0x02
#define BLUFI_TYPE_DATA_SUBTYPE_STA_PASSWD              0x03
#define BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_SSID             0x04
#define BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_PASSWD           0x05
#define BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_MAX_CONN_NUM     0x06
#define BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_AUTH_MODE        0x07
#define BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_CHANNEL          0x08
#define BLUFI_TYPE_DATA_SUBTYPE_USERNAME                0x09
#define BLUFI_TYPE_DATA_SUBTYPE_CA                      0x0a
#define BLUFI_TYPE_DATA_SUBTYPE_CLIENT_CERT             0x0b
#define BLUFI_TYPE_DATA_SUBTYPE_SERVER_CERT             0x0c
#define BLUFI_TYPE_DATA_SUBTYPE_CLIENT_PRIV_KEY         0x0d
#define BLUFI_TYPE_DATA_SUBTYPE_SERVER_PRIV_KEY         0x0e
#define BLUFI_TYPE_DATA_SUBTYPE_WIFI_REP                0x0f

#define BLUFI_TYPE_IS_CTRL(type)        (BLUFI_GET_TYPE((type)) == BLUFI_TYPE_CTRL)
#define BLUFI_TYPE_IS_DATA(type)        (BLUFI_GET_TYPE((type)) == BLUFI_TYPE_DATA)

#define BLUFI_TYPE_IS_CTRL_ACK(type)                 (BLUFI_TYPE_IS_CTRL((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_CTRL_SUBTYPE_ACK)
#define BLUFI_TYPE_IS_CTRL_START_NEG(type)           (BLUFI_TYPE_IS_CTRL((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_CTRL_SUBTYPE_START_NEG)
#define BLUFI_TYPE_IS_CTRL_STOP_NEG(type)            (BLUFI_TYPE_IS_CTRL((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_CTRL_SUBTYPE_STOP_NEG)
#define BLUFI_TYPE_IS_CTRL_SET_WIFI_OPMODE(type)     (BLUFI_TYPE_IS_CTRL((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_CTRL_SUBTYPE_SET_WIFI_OPMODE)
#define BLUFI_TYPE_IS_CTRL_CONN_WIFI(type)           (BLUFI_TYPE_IS_CTRL((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_CTRL_SUBTYPE_CONN_TO_AP)
#define BLUFI_TYPE_IS_CTRL_DISCONN_WIFI(type)        (BLUFI_TYPE_IS_CTRL((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_CTRL_SUBTYPE_DISCONN_FROM_AP)
#define BLUFI_TYPE_IS_CTRL_GET_WIFI_STATUS(type)     (BLUFI_TYPE_IS_CTRL((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_CTRL_SUBTYPE_GET_WIFI_STATUS)
#define BLUFI_TYPE_IS_CTRL_DEAUTHENTICATE_STA(type)         (BLUFI_TYPE_IS_CTRL((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_CTRL_SUBTYPE_DEAUTHENTICATE_STA)

#define BLUFI_TYPE_IS_DATA_NEG(type)                 (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_NEG)
#define BLUFI_TYPE_IS_DATA_STA_BSSID(type)           (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_STA_BSSID)
#define BLUFI_TYPE_IS_DATA_STA_SSID(type)            (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_STA_SSID)
#define BLUFI_TYPE_IS_DATA_STA_PASSWD(type)          (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_STA_PASSWD)
#define BLUFI_TYPE_IS_DATA_SOFTAP_SSID(type)         (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_SSID)
#define BLUFI_TYPE_IS_DATA_SOFTAP_PASSWD(type)       (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_PASSWD)
#define BLUFI_TYPE_IS_DATA_SOFTAP_MAX_CONN_NUM(type) (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_MAX_CONN_NUM)
#define BLUFI_TYPE_IS_DATA_SOFTAP_AUTH_MODE(type)    (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_AUTH_MODE)
#define BLUFI_TYPE_IS_DATA_SOFTAP_CHANNEL(type)      (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_CHANNEL)
#define BLUFI_TYPE_IS_DATA_USERNAME(type)            (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_USERNAME)
#define BLUFI_TYPE_IS_DATA_CA(type)                  (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_CA)
#define BLUFI_TYPE_IS_DATA_CLEINT_CERT(type)         (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_CLIENT_CERT)
#define BLUFI_TYPE_IS_DATA_SERVER_CERT(type)         (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_SERVER_CERT)
#define BLUFI_TYPE_IS_DATA_CLIENT_PRIV_KEY(type)     (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_CLIENT_PRIV_KEY)
#define BLUFI_TYPE_IS_DATA_SERVER_PRIV_KEY(type)     (BLUFI_TYPE_IS_DATA((type)) && BLUFI_GET_SUBTYPE((type)) == BLUFI_TYPE_DATA_SUBTYPE_SERVER_PRIV_KEY)

// packet frame control
#define BLUFI_FC_ENC_MASK       0x01
#define BLUFI_FC_CHECK_MASK     0x02
#define BLUFI_FC_DIR_MASK       0x04
#define BLUFI_FC_REQ_ACK_MASK   0x08
#define BLUFI_FC_FRAG_MASK      0x10

#define BLUFI_FC_ENC            0x01
#define BLUFI_FC_CHECK          0x02
#define BLUFI_FC_DIR_P2E        0x00
#define BLUFI_FC_DIR_E2P        0x04
#define BLUFI_FC_REQ_ACK        0x08
#define BLUFI_FC_FRAG           0x10

#define BLUFI_FC_IS_ENC(fc)       ((fc) & BLUFI_FC_ENC_MASK) 
#define BLUFI_FC_IS_CHECK(fc)     ((fc) & BLUFI_FC_CHECK_MASK) 
#define BLUFI_FC_IS_DIR_P2E(fc)   ((fc) & BLUFI_FC_DIR_P2E_MASK) 
#define BLUFI_FC_IS_DIR_E2P(fc)   (!((fc) & BLUFI_DIR_P2E_MASK)) 
#define BLUFI_FC_IS_REQ_ACK(fc)   ((fc) & BLUFI_FC_REQ_ACK_MASK) 
#define BLUFI_FC_IS_FRAG(fc)      ((fc) & BLUFI_FC_FRAG_MASK) 

#define BLUFI_FRAG_DATA_MAX_LEN  50

//function declare
void btc_blufi_protocol_handler(uint8_t type, uint8_t *data, int len);

void btc_blufi_send_encap(uint8_t type, uint8_t *data, int data_len);

void btc_blufi_set_callbacks(esp_blufi_callbacks_t *callbacks);

void btc_blufi_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

void btc_blufi_cb_deep_free(btc_msg_t *msg);

#endif /* __BLUFI_INT_H__ */
