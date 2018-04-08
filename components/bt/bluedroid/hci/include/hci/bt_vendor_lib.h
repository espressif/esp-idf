/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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

#ifndef BT_VENDOR_LIB_H
#define BT_VENDOR_LIB_H

#include <stdint.h>
//#include <sys/cdefs.h>
//#include <sys/types.h>

/** Struct types */


/** Typedefs and defines */

/** Vendor specific operations OPCODE */
typedef enum {
    /*  [operation]
     *      Power on or off the BT Controller.
     *  [input param]
     *      A pointer to int type with content of bt_vendor_power_state_t.
     *      Typecasting conversion: (int *) param.
     *  [return]
     *      0 - default, don't care.
     *  [callback]
     *      None.
     */
    BT_VND_OP_POWER_CTRL,

    /*  [operation]
     *      Perform any vendor specific initialization or configuration
     *      on the BT Controller. This is called before stack initialization.
     *  [input param]
     *      None.
     *  [return]
     *      0 - default, don't care.
     *  [callback]
     *      Must call fwcfg_cb to notify the stack of the completion of vendor
     *      specific initialization once it has been done.
     */
    BT_VND_OP_FW_CFG,

    /*  [operation]
     *      Perform any vendor specific SCO/PCM configuration on the BT Controller.
     *      This is called after stack initialization.
     *  [input param]
     *      None.
     *  [return]
     *      0 - default, don't care.
     *  [callback]
     *      Must call scocfg_cb to notify the stack of the completion of vendor
     *      specific SCO configuration once it has been done.
     */
    BT_VND_OP_SCO_CFG,

    /*  [operation]
     *      Open UART port on where the BT Controller is attached.
     *      This is called before stack initialization.
     *  [input param]
     *      A pointer to int array type for open file descriptors.
     *      The mapping of HCI channel to fd slot in the int array is given in
     *      bt_vendor_hci_channels_t.
     *      And, it requires the vendor lib to fill up the content before returning
     *      the call.
     *      Typecasting conversion: (int (*)[]) param.
     *  [return]
     *      Numbers of opened file descriptors.
     *      Valid number:
     *          1 - CMD/EVT/ACL-In/ACL-Out via the same fd (e.g. UART)
     *          2 - CMD/EVT on one fd, and ACL-In/ACL-Out on the other fd
     *          4 - CMD, EVT, ACL-In, ACL-Out are on their individual fd
     *  [callback]
     *      None.
     */
    BT_VND_OP_USERIAL_OPEN,

    /*  [operation]
     *      Close the previously opened UART port.
     *  [input param]
     *      None.
     *  [return]
     *      0 - default, don't care.
     *  [callback]
     *      None.
     */
    BT_VND_OP_USERIAL_CLOSE,

    /*  [operation]
     *      Get the LPM idle timeout in milliseconds.
     *      The stack uses this information to launch a timer delay before it
     *      attempts to de-assert LPM WAKE signal once downstream HCI packet
     *      has been delivered.
     *  [input param]
     *      A pointer to uint32_t type which is passed in by the stack. And, it
     *      requires the vendor lib to fill up the content before returning
     *      the call.
     *      Typecasting conversion: (uint32_t *) param.
     *  [return]
     *      0 - default, don't care.
     *  [callback]
     *      None.
     */
    BT_VND_OP_GET_LPM_IDLE_TIMEOUT,

    /*  [operation]
     *      Enable or disable LPM mode on BT Controller.
     *  [input param]
     *      A pointer to uint8_t type with content of bt_vendor_lpm_mode_t.
     *      Typecasting conversion: (uint8_t *) param.
     *  [return]
     *      0 - default, don't care.
     *  [callback]
     *      Must call lpm_cb to notify the stack of the completion of LPM
     *      disable/enable process once it has been done.
     */
    BT_VND_OP_LPM_SET_MODE,

    /*  [operation]
     *      Assert or Deassert LPM WAKE on BT Controller.
     *  [input param]
     *      A pointer to uint8_t type with content of bt_vendor_lpm_wake_state_t.
     *      Typecasting conversion: (uint8_t *) param.
     *  [return]
     *      0 - default, don't care.
     *  [callback]
     *      None.
     */
    BT_VND_OP_LPM_WAKE_SET_STATE,

    /*  [operation]
     *      Perform any vendor specific commands related to audio state changes.
     *  [input param]
     *      a pointer to bt_vendor_op_audio_state_t indicating what audio state is
     *      set.
     *  [return]
     *      0 - default, don't care.
     *  [callback]
     *      None.
     */
    BT_VND_OP_SET_AUDIO_STATE,

    /*  [operation]
     *      The epilog call to the vendor module so that it can perform any
     *      vendor-specific processes (e.g. send a HCI_RESET to BT Controller)
     *      before the caller calls for cleanup().
     *  [input param]
     *      None.
     *  [return]
     *      0 - default, don't care.
     *  [callback]
     *      Must call epilog_cb to notify the stack of the completion of vendor
     *      specific epilog process once it has been done.
     */
    BT_VND_OP_EPILOG,
} bt_vendor_opcode_t;

/** Power on/off control states */
typedef enum {
    BT_VND_PWR_OFF,
    BT_VND_PWR_ON,
}  bt_vendor_power_state_t;

/** Define HCI channel identifier in the file descriptors array
    used in BT_VND_OP_USERIAL_OPEN operation.
 */
typedef enum {
    CH_CMD,     // HCI Command channel
    CH_EVT,     // HCI Event channel
    CH_ACL_OUT, // HCI ACL downstream channel
    CH_ACL_IN,  // HCI ACL upstream channel

    CH_MAX      // Total channels
}  bt_vendor_hci_channels_t;

