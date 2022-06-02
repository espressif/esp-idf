/******************************************************************************
 *
 *  Copyright (C) 1999-2014 Broadcom Corporation
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

/******************************************************************************
 *
 *  This file contains functions that handle inquiries. These include
 *  setting discoverable mode, controlling the mode of the Baseband, and
 *  maintaining a small database of inquiry responses, with API for people
 *  to browse it.
 *
 ******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include "osi/alarm.h"
#include "stack/bt_types.h"
#include "device/controller.h"
#include "stack/hcimsgs.h"
#include "stack/btu.h"
#include "stack/btm_api.h"
#include "btm_int.h"
#include "stack/hcidefs.h"
#if (defined(SDP_INCLUDED) && SDP_INCLUDED == TRUE)
#include "stack/sdpdefs.h"
#endif

#define BTM_INQ_REPLY_TIMEOUT   3       /* 3 second timeout waiting for responses */

/* TRUE to enable DEBUG traces for btm_inq */
#ifndef BTM_INQ_DEBUG
#define BTM_INQ_DEBUG   FALSE
#endif
/********************************************************************************/
/*                 L O C A L    D A T A    D E F I N I T I O N S                */
/********************************************************************************/
static const LAP general_inq_lap = {0x9e, 0x8b, 0x33};
static const LAP limited_inq_lap = {0x9e, 0x8b, 0x00};

#if (defined(SDP_INCLUDED) && SDP_INCLUDED == TRUE)
static const UINT16 BTM_EIR_UUID_LKUP_TBL[BTM_EIR_MAX_SERVICES] = {
    UUID_SERVCLASS_SERVICE_DISCOVERY_SERVER,
    /*    UUID_SERVCLASS_BROWSE_GROUP_DESCRIPTOR,   */
    /*    UUID_SERVCLASS_PUBLIC_BROWSE_GROUP,       */
    UUID_SERVCLASS_SERIAL_PORT,
    UUID_SERVCLASS_LAN_ACCESS_USING_PPP,
    UUID_SERVCLASS_DIALUP_NETWORKING,
    UUID_SERVCLASS_IRMC_SYNC,
    UUID_SERVCLASS_OBEX_OBJECT_PUSH,
    UUID_SERVCLASS_OBEX_FILE_TRANSFER,
    UUID_SERVCLASS_IRMC_SYNC_COMMAND,
    UUID_SERVCLASS_HEADSET,
    UUID_SERVCLASS_CORDLESS_TELEPHONY,
    UUID_SERVCLASS_AUDIO_SOURCE,
    UUID_SERVCLASS_AUDIO_SINK,
    UUID_SERVCLASS_AV_REM_CTRL_TARGET,
    /*    UUID_SERVCLASS_ADV_AUDIO_DISTRIBUTION,    */
    UUID_SERVCLASS_AV_REMOTE_CONTROL,
    /*    UUID_SERVCLASS_VIDEO_CONFERENCING,        */
    UUID_SERVCLASS_INTERCOM,
    UUID_SERVCLASS_FAX,
    UUID_SERVCLASS_HEADSET_AUDIO_GATEWAY,
    /*    UUID_SERVCLASS_WAP,                       */
    /*    UUID_SERVCLASS_WAP_CLIENT,                */
    UUID_SERVCLASS_PANU,
    UUID_SERVCLASS_NAP,
    UUID_SERVCLASS_GN,
    UUID_SERVCLASS_DIRECT_PRINTING,
    /*    UUID_SERVCLASS_REFERENCE_PRINTING,        */
    UUID_SERVCLASS_IMAGING,
    UUID_SERVCLASS_IMAGING_RESPONDER,
    UUID_SERVCLASS_IMAGING_AUTO_ARCHIVE,
    UUID_SERVCLASS_IMAGING_REF_OBJECTS,
    UUID_SERVCLASS_HF_HANDSFREE,
    UUID_SERVCLASS_AG_HANDSFREE,
    UUID_SERVCLASS_DIR_PRT_REF_OBJ_SERVICE,
    /*    UUID_SERVCLASS_REFLECTED_UI,              */
    UUID_SERVCLASS_BASIC_PRINTING,
    UUID_SERVCLASS_PRINTING_STATUS,
    UUID_SERVCLASS_HUMAN_INTERFACE,
    UUID_SERVCLASS_CABLE_REPLACEMENT,
    UUID_SERVCLASS_HCRP_PRINT,
    UUID_SERVCLASS_HCRP_SCAN,
    /*    UUID_SERVCLASS_COMMON_ISDN_ACCESS,        */
    /*    UUID_SERVCLASS_VIDEO_CONFERENCING_GW,     */
    /*    UUID_SERVCLASS_UDI_MT,                    */
    /*    UUID_SERVCLASS_UDI_TA,                    */
    /*    UUID_SERVCLASS_VCP,                       */
    UUID_SERVCLASS_SAP,
    UUID_SERVCLASS_PBAP_PCE,
    UUID_SERVCLASS_PBAP_PSE,
    UUID_SERVCLASS_PHONE_ACCESS,
    UUID_SERVCLASS_HEADSET_HS,
    UUID_SERVCLASS_PNP_INFORMATION,
    /*    UUID_SERVCLASS_GENERIC_NETWORKING,        */
    /*    UUID_SERVCLASS_GENERIC_FILETRANSFER,      */
    /*    UUID_SERVCLASS_GENERIC_AUDIO,             */
    /*    UUID_SERVCLASS_GENERIC_TELEPHONY,         */
    /*    UUID_SERVCLASS_UPNP_SERVICE,              */
    /*    UUID_SERVCLASS_UPNP_IP_SERVICE,           */
    /*    UUID_SERVCLASS_ESDP_UPNP_IP_PAN,          */
    /*    UUID_SERVCLASS_ESDP_UPNP_IP_LAP,          */
    /*    UUID_SERVCLASS_ESDP_UPNP_IP_L2CAP,        */
    UUID_SERVCLASS_VIDEO_SOURCE,
    UUID_SERVCLASS_VIDEO_SINK,
    /*    UUID_SERVCLASS_VIDEO_DISTRIBUTION         */
    UUID_SERVCLASS_MESSAGE_ACCESS,
    UUID_SERVCLASS_MESSAGE_NOTIFICATION,
    UUID_SERVCLASS_HDP_SOURCE,
    UUID_SERVCLASS_HDP_SINK
};
#else
static const UINT16 BTM_EIR_UUID_LKUP_TBL[BTM_EIR_MAX_SERVICES];
#endif

/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/
static void         btm_initiate_inquiry (tBTM_INQUIRY_VAR_ST *p_inq);
static tBTM_STATUS  btm_set_inq_event_filter (UINT8 filter_cond_type, tBTM_INQ_FILT_COND *p_filt_cond);
static void         btm_clr_inq_result_flt (void);

static UINT8        btm_convert_uuid_to_eir_service( UINT16 uuid16 );
static void         btm_set_eir_uuid( UINT8 *p_eir, tBTM_INQ_RESULTS *p_results );
static UINT8       *btm_eir_get_uuid_list( UINT8 *p_eir, UINT8 uuid_size,
        UINT8 *p_num_uuid, UINT8 *p_uuid_list_type );
static UINT16       btm_convert_uuid_to_uuid16( UINT8 *p_uuid, UINT8 uuid_size );

/*******************************************************************************
**
** Function         BTM_SetDiscoverability
**
** Description      This function is called to set the device into or out of
**                  discoverable mode. Discoverable mode means inquiry
**                  scans are enabled.  If a value of '0' is entered for window or
**                  interval, the default values are used.
**
** Returns          BTM_SUCCESS if successful
**                  BTM_BUSY if a setting of the filter is already in progress
**                  BTM_NO_RESOURCES if couldn't get a memory pool buffer
**                  BTM_ILLEGAL_VALUE if a bad parameter was detected
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
tBTM_STATUS BTM_SetDiscoverability (UINT16 inq_mode, UINT16 window, UINT16 interval)
{
    UINT8        scan_mode = 0;
    UINT16       service_class;
    UINT8       *p_cod;
    UINT8        major, minor;
    DEV_CLASS    cod;
    LAP          temp_lap[2];
    BOOLEAN      is_limited;
    BOOLEAN      cod_limited;

    BTM_TRACE_API ("BTM_SetDiscoverability\n");

    /*** Check mode parameter ***/
    if (inq_mode > BTM_MAX_DISCOVERABLE) {
        return (BTM_ILLEGAL_VALUE);
    }

    /* Make sure the controller is active */
    if (!controller_get_interface()->get_is_ready()) {
        return (BTM_DEV_RESET);
    }

    /* If the window and/or interval is '0', set to default values */
    if (!window) {
        window = BTM_DEFAULT_DISC_WINDOW;
    }

    if (!interval) {
        interval = BTM_DEFAULT_DISC_INTERVAL;
    }

    BTM_TRACE_API ("BTM_SetDiscoverability: mode %d [NonDisc-0, Lim-1, Gen-2], window 0x%04x, interval 0x%04x\n",
                   inq_mode, window, interval);

    /*** Check for valid window and interval parameters ***/
    /*** Only check window and duration if mode is connectable ***/
    if (inq_mode != BTM_NON_DISCOVERABLE) {
        /* window must be less than or equal to interval */
        if (window < HCI_MIN_INQUIRYSCAN_WINDOW     ||
                window > HCI_MAX_INQUIRYSCAN_WINDOW     ||
                interval < HCI_MIN_INQUIRYSCAN_INTERVAL ||
                interval > HCI_MAX_INQUIRYSCAN_INTERVAL ||
                window > interval) {
            return (BTM_ILLEGAL_VALUE);
        }
    }

    /* Set the IAC if needed */
    if (inq_mode != BTM_NON_DISCOVERABLE) {
        if (inq_mode & BTM_LIMITED_DISCOVERABLE) {
            /* Use the GIAC and LIAC codes for limited discoverable mode */
            memcpy (temp_lap[0], limited_inq_lap, LAP_LEN);
            memcpy (temp_lap[1], general_inq_lap, LAP_LEN);

            if (!btsnd_hcic_write_cur_iac_lap (2, (LAP * const) temp_lap)) {
                return (BTM_NO_RESOURCES);    /* Cannot continue */
            }
        } else {
            if (!btsnd_hcic_write_cur_iac_lap (1, (LAP * const) &general_inq_lap)) {
                return (BTM_NO_RESOURCES);    /* Cannot continue */
            }
        }

        scan_mode |= HCI_INQUIRY_SCAN_ENABLED;
    }

    /* Send down the inquiry scan window and period if changed */
    if ((window != btm_cb.btm_inq_vars.inq_scan_window) ||
            (interval != btm_cb.btm_inq_vars.inq_scan_period)) {
        if (btsnd_hcic_write_inqscan_cfg (interval, window)) {
            btm_cb.btm_inq_vars.inq_scan_window = window;
            btm_cb.btm_inq_vars.inq_scan_period = interval;
        } else {
            return (BTM_NO_RESOURCES);
        }
    }

    if (btm_cb.btm_inq_vars.connectable_mode & BTM_CONNECTABLE_MASK) {
        scan_mode |= HCI_PAGE_SCAN_ENABLED;
    }

    if (btsnd_hcic_write_scan_enable (scan_mode)) {
        btm_cb.btm_inq_vars.discoverable_mode &= (~BTM_DISCOVERABLE_MASK);
        btm_cb.btm_inq_vars.discoverable_mode |= inq_mode;
    } else {
        return (BTM_NO_RESOURCES);
    }

    /* Change the service class bit if mode has changed */
    p_cod = BTM_ReadDeviceClass();
    BTM_COD_SERVICE_CLASS(service_class, p_cod);
    is_limited = (inq_mode & BTM_LIMITED_DISCOVERABLE) ? TRUE : FALSE;
    cod_limited = (service_class & BTM_COD_SERVICE_LMTD_DISCOVER) ? TRUE : FALSE;
    if (is_limited ^ cod_limited) {
        BTM_COD_MINOR_CLASS(minor, p_cod );
        BTM_COD_MAJOR_CLASS(major, p_cod );
        if (is_limited) {
            service_class |= BTM_COD_SERVICE_LMTD_DISCOVER;
        } else {
            service_class &= ~BTM_COD_SERVICE_LMTD_DISCOVER;
        }

        FIELDS_TO_COD(cod, minor, major, service_class);
        (void) BTM_SetDeviceClass (cod);
    }

    return (BTM_SUCCESS);
}

/*******************************************************************************
**
** Function         BTM_SetInquiryScanType
**
** Description      This function is called to set the iquiry scan-type to
**                  standard or interlaced.
**
** Returns          BTM_SUCCESS if successful
**                  BTM_MODE_UNSUPPORTED if not a 1.2 device
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
tBTM_STATUS BTM_SetInquiryScanType (UINT16 scan_type)
{

    BTM_TRACE_API ("BTM_SetInquiryScanType\n");
    if (scan_type != BTM_SCAN_TYPE_STANDARD && scan_type != BTM_SCAN_TYPE_INTERLACED) {
        return (BTM_ILLEGAL_VALUE);
    }

    /* whatever app wants if device is not 1.2 scan type should be STANDARD */
    if (!controller_get_interface()->supports_interlaced_inquiry_scan()) {
        return (BTM_MODE_UNSUPPORTED);
    }

    /* Check for scan type if configuration has been changed */
    if (scan_type != btm_cb.btm_inq_vars.inq_scan_type) {
        if (BTM_IsDeviceUp()) {
            if (btsnd_hcic_write_inqscan_type ((UINT8)scan_type)) {
                btm_cb.btm_inq_vars.inq_scan_type = scan_type;
            } else {
                return (BTM_NO_RESOURCES);
            }
        } else {
            return (BTM_WRONG_MODE);
        }
    }
    return (BTM_SUCCESS);
}

