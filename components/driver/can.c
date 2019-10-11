// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_types.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "esp_pm.h"
#include "soc/can_periph.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "driver/can.h"

/* ---------------------------- Definitions --------------------------------- */
//Internal Macros
#define CAN_CHECK(cond, ret_val) ({                                         \
            if (!(cond)) {                                                  \
                return (ret_val);                                           \
            }                                                               \
})
#define CAN_CHECK_FROM_CRIT(cond, ret_val) ({                               \
            if (!(cond)) {                                                  \
                CAN_EXIT_CRITICAL();                                        \
                return ret_val;                                             \
            }                                                               \
})
#define CAN_SET_FLAG(var, mask)     ((var) |= (mask))
#define CAN_RESET_FLAG(var, mask)   ((var) &= ~(mask))
#define CAN_TAG "CAN"

//Driver default config/values
#define DRIVER_DEFAULT_EWL          96          //Default Error Warning Limit value
#define DRIVER_DEFAULT_TEC          0           //TX Error Counter starting value
#define DRIVER_DEFAULT_REC          0           //RX Error Counter starting value
#define DRIVER_DEFAULT_CLKOUT_DIV   14          //APB CLK divided by two
#define DRIVER_DEFAULT_INTERRUPTS   0xE7        //Exclude data overrun
#define DRIVER_DEFAULT_ERR_PASS_CNT 128         //Error counter threshold for error passive

//Command Bit Masks
#define CMD_TX_REQ                  0x01        //Transmission Request
#define CMD_ABORT_TX                0x02        //Abort Transmission
#define CMD_RELEASE_RX_BUFF         0x04        //Release Receive Buffer
#define CMD_CLR_DATA_OVRN           0x08        //Clear Data Overrun
#define CMD_SELF_RX_REQ             0x10        //Self Reception Request
#define CMD_TX_SINGLE_SHOT          0x03        //Single Shot Transmission
#define CMD_SELF_RX_SINGLE_SHOT     0x12        //Single Shot Self Reception

//Control flags
#define CTRL_FLAG_STOPPED           0x001       //CAN peripheral in stopped state
#define CTRL_FLAG_RECOVERING        0x002       //Bus is undergoing bus recovery
#define CTRL_FLAG_ERR_WARN          0x004       //TEC or REC is >= error warning limit
#define CTRL_FLAG_ERR_PASSIVE       0x008       //TEC or REC is >= 128
#define CTRL_FLAG_BUS_OFF           0x010       //Bus-off due to TEC >= 256
#define CTRL_FLAG_TX_BUFF_OCCUPIED  0x020       //Transmit buffer is occupied
#define CTRL_FLAG_SELF_TEST         0x040       //Configured to Self Test Mode
#define CTRL_FLAG_LISTEN_ONLY       0x080       //Configured to Listen Only Mode

//Constants use for frame formatting and parsing
#define FRAME_MAX_LEN               13          //EFF with 8 bytes of data
#define FRAME_MAX_DATA_LEN          8           //Max data bytes allowed in CAN2.0
#define FRAME_EXTD_ID_LEN           4           //EFF ID requires 4 bytes (29bit)
#define FRAME_STD_ID_LEN            2           //SFF ID requires 2 bytes (11bit)
#define FRAME_INFO_LEN              1           //Frame info requires 1 byte

#define ALERT_LOG_LEVEL_WARNING     CAN_ALERT_ARB_LOST  //Alerts above and including this level use ESP_LOGW
#define ALERT_LOG_LEVEL_ERROR       CAN_ALERT_TX_FAILED //Alerts above and including this level use ESP_LOGE

/* ------------------ Typedefs, structures, and variables ------------------- */

/* Formatted frame structure has identical layout as TX/RX buffer registers.
   This allows for direct copy to/from TX/RX buffer. The two reserved bits in TX
   buffer are used in the frame structure to store the self_reception and
   single_shot flags. */
typedef union {
    struct {
        struct {
            uint8_t dlc: 4;                         //Data length code (0 to 8) of the frame
            uint8_t self_reception: 1;              //This frame should be transmitted using self reception command
            uint8_t single_shot: 1;                 //This frame should be transmitted using single shot command
            uint8_t rtr: 1;                         //This frame is a remote transmission request
            uint8_t frame_format: 1;                //Format of the frame (1 = extended, 0 = standard)
        };
        union {
            struct {
                uint8_t id[FRAME_STD_ID_LEN];       //11 bit standard frame identifier
                uint8_t data[FRAME_MAX_DATA_LEN];   //Data bytes (0 to 8)
                uint8_t reserved8[2];
            } standard;
            struct {
                uint8_t id[FRAME_EXTD_ID_LEN];      //29 bit extended frame identifier
                uint8_t data[FRAME_MAX_DATA_LEN];   //Data bytes (0 to 8)
            } extended;
        };
    };
    uint8_t bytes[FRAME_MAX_LEN];
} can_frame_t;

//Control structure for CAN driver
typedef struct {
    //Control and status members
    uint32_t control_flags;
    uint32_t rx_missed_count;
    uint32_t tx_failed_count;
    uint32_t arb_lost_count;
    uint32_t bus_error_count;
    intr_handle_t isr_handle;
    //TX and RX
    QueueHandle_t tx_queue;
    QueueHandle_t rx_queue;
    int tx_msg_count;
    int rx_msg_count;
    //Alerts
    SemaphoreHandle_t alert_semphr;
    uint32_t alerts_enabled;
    uint32_t alerts_triggered;
#ifdef CONFIG_PM_ENABLE
    //Power Management
    esp_pm_lock_handle_t pm_lock;
#endif
} can_obj_t;

