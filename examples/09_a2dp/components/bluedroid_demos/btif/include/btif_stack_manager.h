#ifndef __BTIF_STACK_MANAGER_H__
#define __BTIF_STACK_MANAGER_H__

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

bt_status_t BTIF_InitStack(void);

bt_status_t BTIF_EnableStack(void);

bt_status_t BTIF_DisableStack(void);

bt_status_t BTIF_CleanUpStack(void);

#endif /* __BTIF_STACK_MANAGER_H__ */