/*******************************************************************************
**
** Function         BTM_SetPageScanType
**
** Description      This function is called to set the page scan-type to
**                  standard or interlaced.
**
** Returns          BTM_SUCCESS if successful
**                  BTM_MODE_UNSUPPORTED if not a 1.2 device
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
tBTM_STATUS BTM_SetPageScanType (UINT16 scan_type)
{
    BTM_TRACE_API ("BTM_SetPageScanType\n");
    if (scan_type != BTM_SCAN_TYPE_STANDARD && scan_type != BTM_SCAN_TYPE_INTERLACED) {
        return (BTM_ILLEGAL_VALUE);
    }

    /* whatever app wants if device is not 1.2 scan type should be STANDARD */
    if (!controller_get_interface()->supports_interlaced_inquiry_scan()) {
        return (BTM_MODE_UNSUPPORTED);
    }

    /* Check for scan type if configuration has been changed */
    if (scan_type != btm_cb.btm_inq_vars.page_scan_type) {
        if (BTM_IsDeviceUp()) {
            if (btsnd_hcic_write_pagescan_type ((UINT8)scan_type)) {
                btm_cb.btm_inq_vars.page_scan_type  = scan_type;
            } else {
                return (BTM_NO_RESOURCES);
            }
        } else {
            return (BTM_WRONG_MODE);
        }
    }
    return (BTM_SUCCESS);
}


/*******************************************************************************
**
** Function         BTM_SetInquiryMode
**
** Description      This function is called to set standard or with RSSI
**                  mode of the inquiry for local device.
**
** Output Params:   mode - standard, with RSSI, extended
**
** Returns          BTM_SUCCESS if successful
**                  BTM_NO_RESOURCES if couldn't get a memory pool buffer
**                  BTM_ILLEGAL_VALUE if a bad parameter was detected
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
tBTM_STATUS BTM_SetInquiryMode (UINT8 mode)
{
    const controller_t *controller = controller_get_interface();
    BTM_TRACE_API ("BTM_SetInquiryMode\n");
    if (mode == BTM_INQ_RESULT_STANDARD) {
        /* mandatory mode */
    } else if (mode == BTM_INQ_RESULT_WITH_RSSI) {
        if (!controller->supports_rssi_with_inquiry_results()) {
            return (BTM_MODE_UNSUPPORTED);
        }
    } else if (mode == BTM_INQ_RESULT_EXTENDED) {
        if (!controller->supports_extended_inquiry_response()) {
            return (BTM_MODE_UNSUPPORTED);
        }
    } else {
        return (BTM_ILLEGAL_VALUE);
    }

    if (!BTM_IsDeviceUp()) {
        return (BTM_WRONG_MODE);
    }

    if (!btsnd_hcic_write_inquiry_mode (mode)) {
        return (BTM_NO_RESOURCES);
    }

    return (BTM_SUCCESS);
}

/*******************************************************************************
**
** Function         BTM_ReadDiscoverability
**
** Description      This function is called to read the current discoverability
**                  mode of the device.
**
** Output Params:   p_window - current inquiry scan duration
**                  p_interval - current inquiry scan interval
**
** Returns          BTM_NON_DISCOVERABLE, BTM_LIMITED_DISCOVERABLE, or
**                  BTM_GENERAL_DISCOVERABLE
**
*******************************************************************************/
UINT16 BTM_ReadDiscoverability (UINT16 *p_window, UINT16 *p_interval)
{
    BTM_TRACE_API ("BTM_ReadDiscoverability\n");
    if (p_window) {
        *p_window = btm_cb.btm_inq_vars.inq_scan_window;
    }

    if (p_interval) {
        *p_interval = btm_cb.btm_inq_vars.inq_scan_period;
    }

    return (btm_cb.btm_inq_vars.discoverable_mode);
}


/*******************************************************************************
**
** Function         BTM_SetPeriodicInquiryMode
**
** Description      This function is called to set the device periodic inquiry mode.
**                  If the duration is zero, the periodic inquiry mode is cancelled.
**
**                  Note: We currently do not allow concurrent inquiry and periodic inquiry.
**
** Parameters:      p_inqparms - pointer to the inquiry information
**                      mode - GENERAL or LIMITED inquiry
**                      duration - length in 1.28 sec intervals (If '0', the inquiry is CANCELLED)
**                      max_resps - maximum amount of devices to search for before ending the inquiry
**                      filter_cond_type - BTM_CLR_INQUIRY_FILTER, BTM_FILTER_COND_DEVICE_CLASS, or
**                                         BTM_FILTER_COND_BD_ADDR
**                      filter_cond - value for the filter (based on filter_cond_type)
**
**                  max_delay - maximum amount of time between successive inquiries
**                  min_delay - minimum amount of time between successive inquiries
**                  p_results_cb - callback returning pointer to results (tBTM_INQ_RESULTS)
**
** Returns          BTM_CMD_STARTED if successfully started
**                  BTM_ILLEGAL_VALUE if a bad parameter is detected
**                  BTM_NO_RESOURCES if could not allocate a message buffer
**                  BTM_SUCCESS - if cancelling the periodic inquiry
**                  BTM_BUSY - if an inquiry is already active
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
tBTM_STATUS BTM_SetPeriodicInquiryMode (tBTM_INQ_PARMS *p_inqparms, UINT16 max_delay,
                                        UINT16 min_delay, tBTM_INQ_RESULTS_CB *p_results_cb)
{
    tBTM_STATUS  status;
    tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;

    BTM_TRACE_API ("BTM_SetPeriodicInquiryMode: mode: %d, dur: %d, rsps: %d, flt: %d, min: %d, max: %d\n",
                   p_inqparms->mode, p_inqparms->duration, p_inqparms->max_resps,
                   p_inqparms->filter_cond_type, min_delay, max_delay);

    /*** Make sure the device is ready ***/
    if (!BTM_IsDeviceUp()) {
        return (BTM_WRONG_MODE);
    }

    /* Only one active inquiry is allowed in this implementation.
       Also do not allow an inquiry if the inquiry filter is being updated */
    if (p_inq->inq_active || p_inq->inqfilt_active) {
        return (BTM_BUSY);
    }

    /* If illegal parameters return FALSE */
    if (p_inqparms->mode != BTM_GENERAL_INQUIRY &&
            p_inqparms->mode != BTM_LIMITED_INQUIRY) {
        return (BTM_ILLEGAL_VALUE);
    }

    /* Verify the parameters for this command */
    if (p_inqparms->duration < BTM_MIN_INQUIRY_LEN     ||
            p_inqparms->duration > BTM_MAX_INQUIRY_LENGTH  ||
            min_delay <= p_inqparms->duration              ||
            min_delay < BTM_PER_INQ_MIN_MIN_PERIOD         ||
            min_delay > BTM_PER_INQ_MAX_MIN_PERIOD         ||
            max_delay <= min_delay                         ||
            max_delay < BTM_PER_INQ_MIN_MAX_PERIOD)
        /*       max_delay > BTM_PER_INQ_MAX_MAX_PERIOD)*/
        /*  BTM_PER_INQ_MAX_MAX_PERIOD set to 1's in all bits. Condition resulting in false always*/
    {
        return (BTM_ILLEGAL_VALUE);
    }

    /* Save the inquiry parameters to be used upon the completion of setting/clearing the inquiry filter */
    p_inq->inqparms = *p_inqparms;
    p_inq->per_min_delay = min_delay;
    p_inq->per_max_delay = max_delay;
    p_inq->inq_cmpl_info.num_resp = 0;         /* Clear the results counter */
    p_inq->p_inq_results_cb = p_results_cb;

    p_inq->inq_active = (UINT8)((p_inqparms->mode == BTM_LIMITED_INQUIRY) ?
                                (BTM_LIMITED_INQUIRY_ACTIVE | BTM_PERIODIC_INQUIRY_ACTIVE) :
                                (BTM_GENERAL_INQUIRY_ACTIVE | BTM_PERIODIC_INQUIRY_ACTIVE));

    /* If a filter is specified, then save it for later and clear the current filter.
       The setting of the filter is done upon completion of clearing of the previous
       filter.
    */
    if (p_inqparms->filter_cond_type != BTM_CLR_INQUIRY_FILTER) {
        p_inq->state = BTM_INQ_CLR_FILT_STATE;
        p_inqparms->filter_cond_type = BTM_CLR_INQUIRY_FILTER;
    } else { /* The filter is not being used so simply clear it; the inquiry can start after this operation */
        p_inq->state = BTM_INQ_SET_FILT_STATE;
    }

    /* Before beginning the inquiry the current filter must be cleared, so initiate the command */
    if ((status = btm_set_inq_event_filter (p_inqparms->filter_cond_type, &p_inqparms->filter_cond)) != BTM_CMD_STARTED) {
        /* If set filter command is not succesful reset the state */
        p_inq->p_inq_results_cb = NULL;
        p_inq->state = BTM_INQ_INACTIVE_STATE;

    }

    return (status);
}


/*******************************************************************************
**
** Function         BTM_CancelPeriodicInquiry
**
** Description      This function cancels a periodic inquiry
**
** Returns
**                  BTM_NO_RESOURCES if could not allocate a message buffer
**                  BTM_SUCCESS - if cancelling the periodic inquiry
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
tBTM_STATUS BTM_CancelPeriodicInquiry(void)
{
    tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
    tBTM_STATUS          status = BTM_SUCCESS;
    BTM_TRACE_API ("BTM_CancelPeriodicInquiry called\n");

    /*** Make sure the device is ready ***/
    if (!BTM_IsDeviceUp()) {
        return (BTM_WRONG_MODE);
    }

    /* Only cancel if one is active */
    if (btm_cb.btm_inq_vars.inq_active & BTM_PERIODIC_INQUIRY_ACTIVE) {
        btm_cb.btm_inq_vars.inq_active = BTM_INQUIRY_INACTIVE;
        btm_cb.btm_inq_vars.p_inq_results_cb = (tBTM_INQ_RESULTS_CB *) NULL;

        if (!btsnd_hcic_exit_per_inq ()) {
            status = BTM_NO_RESOURCES;
        }

        /* If the event filter is in progress, mark it so that the processing of the return
           event will be ignored */
        if (p_inq->inqfilt_active) {
            p_inq->pending_filt_complete_event++;
        }

        p_inq->inqfilt_active = FALSE;
        p_inq->inq_counter++;
    }

    return (status);
}


/*******************************************************************************
**
** Function         BTM_SetConnectability
**
** Description      This function is called to set the device into or out of
**                  connectable mode. Discoverable mode means page scans enabled.
**
** Returns          BTM_SUCCESS if successful
**                  BTM_ILLEGAL_VALUE if a bad parameter is detected
**                  BTM_NO_RESOURCES if could not allocate a message buffer
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
tBTM_STATUS BTM_SetConnectability (UINT16 page_mode, UINT16 window, UINT16 interval)
{
    UINT8    scan_mode = 0;
    tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;

    BTM_TRACE_API ("BTM_SetConnectability\n");

    /*** Check mode parameter ***/
    if (page_mode != BTM_NON_CONNECTABLE && page_mode != BTM_CONNECTABLE) {
        return (BTM_ILLEGAL_VALUE);
    }

    /* Make sure the controller is active */
    if (!controller_get_interface()->get_is_ready()) {
        return (BTM_DEV_RESET);
    }

    /* If the window and/or interval is '0', set to default values */
    if (!window) {
        window = BTM_DEFAULT_CONN_WINDOW;
    }

    if (!interval) {
        interval = BTM_DEFAULT_CONN_INTERVAL;
    }

    BTM_TRACE_API ("BTM_SetConnectability: mode %d [NonConn-0, Conn-1], window 0x%04x, interval 0x%04x\n",
                   page_mode, window, interval);

    /*** Check for valid window and interval parameters ***/
    /*** Only check window and duration if mode is connectable ***/
    if (page_mode == BTM_CONNECTABLE) {
        /* window must be less than or equal to interval */
        if (window < HCI_MIN_PAGESCAN_WINDOW     ||
                window > HCI_MAX_PAGESCAN_WINDOW     ||
                interval < HCI_MIN_PAGESCAN_INTERVAL ||
                interval > HCI_MAX_PAGESCAN_INTERVAL ||
                window > interval) {
            return (BTM_ILLEGAL_VALUE);
        }

        scan_mode |= HCI_PAGE_SCAN_ENABLED;
    }

    if ((window != p_inq->page_scan_window) ||
            (interval != p_inq->page_scan_period)) {
        p_inq->page_scan_window = window;
        p_inq->page_scan_period = interval;
        if (!btsnd_hcic_write_pagescan_cfg (interval, window)) {
            return (BTM_NO_RESOURCES);
        }
    }

    /* Keep the inquiry scan as previouosly set */
    if (p_inq->discoverable_mode & BTM_DISCOVERABLE_MASK) {
        scan_mode |= HCI_INQUIRY_SCAN_ENABLED;
    }

    if (btsnd_hcic_write_scan_enable (scan_mode)) {
        p_inq->connectable_mode &= (~BTM_CONNECTABLE_MASK);
        p_inq->connectable_mode |= page_mode;

        return (BTM_SUCCESS);
    }

    return (BTM_NO_RESOURCES);
}


/*******************************************************************************
**
** Function         BTM_ReadConnectability
**
** Description      This function is called to read the current discoverability
**                  mode of the device.
** Output Params    p_window - current page scan duration
**                  p_interval - current time between page scans
**
** Returns          BTM_NON_CONNECTABLE or BTM_CONNECTABLE
**
*******************************************************************************/
UINT16 BTM_ReadConnectability (UINT16 *p_window, UINT16 *p_interval)
{
    BTM_TRACE_API ("BTM_ReadConnectability\n");
    if (p_window) {
        *p_window = btm_cb.btm_inq_vars.page_scan_window;
    }

    if (p_interval) {
        *p_interval = btm_cb.btm_inq_vars.page_scan_period;
    }

    return (btm_cb.btm_inq_vars.connectable_mode);
}



/*******************************************************************************
**
** Function         BTM_IsInquiryActive
**
** Description      This function returns a bit mask of the current inquiry state
**
** Returns          BTM_INQUIRY_INACTIVE if inactive (0)
**                  BTM_LIMITED_INQUIRY_ACTIVE if a limted inquiry is active
**                  BTM_GENERAL_INQUIRY_ACTIVE if a general inquiry is active
**                  BTM_PERIODIC_INQUIRY_ACTIVE if a periodic inquiry is active
**
*******************************************************************************/
UINT16 BTM_IsInquiryActive (void)
{
    BTM_TRACE_API ("BTM_IsInquiryActive\n");

    return (btm_cb.btm_inq_vars.inq_active);
}



