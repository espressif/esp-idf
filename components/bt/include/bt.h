#ifndef __BT_H__
#define __BT_H__

#include "freertos/FreeRTOS.h"
#include "esp_err.h"

#define BT_TASK_PRIO_MAX (configMAX_PRIORITIES)
#define BT_TASK_PRIO_MIN (0)

/* bt init */
#define BT_INIT_TASK_PRIO	(BT_TASK_PRIO_MAX-1)
#define BT_INIT_TASK_STACK_SIZE	(2048)
/* controller */
#define BT_CONTROLLER_TASK_PRIO	(BT_TASK_PRIO_MAX-3)
#define BT_CONTROLLER_TASK_STACK_SIZE	(4096)

typedef void (* bt_app_startup_cb_t)(void *param);

esp_err_t esp_bt_startup(bt_app_startup_cb_t cb, void *ctx);

typedef struct vhci_host_callback {
    void (*notify_host_send_available)(void);
    int (*notify_host_recv)(uint8_t *data, uint16_t len);
} vhci_host_callback_t;

extern bool API_vhci_host_check_send_available(void);
extern void API_vhci_host_send_packet(uint8_t *data, uint16_t len);
extern void API_vhci_host_register_callback(const vhci_host_callback_t *callback);

#endif /* __BT_H__ */