static can_obj_t *p_can_obj = NULL;
static portMUX_TYPE can_spinlock = portMUX_INITIALIZER_UNLOCKED;
#define CAN_ENTER_CRITICAL()  portENTER_CRITICAL(&can_spinlock)
#define CAN_EXIT_CRITICAL()   portEXIT_CRITICAL(&can_spinlock)

/* ------------------- Configuration Register Functions---------------------- */

static inline esp_err_t can_enter_reset_mode(void)
{
    /* Enter reset mode (required to write to configuration registers). Reset mode
       also prevents all CAN activity on the current module and is automatically
       set upon entering a BUS-OFF condition. */
    CAN.mode_reg.reset = 1;                                     //Set reset mode bit
    CAN_CHECK(CAN.mode_reg.reset == 1, ESP_ERR_INVALID_STATE);  //Check bit was set
    return ESP_OK;
}

static inline esp_err_t can_exit_reset_mode(void)
{
    /* Exiting reset mode will return the CAN module to operating mode. Reset mode
       must also be exited in order to trigger BUS-OFF recovery sequence. */
    CAN.mode_reg.reset = 0;                                     //Exit reset mode
    CAN_CHECK(CAN.mode_reg.reset == 0, ESP_ERR_INVALID_STATE);  //Check bit was reset
    return ESP_OK;
}

static inline void can_config_pelican(void)
{
    //Use PeliCAN address layout. Exposes extra registers
    CAN.clock_divider_reg.can_mode = 1;
}

static inline void can_config_mode(can_mode_t mode)
{
    //Configure CAN mode of operation
    can_mode_reg_t mode_reg;
    mode_reg.val = CAN.mode_reg.val;        //Get current value of mode register
    if (mode == CAN_MODE_NO_ACK) {
        mode_reg.self_test = 1;
        mode_reg.listen_only = 0;
    } else if (mode == CAN_MODE_LISTEN_ONLY) {
        mode_reg.self_test = 0;
        mode_reg.listen_only = 1;
    } else {
        //Default to normal operating mode
        mode_reg.self_test = 0;
        mode_reg.listen_only = 0;
    }
    CAN.mode_reg.val = mode_reg.val;        //Write back modified value to register
}

static inline void can_config_interrupts(uint32_t interrupts)
{
    //Enable interrupt sources
    CAN.interrupt_enable_reg.val = interrupts;
}

static inline void can_config_bus_timing(uint32_t brp, uint32_t sjw, uint32_t tseg_1, uint32_t tseg_2, bool triple_sampling)
{
    /* Configure bus/bit timing of CAN peripheral.
       - BRP (even from 2 to 128) divide APB to CAN system clock (T_scl)
       - SJW (1 to 4) is number of T_scl to shorten/lengthen for bit synchronization
       - TSEG_1 (1 to 16) is number of T_scl in a bit time before sample point
       - TSEG_2 (1 to 8) is number of T_scl in a bit time after sample point
       - triple_sampling will cause each bit time to be sampled 3 times*/
    can_bus_tim_0_reg_t timing_reg_0;
    can_bus_tim_1_reg_t timing_reg_1;
    timing_reg_0.baud_rate_prescaler = (brp / 2) - 1;
    timing_reg_0.sync_jump_width = sjw - 1;
    timing_reg_1.time_seg_1 = tseg_1 - 1;
    timing_reg_1.time_seg_2 = tseg_2 - 1;
    timing_reg_1.sampling = triple_sampling;
    CAN.bus_timing_0_reg.val = timing_reg_0.val;
    CAN.bus_timing_1_reg.val = timing_reg_1.val;
}

static inline void can_config_error(int err_warn_lim, int rx_err_cnt, int tx_err_cnt)
{
    /* Set error warning limit, RX error counter, and TX error counter. Note that
       forcibly setting RX/TX error counters will incur the expected status changes
       and interrupts as soon as reset mode exits. */
    if (err_warn_lim >= 0 && err_warn_lim <= UINT8_MAX) {
        //Defaults to 96 after hardware reset.
        CAN.error_warning_limit_reg.byte = err_warn_lim;
    }
    if (rx_err_cnt >= 0 && rx_err_cnt <= UINT8_MAX) {
        //Defaults to 0 after hardware reset.
        CAN.rx_error_counter_reg.byte = rx_err_cnt;
    }
    if (tx_err_cnt >= 0 && tx_err_cnt <= UINT8_MAX) {
        //Defaults to 0 after hardware reset, and 127 after BUS-OFF event
        CAN.tx_error_counter_reg.byte = tx_err_cnt;
    }
}

static inline void can_config_acceptance_filter(uint32_t code, uint32_t mask, bool single_filter)
{
    //Set filter mode
    CAN.mode_reg.acceptance_filter = (single_filter) ? 1 : 0;
    //Swap code and mask to match big endian registers
    uint32_t code_swapped = __builtin_bswap32(code);
    uint32_t mask_swapped = __builtin_bswap32(mask);
    for (int i = 0; i < 4; i++) {
        CAN.acceptance_filter.code_reg[i].byte = ((code_swapped >> (i * 8)) & 0xFF);
        CAN.acceptance_filter.mask_reg[i].byte = ((mask_swapped >> (i * 8)) & 0xFF);
    }
}

static inline void can_config_clk_out(uint32_t divider)
{
    /* Configure CLKOUT. CLKOUT is a pre-scaled version of APB CLK. Divider can be
       1, or any even number from 2 to 14. Set to out of range value (0) to disable
       CLKOUT. */
    can_clk_div_reg_t clock_divider_reg;
    clock_divider_reg.val = CAN.clock_divider_reg.val;
    if (divider >= 2 && divider <= 14) {
        clock_divider_reg.clock_off = 0;
        clock_divider_reg.clock_divider = (divider / 2) - 1;
    } else if (divider == 1) {
        clock_divider_reg.clock_off = 0;
        clock_divider_reg.clock_divider = 7;
    } else {
        clock_divider_reg.clock_off = 1;
        clock_divider_reg.clock_divider = 0;
    }
    CAN.clock_divider_reg.val = clock_divider_reg.val;
}

