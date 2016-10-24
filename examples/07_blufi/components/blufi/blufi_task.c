#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "gki.h"
#include "bt_defs.h"
#include "bt_trace.h"
#include "bt_types.h"
#include "allocator.h"

#include "bta_api.h"
#include "bta_gatt_api.h"

#include "controller.h"

#include "hash_map.h"
#include "hash_functions.h"
#include "alarm.h"
#include "thread.h"

#include "blufi.h"
#include "blufi_adv.h"

xQueueHandle xBlufiTaskQueue;
xTaskHandle xBlufiTaskHandle;

extern void ble_server_test(void);

static void blufi_task(void *arg)
{
    BtTaskEvt_t *e;

	for (;;) {
		if (pdTRUE == xQueueReceive(xBlufiTaskQueue, &e, (portTickType)portMAX_DELAY)) {
			switch (e->sig) {
				case BLUFI_SIG_SWITCH_CONTEXT:
					if (e->cb) {
						((BtTaskCb_t)e->cb)(e->arg);
					}
					break;
				default:
					break;
			}
		    osi_free(e);
		}
    }
}

static int blufi_task_post(uint32_t sig, void *par, void *cb, void *arg)
{

     BtTaskEvt_t *evt = (BtTaskEvt_t *)osi_malloc(sizeof(BtTaskEvt_t));
     if (evt == NULL)
        return -1;

     evt->sig = sig;
     evt->par = par;
     evt->cb = cb;
     evt->arg = arg;

     if (xQueueSend(xBlufiTaskQueue, &evt, 10/portTICK_RATE_MS) != pdTRUE) {
         LOG_ERROR("Blufi Post failed\n");
         return -1;
     }

	return 0;
}

BtStatus_t blufi_transfer_context(BtTaskCb_t cb, void *arg)
{
    LOG_ERROR("%s cb %08x, arg %u\n", __func__, cb, arg);

    return blufi_task_post(BLUFI_SIG_SWITCH_CONTEXT, 0, cb, arg);
}

static void blufi_task_deinit(void)
{
    vTaskDelete(xBlufiTaskHandle);
    vQueueDelete(xBlufiTaskQueue);
}


static void blufi_task_init(void)
{
    xBlufiTaskQueue = xQueueCreate(10, sizeof(void *));
    xTaskCreate(blufi_task, "BlUFI", 8192, NULL, configMAX_PRIORITIES - 3, xBlufiTaskHandle);
}

void blufi_init(void) {
	blufi_task_init();
	blufi_transfer_context(blufi_enable, NULL);
}

