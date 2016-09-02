#include "esp_crypto.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t esp_crypto_mutex[MUTEX_MAX_NUM];
static int esp_crypto_sig[MUTEX_MAX_NUM];

#if 0
#define ESP_DEBUG ets_printf
#else
#define ESP_DEBUG(...)
#endif

int esp_crypto_init(void)
{
    int i;

    for (i = 0; i < MUTEX_MAX_NUM; i++) {
        esp_crypto_mutex[i] = xSemaphoreCreateMutex();
        ESP_DEBUG("init num %d mutex %p\n", i, esp_crypto_mutex[i]);
        if (!esp_crypto_mutex[i]) {
            goto failed1;
        }
        esp_crypto_sig[i] = 0;
    }

    return 0;

failed1:
    ESP_DEBUG("esp_crypto_init failed\n");
    for (i--; i >= 0; i--) {
        vQueueDelete(esp_crypto_mutex[i]);
    }

    return -1;
}

void esp_crypto_lock(unsigned int num)
{
    ESP_DEBUG("1num %d, mutex %p\n", num, esp_crypto_mutex[num]);
    xSemaphoreTake(esp_crypto_mutex[num], portMAX_DELAY);
}

void esp_crypto_unlock(unsigned int num)
{
    ESP_DEBUG("2num %d, mutex %p\n", num, esp_crypto_mutex[num]);
    xSemaphoreGive(esp_crypto_mutex[num]);
}

void esp_crypto_take(unsigned int num)
{
    esp_crypto_sig[num]++;
}

void esp_crypto_give(unsigned int num)
{
    if (esp_crypto_sig[num]) {
        esp_crypto_sig[num]--;
    }
}

bool esp_crypto_is_used(unsigned int num)
{
    return (esp_crypto_sig[num] != 0) ? true : false;
}