/** LPM disable/enable request */
typedef enum {
    BT_VND_LPM_DISABLE,
    BT_VND_LPM_ENABLE,
} bt_vendor_lpm_mode_t;

/** LPM WAKE set state request */
typedef enum {
    BT_VND_LPM_WAKE_ASSERT,
    BT_VND_LPM_WAKE_DEASSERT,
} bt_vendor_lpm_wake_state_t;

/** Callback result values */
typedef enum {
    BT_VND_OP_RESULT_SUCCESS,
    BT_VND_OP_RESULT_FAIL,
} bt_vendor_op_result_t;

/** audio (SCO) state changes triggering VS commands for configuration */
typedef struct {
    uint16_t    handle;
    uint16_t    peer_codec;
    uint16_t    state;
} bt_vendor_op_audio_state_t;

/*
 * Bluetooth Host/Controller Vendor callback structure.
 */

/* vendor initialization/configuration callback */
typedef void (*cfg_result_cb)(bt_vendor_op_result_t result);

/* datapath buffer allocation callback (callout)
 *
 *  Vendor lib needs to request a buffer through the alloc callout function
 *  from HCI lib if the buffer is for constructing a HCI Command packet which
 *  will be sent through xmit_cb to BT Controller.
 *
 *  For each buffer allocation, the requested size needs to be big enough to
 *  accommodate the below header plus a complete HCI packet --
 *      typedef struct
 *      {
 *          uint16_t          event;
 *          uint16_t          len;
 *          uint16_t          offset;
 *          uint16_t          layer_specific;
 *      } HC_BT_HDR;
 *
 *  HCI lib returns a pointer to the buffer where Vendor lib should use to
 *  construct a HCI command packet as below format:
 *
 *  --------------------------------------------
 *  |  HC_BT_HDR  |  HCI command               |
 *  --------------------------------------------
 *  where
 *      HC_BT_HDR.event = 0x2000;
 *      HC_BT_HDR.len = Length of HCI command;
 *      HC_BT_HDR.offset = 0;
 *      HC_BT_HDR.layer_specific = 0;
 *
 *  For example, a HCI_RESET Command will be formed as
 *  ------------------------
 *  |  HC_BT_HDR  |03|0c|00|
 *  ------------------------
 *  with
 *      HC_BT_HDR.event = 0x2000;
 *      HC_BT_HDR.len = 3;
 *      HC_BT_HDR.offset = 0;
 *      HC_BT_HDR.layer_specific = 0;
 */
typedef void *(*malloc_cb)(int size);

/* datapath buffer deallocation callback (callout) */
typedef void (*mdealloc_cb)(void *p_buf);

/* define callback of the cmd_xmit_cb
 *
 *  The callback function which HCI lib will call with the return of command
 *  complete packet. Vendor lib is responsible for releasing the buffer passed
 *  in at the p_mem parameter by calling dealloc callout function.
 */
typedef void (*tINT_CMD_CBACK)(void *p_mem);

/* hci command packet transmit callback (callout)
 *
 *  Vendor lib calls xmit_cb callout function in order to send a HCI Command
 *  packet to BT Controller. The buffer carrying HCI Command packet content
 *  needs to be first allocated through the alloc callout function.
 *  HCI lib will release the buffer for Vendor lib once it has delivered the
 *  packet content to BT Controller.
 *
 *  Vendor lib needs also provide a callback function (p_cback) which HCI lib
 *  will call with the return of command complete packet.
 *
 *  The opcode parameter gives the HCI OpCode (combination of OGF and OCF) of
 *  HCI Command packet. For example, opcode = 0x0c03 for the HCI_RESET command
 *  packet.
 */
typedef uint8_t (*cmd_xmit_cb)(uint16_t opcode, void *p_buf, tINT_CMD_CBACK p_cback);

typedef struct {
    /** set to sizeof(bt_vendor_callbacks_t) */
    size_t         size;

    /*
     * Callback and callout functions have implemented in HCI libray
     * (libbt-hci.so).
     */

    /* notifies caller result of firmware configuration request */
    cfg_result_cb  fwcfg_cb;

    /* notifies caller result of sco configuration request */
    cfg_result_cb  scocfg_cb;

    /* notifies caller result of lpm enable/disable */
    cfg_result_cb  lpm_cb;

    /* notifies the result of codec setting */
    cfg_result_cb audio_state_cb;

    /* buffer allocation request */
    malloc_cb   alloc;

    /* buffer deallocation request */
    mdealloc_cb dealloc;

    /* hci command packet transmit request */
    cmd_xmit_cb xmit_cb;

    /* notifies caller completion of epilog process */
    cfg_result_cb epilog_cb;
} bt_vendor_callbacks_t;

/*
 * Bluetooth Host/Controller VENDOR Interface
 */
typedef struct {
    /** Set to sizeof(bt_vndor_interface_t) */
    size_t          size;

    /*
     * Functions need to be implemented in Vendor libray (libbt-vendor.so).
     */

    /**
     * Caller will open the interface and pass in the callback routines
     * to the implemenation of this interface.
     */
    int   (*init)(const bt_vendor_callbacks_t *p_cb, unsigned char *local_bdaddr);

    /**  Vendor specific operations */
    int (*op)(bt_vendor_opcode_t opcode, void *param);

    /** Closes the interface */
    void  (*cleanup)(void);
} bt_vendor_interface_t;


/*
 * External shared lib functions/data
 */

/* Entry point of DLib --
 *      Vendor library needs to implement the body of bt_vendor_interface_t
 *      structure and uses the below name as the variable name. HCI library
 *      will use this symbol name to get address of the object through the
 *      dlsym call.
 */
//extern const bt_vendor_interface_t BLUETOOTH_VENDOR_LIB_INTERFACE;

#endif /* BT_VENDOR_LIB_H */