/*******************************************************************************
**
** Function         BTM_CancelInquiry
**
** Description      This function cancels an inquiry if active
**
** Returns          BTM_SUCCESS if successful
**                  BTM_NO_RESOURCES if could not allocate a message buffer
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
tBTM_STATUS BTM_CancelInquiry(void)
{
    tBTM_STATUS           status = BTM_SUCCESS;
    tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
    UINT8 active_mode = p_inq->inq_active;
#endif
    BTM_TRACE_API ("BTM_CancelInquiry called\n");

    /*** Make sure the device is ready ***/
    if (!BTM_IsDeviceUp()) {
        return (BTM_WRONG_MODE);
    }

    /* Only cancel if not in periodic mode, otherwise the caller should call BTM_CancelPeriodicMode */
    if ((p_inq->inq_active & BTM_INQUIRY_ACTIVE_MASK) != 0 &&
            (!(p_inq->inq_active & BTM_PERIODIC_INQUIRY_ACTIVE))) {
        p_inq->inq_active = BTM_INQUIRY_INACTIVE;
        p_inq->state = BTM_INQ_INACTIVE_STATE;
        p_inq->p_inq_results_cb = (tBTM_INQ_RESULTS_CB *) NULL; /* Do not notify caller anymore */
        p_inq->p_inq_cmpl_cb = (tBTM_CMPL_CB *) NULL;    /* Do not notify caller anymore */

        /* If the event filter is in progress, mark it so that the processing of the return
            event will be ignored */
        if (p_inq->inqfilt_active) {
            p_inq->inqfilt_active = FALSE;
            p_inq->pending_filt_complete_event++;
        }
        /* Initiate the cancel inquiry */
        else {
            if (((p_inq->inqparms.mode & BTM_BR_INQUIRY_MASK) != 0)
#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
                    && (active_mode & BTM_BR_INQUIRY_MASK)
#endif
               ) {
                if (!btsnd_hcic_inq_cancel()) {
                    status = BTM_NO_RESOURCES;
                }
            }
#if BLE_INCLUDED == TRUE
            if (((p_inq->inqparms.mode & BTM_BLE_INQUIRY_MASK) != 0)
#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
                    && (active_mode & BTM_BLE_INQ_ACTIVE_MASK)
#endif
               ) {
                btm_ble_stop_inquiry();
            }
#endif
        }

        /* Do not send the BUSY_LEVEL event yet. Wait for the cancel_complete event
         * and then send the BUSY_LEVEL event
         * btm_acl_update_busy_level (BTM_BLI_INQ_DONE_EVT);
         */

        p_inq->inq_counter++;
        btm_clr_inq_result_flt();
    }

    return (status);
}


/*******************************************************************************
**
** Function         BTM_StartInquiry
**
** Description      This function is called to start an inquiry.
**
** Parameters:      p_inqparms - pointer to the inquiry information
**                      mode - GENERAL or LIMITED inquiry, BR/LE bit mask seperately
**                      duration - length in 1.28 sec intervals (If '0', the inquiry is CANCELLED)
**                      max_resps - maximum amount of devices to search for before ending the inquiry
**                      filter_cond_type - BTM_CLR_INQUIRY_FILTER, BTM_FILTER_COND_DEVICE_CLASS, or
**                                         BTM_FILTER_COND_BD_ADDR
**                      filter_cond - value for the filter (based on filter_cond_type)
**
**                  p_results_cb   - Pointer to the callback routine which gets called
**                                upon receipt of an inquiry result. If this field is
**                                NULL, the application is not notified.
**
**                  p_cmpl_cb   - Pointer to the callback routine which gets called
**                                upon completion.  If this field is NULL, the
**                                application is not notified when completed.
** Returns          tBTM_STATUS
**                  BTM_CMD_STARTED if successfully initiated
**                  BTM_BUSY if already in progress
**                  BTM_ILLEGAL_VALUE if parameter(s) are out of range
**                  BTM_NO_RESOURCES if could not allocate resources to start the command
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
tBTM_STATUS BTM_StartInquiry (tBTM_INQ_PARMS *p_inqparms, tBTM_INQ_RESULTS_CB *p_results_cb,
                              tBTM_CMPL_CB *p_cmpl_cb)
{
    tBTM_STATUS  status = BTM_CMD_STARTED;
    tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;

    BTM_TRACE_API ("BTM_StartInquiry: mode: %d, dur: %d, rsps: %d, flt: %d\n",
                   p_inqparms->mode, p_inqparms->duration, p_inqparms->max_resps,
                   p_inqparms->filter_cond_type);

    /* Only one active inquiry is allowed in this implementation.
       Also do not allow an inquiry if the inquiry filter is being updated */
    if (p_inq->inq_active || p_inq->inqfilt_active) {
#if (defined BLE_INCLUDED && BLE_INCLUDED == TRUE)
        /*check if LE observe is already running*/
        if (p_inq->scan_type == INQ_LE_OBSERVE && p_inq->p_inq_ble_results_cb != NULL) {
            BTM_TRACE_API("BTM_StartInquiry: LE observe in progress");
            p_inq->scan_type = INQ_GENERAL;
            p_inq->inq_active = BTM_INQUIRY_INACTIVE;
            btm_cb.ble_ctr_cb.inq_var.scan_type = BTM_BLE_SCAN_MODE_NONE;
            btsnd_hcic_ble_set_scan_enable (BTM_BLE_SCAN_DISABLE, BTM_BLE_DUPLICATE_ENABLE);
        } else
#endif
        {
            return (BTM_BUSY);
            BTM_TRACE_API("BTM_StartInquiry: return BUSY\n");
        }
    } else {
        p_inq->scan_type = INQ_GENERAL;
    }

    /*** Make sure the device is ready ***/
    if (!BTM_IsDeviceUp()) {
        return (BTM_WRONG_MODE);
    }

    if ((p_inqparms->mode & BTM_BR_INQUIRY_MASK) != BTM_GENERAL_INQUIRY &&
            (p_inqparms->mode & BTM_BR_INQUIRY_MASK) != BTM_LIMITED_INQUIRY
#if (BLE_INCLUDED == TRUE)
            && (p_inqparms->mode & BTM_BLE_INQUIRY_MASK) != BTM_BLE_GENERAL_INQUIRY
            && (p_inqparms->mode & BTM_BLE_INQUIRY_MASK) != BTM_BLE_LIMITED_INQUIRY
#endif
       ) {
        return (BTM_ILLEGAL_VALUE);
    }

#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
    if (p_inq->next_state == BTM_FINISH) {
        return BTM_ILLEGAL_VALUE;
    }
#endif


    /* Save the inquiry parameters to be used upon the completion of setting/clearing the inquiry filter */
    p_inq->inqparms = *p_inqparms;

    /* Initialize the inquiry variables */
    p_inq->state = BTM_INQ_ACTIVE_STATE;
    p_inq->p_inq_cmpl_cb = p_cmpl_cb;
    p_inq->p_inq_results_cb = p_results_cb;
    p_inq->inq_cmpl_info.num_resp = 0;         /* Clear the results counter */
    p_inq->inq_active = p_inqparms->mode;

    BTM_TRACE_DEBUG("BTM_StartInquiry: p_inq->inq_active = 0x%02x\n", p_inq->inq_active);

    /* interleave scan minimal conditions */
#if (BLE_INCLUDED==TRUE && (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE))

    /* check if both modes are present */
    if ((p_inqparms->mode & BTM_BLE_INQUIRY_MASK) && (p_inqparms->mode & BTM_BR_INQUIRY_MASK)) {
        BTM_TRACE_API("BTM:Interleave Inquiry Mode Set\n");
        p_inqparms->duration = p_inqparms->intl_duration[p_inq->next_state];
        p_inq->inqparms.duration = p_inqparms->duration;
    } else {
        BTM_TRACE_API("BTM:Single Mode: No interleaving, Mode:0x%02x\n", p_inqparms->mode);
        p_inq->next_state = BTM_NO_INTERLEAVING;
    }
#endif



    /* start LE inquiry here if requested */
#if BLE_INCLUDED == TRUE
    if ((p_inqparms->mode & BTM_BLE_INQUIRY_MASK)
#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
            && (p_inq->next_state == BTM_BLE_ONE || p_inq->next_state == BTM_BLE_TWO ||
                p_inq->next_state == BTM_NO_INTERLEAVING)
#endif
       )

    {
#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
        p_inq->inq_active = (p_inqparms->mode & BTM_BLE_INQUIRY_MASK);
        BTM_TRACE_API("BTM:Starting LE Scan with duration %d and activeMode:0x%02x\n",
                      p_inqparms->duration, (p_inqparms->mode & BTM_BLE_INQUIRY_MASK));
#endif
        if (!controller_get_interface()->supports_ble()) {
            p_inq->inqparms.mode &= ~ BTM_BLE_INQUIRY_MASK;
            status = BTM_ILLEGAL_VALUE;
        }
        /* BLE for now does not support filter condition for inquiry */
        else if ((status = btm_ble_start_inquiry((UINT8)(p_inqparms->mode & BTM_BLE_INQUIRY_MASK),
                           p_inqparms->duration)) != BTM_CMD_STARTED) {
            BTM_TRACE_ERROR("Err Starting LE Inquiry.\n");
            p_inq->inqparms.mode &= ~ BTM_BLE_INQUIRY_MASK;
        }
#if (!defined(BTA_HOST_INTERLEAVE_SEARCH) || BTA_HOST_INTERLEAVE_SEARCH == FALSE)
        p_inqparms->mode &= ~BTM_BLE_INQUIRY_MASK;
#endif

#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
        if (p_inq->next_state == BTM_NO_INTERLEAVING) {
            p_inq->next_state = BTM_FINISH;
        } else {
            BTM_TRACE_API("BTM:Interleaving: started LE scan, Advancing to next state: %d\n",
                          p_inq->next_state + 1);
            p_inq->next_state += 1;
        }
        /* reset next_state if status <> BTM_Started */
        if (status != BTM_CMD_STARTED) {
            p_inq->next_state = BTM_BR_ONE;
        }

        /* if interleave scan..return here */
        return status;
#endif


        BTM_TRACE_DEBUG("BTM_StartInquiry: mode = %02x\n", p_inqparms->mode);
    }
#endif /* end of BLE_INCLUDED */

    /* we're done with this routine if BR/EDR inquiry is not desired. */
    if ((p_inqparms->mode & BTM_BR_INQUIRY_MASK) == BTM_INQUIRY_NONE) {
        return status;
    }

    /* BR/EDR inquiry portion */
#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
    if ((p_inq->next_state == BTM_BR_ONE || p_inq->next_state == BTM_BR_TWO ||
            p_inq->next_state == BTM_NO_INTERLEAVING )) {
        p_inq->inq_active = (p_inqparms->mode & BTM_BR_INQUIRY_MASK);
#endif
        /* If a filter is specified, then save it for later and clear the current filter.
           The setting of the filter is done upon completion of clearing of the previous
           filter.
        */
        switch (p_inqparms->filter_cond_type) {
        case BTM_CLR_INQUIRY_FILTER:
            p_inq->state = BTM_INQ_SET_FILT_STATE;
            break;

        case BTM_FILTER_COND_DEVICE_CLASS:
        case BTM_FILTER_COND_BD_ADDR:
            /* The filter is not being used so simply clear it;
                the inquiry can start after this operation */
            p_inq->state = BTM_INQ_CLR_FILT_STATE;
            p_inqparms->filter_cond_type = BTM_CLR_INQUIRY_FILTER;
            /* =============>>>> adding LE filtering here ????? */
            break;

        default:
            return (BTM_ILLEGAL_VALUE);
        }

        /* Before beginning the inquiry the current filter must be cleared, so initiate the command */
        if ((status = btm_set_inq_event_filter (p_inqparms->filter_cond_type,
                                                &p_inqparms->filter_cond)) != BTM_CMD_STARTED) {
            p_inq->state = BTM_INQ_INACTIVE_STATE;
        }

#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
        if (p_inq->next_state == BTM_NO_INTERLEAVING) {
            p_inq->next_state = BTM_FINISH;
        } else {
            BTM_TRACE_API("BTM:Interleaving: Started BTM inq, Advancing to next state: %d\n",
                          p_inq->next_state + 1);
            p_inq->next_state += 1;
        }
    }
    if (status != BTM_CMD_STARTED) {
        /* Some error beginning the scan process.
           Reset the next_state parameter.. Do we need to reset the inq_active also?
        */
        BTM_TRACE_API("BTM:Interleaving: Error in Starting inquiry, status: 0x%02x\n", status);
        p_inq->next_state = BTM_BR_ONE;
    }
#endif


    return (status);
}


/*******************************************************************************
**
** Function         BTM_ReadRemoteDeviceName
**
** Description      This function initiates a remote device HCI command to the
**                  controller and calls the callback when the process has completed.
**
** Input Params:    remote_bda      - device address of name to retrieve
**                  p_cb            - callback function called when BTM_CMD_STARTED
**                                    is returned.
**                                    A pointer to tBTM_REMOTE_DEV_NAME is passed to the
**                                    callback.
**
** Returns
**                  BTM_CMD_STARTED is returned if the request was successfully sent
**                                  to HCI.
**                  BTM_BUSY if already in progress
**                  BTM_UNKNOWN_ADDR if device address is bad
**                  BTM_NO_RESOURCES if could not allocate resources to start the command
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
tBTM_STATUS  BTM_ReadRemoteDeviceName (BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb
                                       , tBT_TRANSPORT transport)
{
    tBTM_INQ_INFO   *p_cur = NULL;
    tINQ_DB_ENT     *p_i;

    BTM_TRACE_API ("BTM_ReadRemoteDeviceName: bd addr [%02x%02x%02x%02x%02x%02x]\n",
                   remote_bda[0], remote_bda[1], remote_bda[2],
                   remote_bda[3], remote_bda[4], remote_bda[5]);
    
    /* Use the remote device's clock offset if it is in the local inquiry database */
    if ((p_i = btm_inq_db_find (remote_bda)) != NULL) {
        p_cur = &p_i->inq_info;
    }
    BTM_TRACE_API ("no device found in inquiry db\n");

