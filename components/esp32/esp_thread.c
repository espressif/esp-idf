#include "esp_thread.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static xSemaphoreHandle esp_thread_mutex[MUTEX_MAX_NUM];
static int esp_thread_sig[MUTEX_MAX_NUM];

int esp_thread_init(void)
{
	int i;

	for (i = 0; i < MUTEX_MAX_NUM; i++) {
		esp_thread_mutex[i] = xSemaphoreCreateMutex();
		if (!esp_thread_mutex[i]) {
			goto failed1;
		}
		esp_thread_sig[i] = 0;
	}

	return 0;

failed1:
	for (i--; i >= 0; i--)
		vQueueDelete(esp_thread_mutex[i]);

	return -1;
}

void esp_thread_lock(unsigned int num)
{
	xSemaphoreTake(esp_thread_mutex[num], portMAX_DELAY);
}

void esp_thread_unlock(unsigned int num)
{
	xSemaphoreGive(esp_thread_mutex[num]);
}

void esp_thread_take(unsigned int num)
{
	esp_thread_sig[num]++;
}

void esp_thread_give(unsigned int num)
{
	esp_thread_sig[num]--;
}

bool esp_thread_is_used(unsigned int num)
{
	return (esp_thread_sig[num] != 0) ? true : false;
}

