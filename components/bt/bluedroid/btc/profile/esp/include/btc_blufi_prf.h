#ifndef __BTC_BLUFI_PRF_H__
#define __BTC_BLUFI_PRF_H__

#include "bt_target.h"
#include "btc_task.h"
#include "esp_blufi_api.h"

typedef enum {
	BTC_BLUFI_ACT_INIT = 0,
	BTC_BLUFI_ACT_DEINIT,
	BTC_BLUFI_ACT_SEND_CFG_STATE,
} btc_blufi_act_t;

typedef enum {
	BTC_BLUFI_CB_ACT_INIT_FINISH = 0,
	BTC_BLUFI_CB_ACT_DEINIT_FINISH,
	BTC_BLUFI_CB_ACT_RECV_DATA,
} btc_blufi_cb_act_t;

typedef union {
#if 0
	//BTC_BLUFI_ACT_INIT = 0,
	struct blufi_init_param {
	} init;
	//BTC_BLUFI_ACT_DEINIT,
	struct blufi_deinit_param {
	} deinit;
#endif
	//BTC_BLUFI_ACT_SEND_CFG_STATE,
	struct blufi_send_cfg_state_pram {
		esp_blufi_config_state_t state;
	} cfg_state;
} btc_blufi_args_t;

void btc_blufi_cb_handler(btc_msg_t *msg);
void btc_blufi_call_handler(btc_msg_t *msg);

#endif /* __BTC_BLUFI_PRF_H__ */
