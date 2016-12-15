/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#include <string.h>
#include "bt.h"
#include "bt_defs.h"
#include "bt_trace.h"
#include "hcidefs.h"
#include "hcimsgs.h"
#include "bt_vendor_lib.h"
#include "hci_internals.h"
#include "hci_hal.h"
#include "hci_layer.h"
#include "allocator.h"
#include "packet_fragmenter.h"
#include "buffer_allocator.h"
#include "list.h"
#include "alarm.h"
#include "thread.h"

typedef struct {
    uint16_t opcode;
    future_t *complete_future;
    command_complete_cb complete_callback;
    command_status_cb status_callback;
    void *context;
    uint32_t sent_time;
    BT_HDR *command;
} waiting_command_t;

typedef struct {
    bool timer_is_set;
    osi_alarm_t *command_response_timer;
    list_t *commands_pending_response;
    pthread_mutex_t commands_pending_response_lock;
} command_waiting_response_t;

typedef struct {
    int command_credits;
    fixed_queue_t *command_queue;
    fixed_queue_t *packet_queue;

    // The hand-off point for data going to a higher layer, set by the higher layer
    fixed_queue_t *upwards_data_queue;

    command_waiting_response_t cmd_waiting_q;

    /*
      non_repeating_timer_t *command_response_timer;
      list_t *commands_pending_response;
      pthread_mutex_t commands_pending_response_lock;
    */
} hci_host_env_t;

// Using a define here, because it can be stringified for the property lookup
static const uint32_t COMMAND_PENDING_TIMEOUT = 8000;

// Our interface
static bool interface_created;
static hci_t interface;
static hci_host_env_t hci_host_env;

static xTaskHandle  xHciHostTaskHandle;
static xQueueHandle xHciHostQueue;

static bool hci_host_startup_flag;

// Modules we import and callbacks we export
static const allocator_t *buffer_allocator;
static const hci_hal_t *hal;
static const hci_hal_callbacks_t hal_callbacks;
static const packet_fragmenter_t *packet_fragmenter;
static const packet_fragmenter_callbacks_t packet_fragmenter_callbacks;

static int hci_layer_init_env(void);
static void hci_layer_deinit_env(void);
static void hci_host_thread_handler(void *arg);
static void event_command_ready(fixed_queue_t *queue);
static void event_packet_ready(fixed_queue_t *queue);
static void restart_comamnd_waiting_response_timer(
    command_waiting_response_t *cmd_wait_q,
    bool tigger_by_sending_command);
static void command_timed_out(void *context);
static void hal_says_packet_ready(BT_HDR *packet);
static bool filter_incoming_event(BT_HDR *packet);
static serial_data_type_t event_to_data_type(uint16_t event);
static waiting_command_t *get_waiting_command(command_opcode_t opcode);
static void dispatch_reassembled(BT_HDR *packet);

// Module lifecycle functions
int hci_start_up(void)
{
    if (hci_layer_init_env()) {
        goto error;
    }

    xHciHostQueue = xQueueCreate(HCI_HOST_QUEUE_NUM, sizeof(BtTaskEvt_t));
    xTaskCreate(hci_host_thread_handler, HCI_HOST_TASK_NAME, HCI_HOST_TASK_STACK_SIZE, NULL, HCI_HOST_TASK_PRIO, &xHciHostTaskHandle);

    packet_fragmenter->init(&packet_fragmenter_callbacks);
    hal->open(&hal_callbacks);

    hci_host_startup_flag = true;
    return 0;
error:
    hci_shut_down();
    return -1;
}

void hci_shut_down(void)
{
    hci_host_startup_flag  = false;
    hci_layer_deinit_env();

    packet_fragmenter->cleanup();

    //low_power_manager->cleanup();
    hal->close();
    vTaskDelete(xHciHostTaskHandle);
    vQueueDelete(xHciHostQueue);
}


void hci_host_task_post(void)
{
    BtTaskEvt_t evt;

    if (hci_host_startup_flag == false) {
        return;
    }

    evt.sig = 0xff;
    evt.par = 0;

    if (xQueueSend(xHciHostQueue, &evt, 10 / portTICK_RATE_MS) != pdTRUE) {
        LOG_ERROR("xHciHostQueue failed\n");
    }
}

