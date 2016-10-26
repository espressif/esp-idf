#ifndef __ESP_CROSSCORE_INT_H
#define __ESP_CROSSCORE_INT_H


void esp_crosscore_int_init();
void esp_crosscore_int_send_yield(int coreId);

#endif