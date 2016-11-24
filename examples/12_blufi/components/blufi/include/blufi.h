#ifndef __BT_APP_COMMON_H__
#define __BT_APP_COMMON_H__

#include <stdint.h>
#include "osi.h"
#include "bt_common_types.h"
#include "esp_err.h"

enum BLUFI_SIG {
	BLUFI_SIG_SWITCH_CONTEXT = 0,
	BLUFI_SIG_ENABLE,
	BLUFI_SIG_DISABLE,
};

typedef esp_err_t (*blufi_task_cb_t)(void *arg);

void blufi_init(void);
void blufi_deinit(void);

esp_err_t blufi_enable(void *arg);
esp_err_t blufi_disable(void *arg);

esp_err_t blufi_transfer_context(blufi_task_cb_t cb, void *arg);

#endif /* __BT_APP_COMMON_H__ */