static int hci_layer_init_env(void)
{
    command_waiting_response_t *cmd_wait_q;

    // The host is only allowed to send at most one command initially,
    // as per the Bluetooth spec, Volume 2, Part E, 4.4 (Command Flow Control)
    // This value can change when you get a command complete or command status event.
    hci_host_env.command_credits = 1;
    hci_host_env.command_queue = fixed_queue_new(SIZE_MAX);
    if (hci_host_env.command_queue) {
        fixed_queue_register_dequeue(hci_host_env.command_queue, event_command_ready);
    } else {
        LOG_ERROR("%s unable to create pending command queue.", __func__);
        return -1;
    }

    hci_host_env.packet_queue = fixed_queue_new(SIZE_MAX);
    if (hci_host_env.packet_queue) {
        fixed_queue_register_dequeue(hci_host_env.packet_queue, event_packet_ready);
    } else {
        LOG_ERROR("%s unable to create pending packet queue.", __func__);
        return -1;
    }

    // Init Commands waiting response list and timer
    cmd_wait_q = &hci_host_env.cmd_waiting_q;
    cmd_wait_q->timer_is_set = false;
    cmd_wait_q->commands_pending_response = list_new(NULL);
    if (!cmd_wait_q->commands_pending_response) {
        LOG_ERROR("%s unable to create list for commands pending response.", __func__);
        return -1;
    }
    pthread_mutex_init(&cmd_wait_q->commands_pending_response_lock, NULL);
    cmd_wait_q->command_response_timer = osi_alarm_new("cmd_rsp_to", command_timed_out, cmd_wait_q, COMMAND_PENDING_TIMEOUT);
    if (!cmd_wait_q->command_response_timer) {
        LOG_ERROR("%s unable to create command response timer.", __func__);
        return -1;
    }

    return 0;
}

static void hci_layer_deinit_env(void)
{
    command_waiting_response_t *cmd_wait_q;

    if (hci_host_env.command_queue) {
        fixed_queue_free(hci_host_env.command_queue, allocator_calloc.free);
    }
    if (hci_host_env.packet_queue) {
        fixed_queue_free(hci_host_env.packet_queue, buffer_allocator->free);
    }

    cmd_wait_q = &hci_host_env.cmd_waiting_q;
    list_free(cmd_wait_q->commands_pending_response);
    pthread_mutex_destroy(&cmd_wait_q->commands_pending_response_lock);
    osi_alarm_free(cmd_wait_q->command_response_timer);
    cmd_wait_q->command_response_timer = NULL;
}

static void hci_host_thread_handler(void *arg)
{
    /*
     * Previous task handles RX queue and two TX Queues, Since there is
     * a RX Thread Task in H4 layer which receives packet from driver layer.
     * Now HCI Host Task has been optimized to only process TX Queue
     * including command and data queue. And command queue has high priority,
     * All packets will be directly copied to single queue in driver layer with
     * H4 type header added (1 byte).
     */

    BtTaskEvt_t e;

    for (;;) {
        if (pdTRUE == xQueueReceive(xHciHostQueue, &e, (portTickType)portMAX_DELAY)) {

            if (e.sig == 0xff) {
                if (API_vhci_host_check_send_available()) {
                    /*Now Target only allowed one packet per TX*/
                    BT_HDR *pkt = packet_fragmenter->fragment_current_packet();
                    if (pkt != NULL) {
                        packet_fragmenter->fragment_and_dispatch(pkt);
                    } else {
                        if (!fixed_queue_is_empty(hci_host_env.command_queue) &&
                                hci_host_env.command_credits > 0) {
                            fixed_queue_process(hci_host_env.command_queue);
                        } else if (!fixed_queue_is_empty(hci_host_env.packet_queue)) {
                            fixed_queue_process(hci_host_env.packet_queue);
                        }
                    }
                }
            }
        }
    }
}

static void set_data_queue(fixed_queue_t *queue)
{
    hci_host_env.upwards_data_queue = queue;
}