#if (BLE_INCLUDED == TRUE)
    if (transport == BT_TRANSPORT_LE) {
        return btm_ble_read_remote_name(remote_bda, p_cur, p_cb);
    } else
#endif
    {
        return (btm_initiate_rem_name (remote_bda, p_cur, BTM_RMT_NAME_EXT,
                                       BTM_EXT_RMT_NAME_TIMEOUT, p_cb));
    }
}

/*******************************************************************************
**
** Function         BTM_CancelRemoteDeviceName
**
** Description      This function initiates the cancel request for the specified
**                  remote device.
**
** Input Params:    None
**
** Returns
**                  BTM_CMD_STARTED is returned if the request was successfully sent
**                                  to HCI.
**                  BTM_NO_RESOURCES if could not allocate resources to start the command
**                  BTM_WRONG_MODE if there is not an active remote name request.
**
*******************************************************************************/
tBTM_STATUS  BTM_CancelRemoteDeviceName (void)
{
    tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;

    BTM_TRACE_API ("BTM_CancelRemoteDeviceName()\n");

    /* Make sure there is not already one in progress */
    if (p_inq->remname_active) {
#if BLE_INCLUDED == TRUE
        if (BTM_UseLeLink(p_inq->remname_bda)) {
            if (btm_ble_cancel_remote_name(p_inq->remname_bda)) {
                return (BTM_CMD_STARTED);
            } else {
                return (BTM_UNKNOWN_ADDR);
            }
        } else
#endif
        {
            if (btsnd_hcic_rmt_name_req_cancel (p_inq->remname_bda)) {
                return (BTM_CMD_STARTED);
            } else {
                return (BTM_NO_RESOURCES);
            }
        }
    } else {
        return (BTM_WRONG_MODE);
    }
}

/*******************************************************************************
**
** Function         BTM_InqDbRead
**
** Description      This function looks through the inquiry database for a match
**                  based on Bluetooth Device Address. This is the application's
**                  interface to get the inquiry details of a specific BD address.
**
** Returns          pointer to entry, or NULL if not found
**
*******************************************************************************/
tBTM_INQ_INFO *BTM_InqDbRead (BD_ADDR p_bda)
{
    BTM_TRACE_API ("BTM_InqDbRead: bd addr [%02x%02x%02x%02x%02x%02x]\n",
                   p_bda[0], p_bda[1], p_bda[2], p_bda[3], p_bda[4], p_bda[5]);

    tINQ_DB_ENT *p_ent = btm_inq_db_find(p_bda);
    if (!p_ent) {
        return NULL;
    }

    return &p_ent->inq_info;
}


/*******************************************************************************
**
** Function         BTM_InqDbFirst
**
** Description      This function looks through the inquiry database for the first
**                  used entry, and returns that. This is used in conjunction with
**                  BTM_InqDbNext by applications as a way to walk through the
**                  inquiry database.
**
** Returns          pointer to first in-use entry, or NULL if DB is empty
**
*******************************************************************************/
tBTM_INQ_INFO *BTM_InqDbFirst (void)
{
    UINT16       xx;
    tINQ_DB_ENT  *p_ent = btm_cb.btm_inq_vars.inq_db;

    for (xx = 0; xx < BTM_INQ_DB_SIZE; xx++, p_ent++) {
        if (p_ent->in_use) {
            return (&p_ent->inq_info);
        }
    }

    /* If here, no used entry found */
    return ((tBTM_INQ_INFO *)NULL);
}


/*******************************************************************************
**
** Function         BTM_InqDbNext
**
** Description      This function looks through the inquiry database for the next
**                  used entry, and returns that.  If the input parameter is NULL,
**                  the first entry is returned.
**
** Returns          pointer to next in-use entry, or NULL if no more found.
**
*******************************************************************************/
tBTM_INQ_INFO *BTM_InqDbNext (tBTM_INQ_INFO *p_cur)
{
    tINQ_DB_ENT  *p_ent;
    UINT16        inx;

    if (p_cur) {
        p_ent = (tINQ_DB_ENT *) ((UINT8 *)p_cur - offsetof (tINQ_DB_ENT, inq_info));
        inx = (UINT16)((p_ent - btm_cb.btm_inq_vars.inq_db) + 1);

        for (p_ent = &btm_cb.btm_inq_vars.inq_db[inx]; inx < BTM_INQ_DB_SIZE; inx++, p_ent++) {
            if (p_ent->in_use) {
                return (&p_ent->inq_info);
            }
        }

        /* If here, more entries found */
        return ((tBTM_INQ_INFO *)NULL);
    } else {
        return (BTM_InqDbFirst());
    }
}


/*******************************************************************************
**
** Function         BTM_ClearInqDb
**
** Description      This function is called to clear out a device or all devices
**                  from the inquiry database.
**
** Parameter        p_bda - (input) BD_ADDR ->  Address of device to clear
**                                              (NULL clears all entries)
**
** Returns          BTM_BUSY if an inquiry, get remote name, or event filter
**                          is active, otherwise BTM_SUCCESS
**
*******************************************************************************/
tBTM_STATUS BTM_ClearInqDb (BD_ADDR p_bda)
{
    tBTM_INQUIRY_VAR_ST     *p_inq = &btm_cb.btm_inq_vars;

    /* If an inquiry or remote name is in progress return busy */
    if (p_inq->inq_active != BTM_INQUIRY_INACTIVE ||
            p_inq->inqfilt_active) {
        return (BTM_BUSY);
    }

    btm_clr_inq_db(p_bda);

    return (BTM_SUCCESS);
}

/*******************************************************************************
**
** Function         BTM_ReadInquiryRspTxPower
**
** Description      This command will read the inquiry Transmit Power level used
**                  to transmit the FHS and EIR data packets.
**                  This can be used directly in the Tx Power Level EIR data type.
**
** Returns          BTM_SUCCESS if successful
**
*******************************************************************************/
tBTM_STATUS BTM_ReadInquiryRspTxPower (tBTM_CMPL_CB *p_cb)
{
    if (btm_cb.devcb.p_txpwer_cmpl_cb) {
        return (BTM_BUSY);
    }

    btu_start_timer (&btm_cb.devcb.txpwer_timer, BTU_TTYPE_BTM_ACL, BTM_INQ_REPLY_TIMEOUT );


    btm_cb.devcb.p_txpwer_cmpl_cb = p_cb;

    if (!btsnd_hcic_read_inq_tx_power ()) {
        btm_cb.devcb.p_txpwer_cmpl_cb = NULL;
        btu_stop_timer (&btm_cb.devcb.txpwer_timer);
        return (BTM_NO_RESOURCES);
    } else {
        return (BTM_CMD_STARTED);
    }
}

/*********************************************************************************
**********************************************************************************
**                                                                              **
**                      BTM Internal Inquiry Functions                          **
**                                                                              **
**********************************************************************************
*********************************************************************************/
/*******************************************************************************
**
** Function         btm_inq_db_reset
**
** Description      This function is called at at reset to clear the inquiry
**                  database & pending callback.
**
** Returns          void
**
*******************************************************************************/
void btm_inq_db_reset (void)
{
    tBTM_REMOTE_DEV_NAME     rem_name;
    tBTM_INQUIRY_VAR_ST     *p_inq = &btm_cb.btm_inq_vars;
    UINT8                    num_responses;
    UINT8                    temp_inq_active;
    tBTM_STATUS              status;

    btu_stop_timer (&p_inq->inq_timer_ent);

    /* If an inquiry or periodic inquiry is active, reset the mode to inactive */
    if (p_inq->inq_active != BTM_INQUIRY_INACTIVE) {
        temp_inq_active = p_inq->inq_active;    /* Save so state can change BEFORE
                                                       callback is called */
        p_inq->inq_active = BTM_INQUIRY_INACTIVE;

        /* If not a periodic inquiry, the complete callback must be called to notify caller */
        if (temp_inq_active == BTM_LIMITED_INQUIRY_ACTIVE ||
                temp_inq_active == BTM_GENERAL_INQUIRY_ACTIVE) {
            if (p_inq->p_inq_cmpl_cb) {
                num_responses = 0;
                (*p_inq->p_inq_cmpl_cb)(&num_responses);
            }
        }
    }

    /* Cancel a remote name request if active, and notify the caller (if waiting) */
    if (p_inq->remname_active ) {
        btu_stop_timer (&p_inq->rmt_name_timer_ent);
        p_inq->remname_active = FALSE;
        memset(p_inq->remname_bda, 0, BD_ADDR_LEN);

        if (p_inq->p_remname_cmpl_cb) {
            rem_name.status = BTM_DEV_RESET;

            (*p_inq->p_remname_cmpl_cb)(&rem_name);
            p_inq->p_remname_cmpl_cb = NULL;
        }
    }

    /* Cancel an inquiry filter request if active, and notify the caller (if waiting) */
    if (p_inq->inqfilt_active) {
        p_inq->inqfilt_active = FALSE;

        if (p_inq->p_inqfilter_cmpl_cb) {
            status = BTM_DEV_RESET;
            (*p_inq->p_inqfilter_cmpl_cb)(&status);
        }
    }

    p_inq->state = BTM_INQ_INACTIVE_STATE;
    p_inq->pending_filt_complete_event = 0;
    p_inq->p_inq_results_cb = NULL;
    btm_clr_inq_db(NULL);   /* Clear out all the entries in the database */
    btm_clr_inq_result_flt();

    p_inq->discoverable_mode = BTM_NON_DISCOVERABLE;
    p_inq->connectable_mode  = BTM_NON_CONNECTABLE;
    p_inq->page_scan_type    = BTM_SCAN_TYPE_STANDARD;
    p_inq->inq_scan_type     = BTM_SCAN_TYPE_STANDARD;

#if BLE_INCLUDED == TRUE
    p_inq->discoverable_mode |= BTM_BLE_NON_DISCOVERABLE;
    p_inq->connectable_mode  |= BTM_BLE_NON_CONNECTABLE;
#endif
    return;
}


/*********************************************************************************
**
** Function         btm_inq_db_init
**
** Description      This function is called at startup to initialize the inquiry
**                  database.
**
** Returns          void
**
*******************************************************************************/
void btm_inq_db_init (void)
{
#if 0  /* cleared in btm_init; put back in if called from anywhere else! */
    memset (&btm_cb.btm_inq_vars, 0, sizeof (tBTM_INQUIRY_VAR_ST));
#endif

    btu_free_timer(&btm_cb.btm_inq_vars.rmt_name_timer_ent);
    memset(&btm_cb.btm_inq_vars.rmt_name_timer_ent, 0, sizeof(TIMER_LIST_ENT));
    btu_free_timer(&btm_cb.btm_inq_vars.inq_timer_ent);
    memset(&btm_cb.btm_inq_vars.inq_timer_ent, 0, sizeof(TIMER_LIST_ENT));

    btm_cb.btm_inq_vars.no_inc_ssp = BTM_NO_SSP_ON_INQUIRY;
}

/*********************************************************************************
**
** Function         btm_inq_stop_on_ssp
**
** Description      This function is called on incoming SSP
**
** Returns          void
**
*******************************************************************************/
void btm_inq_stop_on_ssp(void)
{
    UINT8 normal_active = (BTM_GENERAL_INQUIRY_ACTIVE | BTM_LIMITED_INQUIRY_ACTIVE);

#if (BTM_INQ_DEBUG == TRUE)
    BTM_TRACE_DEBUG ("btm_inq_stop_on_ssp: no_inc_ssp=%d inq_active:0x%x state:%d inqfilt_active:%d\n",
                     btm_cb.btm_inq_vars.no_inc_ssp, btm_cb.btm_inq_vars.inq_active, btm_cb.btm_inq_vars.state, btm_cb.btm_inq_vars.inqfilt_active);
#endif
    if (btm_cb.btm_inq_vars.no_inc_ssp) {
        if (btm_cb.btm_inq_vars.state == BTM_INQ_ACTIVE_STATE) {
            if (btm_cb.btm_inq_vars.inq_active & BTM_PERIODIC_INQUIRY_ACTIVE) {
                BTM_CancelPeriodicInquiry();
            } else if (btm_cb.btm_inq_vars.inq_active & normal_active) {
                /* can not call BTM_CancelInquiry() here. We need to report inquiry complete evt */
                btsnd_hcic_inq_cancel();
            }
        }
        /* do not allow inquiry to start */
        btm_cb.btm_inq_vars.inq_active |= BTM_SSP_INQUIRY_ACTIVE;
    }
}

/*********************************************************************************
**
** Function         btm_inq_clear_ssp
**
** Description      This function is called when pairing_state becomes idle
**
** Returns          void
**
*******************************************************************************/
void btm_inq_clear_ssp(void)
{
    btm_cb.btm_inq_vars.inq_active &= ~BTM_SSP_INQUIRY_ACTIVE;
}

/*********************************************************************************
**
** Function         btm_clr_inq_db
**
** Description      This function is called to clear out a device or all devices
**                  from the inquiry database.
**
** Parameter        p_bda - (input) BD_ADDR ->  Address of device to clear
**                                              (NULL clears all entries)
**
** Returns          void
**
*******************************************************************************/
void btm_clr_inq_db (BD_ADDR p_bda)
{
    tBTM_INQUIRY_VAR_ST     *p_inq = &btm_cb.btm_inq_vars;
    tINQ_DB_ENT             *p_ent = p_inq->inq_db;
    UINT16                   xx;

#if (BTM_INQ_DEBUG == TRUE)
    BTM_TRACE_DEBUG ("btm_clr_inq_db: inq_active:0x%x state:%d\n",
                     btm_cb.btm_inq_vars.inq_active, btm_cb.btm_inq_vars.state);
#endif
    for (xx = 0; xx < BTM_INQ_DB_SIZE; xx++, p_ent++) {
        if (p_ent->in_use) {
            /* If this is the specified BD_ADDR or clearing all devices */
            if (p_bda == NULL ||
                    (!memcmp (p_ent->inq_info.results.remote_bd_addr, p_bda, BD_ADDR_LEN))) {
                p_ent->in_use = FALSE;
            }
        }
    }
#if (BTM_INQ_DEBUG == TRUE)
    BTM_TRACE_DEBUG ("inq_active:0x%x state:%d\n",
                     btm_cb.btm_inq_vars.inq_active, btm_cb.btm_inq_vars.state);
#endif
}


