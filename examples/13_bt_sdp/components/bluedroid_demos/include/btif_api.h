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

/*******************************************************************************
 *
 *  Filename:      btif_api.h
 *
 *  Description:   Main API header file for all BTIF functions accessed
 *                 from main bluetooth HAL. All HAL extensions will not
 *                 require headerfiles as they would be accessed through
 *                 callout/callins.
 *
 *******************************************************************************/

#ifndef BTIF_API_H
#define BTIF_API_H

#include "btif_common.h"
#include "btif_dm.h"

/*******************************************************************************
**  BTIF CORE API
********************************************************************************/

/*******************************************************************************
**
** Function         btif_init_bluetooth
**
** Description      Creates BTIF task and prepares BT scheduler for startup
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_init_bluetooth(void);

/*******************************************************************************
**
** Function         btif_enable_bluetooth
**
** Description      Performs chip power on and kickstarts OS scheduler
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_enable_bluetooth(void);

/*******************************************************************************
**
** Function         btif_disable_bluetooth
**
** Description      Inititates shutdown of Bluetooth system.
**                  Any active links will be dropped and device entering
**                  non connectable/discoverable mode
**
** Returns          void
**
*******************************************************************************/
bt_status_t btif_disable_bluetooth(void);

/*******************************************************************************
**
** Function         btif_shutdown_bluetooth
**
** Description      Finalizes BT scheduler shutdown and terminates BTIF
**                  task.
**
**
** Returns          void
**
*******************************************************************************/
bt_status_t btif_shutdown_bluetooth(void);

#endif /* BTIF_API_H */
