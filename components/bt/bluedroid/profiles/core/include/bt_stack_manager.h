#ifndef __BT_STACK_MANAGER_H__
#define __BT_STACK_MANAGER_H__

#include "bt_defs.h"

/** Bluetooth Adapter State */
typedef enum {
    BT_STATE_OFF,
    BT_STATE_ON
}   bt_state_t;

/** Bluetooth Interface callbacks */

/** Bluetooth Enable/Disable Callback. */
typedef void (*adapter_state_changed_callback)(bt_state_t state);


/** Bluetooth Device callback structure. */
typedef struct {
    adapter_state_changed_callback adapter_state_changed_cb;
} bt_callbacks_t;

bt_status_t API_BTDM_InitStack(bt_callbacks_t *cb);

bt_status_t API_BTDM_EnableStack(void);

bt_status_t API_BTDM_DisableStack(void);

bt_status_t API_BTDM_CleanUpStack(void);

#endif /* __BT_STACK_MANAGER_H__ */