/*******************************************************************************
**
** Function         btm_clr_inq_result_flt
**
** Description      This function looks through the bdaddr database for a match
**                  based on Bluetooth Device Address
**
** Returns          TRUE if found, else FALSE (new entry)
**
*******************************************************************************/
static void btm_clr_inq_result_flt (void)
{
    tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;

    if (p_inq->p_bd_db) {
        osi_free(p_inq->p_bd_db);
        p_inq->p_bd_db = NULL;
    }
    p_inq->num_bd_entries = 0;
    p_inq->max_bd_entries = 0;
}

/*******************************************************************************
**
** Function         btm_inq_find_bdaddr
**
** Description      This function looks through the bdaddr database for a match
**                  based on Bluetooth Device Address
**
** Returns          TRUE if found, else FALSE (new entry)
**
*******************************************************************************/
BOOLEAN btm_inq_find_bdaddr (BD_ADDR p_bda)
{
    tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
    tINQ_BDADDR         *p_db = &p_inq->p_bd_db[0];
    UINT16       xx;

    /* Don't bother searching, database doesn't exist or periodic mode */
    if ((p_inq->inq_active & BTM_PERIODIC_INQUIRY_ACTIVE) || !p_db) {
        return (FALSE);
    }

    for (xx = 0; xx < p_inq->num_bd_entries; xx++, p_db++) {
        if (!memcmp(p_db->bd_addr, p_bda, BD_ADDR_LEN)
                && p_db->inq_count == p_inq->inq_counter) {
            return (TRUE);
        }
    }

    if (xx < p_inq->max_bd_entries) {
        p_db->inq_count = p_inq->inq_counter;
        memcpy(p_db->bd_addr, p_bda, BD_ADDR_LEN);
        p_inq->num_bd_entries++;
    }

    /* If here, New Entry */
    return (FALSE);
}

/*******************************************************************************
**
** Function         btm_inq_db_find
**
** Description      This function looks through the inquiry database for a match
**                  based on Bluetooth Device Address
**
** Returns          pointer to entry, or NULL if not found
**
*******************************************************************************/
tINQ_DB_ENT *btm_inq_db_find (BD_ADDR p_bda)
{
    UINT16       xx;
    tINQ_DB_ENT  *p_ent = btm_cb.btm_inq_vars.inq_db;

    for (xx = 0; xx < BTM_INQ_DB_SIZE; xx++, p_ent++) {
        if ((p_ent->in_use) && (!memcmp (p_ent->inq_info.results.remote_bd_addr, p_bda, BD_ADDR_LEN))) {
            return (p_ent);
        }
    }

    /* If here, not found */
    return (NULL);
}


/*******************************************************************************
**
** Function         btm_inq_db_new
**
** Description      This function looks through the inquiry database for an unused
**                  entry. If no entry is free, it allocates the oldest entry.
**
** Returns          pointer to entry
**
*******************************************************************************/
tINQ_DB_ENT *btm_inq_db_new (BD_ADDR p_bda)
{
    UINT16       xx;
    tINQ_DB_ENT  *p_ent = btm_cb.btm_inq_vars.inq_db;
    tINQ_DB_ENT  *p_old = btm_cb.btm_inq_vars.inq_db;
    UINT32       ot = 0xFFFFFFFF;

    for (xx = 0; xx < BTM_INQ_DB_SIZE; xx++, p_ent++) {
        if (!p_ent->in_use) {
            memset (p_ent, 0, sizeof (tINQ_DB_ENT));
            memcpy (p_ent->inq_info.results.remote_bd_addr, p_bda, BD_ADDR_LEN);
            p_ent->in_use = TRUE;

            return (p_ent);
        }

        if (p_ent->time_of_resp < ot) {
            p_old = p_ent;
            ot    = p_ent->time_of_resp;
        }
    }

    /* If here, no free entry found. Return the oldest. */

    memset (p_old, 0, sizeof (tINQ_DB_ENT));
    memcpy (p_old->inq_info.results.remote_bd_addr, p_bda, BD_ADDR_LEN);
    p_old->in_use = TRUE;

    return (p_old);
}


/*******************************************************************************
**
** Function         btm_set_inq_event_filter
**
** Description      This function is called to set the inquiry event filter.
**                  It is called by either internally, or by the external API function
**                  (BTM_SetInqEventFilter).  It is used internally as part of the
**                  inquiry processing.
**
** Input Params:
**                  filter_cond_type - this is the type of inquiry filter to apply:
**                          BTM_FILTER_COND_DEVICE_CLASS,
**                          BTM_FILTER_COND_BD_ADDR, or
**                          BTM_CLR_INQUIRY_FILTER
**
**                  p_filt_cond - this is either a BD_ADDR or DEV_CLASS depending on the
**                          filter_cond_type  (See section 4.7.3 of Core Spec 1.0b).
**
** Returns          BTM_CMD_STARTED if successfully initiated
**                  BTM_NO_RESOURCES if couldn't get a memory pool buffer
**                  BTM_ILLEGAL_VALUE if a bad parameter was detected
**
*******************************************************************************/
static tBTM_STATUS btm_set_inq_event_filter (UINT8 filter_cond_type,
        tBTM_INQ_FILT_COND *p_filt_cond)
{
    UINT8    condition_length = DEV_CLASS_LEN * 2;
    UINT8    condition_buf[DEV_CLASS_LEN * 2];
    UINT8   *p_cond = condition_buf;                    /* points to the condition to pass to HCI */

#if (BTM_INQ_DEBUG == TRUE)
    BTM_TRACE_DEBUG ("btm_set_inq_event_filter: filter type %d [Clear-0, COD-1, BDADDR-2]\n",
                     filter_cond_type);
    BTM_TRACE_DEBUG ("                       condition [%02x%02x%02x %02x%02x%02x]\n",
                     p_filt_cond->bdaddr_cond[0], p_filt_cond->bdaddr_cond[1], p_filt_cond->bdaddr_cond[2],
                     p_filt_cond->bdaddr_cond[3], p_filt_cond->bdaddr_cond[4], p_filt_cond->bdaddr_cond[5]);
#endif

    /* Load the correct filter condition to pass to the lower layer */
    switch (filter_cond_type) {
    case BTM_FILTER_COND_DEVICE_CLASS:
        /* copy the device class and device class fields into contiguous memory to send to HCI */
        memcpy (condition_buf, p_filt_cond->cod_cond.dev_class, DEV_CLASS_LEN);
        memcpy (&condition_buf[DEV_CLASS_LEN],
                p_filt_cond->cod_cond.dev_class_mask, DEV_CLASS_LEN);

        /* condition length should already be set as the default */
        break;

    case BTM_FILTER_COND_BD_ADDR:
        p_cond = p_filt_cond->bdaddr_cond;

        /* condition length should already be set as the default */
        break;

    case BTM_CLR_INQUIRY_FILTER:
        condition_length = 0;
        break;

    default:
        return (BTM_ILLEGAL_VALUE);     /* Bad parameter was passed in */
    }

    btm_cb.btm_inq_vars.inqfilt_active = TRUE;

    /* Filter the inquiry results for the specified condition type and value */
    if (btsnd_hcic_set_event_filter(HCI_FILTER_INQUIRY_RESULT, filter_cond_type,
                                    p_cond, condition_length))

    {
        return (BTM_CMD_STARTED);
    } else {
        return (BTM_NO_RESOURCES);
    }
}


/*******************************************************************************
**
** Function         btm_event_filter_complete
**
** Description      This function is called when a set event filter has completed.
**                  Note: This routine currently only handles inquiry filters.
**                      Connection filters are ignored for now.
**
** Returns          void
**
*******************************************************************************/
void btm_event_filter_complete (UINT8 *p)
{
    UINT8            hci_status;
    tBTM_STATUS      status;
    tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
    tBTM_CMPL_CB   *p_cb = p_inq->p_inqfilter_cmpl_cb;

#if (BTM_INQ_DEBUG == TRUE)
    BTM_TRACE_DEBUG ("btm_event_filter_complete: inq_active:0x%x state:%d inqfilt_active:%d\n",
                     btm_cb.btm_inq_vars.inq_active, btm_cb.btm_inq_vars.state, btm_cb.btm_inq_vars.inqfilt_active);
#endif
    /* If the filter complete event is from an old or cancelled request, ignore it */
    if (p_inq->pending_filt_complete_event) {
        p_inq->pending_filt_complete_event--;
        return;
    }

    /* Only process the inquiry filter; Ignore the connection filter until it
       is used by the upper layers */
    if (p_inq->inqfilt_active == TRUE ) {
        /* Extract the returned status from the buffer */
        STREAM_TO_UINT8 (hci_status, p);
        if (hci_status != HCI_SUCCESS) {
            /* If standalone operation, return the error status; if embedded in the inquiry, continue the inquiry */
            BTM_TRACE_WARNING ("BTM Warning: Set Event Filter Failed (HCI returned 0x%x)\n", hci_status);
            status = BTM_ERR_PROCESSING;
        } else {
            status = BTM_SUCCESS;
        }

        /* If the set filter was initiated externally (via BTM_SetInqEventFilter), call the
           callback function to notify the initiator that it has completed */
        if (p_inq->state == BTM_INQ_INACTIVE_STATE) {
            p_inq->inqfilt_active = FALSE;
            if (p_cb) {
                (*p_cb) (&status);
            }
        } else    /* An inquiry is active (the set filter command was internally generated),
                   process the next state of the process (Set a new filter or start the inquiry). */
        {
            if (status != BTM_SUCCESS) {
                /* Process the inquiry complete (Error Status) */
                btm_process_inq_complete (BTM_ERR_PROCESSING, (UINT8)(p_inq->inqparms.mode & BTM_BR_INQUIRY_MASK));

                /* btm_process_inq_complete() does not restore the following settings on periodic inquiry */
                p_inq->inqfilt_active = FALSE;
                p_inq->inq_active = BTM_INQUIRY_INACTIVE;
                p_inq->state = BTM_INQ_INACTIVE_STATE;

                return;
            }

            /* Check to see if a new filter needs to be set up */
            if (p_inq->state == BTM_INQ_CLR_FILT_STATE) {
                if ((status = btm_set_inq_event_filter (p_inq->inqparms.filter_cond_type, &p_inq->inqparms.filter_cond)) == BTM_CMD_STARTED) {
                    p_inq->state = BTM_INQ_SET_FILT_STATE;
                } else { /* Error setting the filter: Call the initiator's callback function to indicate a failure */
                    p_inq->inqfilt_active = FALSE;

                    /* Process the inquiry complete (Error Status) */
                    btm_process_inq_complete (BTM_ERR_PROCESSING, (UINT8)(p_inq->inqparms.mode & BTM_BR_INQUIRY_MASK));
                }
            } else { /* Initiate the Inquiry or Periodic Inquiry */
                p_inq->state = BTM_INQ_ACTIVE_STATE;
                p_inq->inqfilt_active = FALSE;
                btm_initiate_inquiry (p_inq);
            }
        }
    }
}


/*******************************************************************************
**
** Function         btm_initiate_inquiry
**
** Description      This function is called to start an inquiry or periodic inquiry
**                  upon completion of the setting and/or clearing of the inquiry filter.
**
** Inputs:          p_inq (btm_cb.btm_inq_vars) - pointer to saved inquiry information
**                      mode - GENERAL or LIMITED inquiry
**                      duration - length in 1.28 sec intervals (If '0', the inquiry is CANCELLED)
**                      max_resps - maximum amount of devices to search for before ending the inquiry
**                      filter_cond_type - BTM_CLR_INQUIRY_FILTER, BTM_FILTER_COND_DEVICE_CLASS, or
**                                         BTM_FILTER_COND_BD_ADDR
**                      filter_cond - value for the filter (based on filter_cond_type)
**
** Returns          If an error occurs the initiator's callback is called with the error status.
**
*******************************************************************************/
static void btm_initiate_inquiry (tBTM_INQUIRY_VAR_ST *p_inq)
{
    const LAP       *lap;
    tBTM_INQ_PARMS  *p_inqparms = &p_inq->inqparms;

#if (BTM_INQ_DEBUG == TRUE)
    BTM_TRACE_DEBUG ("btm_initiate_inquiry: inq_active:0x%x state:%d inqfilt_active:%d\n",
                     btm_cb.btm_inq_vars.inq_active, btm_cb.btm_inq_vars.state, btm_cb.btm_inq_vars.inqfilt_active);
#endif
    btm_acl_update_busy_level (BTM_BLI_INQ_EVT);

    if (p_inq->inq_active & BTM_SSP_INQUIRY_ACTIVE) {
        btm_process_inq_complete (BTM_NO_RESOURCES, (UINT8)(p_inqparms->mode & BTM_BR_INQUIRY_MASK));
        return;
    }

    /* Make sure the number of responses doesn't overflow the database configuration */
    p_inqparms->max_resps = (UINT8)((p_inqparms->max_resps <= BTM_INQ_DB_SIZE) ? p_inqparms->max_resps : BTM_INQ_DB_SIZE);

    lap = (p_inq->inq_active & BTM_LIMITED_INQUIRY_ACTIVE) ? &limited_inq_lap : &general_inq_lap;

    if (p_inq->inq_active & BTM_PERIODIC_INQUIRY_ACTIVE) {
        if (!btsnd_hcic_per_inq_mode (p_inq->per_max_delay,
                                      p_inq->per_min_delay,
                                      *lap, p_inqparms->duration,
                                      p_inqparms->max_resps)) {
            btm_process_inq_complete (BTM_NO_RESOURCES, (UINT8)(p_inqparms->mode & BTM_BR_INQUIRY_MASK));
        }
    } else {
        btm_clr_inq_result_flt();

        /* Allocate memory to hold bd_addrs responding */
        if ((p_inq->p_bd_db = (tINQ_BDADDR *)osi_calloc(BT_DEFAULT_BUFFER_SIZE)) != NULL) {
            p_inq->max_bd_entries = (UINT16)(BT_DEFAULT_BUFFER_SIZE / sizeof(tINQ_BDADDR));
            /*            BTM_TRACE_DEBUG("btm_initiate_inquiry: memory allocated for %d bdaddrs",
                                          p_inq->max_bd_entries); */
        }

        if (!btsnd_hcic_inquiry(*lap, p_inqparms->duration, 0)) {
            btm_process_inq_complete (BTM_NO_RESOURCES, (UINT8)(p_inqparms->mode & BTM_BR_INQUIRY_MASK));
        }
    }
}

