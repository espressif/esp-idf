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

#ifndef __ESP_SPP_API_H__
#define __ESP_SPP_API_H__

#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_SPP_SUCCESS   = 0,          /*!< Successful operation. */
    ESP_SPP_FAILURE,                /*!< Generic failure. */
    ESP_SPP_BUSY,                   /*!< Temporarily can not handle this request. */
    ESP_SPP_NO_DATA,                /*!< no data. */
    ESP_SPP_NO_RESOURCE             /*!< No more set pm control block */
} esp_spp_status_t;

/* Security Setting Mask */
#define ESP_SPP_SEC_NONE            0x0000    /*!< No security. relate to BTA_SEC_NONE in bta/bta_api.h */
#define ESP_SPP_SEC_AUTHORIZE       0x0001    /*!< Authorization required (only needed for out going connection ) relate to BTA_SEC_AUTHORIZE in bta/bta_api.h*/
#define ESP_SPP_SEC_AUTHENTICATE    0x0012    /*!< Authentication required.  relate to BTA_SEC_AUTHENTICATE in bta/bta_api.h*/
#define ESP_SPP_SEC_ENCRYPT         0x0024    /*!< Encryption required.  relate to BTA_SEC_ENCRYPT in bta/bta_api.h*/
#define ESP_SPP_SEC_MODE4_LEVEL4    0x0040    /*!< Mode 4 level 4 service, i.e. incoming/outgoing MITM and P-256 encryption  relate to BTA_SEC_MODE4_LEVEL4 in bta/bta_api.h*/
#define ESP_SPP_SEC_MITM            0x3000    /*!< Man-In-The_Middle protection  relate to BTA_SEC_MITM in bta/bta_api.h*/
#define ESP_SPP_SEC_IN_16_DIGITS    0x4000    /*!< Min 16 digit for pin code  relate to BTA_SEC_IN_16_DIGITS in bta/bta_api.h*/
typedef uint16_t esp_spp_sec_t;

typedef enum {
    ESP_SPP_ROLE_MASTER     = 0,          /*!< Role: master */
    ESP_SPP_ROLE_SLAVE      = 1,          /*!< Role: slave */
} esp_spp_role_t;

typedef enum {
    ESP_SPP_MODE_CB         = 0,          /*!< When data is coming, a callback will come with data */
    ESP_SPP_MODE_VFS        = 1,          /*!< Use VFS to write/read data */
} esp_spp_mode_t;

#define ESP_SPP_MAX_MTU                 (3*330)     /*!< SPP max MTU */
#define ESP_SPP_MAX_SCN                 31          /*!< SPP max SCN */
/**
 * @brief SPP callback function events
 */
typedef enum {
    ESP_SPP_INIT_EVT                    = 0,                /*!< When SPP is inited, the event comes */
    ESP_SPP_DISCOVERY_COMP_EVT          = 8,                /*!< When SDP discovery complete, the event comes */
    ESP_SPP_OPEN_EVT                    = 26,               /*!< When SPP Client connection open, the event comes */
    ESP_SPP_CLOSE_EVT                   = 27,               /*!< When SPP connection closed, the event comes */
    ESP_SPP_START_EVT                   = 28,               /*!< When SPP server started, the event comes */
    ESP_SPP_CL_INIT_EVT                 = 29,               /*!< When SPP client initiated a connection, the event comes */
    ESP_SPP_DATA_IND_EVT                = 30,               /*!< When SPP connection received data, the event comes, only for ESP_SPP_MODE_CB */
    ESP_SPP_CONG_EVT                    = 31,               /*!< When SPP connection congestion status changed, the event comes, only for ESP_SPP_MODE_CB */
    ESP_SPP_WRITE_EVT                   = 33,               /*!< When SPP write operation completes, the event comes, only for ESP_SPP_MODE_CB */
    ESP_SPP_SRV_OPEN_EVT                = 34,               /*!< When SPP Server connection open, the event comes */
} esp_spp_cb_event_t;


/**
 * @brief SPP callback parameters union
 */
