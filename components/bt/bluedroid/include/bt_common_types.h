



#ifndef _BT_COMMON_TYPES_H_
#define _BT_COMMON_TYPES_H_

#include "bt_defs.h"
#include "thread.h"


extern xQueueHandle xBtaApp1Queue;
extern xTaskHandle xBtaApp1TaskHandle;

typedef struct {
    uint8_t client_if;
    uint8_t filt_index;
    uint8_t advertiser_state;
    uint8_t advertiser_info_present;
    uint8_t addr_type;
    uint8_t tx_power;
    int8_t rssi_value;
    uint16_t time_stamp;
    bt_bdaddr_t bd_addr;
    uint8_t adv_pkt_len;
    uint8_t *p_adv_pkt_data;
    uint8_t scan_rsp_len;
    uint8_t *p_scan_rsp_data;
} btgatt_track_adv_info_t;

#endif
