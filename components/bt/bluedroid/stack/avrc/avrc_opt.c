/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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
 *  Interface to AVRCP optional commands
 *
 ******************************************************************************/
// #include <assert.h>
#include "bt_target.h"
#include <string.h>
#include "gki.h"
#include "avrc_api.h"
#include "avrc_int.h"

#if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE)

/******************************************************************************
**
** Function         avrc_vendor_msg
**
** Description      Compose a VENDOR DEPENDENT command according to p_msg
**
**                  Input Parameters:
**                      p_msg: Pointer to VENDOR DEPENDENT message structure.
**
**                  Output Parameters:
**                      None.
**
** Returns          pointer to a valid GKI buffer if successful.
**                  NULL if p_msg is NULL.
**
******************************************************************************/
static BT_HDR   *avrc_vendor_msg(tAVRC_MSG_VENDOR *p_msg)
{
    BT_HDR  *p_cmd;
    UINT8   *p_data;

    assert(p_msg != NULL);

#if AVRC_METADATA_INCLUDED == TRUE
    assert(AVRC_META_CMD_POOL_SIZE > (AVRC_MIN_CMD_LEN + p_msg->vendor_len));
    if ((p_cmd = (BT_HDR *) GKI_getpoolbuf(AVRC_META_CMD_POOL_ID)) != NULL)
#else
    assert(AVRC_CMD_POOL_SIZE > (AVRC_MIN_CMD_LEN + p_msg->vendor_len));
    if ((p_cmd = (BT_HDR *) GKI_getpoolbuf(AVRC_CMD_POOL_ID)) != NULL)
#endif
    {
        p_cmd->offset   = AVCT_MSG_OFFSET;
        p_data          = (UINT8 *)(p_cmd + 1) + p_cmd->offset;
        *p_data++       = (p_msg->hdr.ctype & AVRC_CTYPE_MASK);
        *p_data++       = (p_msg->hdr.subunit_type << AVRC_SUBTYPE_SHIFT) | p_msg->hdr.subunit_id;
        *p_data++       = AVRC_OP_VENDOR;
        AVRC_CO_ID_TO_BE_STREAM(p_data, p_msg->company_id);
        if (p_msg->vendor_len && p_msg->p_vendor_data) {
            memcpy(p_data, p_msg->p_vendor_data, p_msg->vendor_len);
        }
        p_cmd->len  = (UINT16) (p_data + p_msg->vendor_len - (UINT8 *)(p_cmd + 1) - p_cmd->offset);
        p_cmd->layer_specific   = AVCT_DATA_CTRL;
    }
    return p_cmd;
}

/******************************************************************************
**
** Function         AVRC_UnitCmd
**
** Description      Send a UNIT INFO command to the peer device.  This
**                  function can only be called for controller role connections.
**                  Any response message from the peer is passed back through
**                  the tAVRC_MSG_CBACK callback function.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                      label: Transaction label.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
UINT16 AVRC_UnitCmd(UINT8 handle, UINT8 label)
{
    BT_HDR  *p_cmd;
    UINT8   *p_data;

    if ((p_cmd = (BT_HDR *) GKI_getpoolbuf(AVRC_CMD_POOL_ID)) != NULL) {
        p_cmd->offset   = AVCT_MSG_OFFSET;
        p_data          = (UINT8 *)(p_cmd + 1) + p_cmd->offset;
        *p_data++       = AVRC_CMD_STATUS;
        /* unit & id ignore */
        *p_data++       = (AVRC_SUB_UNIT << AVRC_SUBTYPE_SHIFT) | AVRC_SUBID_IGNORE;
        *p_data++       = AVRC_OP_UNIT_INFO;
        memset(p_data, AVRC_CMD_OPRND_PAD, AVRC_UNIT_OPRND_BYTES);
        p_cmd->len      = p_data + AVRC_UNIT_OPRND_BYTES - (UINT8 *)(p_cmd + 1) - p_cmd->offset;
        p_cmd->layer_specific   = AVCT_DATA_CTRL;
    }
    return AVCT_MsgReq( handle, label, AVCT_CMD, p_cmd);
}