typedef union {
    /**
     * @brief SPP_INIT_EVT
     */
    struct spp_init_evt_param {
        esp_spp_status_t    status;         /*!< status */
    } init;                                 /*!< SPP callback param of SPP_INIT_EVT */

    /**
     * @brief SPP_DISCOVERY_COMP_EVT
     */
    struct spp_discovery_comp_evt_param {
        esp_spp_status_t    status;         /*!< status */
        uint8_t             scn_num;        /*!< The num of scn_num */
        uint8_t             scn[ESP_SPP_MAX_SCN];    /*!< channel # */
    } disc_comp;                            /*!< SPP callback param of SPP_DISCOVERY_COMP_EVT */

    /**
     * @brief ESP_SPP_OPEN_EVT
     */
    struct spp_open_evt_param {
        esp_spp_status_t    status;         /*!< status */
        uint32_t            handle;         /*!< The connection handle */
        int                 fd;             /*!< The file descriptor only for ESP_SPP_MODE_VFS */
        esp_bd_addr_t       rem_bda;        /*!< The peer address */
    } open;                                 /*!< SPP callback param of ESP_SPP_OPEN_EVT */

    /**
     * @brief ESP_SPP_SRV_OPEN_EVT
     */
    struct spp_srv_open_evt_param {
        esp_spp_status_t    status;         /*!< status */
        uint32_t            handle;         /*!< The connection handle */
        uint32_t            new_listen_handle;  /*!< The new listen handle */
        int                 fd;             /*!< The file descriptor only for ESP_SPP_MODE_VFS */
        esp_bd_addr_t       rem_bda;        /*!< The peer address */
    } srv_open;                             /*!< SPP callback param of ESP_SPP_SRV_OPEN_EVT */
    /**
     * @brief ESP_SPP_CLOSE_EVT
     */
    struct spp_close_evt_param {
        esp_spp_status_t    status;         /*!< status */
        uint32_t            port_status;    /*!< PORT status */
        uint32_t            handle;         /*!< The connection handle */
        bool                async;          /*!< FALSE, if local initiates disconnect */
    } close;                                /*!< SPP callback param of ESP_SPP_CLOSE_EVT */

    /**
     * @brief ESP_SPP_START_EVT
     */
    struct spp_start_evt_param {
        esp_spp_status_t    status;         /*!< status */
        uint32_t            handle;         /*!< The connection handle */
        uint8_t             sec_id;         /*!< security ID used by this server */
        bool                use_co;         /*!< TRUE to use co_rfc_data */
    } start;                                /*!< SPP callback param of ESP_SPP_START_EVT */
    /**
     * @brief ESP_SPP_CL_INIT_EVT
     */
    struct spp_cl_init_evt_param {
        esp_spp_status_t    status;         /*!< status */
        uint32_t            handle;         /*!< The connection handle */
        uint8_t             sec_id;         /*!< security ID used by this server */
        bool                use_co;         /*!< TRUE to use co_rfc_data */
    } cl_init;                              /*!< SPP callback param of ESP_SPP_CL_INIT_EVT */

    /**
     * @brief ESP_SPP_WRITE_EVT
     */
    struct spp_write_evt_param {
        esp_spp_status_t    status;         /*!< status */
        uint32_t            handle;         /*!< The connection handle */
        int                 len;            /*!< The length of the data written. */
        bool                cong;           /*!< congestion status */
    } write;                                /*!< SPP callback param of ESP_SPP_WRITE_EVT */

    /**
     * @brief ESP_SPP_DATA_IND_EVT
     */
    struct spp_data_ind_evt_param {
        esp_spp_status_t    status;         /*!< status */
        uint32_t            handle;         /*!< The connection handle */
        uint16_t            len;            /*!< The length of data */
        uint8_t             *data;          /*!< The data received */
    } data_ind;                             /*!< SPP callback param of ESP_SPP_DATA_IND_EVT */

    /**
     * @brief ESP_SPP_CONG_EVT
     */
    struct spp_cong_evt_param {
        esp_spp_status_t    status;         /*!< status */
        uint32_t            handle;         /*!< The connection handle */
        bool                cong;           /*!< TRUE, congested. FALSE, uncongested */
    } cong;                                 /*!< SPP callback param of ESP_SPP_CONG_EVT */
} esp_spp_cb_param_t;                       /*!< SPP callback parameter union type */

/**
 * @brief       SPP callback function type
 * @param       event:      Event type
 * @param       param:      Point to callback parameter, currently is union type
 */
typedef void (esp_spp_cb_t)(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

/**
 * @brief       This function is called to init callbacks
 *              with SPP module.
 *
 * @param[in]   callback:   pointer to the init callback function.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_spp_register_callback(esp_spp_cb_t callback);

/**
 * @brief       This function is called to init SPP.
 *
 * @param[in]   mode: Choose the mode of SPP, ESP_SPP_MODE_CB or ESP_SPP_MODE_VFS.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_spp_init(esp_spp_mode_t mode);

/**
 * @brief       This function is called to uninit SPP.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_spp_deinit();


/**
 * @brief       This function is called to performs service discovery for
 *              the services provided by the given peer device. When the
 *              operation is complete the callback function will be called
 *              with a ESP_SPP_DISCOVERY_COMP_EVT.
 *
 * @param[in]   bd_addr:   Remote device bluetooth device address.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_spp_start_discovery(esp_bd_addr_t bd_addr);

/**
 * @brief       This function makes an SPP connection to a remote BD Address.
 *              When the connection is initiated or failed to initiate,
 *              the callback is called with ESP_SPP_CL_INIT_EVT.
 *              When the connection is established or failed,
 *              the callback is called with ESP_SPP_OPEN_EVT.
 *
 * @param[in]   sec_mask:     Security Setting Mask .
 * @param[in]   role:         Master or slave.
 * @param[in]   remote_scn:   Remote device bluetooth device SCN.
 * @param[in]   peer_bd_addr: Remote device bluetooth device address.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_spp_connect(esp_spp_sec_t sec_mask,
                          esp_spp_role_t role, uint8_t remote_scn, esp_bd_addr_t peer_bd_addr);

/**
 * @brief       This function closes an SPP connection.
 *
 * @param[in]   handle:    The connection handle.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_spp_disconnect(uint32_t handle);

/**
 * @brief       This function create a SPP server and starts listening for an
 *              SPP connection request from a remote Bluetooth device.
 *              When the server is started successfully, the callback is called
 *              with ESP_SPP_START_EVT.
 *              When the connection is established, the callback is called
 *              with ESP_SPP_SRV_OPEN_EVT.
 *
 * @param[in]   sec_mask:     Security Setting Mask .
 * @param[in]   role:         Master or slave.
 * @param[in]   local_scn:    The specific channel you want to get.
 *                            If channel is 0, means get any channel.
 * @param[in]   name:         Server's name.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_spp_start_srv(esp_spp_sec_t sec_mask,
                            esp_spp_role_t role, uint8_t local_scn, const char *name);


/**
 * @brief       This function is used to write data, only for ESP_SPP_MODE_CB.
 *
 * @param[in]   handle: The connection handle.
 * @param[in]   len:    The length of the data written.
 * @param[in]   p_data: The data written.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_spp_write(uint32_t handle, int len, uint8_t *p_data);


/**
 * @brief       This function is used to register VFS.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_spp_vfs_register(void);

#ifdef __cplusplus
}
#endif

#endif ///__ESP_SPP_API_H__