static void transmit_command(
    BT_HDR *command,
    command_complete_cb complete_callback,
    command_status_cb status_callback,
    void *context)
{
    uint8_t *stream;
    waiting_command_t *wait_entry = osi_calloc(sizeof(waiting_command_t));
    if (!wait_entry) {
        LOG_ERROR("%s couldn't allocate space for wait entry.", __func__);
        return;
    }

    stream = command->data + command->offset;
    STREAM_TO_UINT16(wait_entry->opcode, stream);
    wait_entry->complete_callback = complete_callback;
    wait_entry->status_callback = status_callback;
    wait_entry->command = command;
    wait_entry->context = context;

    // Store the command message type in the event field
    // in case the upper layer didn't already
    command->event = MSG_STACK_TO_HC_HCI_CMD;
    LOG_DEBUG("HCI Enqueue Comamnd opcode=0x%x\n", wait_entry->opcode);
    BTTRC_DUMP_BUFFER(NULL, command->data + command->offset, command->len);

    fixed_queue_enqueue(hci_host_env.command_queue, wait_entry);
    hci_host_task_post();
}

static future_t *transmit_command_futured(BT_HDR *command)
{
    waiting_command_t *wait_entry = osi_calloc(sizeof(waiting_command_t));
    assert(wait_entry != NULL);

    future_t *future = future_new();

    uint8_t *stream = command->data + command->offset;
    STREAM_TO_UINT16(wait_entry->opcode, stream);
    wait_entry->complete_future = future;
    wait_entry->command = command;

    // Store the command message type in the event field
    // in case the upper layer didn't already
    command->event = MSG_STACK_TO_HC_HCI_CMD;

    fixed_queue_enqueue(hci_host_env.command_queue, wait_entry);
    hci_host_task_post();
    return future;
}

static void transmit_downward(uint16_t type, void *data)
{
    if (type == MSG_STACK_TO_HC_HCI_CMD) {
        transmit_command((BT_HDR *)data, NULL, NULL, NULL);
        LOG_WARN("%s legacy transmit of command. Use transmit_command instead.\n", __func__);
    } else {
        fixed_queue_enqueue(hci_host_env.packet_queue, data);
    }
    //ke_event_set(KE_EVENT_HCI_HOST_THREAD);
    hci_host_task_post();
}


// Command/packet transmitting functions
static void event_command_ready(fixed_queue_t *queue)
{
    waiting_command_t *wait_entry = NULL;
    command_waiting_response_t *cmd_wait_q = &hci_host_env.cmd_waiting_q;

    wait_entry = fixed_queue_dequeue(queue);
    hci_host_env.command_credits--;

    // Move it to the list of commands awaiting response
    pthread_mutex_lock(&cmd_wait_q->commands_pending_response_lock);
    list_append(cmd_wait_q->commands_pending_response, wait_entry);
    pthread_mutex_unlock(&cmd_wait_q->commands_pending_response_lock);

    // Send it off
    packet_fragmenter->fragment_and_dispatch(wait_entry->command);

    wait_entry->sent_time = osi_alarm_now();
    restart_comamnd_waiting_response_timer(cmd_wait_q, true);
}

static void event_packet_ready(fixed_queue_t *queue)
{
    BT_HDR *packet = (BT_HDR *)fixed_queue_dequeue(queue);
    // The queue may be the command queue or the packet queue, we don't care

    packet_fragmenter->fragment_and_dispatch(packet);
}

// Callback for the fragmenter to send a fragment
static void transmit_fragment(BT_HDR *packet, bool send_transmit_finished)
{
    uint16_t event = packet->event & MSG_EVT_MASK;
    serial_data_type_t type = event_to_data_type(event);

    hal->transmit_data(type, packet->data + packet->offset, packet->len);

    if (event != MSG_STACK_TO_HC_HCI_CMD && send_transmit_finished) {
        buffer_allocator->free(packet);
    }
}

static void fragmenter_transmit_finished(BT_HDR *packet, bool all_fragments_sent)
{
    if (all_fragments_sent) {
        buffer_allocator->free(packet);
    } else {
        // This is kind of a weird case, since we're dispatching a partially sent packet
        // up to a higher layer.
        // TODO(zachoverflow): rework upper layer so this isn't necessary.
        buffer_allocator->free(packet);
        //dispatch_reassembled(packet);
        //data_dispatcher_dispatch(interface.event_dispatcher, packet->event & MSG_EVT_MASK, packet);
    }
}