/******************************************************************************
**
** Function         AVRC_SubCmd
**
** Description      Send a SUBUNIT INFO command to the peer device.  This
**                  function can only be called for controller role connections.
**                  Any response message from the peer is passed back through
**                  the tAVRC_MSG_CBACK callback function.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                      label: Transaction label.
**
**                      page: Specifies which part of the subunit type table
**                      is requested.  For AVRCP it is typically zero.
**                      Value range is 0-7.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
UINT16 AVRC_SubCmd(UINT8 handle, UINT8 label, UINT8 page)
{
    BT_HDR  *p_cmd;
    UINT8   *p_data;

    if ((p_cmd = (BT_HDR *) GKI_getpoolbuf(AVRC_CMD_POOL_ID)) != NULL) {
        p_cmd->offset   = AVCT_MSG_OFFSET;
        p_data          = (UINT8 *)(p_cmd + 1) + p_cmd->offset;
        *p_data++       = AVRC_CMD_STATUS;
        /* unit & id ignore */
        *p_data++       = (AVRC_SUB_UNIT << AVRC_SUBTYPE_SHIFT) | AVRC_SUBID_IGNORE;
        *p_data++       = AVRC_OP_SUB_INFO;
        *p_data++       = ((page & AVRC_SUB_PAGE_MASK) << AVRC_SUB_PAGE_SHIFT) | AVRC_SUB_EXT_CODE;
        memset(p_data, AVRC_CMD_OPRND_PAD, AVRC_SUB_OPRND_BYTES);
        p_cmd->len      = p_data + AVRC_SUB_OPRND_BYTES - (UINT8 *)(p_cmd + 1) - p_cmd->offset;
        p_cmd->layer_specific   = AVCT_DATA_CTRL;
    }
    return AVCT_MsgReq( handle, label, AVCT_CMD, p_cmd);
}

/******************************************************************************
**
** Function         AVRC_VendorCmd
**
** Description      Send a VENDOR DEPENDENT command to the peer device.  This
**                  function can only be called for controller role connections.
**                  Any response message from the peer is passed back through
**                  the tAVRC_MSG_CBACK callback function.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                      label: Transaction label.
**
**                      p_msg: Pointer to VENDOR DEPENDENT message structure.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
UINT16 AVRC_VendorCmd(UINT8  handle, UINT8  label, tAVRC_MSG_VENDOR *p_msg)
{
    BT_HDR *p_buf = avrc_vendor_msg(p_msg);
    if (p_buf) {
        return AVCT_MsgReq( handle, label, AVCT_CMD, p_buf);
    } else {
        return AVCT_NO_RESOURCES;
    }
}


/******************************************************************************
**
** Function         AVRC_VendorRsp
**
** Description      Send a VENDOR DEPENDENT response to the peer device.  This
**                  function can only be called for target role connections.
**                  This function must be called when a VENDOR DEPENDENT
**                  command message is received from the peer through the
**                  tAVRC_MSG_CBACK callback function.
**
**                  Input Parameters:
**                      handle: Handle of this connection.
**
**                      label: Transaction label.  Must be the same value as
**                      passed with the command message in the callback function.
**
**                      p_msg: Pointer to VENDOR DEPENDENT message structure.
**
**                  Output Parameters:
**                      None.
**
** Returns          AVRC_SUCCESS if successful.
**                  AVRC_BAD_HANDLE if handle is invalid.
**
******************************************************************************/
UINT16 AVRC_VendorRsp(UINT8  handle, UINT8  label, tAVRC_MSG_VENDOR *p_msg)
{
    BT_HDR *p_buf = avrc_vendor_msg(p_msg);
    if (p_buf) {
        return AVCT_MsgReq( handle, label, AVCT_RSP, p_buf);
    } else {
        return AVCT_NO_RESOURCES;
    }
}

#endif /* #if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE) */
