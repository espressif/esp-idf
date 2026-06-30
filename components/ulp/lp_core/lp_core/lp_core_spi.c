/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"

#if SOC_LP_SPI_SUPPORTED

#include <stdint.h>
#include <string.h>
#include "esp_err.h"
#include "ulp_lp_core_spi.h"
#include "ulp_lp_core_utils.h"
#include "hal/lp_spi_ll.h"

static lp_spi_ll_dev_t *lp_spi_dev = LP_SPI_LL_GET_HW();

/* Tracks an outstanding lp_core_lp_spi_slave_arm() that has not yet been
 * paired with a slave_wait(). The LP_SPI_CMD.reg_usr bit is not a reliable
 * "busy" indicator in slave mode (the slave holds it set while merely armed
 * and waiting for the master's SCK), so we serialise arm/wait in software.
 */
static volatile bool s_slave_armed = false;

/* LP SPI data buffer is W0..W15 (16 x 32-bit = 64 B). Per TRM, transfers
 * beyond 64 B repeatedly fetch from W15[31:24], so byte 63 is replayed for
 * every byte past 64. Skipping W15 (cap at 60 B / W0..W14) avoids that
 * aliasing region entirely; longer transfers are split into back-to-back
 * 60 B hardware transactions.
 */
#define LP_SPI_MAX_DATA_REG_NUM    ((LP_SPI_LL_MAX_BUFFER_SIZE / 4) - 1)   /* 15 */
#define LP_SPI_CHUNK_BYTES         (LP_SPI_MAX_DATA_REG_NUM * 4)            /* 60 */

static inline esp_err_t lp_core_spi_wait_for_interrupt(int32_t cycles_to_wait)
{
    uint32_t timeout_start = ulp_lp_core_get_cpu_cycles();
    while (!lp_spi_ll_get_int_trans_done(lp_spi_dev)) {
        if (ulp_lp_core_is_timeout_elapsed(timeout_start, cycles_to_wait)) {
            lp_spi_ll_clear_int_trans_done(lp_spi_dev);
            return ESP_ERR_TIMEOUT;
        }
    }

    return ESP_OK;
}

//////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Public APIs ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