static void restart_comamnd_waiting_response_timer(
    command_waiting_response_t *cmd_wait_q,
    bool tigger_by_sending_command)
{
    uint32_t timeout;
    waiting_command_t *wait_entry;
    if (!cmd_wait_q) {
        return;
    }

    if (cmd_wait_q->timer_is_set) {
        if (tigger_by_sending_command) {
            return;
        }

        //Cancel Previous command timeout timer setted when sending command
        osi_alarm_cancel(cmd_wait_q->command_response_timer);
        cmd_wait_q->timer_is_set = false;
    }

    pthread_mutex_lock(&cmd_wait_q->commands_pending_response_lock);
    wait_entry = (list_is_empty(cmd_wait_q->commands_pending_response) ?
                  NULL : list_front(cmd_wait_q->commands_pending_response));
    pthread_mutex_unlock(&cmd_wait_q->commands_pending_response_lock);

    if (wait_entry == NULL) {
        return;
    }

    timeout = osi_alarm_time_diff(osi_alarm_now(), wait_entry->sent_time);
    timeout = osi_alarm_time_diff(COMMAND_PENDING_TIMEOUT, timeout);
    timeout = (timeout <= COMMAND_PENDING_TIMEOUT) ? timeout : COMMAND_PENDING_TIMEOUT;

    cmd_wait_q->timer_is_set = true;
    osi_alarm_set(cmd_wait_q->command_response_timer, timeout);
}

static void command_timed_out(void *context)
{
    command_waiting_response_t *cmd_wait_q = (command_waiting_response_t *)context;
    waiting_command_t *wait_entry;

    pthread_mutex_lock(&cmd_wait_q->commands_pending_response_lock);
    wait_entry = (list_is_empty(cmd_wait_q->commands_pending_response) ?
                  NULL : list_front(cmd_wait_q->commands_pending_response));
    pthread_mutex_unlock(&cmd_wait_q->commands_pending_response_lock);

    if (wait_entry == NULL) {
        LOG_ERROR("%s with no commands pending response", __func__);
    } else
        // We shouldn't try to recover the stack from this command timeout.
        // If it's caused by a software bug, fix it. If it's a hardware bug, fix it.
    {
        LOG_ERROR("%s hci layer timeout waiting for response to a command. opcode: 0x%x", __func__, wait_entry->opcode);
    }
}

// Event/packet receiving functions
static void hal_says_packet_ready(BT_HDR *packet)
{
    if (packet->event != MSG_HC_TO_STACK_HCI_EVT) {
        packet_fragmenter->reassemble_and_dispatch(packet);
    } else if (!filter_incoming_event(packet)) {
        dispatch_reassembled(packet);
    }
}

// Returns true if the event was intercepted and should not proceed to
// higher layers. Also inspects an incoming event for interesting
// information, like how many commands are now able to be sent.
static bool filter_incoming_event(BT_HDR *packet)
{
    waiting_command_t *wait_entry = NULL;
    uint8_t *stream = packet->data + packet->offset;
    uint8_t event_code;
    command_opcode_t opcode;

    STREAM_TO_UINT8(event_code, stream);
    STREAM_SKIP_UINT8(stream); // Skip the parameter total length field

    LOG_DEBUG("Receive packet event_code=0x%x\n", event_code);

    if (event_code == HCI_COMMAND_COMPLETE_EVT) {
        STREAM_TO_UINT8(hci_host_env.command_credits, stream);
        STREAM_TO_UINT16(opcode, stream);

        wait_entry = get_waiting_command(opcode);
        if (!wait_entry) {
            LOG_WARN("%s command complete event with no matching command. opcode: 0x%x.", __func__, opcode);
        } else if (wait_entry->complete_callback) {
            wait_entry->complete_callback(packet, wait_entry->context);
        } else if (wait_entry->complete_future) {
            future_ready(wait_entry->complete_future, packet);
        }

        goto intercepted;
    } else if (event_code == HCI_COMMAND_STATUS_EVT) {
        uint8_t status;
        STREAM_TO_UINT8(status, stream);
        STREAM_TO_UINT8(hci_host_env.command_credits, stream);
        STREAM_TO_UINT16(opcode, stream);

        // If a command generates a command status event, it won't be getting a command complete event

        wait_entry = get_waiting_command(opcode);
        if (!wait_entry) {
            LOG_WARN("%s command status event with no matching command. opcode: 0x%x", __func__, opcode);
        } else if (wait_entry->status_callback) {
            wait_entry->status_callback(status, wait_entry->command, wait_entry->context);
        }

        goto intercepted;
    }

    return false;
intercepted:
    restart_comamnd_waiting_response_timer(&hci_host_env.cmd_waiting_q, false);

    /*Tell HCI Host Task to continue TX Pending commands*/
    if (hci_host_env.command_credits &&
            !fixed_queue_is_empty(hci_host_env.command_queue)) {
        hci_host_task_post();
    }
    //ke_event_set(KE_EVENT_HCI_HOST_THREAD);

    if (wait_entry) {
        // If it has a callback, it's responsible for freeing the packet
        if (event_code == HCI_COMMAND_STATUS_EVT ||
                (!wait_entry->complete_callback && !wait_entry->complete_future)) {
            buffer_allocator->free(packet);
        }

        // If it has a callback, it's responsible for freeing the command
        if (event_code == HCI_COMMAND_COMPLETE_EVT || !wait_entry->status_callback) {
            buffer_allocator->free(wait_entry->command);
        }

        osi_free(wait_entry);
    } else {
        buffer_allocator->free(packet);
    }

    return true;
}