/* ---------------------- Runtime Register Functions------------------------- */

static inline void can_set_command(uint8_t commands)
{
    CAN.command_reg.val = commands;
}

static void can_set_tx_buffer_and_transmit(can_frame_t *frame)
{
    //Copy frame structure into TX buffer registers
    for (int i = 0; i < FRAME_MAX_LEN; i++) {
        CAN.tx_rx_buffer[i].val = frame->bytes[i];
    }

    //Set correct transmit command
    uint8_t command;
    if (frame->self_reception) {
        command = (frame->single_shot) ? CMD_SELF_RX_SINGLE_SHOT : CMD_SELF_RX_REQ;
    } else {
        command = (frame->single_shot) ? CMD_TX_SINGLE_SHOT : CMD_TX_REQ;
    }
    can_set_command(command);
}

static inline uint32_t can_get_status(void)
{
    return CAN.status_reg.val;
}

static inline uint32_t can_get_interrupt_reason(void)
{
    return CAN.interrupt_reg.val;
}

static inline uint32_t can_get_arbitration_lost_capture(void)
{
    return CAN.arbitration_lost_captue_reg.val;
    //Todo: ALC read only to re-arm arb lost interrupt. Add function to decode ALC
}

static inline uint32_t can_get_error_code_capture(void)
{
    return CAN.error_code_capture_reg.val;
    //Todo: ECC read only to re-arm bus error interrupt. Add function to decode ECC
}

static inline void can_get_error_counters(uint32_t *tx_error_cnt, uint32_t *rx_error_cnt)
{
    if (tx_error_cnt != NULL) {
        *tx_error_cnt = CAN.tx_error_counter_reg.byte;
    }
    if (rx_error_cnt != NULL) {
        *rx_error_cnt = CAN.rx_error_counter_reg.byte;
    }
}

static inline void can_get_rx_buffer_and_clear(can_frame_t *frame)
{
    //Copy RX buffer registers into frame structure
    for (int i = 0; i < FRAME_MAX_LEN; i++) {
        frame->bytes[i] = CAN.tx_rx_buffer[i].val;
    }
    //Clear RX buffer
    can_set_command(CMD_RELEASE_RX_BUFF);
}

static inline uint32_t can_get_rx_message_counter(void)
{
    return CAN.rx_message_counter_reg.val;
}

/* -------------------- Interrupt and Alert Handlers ------------------------ */

static void can_alert_handler(uint32_t alert_code, int *alert_req)
{
    if (p_can_obj->alerts_enabled & alert_code) {
        //Signify alert has occurred
        CAN_SET_FLAG(p_can_obj->alerts_triggered, alert_code);
        *alert_req = 1;
        if (p_can_obj->alerts_enabled & CAN_ALERT_AND_LOG) {
            if (alert_code >= ALERT_LOG_LEVEL_ERROR) {
                ESP_EARLY_LOGE(CAN_TAG, "Alert %d", alert_code);
            } else if (alert_code >= ALERT_LOG_LEVEL_WARNING) {
                ESP_EARLY_LOGW(CAN_TAG, "Alert %d", alert_code);
            } else {
                ESP_EARLY_LOGI(CAN_TAG, "Alert %d", alert_code);
            }
        }
    }
}

static void can_intr_handler_err_warn(can_status_reg_t *status, int *alert_req)
{
    if (status->bus) {
        if (status->error) {
            //Bus-Off condition. TEC should set and held at 127, REC should be 0, reset mode entered
            CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_BUS_OFF);
            /* Note: REC is still allowed to increase during bus-off. REC > err_warn
               can prevent "bus recovery complete" interrupt from occurring. Set to
               listen only mode to freeze REC. */
            can_config_mode(CAN_MODE_LISTEN_ONLY);
            can_alert_handler(CAN_ALERT_BUS_OFF, alert_req);
        } else {
            //Bus-recovery in progress. TEC has dropped below error warning limit
            can_alert_handler(CAN_ALERT_RECOVERY_IN_PROGRESS, alert_req);
        }
    } else {
        if (status->error) {
            //TEC or REC surpassed error warning limit
            CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_ERR_WARN);
            can_alert_handler(CAN_ALERT_ABOVE_ERR_WARN, alert_req);
        } else if (p_can_obj->control_flags & CTRL_FLAG_RECOVERING) {
            //Bus recovery complete.
            can_enter_reset_mode();
            //Reset and set flags to the equivalent of the stopped state
            CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_RECOVERING | CTRL_FLAG_ERR_WARN |
                                                     CTRL_FLAG_ERR_PASSIVE | CTRL_FLAG_BUS_OFF |
                                                     CTRL_FLAG_TX_BUFF_OCCUPIED);
            CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_STOPPED);
            can_alert_handler(CAN_ALERT_BUS_RECOVERED, alert_req);
        } else {
            //TEC and REC are both below error warning
            CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_ERR_WARN);
            can_alert_handler(CAN_ALERT_BELOW_ERR_WARN, alert_req);
        }
    }
}

static void can_intr_handler_err_passive(int *alert_req)
{
    uint32_t tec, rec;
    can_get_error_counters(&tec, &rec);
    if (tec >= DRIVER_DEFAULT_ERR_PASS_CNT || rec >= DRIVER_DEFAULT_ERR_PASS_CNT) {
        //Entered error passive
        CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_ERR_PASSIVE);
        can_alert_handler(CAN_ALERT_ERR_PASS, alert_req);
    } else {
        //Returned to error active
        CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_ERR_PASSIVE);
        can_alert_handler(CAN_ALERT_ERR_ACTIVE, alert_req);
    }
}

