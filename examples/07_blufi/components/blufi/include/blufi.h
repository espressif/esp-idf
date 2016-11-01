#ifndef __BT_APP_COMMON_H__
#define __BT_APP_COMMON_H__

#include <stdint.h>
#include "osi.h"
#include "bt_common_types.h"

enum BLUFI_SIG {
	BLUFI_SIG_SWITCH_CONTEXT = 0,
	BLUFI_SIG_ENABLE,
	BLUFI_SIG_DISABLE,
};

void blufi_init(void);
bt_status_t blufi_enable(void *arg);
bt_status_t blufi_transfer_context(BtTaskCb_t cb, void *arg);

#endif /* __BT_APP_COMMON_H__ */