// Callback for the fragmenter to dispatch up a completely reassembled packet
static void dispatch_reassembled(BT_HDR *packet)
{
    // Events should already have been dispatched before this point

    if (hci_host_env.upwards_data_queue) {
        fixed_queue_enqueue(hci_host_env.upwards_data_queue, packet);
        btu_task_post(SIG_BTU_WORK);
        //Tell Up-layer received packet.
    } else {
        LOG_DEBUG("%s had no queue to place upwards data packet in. Dropping it on the floor.", __func__);
        buffer_allocator->free(packet);
    }
}

// Misc internal functions

// TODO(zachoverflow): we seem to do this a couple places, like the HCI inject module. #centralize
static serial_data_type_t event_to_data_type(uint16_t event)
{
    if (event == MSG_STACK_TO_HC_HCI_ACL) {
        return DATA_TYPE_ACL;
    } else if (event == MSG_STACK_TO_HC_HCI_SCO) {
        return DATA_TYPE_SCO;
    } else if (event == MSG_STACK_TO_HC_HCI_CMD) {
        return DATA_TYPE_COMMAND;
    } else {
        LOG_ERROR("%s invalid event type, could not translate 0x%x\n", __func__, event);
    }

    return 0;
}

static waiting_command_t *get_waiting_command(command_opcode_t opcode)
{
    command_waiting_response_t *cmd_wait_q = &hci_host_env.cmd_waiting_q;
    pthread_mutex_lock(&cmd_wait_q->commands_pending_response_lock);

    for (const list_node_t *node = list_begin(cmd_wait_q->commands_pending_response);
            node != list_end(cmd_wait_q->commands_pending_response);
            node = list_next(node)) {
        waiting_command_t *wait_entry = list_node(node);
        if (!wait_entry || wait_entry->opcode != opcode) {
            continue;
        }

        list_remove(cmd_wait_q->commands_pending_response, wait_entry);

        pthread_mutex_unlock(&cmd_wait_q->commands_pending_response_lock);
        return wait_entry;
    }

    pthread_mutex_unlock(&cmd_wait_q->commands_pending_response_lock);
    return NULL;
}

static void init_layer_interface()
{
    if (!interface_created) {
        interface.set_data_queue = set_data_queue;
        interface.transmit_command = transmit_command;
        interface.transmit_command_futured = transmit_command_futured;
        interface.transmit_downward = transmit_downward;
        interface_created = true;
    }
}

static const hci_hal_callbacks_t hal_callbacks = {
    hal_says_packet_ready
};

static const packet_fragmenter_callbacks_t packet_fragmenter_callbacks = {
    transmit_fragment,
    dispatch_reassembled,
    fragmenter_transmit_finished
};

const hci_t *hci_layer_get_interface()
{
    buffer_allocator = buffer_allocator_get_interface();
    hal = hci_hal_h4_get_interface();
    packet_fragmenter = packet_fragmenter_get_interface();

    init_layer_interface();
    return &interface;
}