/*******************************************************************************
**
** Function         btm_process_inq_results
**
** Description      This function is called when inquiry results are received from
**                  the device. It updates the inquiry database. If the inquiry
**                  database is full, the oldest entry is discarded.
**
** Parameters       inq_res_mode - BTM_INQ_RESULT_STANDARD
**                                 BTM_INQ_RESULT_WITH_RSSI
**                                 BTM_INQ_RESULT_EXTENDED
**
** Returns          void
**
*******************************************************************************/
void btm_process_inq_results (UINT8 *p, UINT8 inq_res_mode)
{
    UINT8            num_resp, xx;
    BD_ADDR          bda;
    tINQ_DB_ENT     *p_i;
    tBTM_INQ_RESULTS *p_cur = NULL;
    BOOLEAN          is_new = TRUE;
    BOOLEAN          update = FALSE;
    INT8             i_rssi;
    tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
    tBTM_INQ_RESULTS_CB *p_inq_results_cb = p_inq->p_inq_results_cb;
    UINT8            page_scan_rep_mode = 0;
    UINT8            page_scan_per_mode = 0;
    UINT8            page_scan_mode = 0;
    UINT8            rssi = 0;
    DEV_CLASS        dc;
    UINT16           clock_offset;
    UINT8            *p_eir_data = NULL;

#if (BTM_INQ_DEBUG == TRUE)
    BTM_TRACE_DEBUG ("btm_process_inq_results inq_active:0x%x state:%d inqfilt_active:%d inq_res_mode=%d\n",
                     btm_cb.btm_inq_vars.inq_active, btm_cb.btm_inq_vars.state, btm_cb.btm_inq_vars.inqfilt_active, inq_res_mode);
#endif
    /* Only process the results if the BR inquiry is still active */
    if (!(p_inq->inq_active & BTM_BR_INQ_ACTIVE_MASK)) {
        return;
    }

    STREAM_TO_UINT8 (num_resp, p);

    for (xx = 0; xx < num_resp; xx++) {
        update = FALSE;
        /* Extract inquiry results */
        STREAM_TO_BDADDR   (bda, p);
        STREAM_TO_UINT8    (page_scan_rep_mode, p);
        STREAM_TO_UINT8    (page_scan_per_mode, p);

        if (inq_res_mode == BTM_INQ_RESULT_STANDARD) {
            STREAM_TO_UINT8(page_scan_mode, p);
        }

        STREAM_TO_DEVCLASS (dc, p);
        STREAM_TO_UINT16   (clock_offset, p);
        if (inq_res_mode != BTM_INQ_RESULT_STANDARD) {
            STREAM_TO_UINT8(rssi, p);
        }

        p_i = btm_inq_db_find (bda);

        /* Only process the num_resp is smaller than max_resps.
           If results are queued to BTU task while canceling inquiry,
           or when more than one result is in this response, > max_resp
           responses could be processed which can confuse some apps
        */
        if (p_inq->inqparms.max_resps &&
                p_inq->inq_cmpl_info.num_resp >= p_inq->inqparms.max_resps
#if BLE_INCLUDED == TRUE
                /* new device response */
                && ( p_i == NULL ||
                     /* exisiting device with BR/EDR info */
                     (p_i && (p_i->inq_info.results.device_type & BT_DEVICE_TYPE_BREDR) != 0)
                   )
#endif

           ) {
            BTM_TRACE_WARNING("INQ RES: Extra Response Received...ignoring\n");
            return;
        }

        /* Check if this address has already been processed for this inquiry */
        if (btm_inq_find_bdaddr(bda)) {
            BTM_TRACE_DEBUG("BDA seen before [%02x%02x %02x%02x %02x%02x]\n",
                            bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
            /* By default suppose no update needed */
            i_rssi = (INT8)rssi;

            /* If this new RSSI is higher than the last one */
            if (p_inq->inqparms.report_dup && (rssi != 0) &&
                    p_i && (i_rssi > p_i->inq_info.results.rssi || p_i->inq_info.results.rssi == 0
#if BLE_INCLUDED == TRUE
                            /* BR/EDR inquiry information update */
                            || (p_i->inq_info.results.device_type & BT_DEVICE_TYPE_BREDR) != 0
#endif
                           )) {
                p_cur = &p_i->inq_info.results;
                BTM_TRACE_DEBUG("update RSSI new:%d, old:%d\n", i_rssi, p_cur->rssi);
                p_cur->rssi = i_rssi;
                update = TRUE;
            }
            /* If we received a second Extended Inq Event for an already */
            /* discovered device, this is because for the first one EIR was not received */
            else if ((inq_res_mode == BTM_INQ_RESULT_EXTENDED) && (p_i)) {
                p_cur = &p_i->inq_info.results;
                update = TRUE;
            }
            /* If no update needed continue with next response (if any) */
            else {
                continue;
            }
        }

        /* If existing entry, use that, else get a new one (possibly reusing the oldest) */
        if (p_i == NULL) {
            p_i = btm_inq_db_new (bda);
            is_new = TRUE;
        }

        /* If an entry for the device already exists, overwrite it ONLY if it is from
           a previous inquiry. (Ignore it if it is a duplicate response from the same
           inquiry.
        */
        else if (p_i->inq_count == p_inq->inq_counter
#if (BLE_INCLUDED == TRUE )
                 && (p_i->inq_info.results.device_type == BT_DEVICE_TYPE_BREDR)
#endif
                ) {
            is_new = FALSE;
        }

        /* keep updating RSSI to have latest value */
        if ( inq_res_mode != BTM_INQ_RESULT_STANDARD ) {
            p_i->inq_info.results.rssi = (INT8)rssi;
        } else {
            p_i->inq_info.results.rssi = BTM_INQ_RES_IGNORE_RSSI;
        }

        if (is_new == TRUE) {
            /* Save the info */
            p_cur = &p_i->inq_info.results;
            p_cur->page_scan_rep_mode = page_scan_rep_mode;
            p_cur->page_scan_per_mode = page_scan_per_mode;
            p_cur->page_scan_mode     = page_scan_mode;
            p_cur->dev_class[0]       = dc[0];
            p_cur->dev_class[1]       = dc[1];
            p_cur->dev_class[2]       = dc[2];
            p_cur->clock_offset       = clock_offset  | BTM_CLOCK_OFFSET_VALID;

            p_i->time_of_resp = osi_time_get_os_boottime_ms();

            if (p_i->inq_count != p_inq->inq_counter) {
                p_inq->inq_cmpl_info.num_resp++;    /* A new response was found */
            }

#if (defined BLE_INCLUDED && BLE_INCLUDED == TRUE)
            p_cur->inq_result_type    = BTM_INQ_RESULT_BR;
            if (p_i->inq_count != p_inq->inq_counter) {
                p_cur->device_type  = BT_DEVICE_TYPE_BREDR;
                p_i->scan_rsp       = FALSE;
            } else {
                p_cur->device_type    |= BT_DEVICE_TYPE_BREDR;
            }
#endif
            p_i->inq_count = p_inq->inq_counter;   /* Mark entry for current inquiry */

            /* If the number of responses found and not unlimited, issue a cancel inquiry */
            if (!(p_inq->inq_active & BTM_PERIODIC_INQUIRY_ACTIVE) &&
                    p_inq->inqparms.max_resps &&
                    p_inq->inq_cmpl_info.num_resp == p_inq->inqparms.max_resps
#if BLE_INCLUDED == TRUE
                    /* BLE scanning is active and received adv */
                    && ((((p_inq->inqparms.mode & BTM_BLE_INQUIRY_MASK) != 0) &&
                         p_cur->device_type == BT_DEVICE_TYPE_DUMO && p_i->scan_rsp) ||
                        (p_inq->inqparms.mode & BTM_BLE_INQUIRY_MASK) == 0)
#endif
               ) {
                /*                BTM_TRACE_DEBUG("BTMINQ: Found devices, cancelling inquiry..."); */
                btsnd_hcic_inq_cancel();

#if BLE_INCLUDED == TRUE
                if ((p_inq->inqparms.mode & BTM_BLE_INQUIRY_MASK) != 0) {
                    btm_ble_stop_inquiry();
                }
#endif
                btm_acl_update_busy_level (BTM_BLI_INQ_DONE_EVT);
            }
            /* Initialize flag to FALSE. This flag is set/used by application */
            p_i->inq_info.appl_knows_rem_name = FALSE;
        }

        if (is_new || update) {
            if ( inq_res_mode == BTM_INQ_RESULT_EXTENDED ) {
                memset( p_cur->eir_uuid, 0,
                        BTM_EIR_SERVICE_ARRAY_SIZE * (BTM_EIR_ARRAY_BITS / 8));
                /* set bit map of UUID list from received EIR */
                btm_set_eir_uuid( p, p_cur );
                p_eir_data = p;
            } else {
                p_eir_data = NULL;
            }

            /* If a callback is registered, call it with the results */
            if (p_inq_results_cb) {
                (p_inq_results_cb)((tBTM_INQ_RESULTS *) p_cur, p_eir_data);
            }
        }
    }
}

/*******************************************************************************
**
** Function         btm_sort_inq_result
**
** Description      This function is called when inquiry complete is received
**                  from the device to sort inquiry results based on rssi.
**
** Returns          void
**
*******************************************************************************/
void btm_sort_inq_result(void)
{
    UINT8               xx, yy, num_resp;
    tINQ_DB_ENT         *p_tmp  = NULL;
    tINQ_DB_ENT         *p_ent  = btm_cb.btm_inq_vars.inq_db;
    tINQ_DB_ENT         *p_next = btm_cb.btm_inq_vars.inq_db + 1;
    int                 size;

    num_resp = (btm_cb.btm_inq_vars.inq_cmpl_info.num_resp < BTM_INQ_DB_SIZE) ?
               btm_cb.btm_inq_vars.inq_cmpl_info.num_resp : BTM_INQ_DB_SIZE;

    if ((p_tmp = (tINQ_DB_ENT *)osi_malloc(sizeof(tINQ_DB_ENT))) != NULL) {
        size = sizeof(tINQ_DB_ENT);
        for (xx = 0; xx < num_resp - 1; xx++, p_ent++) {
            for (yy = xx + 1, p_next = p_ent + 1; yy < num_resp; yy++, p_next++) {
                if (p_ent->inq_info.results.rssi < p_next->inq_info.results.rssi) {
                    memcpy (p_tmp,  p_next, size);
                    memcpy (p_next, p_ent,  size);
                    memcpy (p_ent,  p_tmp,  size);
                }
            }
        }

        osi_free(p_tmp);
    }
}

/*******************************************************************************
**
** Function         btm_process_inq_complete
**
** Description      This function is called when inquiry complete is received
**                  from the device.  Call the callback if not in periodic inquiry
**                  mode AND it is not NULL (The caller wants the event).
**
**                  The callback pass back the status and the number of responses
**
** Returns          void
**
*******************************************************************************/
void btm_process_inq_complete (UINT8 status, UINT8 mode)
{
    tBTM_CMPL_CB        *p_inq_cb = btm_cb.btm_inq_vars.p_inq_cmpl_cb;
    tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;

#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
    /* inquiry inactive case happens when inquiry is cancelled.
       Make mode 0 for no further inquiries from the current inquiry process
    */
    if (status != HCI_SUCCESS || p_inq->next_state == BTM_FINISH || !p_inq->inq_active) {
        /* re-initialize for next inquiry request */
        p_inq->next_state = BTM_BR_ONE;
        /* make the mode 0 here */
        p_inq->inqparms.mode &= ~(p_inq->inqparms.mode);

    }
#endif

#if (!defined(BTA_HOST_INTERLEAVE_SEARCH) || BTA_HOST_INTERLEAVE_SEARCH == FALSE)
    p_inq->inqparms.mode &= ~(mode);
#endif

    if (p_inq->scan_type == INQ_LE_OBSERVE && !p_inq->inq_active) {
        /*end of LE observe*/
        p_inq->p_inq_ble_results_cb = (tBTM_INQ_RESULTS_CB *) NULL;
        p_inq->p_inq_ble_cmpl_cb = (tBTM_CMPL_CB *) NULL;
        p_inq->scan_type = INQ_NONE;
    }


#if (BTM_INQ_DEBUG == TRUE)
    BTM_TRACE_DEBUG ("btm_process_inq_complete inq_active:0x%x state:%d inqfilt_active:%d\n",
                     btm_cb.btm_inq_vars.inq_active, btm_cb.btm_inq_vars.state, btm_cb.btm_inq_vars.inqfilt_active);
#endif
    btm_acl_update_busy_level (BTM_BLI_INQ_DONE_EVT);
    /* Ignore any stray or late complete messages if the inquiry is not active */
    if (p_inq->inq_active) {
        p_inq->inq_cmpl_info.status = (tBTM_STATUS)((status == HCI_SUCCESS) ? BTM_SUCCESS : BTM_ERR_PROCESSING);

        /* Notify caller that the inquiry has completed; (periodic inquiries do not send completion events */
        if (!(p_inq->inq_active & BTM_PERIODIC_INQUIRY_ACTIVE) && p_inq->inqparms.mode == 0) {
#if BLE_INCLUDED == TRUE
            btm_clear_all_pending_le_entry();
#endif
            p_inq->state = BTM_INQ_INACTIVE_STATE;

            /* Increment so the start of a next inquiry has a new count */
            p_inq->inq_counter++;

            btm_clr_inq_result_flt();

            if ((p_inq->inq_cmpl_info.status == BTM_SUCCESS) &&
                    controller_get_interface()->supports_rssi_with_inquiry_results()) {
                btm_sort_inq_result();
            }

            /* Clear the results callback if set */
            p_inq->p_inq_results_cb = (tBTM_INQ_RESULTS_CB *) NULL;
            p_inq->inq_active = BTM_INQUIRY_INACTIVE;
            p_inq->p_inq_cmpl_cb = (tBTM_CMPL_CB *) NULL;

            /* If we have a callback registered for inquiry complete, call it */
            BTM_TRACE_DEBUG ("BTM Inq Compl Callback: status 0x%02x, num results %d\n",
                             p_inq->inq_cmpl_info.status, p_inq->inq_cmpl_info.num_resp);

            if (p_inq_cb) {
                (p_inq_cb)((tBTM_INQUIRY_CMPL *) &p_inq->inq_cmpl_info);
            }
        }
#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
        if (p_inq->inqparms.mode != 0 && !(p_inq->inq_active & BTM_PERIODIC_INQUIRY_ACTIVE)) {
            /* make inquiry inactive for next iteration */
            p_inq->inq_active = BTM_INQUIRY_INACTIVE;
            /* call the inquiry again */
            BTM_StartInquiry(&p_inq->inqparms, p_inq->p_inq_results_cb, p_inq->p_inq_cmpl_cb);
        }
#endif
    }
    if (p_inq->inqparms.mode == 0 && p_inq->scan_type == INQ_GENERAL) { //this inquiry is complete
        p_inq->scan_type = INQ_NONE;
#if (defined BLE_INCLUDED && BLE_INCLUDED == TRUE)
        /* check if the LE observe is pending */
        if (p_inq->p_inq_ble_results_cb != NULL) {
            BTM_TRACE_DEBUG("BTM Inq Compl: resuming a pending LE scan");
            BTM_BleObserve(1, 0, p_inq->p_inq_ble_results_cb, p_inq->p_inq_ble_cmpl_cb);
        }
#endif
    }
#if (BTM_INQ_DEBUG == TRUE)
    BTM_TRACE_DEBUG ("inq_active:0x%x state:%d inqfilt_active:%d\n",
                     btm_cb.btm_inq_vars.inq_active, btm_cb.btm_inq_vars.state, btm_cb.btm_inq_vars.inqfilt_active);
#endif
}

/*******************************************************************************
**
** Function         btm_process_cancel_complete
**
** Description      This function is called when inquiry cancel complete is received
**                  from the device.This function will also call the btm_process_inq_complete
**                  This function is needed to differentiate a cancel_cmpl_evt from the
**                  inq_cmpl_evt
**
** Returns          void
**
*******************************************************************************/
void btm_process_cancel_complete(UINT8 status, UINT8 mode)
{
    btm_acl_update_busy_level (BTM_BLI_INQ_CANCEL_EVT);
    btm_process_inq_complete(status, mode);
}
/*******************************************************************************
**
** Function         btm_initiate_rem_name
**
** Description      This function looks initiates a remote name request.  It is called
**                  either by GAP or by the API call BTM_ReadRemoteDeviceName.
**
** Input Params:    p_cur         - pointer to an inquiry result structure (NULL if nonexistent)
**                  p_cb            - callback function called when BTM_CMD_STARTED
**                                    is returned.
**                                    A pointer to tBTM_REMOTE_DEV_NAME is passed to the
**                                    callback.
**
** Returns
**                  BTM_CMD_STARTED is returned if the request was sent to HCI.
**                  BTM_BUSY if already in progress
**                  BTM_NO_RESOURCES if could not allocate resources to start the command
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
tBTM_STATUS btm_initiate_rem_name (BD_ADDR remote_bda, tBTM_INQ_INFO *p_cur,
                                    UINT8 origin, UINT32 timeout, tBTM_CMPL_CB *p_cb)
{
    tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
    BOOLEAN              cmd_ok;

    /*** Make sure the device is ready ***/
    if (!BTM_IsDeviceUp()) {
        return (BTM_WRONG_MODE);
    }

    if (origin == BTM_RMT_NAME_SEC) {
        cmd_ok = btsnd_hcic_rmt_name_req (remote_bda, HCI_PAGE_SCAN_REP_MODE_R1,
                                          HCI_MANDATARY_PAGE_SCAN_MODE, 0);
        if (cmd_ok) {
            return BTM_CMD_STARTED;
        } else {
            return BTM_NO_RESOURCES;
        }
    }
    /* Make sure there are no two remote name requests from external API in progress */
    else if (origin == BTM_RMT_NAME_EXT) {
        if (p_inq->remname_active) {
            return (BTM_BUSY);
        } else {
            /* If there is no remote name request running,call the callback function and start timer */
            p_inq->p_remname_cmpl_cb = p_cb;
            memcpy(p_inq->remname_bda, remote_bda, BD_ADDR_LEN);
            btu_start_timer (&p_inq->rmt_name_timer_ent,
                             BTU_TTYPE_BTM_RMT_NAME,
                             timeout);

            /* If the database entry exists for the device, use its clock offset */
            if (p_cur) {
                cmd_ok = btsnd_hcic_rmt_name_req (remote_bda,
                                                  p_cur->results.page_scan_rep_mode,
                                                  p_cur->results.page_scan_mode,
                                                  (UINT16)(p_cur->results.clock_offset |
                                                          BTM_CLOCK_OFFSET_VALID));
            } else { /* Otherwise use defaults and mark the clock offset as invalid */
                cmd_ok = btsnd_hcic_rmt_name_req (remote_bda, HCI_PAGE_SCAN_REP_MODE_R1,
                                                  HCI_MANDATARY_PAGE_SCAN_MODE, 0);
            }
            if (cmd_ok) {
                p_inq->remname_active = TRUE;
                return BTM_CMD_STARTED;
            } else {
                return BTM_NO_RESOURCES;
            }
        }
    } else {
        return BTM_ILLEGAL_VALUE;
    }
}