static void can_intr_handler_bus_err(int *alert_req)
{
    // ECC register is read to re-arm bus error interrupt. ECC is not used
    (void) can_get_error_code_capture();
    p_can_obj->bus_error_count++;
    can_alert_handler(CAN_ALERT_BUS_ERROR, alert_req);
}

static void can_intr_handler_arb_lost(int *alert_req)
{
    //ALC register is read to re-arm arb lost interrupt. ALC is not used
    (void) can_get_arbitration_lost_capture();
    p_can_obj->arb_lost_count++;
    can_alert_handler(CAN_ALERT_ARB_LOST, alert_req);
}

static void can_intr_handler_rx(BaseType_t *task_woken, int *alert_req)
{
    can_rx_msg_cnt_reg_t msg_count_reg;
    msg_count_reg.val = can_get_rx_message_counter();

    for (int i = 0; i < msg_count_reg.rx_message_counter; i++) {
        can_frame_t frame;
        can_get_rx_buffer_and_clear(&frame);
        //Copy frame into RX Queue
        if (xQueueSendFromISR(p_can_obj->rx_queue, &frame, task_woken) == pdTRUE) {
            p_can_obj->rx_msg_count++;
        } else {
            p_can_obj->rx_missed_count++;
            can_alert_handler(CAN_ALERT_RX_QUEUE_FULL, alert_req);
        }
    }
    //Todo: Add Software Filters
    //Todo: Check for data overrun of RX FIFO, then trigger alert
}

static void can_intr_handler_tx(can_status_reg_t *status, int *alert_req)
{
    //Handle previously transmitted frame
    if (status->tx_complete) {
        can_alert_handler(CAN_ALERT_TX_SUCCESS, alert_req);
    } else {
        p_can_obj->tx_failed_count++;
        can_alert_handler(CAN_ALERT_TX_FAILED, alert_req);
    }

    //Update TX message count
    p_can_obj->tx_msg_count--;
    configASSERT(p_can_obj->tx_msg_count >= 0);     //Sanity check

    //Check if there are more frames to transmit
    if (p_can_obj->tx_msg_count > 0 && p_can_obj->tx_queue != NULL) {
        can_frame_t frame;
        configASSERT(xQueueReceiveFromISR(p_can_obj->tx_queue, &frame, NULL) == pdTRUE);
        can_set_tx_buffer_and_transmit(&frame);
    } else {
        //No more frames to transmit
        CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_TX_BUFF_OCCUPIED);
        can_alert_handler(CAN_ALERT_TX_IDLE, alert_req);
    }
}