esp_err_t lp_core_lp_spi_master_transfer(lp_spi_transaction_t *trans_desc, int32_t cycles_to_wait)
{
    esp_err_t ret = ESP_OK;

    /* Require at least one of tx_buffer/rx_buffer; length must be 0 when its
     * buffer is NULL.
     */
    if (trans_desc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (trans_desc->tx_buffer != NULL && trans_desc->tx_length == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (trans_desc->rx_buffer != NULL && trans_desc->rx_length == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    /* The peripheral has a single shared bit-length register
     * (LP_SPI_MS_DLEN.reg_ms_data_bitlen), so we program it for
     * max(tx_length, rx_length) bytes to avoid truncating the longer side.
     */
    uint32_t tx_total = trans_desc->tx_buffer ? trans_desc->tx_length : 0;
    uint32_t rx_total = trans_desc->rx_buffer ? trans_desc->rx_length : 0;
    uint32_t bus_total = tx_total > rx_total ? tx_total : rx_total;
    if (bus_total == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Make sure that we do not have any ongoing transactions */
    if (lp_spi_ll_is_busy(lp_spi_dev)) {
        return ESP_ERR_INVALID_STATE;
    }

    lp_spi_ll_clear_int_trans_done(lp_spi_dev);

    /* Configure dummy bits */
    lp_spi_ll_set_dummy_en(lp_spi_dev, trans_desc->dummy_bits != 0);
    if (trans_desc->dummy_bits) {
        lp_spi_ll_set_dummy_cyclelen(lp_spi_dev, trans_desc->dummy_bits);
    }

    /* Configure the command and command bit length */
    lp_spi_ll_set_command_en(lp_spi_dev, trans_desc->command_bits != 0);
    if (trans_desc->command_bits) {
        lp_spi_ll_set_command_bitlen(lp_spi_dev, trans_desc->command_bits);
        lp_spi_ll_set_command_value(lp_spi_dev,
                                    lp_spi_ll_get_wr_bit_order(lp_spi_dev)
                                    ? trans_desc->command
                                    : __builtin_bswap32(trans_desc->command << (32 - trans_desc->command_bits)));
    }

    /* Configure the address and address bit length */
    lp_spi_ll_set_address_en(lp_spi_dev, trans_desc->address_bits != 0);
    if (trans_desc->address_bits) {
        lp_spi_ll_set_address_bitlen(lp_spi_dev, trans_desc->address_bits - 1);
        lp_spi_ll_set_address_value(lp_spi_dev,
                                    lp_spi_ll_get_wr_bit_order(lp_spi_dev)
                                    ? __builtin_bswap32(trans_desc->address)
                                    : trans_desc->address << (32 - trans_desc->address_bits));
    }

    /* MOSI gated by tx_buffer to avoid clocking stale W0..W15 on read-only. */
    lp_spi_ll_set_mosi_en(lp_spi_dev, trans_desc->tx_buffer != NULL);
    lp_spi_ll_set_miso_en(lp_spi_dev, trans_desc->rx_buffer != NULL);

    /* Drive the bus one hardware transaction at a time. Each iteration:
     *   1. clip ``chunk`` to the remaining bytes, capped at LP_SPI_CHUNK_BYTES;
     *   2. preload ``tx_chunk`` TX bytes into W0.. (only if the caller still
     *      has TX bytes left for this chunk -- TX may end before RX);
     *   3. program the shared bit-length register for ``chunk * 8`` SCKs;
     *   4. AFIFO reset + apply config + kick (ordering as per the TRM).
     *   5. block on TRANS_DONE.
     *   6. drain ``rx_chunk`` RX bytes from W0.. (only if the caller still
     *      wants RX bytes for this chunk -- RX may end before TX).
     */
    uint32_t bus_done = 0;
    while (bus_done < bus_total) {
        uint32_t chunk = bus_total - bus_done;
        if (chunk > LP_SPI_CHUNK_BYTES) {
            chunk = LP_SPI_CHUNK_BYTES;
        }

        if (trans_desc->tx_buffer != NULL && bus_done < tx_total) {
            uint32_t tx_chunk = tx_total - bus_done;
            if (tx_chunk > chunk) {
                tx_chunk = chunk;
            }
            lp_spi_ll_write_buffer_bytes(lp_spi_dev,
                                         (const uint8_t *)trans_desc->tx_buffer + bus_done,
                                         tx_chunk);
        }

        lp_spi_ll_set_data_bitlen(lp_spi_dev, chunk * 8 - 1);

        lp_spi_ll_reset_fifos(lp_spi_dev);

        lp_spi_ll_apply_config(lp_spi_dev);
        lp_spi_ll_start_user_transaction(lp_spi_dev);

        ret = lp_core_spi_wait_for_interrupt(cycles_to_wait);
        if (ret != ESP_OK) {
            return ret;
        }

        if (trans_desc->rx_buffer != NULL && bus_done < rx_total) {
            uint32_t rx_chunk = rx_total - bus_done;
            if (rx_chunk > chunk) {
                rx_chunk = chunk;
            }
            lp_spi_ll_read_buffer_bytes(lp_spi_dev,
                                        (uint8_t *)trans_desc->rx_buffer + bus_done,
                                        rx_chunk);
        }

        lp_spi_ll_clear_int_trans_done(lp_spi_dev);

        bus_done += chunk;
    }

    return ret;
}

/* Arm = preload TX + start user phase, return immediately. Pair with
 * lp_core_lp_spi_slave_wait(). Splitting arm/wait lets the caller signal
 * the master only after the slave is actually listening for SCK.
 */
esp_err_t lp_core_lp_spi_slave_arm(lp_spi_transaction_t *trans_desc)
{
    /* Require at least one of tx_buffer/rx_buffer; length must be 0 when its
     * buffer is NULL.
     */
    if (trans_desc == NULL ||
            (trans_desc->rx_buffer == NULL && trans_desc->tx_buffer == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (trans_desc->rx_buffer != NULL && trans_desc->rx_length == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (trans_desc->tx_buffer != NULL && trans_desc->tx_length == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Refuse to re-arm while a previous arm has not been waited on,
     * otherwise the preload below would clobber its W0..W15 mid-transfer.
     */
    if (s_slave_armed) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Clear stale TRANS_DONE so the paired wait sees only this arm. */
    lp_spi_ll_clear_int_trans_done(lp_spi_dev);

    /* Slave direction is reversed vs. master: MOSI carries master->slave
     * (caller RX), MISO carries slave->master (caller TX).
     */
    lp_spi_ll_set_mosi_en(lp_spi_dev, trans_desc->rx_buffer != NULL);
    lp_spi_ll_set_miso_en(lp_spi_dev, trans_desc->tx_buffer != NULL);

    /* Same single shared bit-length register as master
     * (LP_SPI_MS_DLEN.reg_ms_data_bitlen). The slave runs a single hardware
     * shot capped at LP_SPI_CHUNK_BYTES (60 B, W0..W14, W15 reserved per
     * TRM); longer transfers must be split by the caller into successive
     * arm/wait pairs.
     */
    uint32_t rx_total = trans_desc->rx_buffer ? trans_desc->rx_length : 0;
    uint32_t tx_total = trans_desc->tx_buffer ? trans_desc->tx_length : 0;
    uint32_t arm_bytes = rx_total > tx_total ? rx_total : tx_total;
    if (arm_bytes > LP_SPI_CHUNK_BYTES) {
        arm_bytes = LP_SPI_CHUNK_BYTES;
    }
    lp_spi_ll_set_data_bitlen(lp_spi_dev, arm_bytes * 8 - 1);

    /* Preload TX into W0.. for the slave to drive on MISO when the master
     * starts clocking.
     */
    if (trans_desc->tx_buffer != NULL) {
        uint32_t tx_preload = tx_total > LP_SPI_CHUNK_BYTES
                              ? LP_SPI_CHUNK_BYTES
                              : tx_total;
        lp_spi_ll_write_buffer_bytes(lp_spi_dev,
                                     (const uint8_t *)trans_desc->tx_buffer,
                                     tx_preload);
    }

    /* Reset AFIFOs after preload, before start. */
    lp_spi_ll_reset_fifos(lp_spi_dev);

    /* Skip apply_config() in slave mode: reg_update is master-only and
     * re-triggering it here was observed to clock out the previous
     * transaction's data.
     */
    lp_spi_ll_start_user_transaction(lp_spi_dev);

    s_slave_armed = true;
    return ESP_OK;
}

/* Block on TRANS_DONE from the matching arm, then drain whatever the master
 * actually clocked into W0..W15. Pair with lp_core_lp_spi_slave_arm().
 */
esp_err_t lp_core_lp_spi_slave_wait(lp_spi_transaction_t *trans_desc, int32_t cycles_to_wait)
{
    if (trans_desc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    /* Reject ``wait()`` without a preceding ``arm()`` -- otherwise we would
     * block on whatever stale TRANS_DONE happens to be latched.
     */
    if (!s_slave_armed) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Block until TRANS_DONE or timeout (cycles_to_wait is in LP CPU cycles). */
    esp_err_t ret = lp_core_spi_wait_for_interrupt(cycles_to_wait);
    if (ret != ESP_OK) {
        /* Clear the armed latch on the timeout path too so the caller can
         * recover by issuing a fresh ``arm()``; otherwise the next
         * ``arm()`` would return ESP_ERR_INVALID_STATE forever.
         * ``lp_core_spi_wait_for_interrupt()`` already cleared
         * TRANS_DONE on its timeout exit, so no extra latch clear here.
         */
        s_slave_armed = false;
        return ret;
    }

    s_slave_armed = false;

    /* Clear the latch so the next arm starts from a clean state. */
    lp_spi_ll_clear_int_trans_done(lp_spi_dev);

    /* The master, not the slave, drives SCK, so the actually-received length
     * is decided by the master and only known after TRANS_DONE. Query the
     * hardware bit counter (LP_SPI_SLAVE1.reg_slv_data_bitlen), clamp it
     * against the caller's rx_length, round up to whole bytes, then drain
     * that many bytes from W0.. into rx_buffer.
     */
    if (trans_desc->rx_buffer != NULL) {
        uint32_t rx_total = trans_desc->rx_length;
        uint32_t slave_bitlen = lp_spi_ll_get_slave_rcv_bitlen(lp_spi_dev);
        uint32_t req_bitlen = rx_total * 8;
        uint32_t valid_bitlen = slave_bitlen > req_bitlen ? req_bitlen : slave_bitlen;
        uint32_t valid_bytes = (valid_bitlen + 7) / 8;
        if (valid_bytes > rx_total) {
            valid_bytes = rx_total;
        }
        if (valid_bytes > 0) {
            lp_spi_ll_read_buffer_bytes(lp_spi_dev,
                                        (uint8_t *)trans_desc->rx_buffer,
                                        valid_bytes);
        }
    }

    return ESP_OK;
}

esp_err_t lp_core_lp_spi_slave_transfer(lp_spi_transaction_t *trans_desc, int32_t cycles_to_wait)
{
    esp_err_t ret = lp_core_lp_spi_slave_arm(trans_desc);
    if (ret != ESP_OK) {
        return ret;
    }
    return lp_core_lp_spi_slave_wait(trans_desc, cycles_to_wait);
}

#endif /* SOC_LP_SPI_SUPPORTED */