/*******************************************************************************
**
** Function         btm_process_remote_name
**
** Description      This function is called when a remote name is received from
**                  the device. If remote names are cached, it updates the inquiry
**                  database.
**
** Returns          void
**
*******************************************************************************/
void btm_process_remote_name (BD_ADDR bda, BD_NAME bdn, UINT16 evt_len, UINT8 hci_status)
{
    tBTM_REMOTE_DEV_NAME    rem_name;
    tBTM_INQUIRY_VAR_ST    *p_inq = &btm_cb.btm_inq_vars;
    tBTM_CMPL_CB           *p_cb = p_inq->p_remname_cmpl_cb;
    UINT8                  *p_n1;

    UINT16                 temp_evt_len;

    if (bda != NULL) {
        BTM_TRACE_EVENT("BDA %02x:%02x:%02x:%02x:%02x:%02x\n", bda[0], bda[1],
                        bda[2], bda[3],
                        bda[4], bda[5]);
    }

    BTM_TRACE_EVENT("Inquire BDA %02x:%02x:%02x:%02x:%02x:%02x\n", p_inq->remname_bda[0], p_inq->remname_bda[1],
                    p_inq->remname_bda[2], p_inq->remname_bda[3],
                    p_inq->remname_bda[4], p_inq->remname_bda[5]);



    /* If the inquire BDA and remote DBA are the same, then stop the timer and set the active to false */
    if ((p_inq->remname_active == TRUE) &&
            (((bda != NULL) &&
              (memcmp(bda, p_inq->remname_bda, BD_ADDR_LEN) == 0)) || bda == NULL))

    {
#if BLE_INCLUDED == TRUE
        if (BTM_UseLeLink(p_inq->remname_bda)) {
            if (hci_status == HCI_ERR_UNSPECIFIED) {
                btm_ble_cancel_remote_name(p_inq->remname_bda);
            }
        }
#endif
        btu_stop_timer (&p_inq->rmt_name_timer_ent);
        p_inq->remname_active = FALSE;
        /* Clean up and return the status if the command was not successful */
        /* Note: If part of the inquiry, the name is not stored, and the    */
        /*       inquiry complete callback is called.                       */

        if (hci_status == HCI_SUCCESS) {
            /* Copy the name from the data stream into the return structure */
            /* Note that even if it is not being returned, it is used as a  */
            /*      temporary buffer.                                       */
            p_n1 = (UINT8 *)rem_name.remote_bd_name;
            rem_name.length = (evt_len < BD_NAME_LEN) ? evt_len : BD_NAME_LEN;
            rem_name.remote_bd_name[rem_name.length] = 0;
            rem_name.status = BTM_SUCCESS;
            temp_evt_len = rem_name.length;

            while (temp_evt_len > 0) {
                *p_n1++ = *bdn++;
                temp_evt_len--;
            }
            rem_name.remote_bd_name[rem_name.length] = 0;
        }
        /* If processing a stand alone remote name then report the error in the callback */
        else {
            rem_name.status = BTM_BAD_VALUE_RET;
            rem_name.length = 0;
            rem_name.remote_bd_name[0] = 0;
        }
        memcpy(rem_name.bd_addr, p_inq->remname_bda, BD_ADDR_LEN);
        /* Reset the remote BAD to zero and call callback if possible */
        memset(p_inq->remname_bda, 0, BD_ADDR_LEN);

        p_inq->p_remname_cmpl_cb = NULL;
        if (p_cb) {
            (p_cb)((tBTM_REMOTE_DEV_NAME *)&rem_name);
        }
    }
}

/*******************************************************************************
**
** Function         btm_inq_rmt_name_failed
**
** Description      This function is if timeout expires while getting remote
**                  name.  This is done for devices that incorrectly do not
**                  report operation failure
**
** Returns          void
**
*******************************************************************************/
void btm_inq_rmt_name_failed (void)
{
    BTM_TRACE_ERROR ("btm_inq_rmt_name_failed()  remname_active=%d\n", btm_cb.btm_inq_vars.remname_active);

    if (btm_cb.btm_inq_vars.remname_active) {
        btm_process_remote_name (btm_cb.btm_inq_vars.remname_bda, NULL, 0, HCI_ERR_UNSPECIFIED);
    } else {
        btm_process_remote_name (NULL, NULL, 0, HCI_ERR_UNSPECIFIED);
    }
#if (SMP_INCLUDED == TRUE)
    btm_sec_rmt_name_request_complete (NULL, NULL, HCI_ERR_UNSPECIFIED);
#endif  ///SMP_INCLUDED == TRUE
}
/*******************************************************************************
**
** Function         btm_read_linq_tx_power_complete
**
** Description      read inquiry tx power level complete callback function.
**
** Returns          void
**
*******************************************************************************/
void btm_read_linq_tx_power_complete(UINT8 *p)
{
    tBTM_CMPL_CB                *p_cb = btm_cb.devcb.p_txpwer_cmpl_cb;
    tBTM_INQ_TXPWR_RESULTS        results;

    btu_stop_timer (&btm_cb.devcb.txpwer_timer);
    /* If there was a callback registered for read inq tx power, call it */
    btm_cb.devcb.p_txpwer_cmpl_cb = NULL;

    if (p_cb) {
        STREAM_TO_UINT8  (results.hci_status, p);

        if (results.hci_status == HCI_SUCCESS) {
            results.status = BTM_SUCCESS;

            STREAM_TO_UINT8 (results.tx_power, p);
            BTM_TRACE_EVENT ("BTM INQ TX POWER Complete: tx_power %d, hci status 0x%02x\n",
                             results.tx_power, results.hci_status);
        } else {
            results.status = BTM_ERR_PROCESSING;
        }

        (*p_cb)(&results);
    }

}
/*******************************************************************************
**
** Function         BTM_WriteEIR
**
** Description      This function is called to write EIR data to controller.
**
** Parameters       p_buff - allocated HCI command buffer including extended
**                           inquriry response
**                  fec_required - FEC is required or not
**
** Returns          BTM_SUCCESS  - if successful
**                  BTM_MODE_UNSUPPORTED - if local device cannot support it
**
*******************************************************************************/
tBTM_STATUS BTM_WriteEIR( BT_HDR *p_buff, BOOLEAN fec_required)
{
    if (controller_get_interface()->supports_extended_inquiry_response()) {
        BTM_TRACE_API("Write Extended Inquiry Response to controller\n");
        btsnd_hcic_write_ext_inquiry_response (p_buff, fec_required);
        return BTM_SUCCESS;
    } else {
        osi_free(p_buff);
        return BTM_MODE_UNSUPPORTED;
    }
}

/*******************************************************************************
**
** Function         BTM_CheckEirData
**
** Description      This function is called to get EIR data from significant part.
**
** Parameters       p_eir - pointer of EIR significant part
**                  type   - finding EIR data type
**                  p_length - return the length of EIR data not including type
**
** Returns          pointer of EIR data
**
*******************************************************************************/
UINT8 *BTM_CheckEirData( UINT8 *p_eir, UINT8 type, UINT8 *p_length )
{
    UINT8 *p = p_eir;
    UINT8 length;
    UINT8 eir_type;
    BTM_TRACE_API("BTM_CheckEirData type=0x%02X\n", type);

    STREAM_TO_UINT8(length, p);
    while ( length && (p - p_eir <= HCI_EXT_INQ_RESPONSE_LEN)) {
        STREAM_TO_UINT8(eir_type, p);
        if ( eir_type == type ) {
            /* length doesn't include itself */
            *p_length = length - 1; /* minus the length of type */
            return p;
        }
        p += length - 1; /* skip the length of data */
        STREAM_TO_UINT8(length, p);
    }

    *p_length = 0;
    return NULL;
}

/*******************************************************************************
**
** Function         btm_convert_uuid_to_eir_service
**
** Description      This function is called to get the bit position of UUID.
**
** Parameters       uuid16 - UUID 16-bit
**
** Returns          BTM EIR service ID if found
**                  BTM_EIR_MAX_SERVICES - if not found
**
*******************************************************************************/
static UINT8 btm_convert_uuid_to_eir_service( UINT16 uuid16 )
{
    UINT8 xx;

    for ( xx = 0; xx < BTM_EIR_MAX_SERVICES; xx++ ) {
        if ( uuid16 == BTM_EIR_UUID_LKUP_TBL[xx]) {
            return xx;
        }
    }
    return BTM_EIR_MAX_SERVICES;
}