static void can_intr_handler_main(void *arg)
{
    BaseType_t task_woken = pdFALSE;
    int alert_req = 0;
    can_status_reg_t status;
    can_intr_reg_t intr_reason;

    CAN_ENTER_CRITICAL();
    status.val = can_get_status();
    intr_reason.val = (p_can_obj != NULL) ? can_get_interrupt_reason() : 0; //Incase intr occurs whilst driver is being uninstalled

#ifdef __clang_analyzer__
    if (intr_reason.val == 0) {  // Teach clang-tidy that all bitfields are zero if a register is zero; othewise it warns about p_can_obj null dereference
        intr_reason.err_warn = intr_reason.err_passive = intr_reason.bus_err = intr_reason.arb_lost = intr_reason.rx = intr_reason.tx = 0;
    }
#endif
    //Handle error counter related interrupts
    if (intr_reason.err_warn) {
        //Triggers when Bus-Status or Error-status bits change
        can_intr_handler_err_warn(&status, &alert_req);
    }
    if (intr_reason.err_passive) {
        //Triggers when entering/returning error passive/active state
        can_intr_handler_err_passive(&alert_req);
    }

    //Handle other error interrupts
    if (intr_reason.bus_err) {
        //Triggers when an error (Bit, Stuff, CRC, Form, ACK) occurs on the CAN bus
        can_intr_handler_bus_err(&alert_req);
    }
    if (intr_reason.arb_lost) {
        //Triggers when arbitration is lost
        can_intr_handler_arb_lost(&alert_req);
    }

    //Handle TX/RX interrupts
    if (intr_reason.rx) {
        //Triggers when RX buffer has one or more frames. Disabled if RX Queue length = 0
        can_intr_handler_rx(&task_woken, &alert_req);
    }
    if (intr_reason.tx) {
        //Triggers when TX buffer becomes free after a transmission
        can_intr_handler_tx(&status, &alert_req);
    }
    /* Todo: Check possible bug where transmitting self reception request then
       clearing rx buffer will cancel the transmission. */
    CAN_EXIT_CRITICAL();

    if (p_can_obj->alert_semphr != NULL && alert_req) {
        //Give semaphore if alerts were triggered
        xSemaphoreGiveFromISR(p_can_obj->alert_semphr, &task_woken);
    }
    if (task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/* ---------------------- Frame and GPIO functions  ------------------------- */

static void can_format_frame(uint32_t id, uint8_t dlc, const uint8_t *data, uint32_t flags, can_frame_t *tx_frame)
{
    /* This function encodes a message into a frame structure. The frame structure has
       an identical layout to the TX buffer, allowing the frame structure to be directly
       copied into TX buffer. */
    //Set frame information
    tx_frame->dlc = dlc;
    tx_frame->rtr = (flags & CAN_MSG_FLAG_RTR) ? 1 : 0;
    tx_frame->frame_format = (flags & CAN_MSG_FLAG_EXTD) ? 1 : 0;
    tx_frame->self_reception = (flags & CAN_MSG_FLAG_SELF) ? 1 : 0;
    tx_frame->single_shot = (flags & CAN_MSG_FLAG_SS) ? 1 : 0;

    //Set ID
    int id_len = (flags & CAN_MSG_FLAG_EXTD) ? FRAME_EXTD_ID_LEN : FRAME_STD_ID_LEN;
    uint8_t *id_buffer = (flags & CAN_MSG_FLAG_EXTD) ? tx_frame->extended.id : tx_frame->standard.id;
    //Split ID into 4 or 2 bytes, and turn into big-endian with left alignment (<< 3 or 5)
    uint32_t id_temp = (flags & CAN_MSG_FLAG_EXTD) ? __builtin_bswap32((id & CAN_EXTD_ID_MASK) << 3) :  //((id << 3) >> 8*(3-i))
                                                     __builtin_bswap16((id & CAN_STD_ID_MASK) << 5);    //((id << 5) >> 8*(1-i))
    for (int i = 0; i < id_len; i++) {
        id_buffer[i] = (id_temp >> (8 * i)) & 0xFF;     //Copy big-endian ID byte by byte
    }

    //Set Data.
    uint8_t *data_buffer = (flags & CAN_MSG_FLAG_EXTD) ? tx_frame->extended.data : tx_frame->standard.data;
    for (int i = 0; (i < dlc) && (i < FRAME_MAX_DATA_LEN); i++) {       //Handle case where dlc is > 8
        data_buffer[i] = data[i];
    }
}

static void can_parse_frame(can_frame_t *rx_frame, uint32_t *id, uint8_t *dlc, uint8_t *data, uint32_t *flags)
{
    //This function decodes a frame structure into it's constituent components.

    //Copy frame information
    *dlc = rx_frame->dlc;
    *flags = 0;
    *flags |= (rx_frame->dlc > FRAME_MAX_DATA_LEN) ? CAN_MSG_FLAG_DLC_NON_COMP : 0;
    *flags |= (rx_frame->rtr) ? CAN_MSG_FLAG_RTR : 0;
    *flags |= (rx_frame->frame_format) ? CAN_MSG_FLAG_EXTD : 0;

    //Copy ID
    int id_len = (rx_frame->frame_format) ? FRAME_EXTD_ID_LEN : FRAME_STD_ID_LEN;
    uint8_t *id_buffer = (rx_frame->frame_format) ? rx_frame->extended.id : rx_frame->standard.id;
    uint32_t id_temp = 0;
    for (int i = 0; i < id_len; i++) {
        id_temp |= id_buffer[i] << (8 * i);     //Copy big-endian ID byte by byte
    }
    //Revert endianness of 4 or 2 byte ID, and shift into 29 or 11 bit ID
    id_temp = (rx_frame->frame_format) ? (__builtin_bswap32(id_temp) >> 3) :    //((byte[i] << 8*(3-i)) >> 3)
                                         (__builtin_bswap16(id_temp) >> 5);     //((byte[i] << 8*(1-i)) >> 5)
    *id = id_temp & ((rx_frame->frame_format) ? CAN_EXTD_ID_MASK : CAN_STD_ID_MASK);

    //Copy data
    uint8_t *data_buffer = (rx_frame->frame_format) ? rx_frame->extended.data : rx_frame->standard.data;
    for (int i = 0; (i < rx_frame->dlc) && (i < FRAME_MAX_DATA_LEN); i++) {
        data[i] = data_buffer[i];
    }
    //Set remaining bytes of data to 0
    for (int i = rx_frame->dlc; i < FRAME_MAX_DATA_LEN; i++) {
        data[i] = 0;
    }
}

static void can_configure_gpio(gpio_num_t tx, gpio_num_t rx, gpio_num_t clkout, gpio_num_t bus_status)
{
    //Set TX pin
    gpio_set_pull_mode(tx, GPIO_FLOATING);
    gpio_matrix_out(tx, CAN_TX_IDX, false, false);
    gpio_pad_select_gpio(tx);

    //Set RX pin
    gpio_set_pull_mode(rx, GPIO_FLOATING);
    gpio_matrix_in(rx, CAN_RX_IDX, false);
    gpio_pad_select_gpio(rx);
    gpio_set_direction(rx, GPIO_MODE_INPUT);

    //Configure output clock pin (Optional)
    if (clkout >= 0 && clkout < GPIO_NUM_MAX) {
        gpio_set_pull_mode(clkout, GPIO_FLOATING);
        gpio_matrix_out(clkout, CAN_CLKOUT_IDX, false, false);
        gpio_pad_select_gpio(clkout);
    }

    //Configure bus status pin (Optional)
    if (bus_status >= 0 && bus_status < GPIO_NUM_MAX) {
        gpio_set_pull_mode(bus_status, GPIO_FLOATING);
        gpio_matrix_out(bus_status, CAN_BUS_OFF_ON_IDX, false, false);
        gpio_pad_select_gpio(bus_status);
    }
}

/* ---------------------------- Public Functions ---------------------------- */

esp_err_t can_driver_install(const can_general_config_t *g_config, const can_timing_config_t *t_config, const can_filter_config_t *f_config)
{
    //Check arguments
    CAN_CHECK(g_config != NULL, ESP_ERR_INVALID_ARG);
    CAN_CHECK(t_config != NULL, ESP_ERR_INVALID_ARG);
    CAN_CHECK(f_config != NULL, ESP_ERR_INVALID_ARG);
    CAN_CHECK(g_config->rx_queue_len > 0, ESP_ERR_INVALID_ARG);
    CAN_CHECK(g_config->tx_io >= 0 && g_config->tx_io < GPIO_NUM_MAX, ESP_ERR_INVALID_ARG);
    CAN_CHECK(g_config->rx_io >= 0 && g_config->rx_io < GPIO_NUM_MAX, ESP_ERR_INVALID_ARG);

    esp_err_t ret;
    can_obj_t *p_can_obj_dummy;

    //Create a CAN object
    p_can_obj_dummy = calloc(1, sizeof(can_obj_t));
    CAN_CHECK(p_can_obj_dummy != NULL, ESP_ERR_NO_MEM);

    //Initialize queues, semaphores, and power management locks
    p_can_obj_dummy->tx_queue = (g_config->tx_queue_len > 0) ? xQueueCreate(g_config->tx_queue_len, sizeof(can_frame_t)) : NULL;
    p_can_obj_dummy->rx_queue = xQueueCreate(g_config->rx_queue_len, sizeof(can_frame_t));
    p_can_obj_dummy->alert_semphr = xSemaphoreCreateBinary();
    if ((g_config->tx_queue_len > 0 && p_can_obj_dummy->tx_queue == NULL) ||
        p_can_obj_dummy->rx_queue == NULL || p_can_obj_dummy->alert_semphr == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
#ifdef CONFIG_PM_ENABLE
    esp_err_t pm_err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "can", &(p_can_obj_dummy->pm_lock));
    if (pm_err != ESP_OK ) {
        ret = pm_err;
        goto err;
    }
#endif

    //Initialize flags and variables
    p_can_obj_dummy->control_flags = CTRL_FLAG_STOPPED;
    p_can_obj_dummy->control_flags |= (g_config->mode == CAN_MODE_NO_ACK) ? CTRL_FLAG_SELF_TEST : 0;
    p_can_obj_dummy->control_flags |= (g_config->mode == CAN_MODE_LISTEN_ONLY) ? CTRL_FLAG_LISTEN_ONLY : 0;
    p_can_obj_dummy->tx_msg_count = 0;
    p_can_obj_dummy->rx_msg_count = 0;
    p_can_obj_dummy->tx_failed_count = 0;
    p_can_obj_dummy->rx_missed_count = 0;
    p_can_obj_dummy->arb_lost_count = 0;
    p_can_obj_dummy->bus_error_count = 0;
    p_can_obj_dummy->alerts_enabled = g_config->alerts_enabled;
    p_can_obj_dummy->alerts_triggered = 0;

    //Initialize CAN peripheral registers, and allocate interrupt
    CAN_ENTER_CRITICAL();
    if (p_can_obj == NULL) {
        p_can_obj = p_can_obj_dummy;
    } else {
        //Check if driver is already installed
        CAN_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto err;
    }
    periph_module_reset(PERIPH_CAN_MODULE);
    periph_module_enable(PERIPH_CAN_MODULE);            //Enable APB CLK to CAN peripheral
    configASSERT(can_enter_reset_mode() == ESP_OK);     //Must enter reset mode to write to config registers
    can_config_pelican();                               //Use PeliCAN addresses
    /* Note: REC is allowed to increase even in reset mode. Listen only mode
       will freeze REC. The desired mode will be set when can_start() is called. */
    can_config_mode(CAN_MODE_LISTEN_ONLY);
    can_config_interrupts(DRIVER_DEFAULT_INTERRUPTS);
    can_config_bus_timing(t_config->brp, t_config->sjw, t_config->tseg_1, t_config->tseg_2, t_config->triple_sampling);
    can_config_error(DRIVER_DEFAULT_EWL, DRIVER_DEFAULT_REC, DRIVER_DEFAULT_TEC);
    can_config_acceptance_filter(f_config->acceptance_code, f_config->acceptance_mask, f_config->single_filter);
    can_config_clk_out(g_config->clkout_divider);
    //Allocate GPIO and Interrupts
    can_configure_gpio(g_config->tx_io, g_config->rx_io, g_config->clkout_io, g_config->bus_off_io);
    (void) can_get_interrupt_reason();                  //Read interrupt reg to clear it before allocating ISR
    ESP_ERROR_CHECK(esp_intr_alloc(ETS_CAN_INTR_SOURCE, 0, can_intr_handler_main, NULL, &p_can_obj->isr_handle));
    //Todo: Allow interrupt to be registered to specified CPU
    CAN_EXIT_CRITICAL();

#ifdef CONFIG_PM_ENABLE
    ESP_ERROR_CHECK(esp_pm_lock_acquire(p_can_obj->pm_lock));     //Acquire pm_lock to keep APB clock at 80MHz
#endif
    return ESP_OK;      //CAN module is still in reset mode, users need to call can_start() afterwards

    err:
    //Cleanup CAN object and return error
    if (p_can_obj_dummy != NULL) {
        if (p_can_obj_dummy->tx_queue != NULL) {
            vQueueDelete(p_can_obj_dummy->tx_queue);
            p_can_obj_dummy->tx_queue = NULL;
        }
        if (p_can_obj_dummy->rx_queue != NULL) {
            vQueueDelete(p_can_obj_dummy->rx_queue);
            p_can_obj_dummy->rx_queue = NULL;
        }
        if (p_can_obj_dummy->alert_semphr != NULL) {
            vSemaphoreDelete(p_can_obj_dummy->alert_semphr);
            p_can_obj_dummy->alert_semphr = NULL;
        }
#ifdef CONFIG_PM_ENABLE
        if (p_can_obj_dummy->pm_lock != NULL) {
            ESP_ERROR_CHECK(esp_pm_lock_delete(p_can_obj_dummy->pm_lock));
        }
#endif
        free(p_can_obj_dummy);
    }
    return ret;
}

esp_err_t can_driver_uninstall(void)
{
    can_obj_t *p_can_obj_dummy;

    CAN_ENTER_CRITICAL();
    //Check state
    CAN_CHECK_FROM_CRIT(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK_FROM_CRIT(p_can_obj->control_flags & (CTRL_FLAG_STOPPED | CTRL_FLAG_BUS_OFF), ESP_ERR_INVALID_STATE);
    configASSERT(can_enter_reset_mode() == ESP_OK); //Enter reset mode to stop any CAN bus activity
    //Clear registers by reading
    (void) can_get_interrupt_reason();
    (void) can_get_arbitration_lost_capture();
    (void) can_get_error_code_capture();

    ESP_ERROR_CHECK(esp_intr_free(p_can_obj->isr_handle));  //Free interrupt
    periph_module_disable(PERIPH_CAN_MODULE);               //Disable CAN peripheral
    p_can_obj_dummy = p_can_obj;        //Use dummy to shorten critical section
    p_can_obj = NULL;
    CAN_EXIT_CRITICAL();

    //Delete queues, semaphores, and power management locks
    if (p_can_obj_dummy->tx_queue != NULL) {
        vQueueDelete(p_can_obj_dummy->tx_queue);
    }
    vQueueDelete(p_can_obj_dummy->rx_queue);
    vSemaphoreDelete(p_can_obj_dummy->alert_semphr);
#ifdef CONFIG_PM_ENABLE
    //Release and delete power management lock
    ESP_ERROR_CHECK(esp_pm_lock_release(p_can_obj_dummy->pm_lock));
    ESP_ERROR_CHECK(esp_pm_lock_delete(p_can_obj_dummy->pm_lock));
#endif
    free(p_can_obj_dummy);        //Free can driver object

    return ESP_OK;
}

esp_err_t can_start(void)
{
    //Check state
    CAN_ENTER_CRITICAL();
    CAN_CHECK_FROM_CRIT(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK_FROM_CRIT(p_can_obj->control_flags & CTRL_FLAG_STOPPED, ESP_ERR_INVALID_STATE);

    //Reset RX queue, and RX message count
    xQueueReset(p_can_obj->rx_queue);
    p_can_obj->rx_msg_count = 0;
    configASSERT(can_enter_reset_mode() == ESP_OK); //Should already be in bus-off mode, set again to make sure

    //Currently in listen only mode, need to set to mode specified by configuration
    can_mode_t mode;
    if (p_can_obj->control_flags & CTRL_FLAG_SELF_TEST) {
        mode = CAN_MODE_NO_ACK;
    } else if (p_can_obj->control_flags & CTRL_FLAG_LISTEN_ONLY) {
        mode = CAN_MODE_LISTEN_ONLY;
    } else {
        mode = CAN_MODE_NORMAL;
    }
    can_config_mode(mode);                              //Set mode
    (void) can_get_interrupt_reason();                  //Clear interrupt register
    configASSERT(can_exit_reset_mode() == ESP_OK);

    CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_STOPPED);
    CAN_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t can_stop(void)
{
    //Check state
    CAN_ENTER_CRITICAL();
    CAN_CHECK_FROM_CRIT(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK_FROM_CRIT(!(p_can_obj->control_flags & (CTRL_FLAG_STOPPED | CTRL_FLAG_BUS_OFF)), ESP_ERR_INVALID_STATE);

    //Clear interrupts and reset flags
    configASSERT(can_enter_reset_mode() == ESP_OK);
    (void) can_get_interrupt_reason();          //Read interrupt register to clear interrupts
    can_config_mode(CAN_MODE_LISTEN_ONLY);      //Set to listen only mode to freeze REC
    CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_TX_BUFF_OCCUPIED);
    CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_STOPPED);

    //Reset TX Queue and message count
    if (p_can_obj->tx_queue != NULL) {
        xQueueReset(p_can_obj->tx_queue);
    }
    p_can_obj->tx_msg_count = 0;

    CAN_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t can_transmit(const can_message_t *message, TickType_t ticks_to_wait)
{
    //Check arguments
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK(message != NULL, ESP_ERR_INVALID_ARG);
    CAN_CHECK((message->data_length_code <= FRAME_MAX_DATA_LEN) || (message->flags & CAN_MSG_FLAG_DLC_NON_COMP), ESP_ERR_INVALID_ARG);

    CAN_ENTER_CRITICAL();
    //Check State
    CAN_CHECK_FROM_CRIT(!(p_can_obj->control_flags & CTRL_FLAG_LISTEN_ONLY), ESP_ERR_NOT_SUPPORTED);
    CAN_CHECK_FROM_CRIT(!(p_can_obj->control_flags & (CTRL_FLAG_STOPPED | CTRL_FLAG_BUS_OFF)), ESP_ERR_INVALID_STATE);
    //Format frame
    esp_err_t ret = ESP_FAIL;
    can_frame_t tx_frame;
    can_format_frame(message->identifier, message->data_length_code, message->data, message->flags, &tx_frame);
    //Check if frame can be sent immediately
    if ((p_can_obj->tx_msg_count == 0) && !(p_can_obj->control_flags & CTRL_FLAG_TX_BUFF_OCCUPIED)) {
        //No other frames waiting to transmit. Bypass queue and transmit immediately
        can_set_tx_buffer_and_transmit(&tx_frame);
        p_can_obj->tx_msg_count++;
        CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_TX_BUFF_OCCUPIED);
        ret = ESP_OK;
    }
    CAN_EXIT_CRITICAL();

    if (ret != ESP_OK) {
        if (p_can_obj->tx_queue == NULL) {
            //TX Queue is disabled and TX buffer is occupied, message was not sent
            ret = ESP_FAIL;
        } else if (xQueueSend(p_can_obj->tx_queue, &tx_frame, ticks_to_wait) == pdTRUE) {
            //Copied to TX Queue
            CAN_ENTER_CRITICAL();
            if (p_can_obj->control_flags & (CTRL_FLAG_STOPPED | CTRL_FLAG_BUS_OFF)) {
                //TX queue was reset (due to stop/bus_off), remove copied frame from queue to prevent transmission
                configASSERT(xQueueReceive(p_can_obj->tx_queue, &tx_frame, 0) == pdTRUE);
                ret = ESP_ERR_INVALID_STATE;
            } else if ((p_can_obj->tx_msg_count == 0) && !(p_can_obj->control_flags & CTRL_FLAG_TX_BUFF_OCCUPIED)) {
                //TX buffer was freed during copy, manually trigger transmission
                configASSERT(xQueueReceive(p_can_obj->tx_queue, &tx_frame, 0) == pdTRUE);
                can_set_tx_buffer_and_transmit(&tx_frame);
                p_can_obj->tx_msg_count++;
                CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_TX_BUFF_OCCUPIED);
                ret = ESP_OK;
            } else {
                //Frame was copied to queue, waiting to be transmitted
                p_can_obj->tx_msg_count++;
                ret = ESP_OK;
            }
            CAN_EXIT_CRITICAL();
        } else {
            //Timed out waiting for free space on TX queue
            ret = ESP_ERR_TIMEOUT;
        }
    }
    return ret;
}

esp_err_t can_receive(can_message_t *message, TickType_t ticks_to_wait)
{
    //Check arguments and state
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK(message != NULL, ESP_ERR_INVALID_ARG);

    //Get frame from RX Queue or RX Buffer
    can_frame_t rx_frame;
    if (xQueueReceive(p_can_obj->rx_queue, &rx_frame, ticks_to_wait) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    CAN_ENTER_CRITICAL();
    p_can_obj->rx_msg_count--;
    CAN_EXIT_CRITICAL();

    //Decode frame
    can_parse_frame(&rx_frame, &(message->identifier), &(message->data_length_code), message->data, &(message->flags));
    return ESP_OK;
}

esp_err_t can_read_alerts(uint32_t *alerts, TickType_t ticks_to_wait)
{
    //Check arguments and state
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK(alerts != NULL, ESP_ERR_INVALID_ARG);

    //Wait for an alert to occur
    if (xSemaphoreTake(p_can_obj->alert_semphr, ticks_to_wait) == pdTRUE) {
        CAN_ENTER_CRITICAL();
        *alerts = p_can_obj->alerts_triggered;
        p_can_obj->alerts_triggered = 0;    //Clear triggered alerts
        CAN_EXIT_CRITICAL();
        return ESP_OK;
    } else {
        *alerts = 0;
        return ESP_ERR_TIMEOUT;
    }
}

esp_err_t can_reconfigure_alerts(uint32_t alerts_enabled, uint32_t *current_alerts)
{
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_ENTER_CRITICAL();
    uint32_t cur_alerts;
    can_read_alerts(&cur_alerts, 0);                    //Clear any unhandled alerts
    p_can_obj->alerts_enabled = alerts_enabled;         //Update enabled alerts
    CAN_EXIT_CRITICAL();

    if (current_alerts != NULL) {
        *current_alerts = cur_alerts;
    }
    return ESP_OK;
}

esp_err_t can_initiate_recovery(void)
{
    CAN_ENTER_CRITICAL();
    //Check state
    CAN_CHECK_FROM_CRIT(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK_FROM_CRIT(p_can_obj->control_flags & CTRL_FLAG_BUS_OFF, ESP_ERR_INVALID_STATE);
    CAN_CHECK_FROM_CRIT(!(p_can_obj->control_flags & CTRL_FLAG_RECOVERING), ESP_ERR_INVALID_STATE);

    //Reset TX Queue/Counters
    if (p_can_obj->tx_queue != NULL) {
        xQueueReset(p_can_obj->tx_queue);
    }
    p_can_obj->tx_msg_count = 0;
    CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_TX_BUFF_OCCUPIED);
    CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_RECOVERING);

    //Trigger start of recovery process
    configASSERT(can_exit_reset_mode() == ESP_OK);
    CAN_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t can_get_status_info(can_status_info_t *status_info)
{
    //Check parameters and state
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK(status_info != NULL, ESP_ERR_INVALID_ARG);

    CAN_ENTER_CRITICAL();
    uint32_t tec, rec;
    can_get_error_counters(&tec, &rec);
    status_info->tx_error_counter = tec;
    status_info->rx_error_counter = rec;
    status_info->msgs_to_tx = p_can_obj->tx_msg_count;
    status_info->msgs_to_rx = p_can_obj->rx_msg_count;
    status_info->tx_failed_count = p_can_obj->tx_failed_count;
    status_info->rx_missed_count = p_can_obj->rx_missed_count;
    status_info->arb_lost_count = p_can_obj->arb_lost_count;
    status_info->bus_error_count = p_can_obj->bus_error_count;
    if (p_can_obj->control_flags & CTRL_FLAG_RECOVERING) {
        status_info->state = CAN_STATE_RECOVERING;
    } else if (p_can_obj->control_flags & CTRL_FLAG_BUS_OFF) {
        status_info->state = CAN_STATE_BUS_OFF;
    } else if (p_can_obj->control_flags & CTRL_FLAG_STOPPED) {
        status_info->state = CAN_STATE_STOPPED;
    } else {
        status_info->state = CAN_STATE_RUNNING;
    }
    CAN_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t can_clear_transmit_queue(void)
{
    //Check State
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK(p_can_obj->tx_queue != NULL, ESP_ERR_NOT_SUPPORTED);

    CAN_ENTER_CRITICAL();
    //If a message is currently undergoing transmission, the tx interrupt handler will decrement tx_msg_count
    p_can_obj->tx_msg_count = (p_can_obj->control_flags & CTRL_FLAG_TX_BUFF_OCCUPIED) ? 1 : 0;
    xQueueReset(p_can_obj->tx_queue);
    CAN_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t can_clear_receive_queue(void)
{
    //Check State
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);

    CAN_ENTER_CRITICAL();
    p_can_obj->rx_msg_count = 0;
    xQueueReset(p_can_obj->rx_queue);
    CAN_EXIT_CRITICAL();

    return ESP_OK;
}
