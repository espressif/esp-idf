// Copyright 2010-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef _DRIVER_SPI_MASTER_INTERNAL_H_
#define _DRIVER_SPI_MASTER_INTERNAL_H_

#include "driver/spi_master.h"


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct spi_device_t spi_device_t;

#define NO_CS 3     //Number of CS pins per SPI host


/// struct to hold private transaction data (like tx and rx buffer for DMA).
typedef struct {        
    spi_transaction_t   *trans; 
    uint32_t *buffer_to_send;   //equals to tx_data, if SPI_TRANS_USE_RXDATA is applied; otherwise if original buffer wasn't in DMA-capable memory, this gets the address of a temporary buffer that is;
                                //otherwise sets to the original buffer or NULL if no buffer is assigned.
    uint32_t *buffer_to_rcv;    // similar to buffer_to_send
} spi_trans_priv;

typedef struct {
    spi_device_t *device[NO_CS];
    intr_handle_t intr;
    spi_dev_t *hw;
    spi_trans_priv cur_trans_buf;
    int cur_cs;
    lldesc_t *dmadesc_tx;
    lldesc_t *dmadesc_rx;
    bool no_gpio_matrix;
    int dma_chan;
    int max_transfer_sz;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
} spi_host_t;

struct spi_device_t {
    QueueHandle_t trans_queue;
    QueueHandle_t ret_queue;
    spi_device_interface_config_t cfg;
    spi_host_t *host;
};


/**
 * @brief Set the SPI clock to a certain frequency
 *
 * Set the SPI clock to a certain frequency. Returns the effective frequency set, which may be slightly
 * different from the requested frequency.
 * 
 * @param hw SPI host device structure pointer
 * @param fapb APB clock frequency
 * @param hz desired SPI clock frequency in HZ
 * @param duty_cycle SPI clock duty cycle
 * @return 
 *         - the effective frequency set
 */
int spi_set_clock(spi_dev_t *hw, int fapb, int hz, int duty_cycle);

#ifdef __cplusplus
}
#endif

#endif