/*******************************************************************************
**
** Function         BTM_HasEirService
**
** Description      This function is called to know if UUID in bit map of UUID.
**
** Parameters       p_eir_uuid - bit map of UUID list
**                  uuid16 - UUID 16-bit
**
** Returns          TRUE - if found
**                  FALSE - if not found
**
*******************************************************************************/
BOOLEAN BTM_HasEirService( UINT32 *p_eir_uuid, UINT16 uuid16 )
{
    UINT8 service_id;

    service_id = btm_convert_uuid_to_eir_service(uuid16);
    if ( service_id < BTM_EIR_MAX_SERVICES ) {
        return ( BTM_EIR_HAS_SERVICE( p_eir_uuid, service_id ));
    } else {
        return ( FALSE );
    }
}

/*******************************************************************************
**
** Function         BTM_HasInquiryEirService
**
** Description      This function is called to know if UUID in bit map of UUID list.
**
** Parameters       p_results - inquiry results
**                  uuid16 - UUID 16-bit
**
** Returns          BTM_EIR_FOUND - if found
**                  BTM_EIR_NOT_FOUND - if not found and it is complete list
**                  BTM_EIR_UNKNOWN - if not found and it is not complete list
**
*******************************************************************************/
tBTM_EIR_SEARCH_RESULT BTM_HasInquiryEirService( tBTM_INQ_RESULTS *p_results, UINT16 uuid16 )
{
    if ( BTM_HasEirService( p_results->eir_uuid, uuid16 )) {
        return BTM_EIR_FOUND;
    } else if ( p_results->eir_complete_list ) {
        return BTM_EIR_NOT_FOUND;
    } else {
        return BTM_EIR_UNKNOWN;
    }
}

/*******************************************************************************
**
** Function         BTM_AddEirService
**
** Description      This function is called to add a service in bit map of UUID list.
**
** Parameters       p_eir_uuid - bit mask of UUID list for EIR
**                  uuid16 - UUID 16-bit
**
** Returns          None
**
*******************************************************************************/
void BTM_AddEirService( UINT32 *p_eir_uuid, UINT16 uuid16 )
{
    UINT8 service_id;

    service_id = btm_convert_uuid_to_eir_service(uuid16);
    if ( service_id < BTM_EIR_MAX_SERVICES ) {
        BTM_EIR_SET_SERVICE( p_eir_uuid, service_id );
    }
}

/*******************************************************************************
**
** Function         BTM_RemoveEirService
**
** Description      This function is called to remove a service in bit map of UUID list.
**
** Parameters       p_eir_uuid - bit mask of UUID list for EIR
**                  uuid16 - UUID 16-bit
**
** Returns          None
**
*******************************************************************************/
void BTM_RemoveEirService( UINT32 *p_eir_uuid, UINT16 uuid16 )
{
    UINT8 service_id;

    service_id = btm_convert_uuid_to_eir_service(uuid16);
    if ( service_id < BTM_EIR_MAX_SERVICES ) {
        BTM_EIR_CLR_SERVICE( p_eir_uuid, service_id );
    }
}

/*******************************************************************************
**
** Function         BTM_GetEirSupportedServices
**
** Description      This function is called to get UUID list from bit map of UUID list.
**
** Parameters       p_eir_uuid - bit mask of UUID list for EIR
**                  p - reference of current pointer of EIR
**                  max_num_uuid16 - max number of UUID can be written in EIR
**                  num_uuid16 - number of UUID have been written in EIR
**
** Returns          BTM_EIR_MORE_16BITS_UUID_TYPE, if it has more than max
**                  BTM_EIR_COMPLETE_16BITS_UUID_TYPE, otherwise
**
*******************************************************************************/
UINT8 BTM_GetEirSupportedServices( UINT32 *p_eir_uuid,    UINT8 **p,
                                   UINT8  max_num_uuid16, UINT8 *p_num_uuid16)
{
    UINT8 service_index;

    *p_num_uuid16 = 0;

    for (service_index = 0; service_index < BTM_EIR_MAX_SERVICES; service_index++) {
        if ( BTM_EIR_HAS_SERVICE( p_eir_uuid, service_index )) {
            if ( *p_num_uuid16 < max_num_uuid16 ) {
                UINT16_TO_STREAM(*p, BTM_EIR_UUID_LKUP_TBL[service_index]);
                (*p_num_uuid16)++;
            }
            /* if max number of UUIDs are stored and found one more */
            else {
                return BTM_EIR_MORE_16BITS_UUID_TYPE;
            }
        }
    }
    return BTM_EIR_COMPLETE_16BITS_UUID_TYPE;
}

/*******************************************************************************
**
** Function         BTM_GetEirUuidList
**
** Description      This function parses EIR and returns UUID list.
**
** Parameters       p_eir - EIR
**                  uuid_size - LEN_UUID_16, LEN_UUID_32, LEN_UUID_128
**                  p_num_uuid - return number of UUID in found list
**                  p_uuid_list - return UUID list
**                  max_num_uuid - maximum number of UUID to be returned
**
** Returns          0 - if not found
**                  BTM_EIR_COMPLETE_16BITS_UUID_TYPE
**                  BTM_EIR_MORE_16BITS_UUID_TYPE
**                  BTM_EIR_COMPLETE_32BITS_UUID_TYPE
**                  BTM_EIR_MORE_32BITS_UUID_TYPE
**                  BTM_EIR_COMPLETE_128BITS_UUID_TYPE
**                  BTM_EIR_MORE_128BITS_UUID_TYPE
**
*******************************************************************************/
UINT8 BTM_GetEirUuidList( UINT8 *p_eir, UINT8 uuid_size, UINT8 *p_num_uuid,
                          UINT8 *p_uuid_list, UINT8 max_num_uuid)
{
    UINT8   *p_uuid_data;
    UINT8   type;
    UINT8   yy, xx;
    UINT16  *p_uuid16 = (UINT16 *)p_uuid_list;
    UINT32  *p_uuid32 = (UINT32 *)p_uuid_list;
    char    buff[LEN_UUID_128 * 2 + 1];

    p_uuid_data = btm_eir_get_uuid_list( p_eir, uuid_size, p_num_uuid, &type );
    if ( p_uuid_data == NULL ) {
        return 0x00;
    }

    if ( *p_num_uuid > max_num_uuid ) {
        BTM_TRACE_WARNING("BTM_GetEirUuidList number of uuid in EIR = %d, size of uuid list = %d\n",
                          *p_num_uuid, max_num_uuid );
        *p_num_uuid = max_num_uuid;
    }

    BTM_TRACE_DEBUG("BTM_GetEirUuidList type = %02X, number of uuid = %d\n", type, *p_num_uuid );

    if ( uuid_size == LEN_UUID_16 ) {
        for ( yy = 0; yy < *p_num_uuid; yy++ ) {
            STREAM_TO_UINT16(*(p_uuid16 + yy), p_uuid_data);
            BTM_TRACE_DEBUG("                     0x%04X\n", *(p_uuid16 + yy));
        }
    } else if ( uuid_size == LEN_UUID_32 ) {
        for ( yy = 0; yy < *p_num_uuid; yy++ ) {
            STREAM_TO_UINT32(*(p_uuid32 + yy), p_uuid_data);
            BTM_TRACE_DEBUG("                     0x%08X\n", *(p_uuid32 + yy));
        }
    } else if ( uuid_size == LEN_UUID_128 ) {
        for ( yy = 0; yy < *p_num_uuid; yy++ ) {
            STREAM_TO_ARRAY16(p_uuid_list + yy * LEN_UUID_128, p_uuid_data);
            for ( xx = 0; xx < LEN_UUID_128; xx++ ) {
                sprintf(buff + xx * 2, "%02X", *(p_uuid_list + yy * LEN_UUID_128 + xx));
            }
            BTM_TRACE_DEBUG("                     0x%s\n", buff);
        }
    }

    return type;
}


/*******************************************************************************
**
** Function         btm_eir_get_uuid_list
**
** Description      This function searches UUID list in EIR.
**
** Parameters       p_eir - address of EIR
**                  uuid_size - size of UUID to find
**                  p_num_uuid - number of UUIDs found
**                  p_uuid_list_type - EIR data type
**
** Returns          NULL - if UUID list with uuid_size is not found
**                  beginning of UUID list in EIR - otherwise
**
*******************************************************************************/
static UINT8 *btm_eir_get_uuid_list( UINT8 *p_eir, UINT8 uuid_size,
                                     UINT8 *p_num_uuid, UINT8 *p_uuid_list_type )
{
    UINT8   *p_uuid_data;
    UINT8   complete_type, more_type;
    UINT8   uuid_len;

    switch ( uuid_size ) {
    case LEN_UUID_16:
        complete_type = BTM_EIR_COMPLETE_16BITS_UUID_TYPE;
        more_type     = BTM_EIR_MORE_16BITS_UUID_TYPE;
        break;
    case LEN_UUID_32:
        complete_type = BTM_EIR_COMPLETE_32BITS_UUID_TYPE;
        more_type     = BTM_EIR_MORE_32BITS_UUID_TYPE;
        break;
    case LEN_UUID_128:
        complete_type = BTM_EIR_COMPLETE_128BITS_UUID_TYPE;
        more_type     = BTM_EIR_MORE_128BITS_UUID_TYPE;
        break;
    default:
        *p_num_uuid = 0;
        return NULL;
        break;
    }

    p_uuid_data = BTM_CheckEirData( p_eir, complete_type, &uuid_len );
    if (p_uuid_data == NULL) {
        p_uuid_data = BTM_CheckEirData( p_eir, more_type, &uuid_len );
        *p_uuid_list_type = more_type;
    } else {
        *p_uuid_list_type = complete_type;
    }

    *p_num_uuid = uuid_len / uuid_size;
    return p_uuid_data;
}

/*******************************************************************************
**
** Function         btm_convert_uuid_to_uuid16
**
** Description      This function converts UUID to UUID 16-bit.
**
** Parameters       p_uuid - address of UUID
**                  uuid_size - size of UUID
**
** Returns          0 - if UUID cannot be converted to UUID 16-bit
**                  UUID 16-bit - otherwise
**
*******************************************************************************/
static UINT16 btm_convert_uuid_to_uuid16( UINT8 *p_uuid, UINT8 uuid_size )
{
    static const UINT8  base_uuid[LEN_UUID_128] = {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
                                                   0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                                                  };
    UINT16 uuid16 = 0;
    UINT32 uuid32;
    BOOLEAN is_base_uuid;
    UINT8  xx;

    switch (uuid_size) {
    case LEN_UUID_16:
        STREAM_TO_UINT16 (uuid16, p_uuid);
        break;
    case LEN_UUID_32:
        STREAM_TO_UINT32 (uuid32, p_uuid);
        if (uuid32 < 0x10000) {
            uuid16 = (UINT16) uuid32;
        }
        break;
    case LEN_UUID_128:
        /* See if we can compress his UUID down to 16 or 32bit UUIDs */
        is_base_uuid = TRUE;
        for (xx = 0; xx < LEN_UUID_128 - 4; xx++) {
            if (p_uuid[xx] != base_uuid[xx]) {
                is_base_uuid = FALSE;
                break;
            }
        }
        if (is_base_uuid) {
            if ((p_uuid[LEN_UUID_128 - 1] == 0) && (p_uuid[LEN_UUID_128 - 2] == 0)) {
                p_uuid += (LEN_UUID_128 - 4);
                STREAM_TO_UINT16(uuid16, p_uuid);
            }
        }
        break;
    default:
        BTM_TRACE_WARNING("btm_convert_uuid_to_uuid16 invalid uuid size\n");
        break;
    }

    return ( uuid16);
}

/*******************************************************************************
**
** Function         btm_set_eir_uuid
**
** Description      This function is called to store received UUID into inquiry result.
**
** Parameters       p_eir - pointer of EIR significant part
**                  p_results - pointer of inquiry result
**
** Returns          None
**
*******************************************************************************/
void btm_set_eir_uuid( UINT8 *p_eir, tBTM_INQ_RESULTS *p_results )
{
    UINT8   *p_uuid_data;
    UINT8   num_uuid;
    UINT16  uuid16;
    UINT8   yy;
    UINT8   type = BTM_EIR_MORE_16BITS_UUID_TYPE;

    p_uuid_data = btm_eir_get_uuid_list( p_eir, LEN_UUID_16, &num_uuid, &type );

    if (type == BTM_EIR_COMPLETE_16BITS_UUID_TYPE) {
        p_results->eir_complete_list = TRUE;
    } else {
        p_results->eir_complete_list = FALSE;
    }

    BTM_TRACE_API("btm_set_eir_uuid eir_complete_list=0x%02X\n", p_results->eir_complete_list);

    if ( p_uuid_data ) {
        for ( yy = 0; yy < num_uuid; yy++ ) {
            STREAM_TO_UINT16(uuid16, p_uuid_data);
            BTM_AddEirService( p_results->eir_uuid, uuid16 );
        }
    }

    p_uuid_data = btm_eir_get_uuid_list( p_eir, LEN_UUID_32, &num_uuid, &type );
    if ( p_uuid_data ) {
        for ( yy = 0; yy < num_uuid; yy++ ) {
            uuid16 = btm_convert_uuid_to_uuid16( p_uuid_data, LEN_UUID_32 );
            p_uuid_data += LEN_UUID_32;
            if ( uuid16 ) {
                BTM_AddEirService( p_results->eir_uuid, uuid16 );
            }
        }
    }

    p_uuid_data = btm_eir_get_uuid_list( p_eir, LEN_UUID_128, &num_uuid, &type );
    if ( p_uuid_data ) {
        for ( yy = 0; yy < num_uuid; yy++ ) {
            uuid16 = btm_convert_uuid_to_uuid16( p_uuid_data, LEN_UUID_128 );
            p_uuid_data += LEN_UUID_128;
            if ( uuid16 ) {
                BTM_AddEirService( p_results->eir_uuid, uuid16 );
            }
        }
    }
}
