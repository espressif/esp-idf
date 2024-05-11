/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
Note: These macros are deprecated. When connecting USB OTG to an external FSLS
PHY, the FSLS Serial Interface signals can be routed to any GPIO via the GPIO
matrix. Thus, these macros are meaningless.

Todo: Remove in IDF v6.0 (IDF-9029)
*/
#define USBPHY_VP_NUM 33
#define USBPHY_VM_NUM 34
#define USBPHY_RCV_NUM 35
#define USBPHY_OEN_NUM 36
#define USBPHY_VPO_NUM 37
#define USBPHY_VMO_NUM 38

/* GPIOs corresponding to the pads of the internal USB PHY */
#define USBPHY_DP_NUM 20
#define USBPHY_DM_NUM 19
