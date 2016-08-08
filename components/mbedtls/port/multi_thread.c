#include "multi_thread.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static xSemaphoreHandle multi_thread_mutex[MUTEX_MAX_NUM];
static int multi_thread_sig[MUTEX_MAX_NUM];

int multi_thread_init(void)
{
	int i;

	for (i = 0; i < MUTEX_MAX_NUM; i++) {
		multi_thread_mutex[i] = xSemaphoreCreateMutex();
		if (!multi_thread_mutex[i]) {
			goto failed1;
		}
		multi_thread_sig[i] = 0;
	}

	return 0;

failed1:
	for (i--; i >= 0; i--)
		vQueueDelete(multi_thread_mutex[i]);

	return -1;
}

void multi_thread_lock(unsigned int num)
{
	xSemaphoreTake(multi_thread_mutex[num], portMAX_DELAY);
}

void multi_thread_unlock(unsigned int num)
{
	xSemaphoreGive(multi_thread_mutex[num]);
}

void multi_thread_take(unsigned int num)
{
	multi_thread_sig[num]++;
}

void multi_thread_give(unsigned int num)
{
	multi_thread_sig[num]--;
}

bool multi_thread_is_used(num)
{
	return (multi_thread_sig[num] != 0) ? true : false;
}

