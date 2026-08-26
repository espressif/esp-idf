/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * !!! DO NOT MODIFY ANYTHING IN THIS FILE !!!
 */

#include <stdint.h>
#include <stdbool.h>
#include "btdm_user_cfg.h"
#if UC_BT_CTRL_BLE_IS_ENABLE
#include "ble_user_cfg.h"
#endif
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
#include "bredr_user_cfg.h"
#endif

#define BTDM_BROKER_MAGIC (0xc55c0000UL)

#define __BTDM_BROKER_ARRAY_SIZE(storage, ...)                                                     \
    ((sizeof((storage void *storage[]){0, __VA_ARGS__}) / sizeof(void *)) - 1)
#define __BTDM_BROKER_ENTRY(storage, ...)                                                          \
    ((storage void *storage[]){                                                                    \
        (void *)(BTDM_BROKER_MAGIC | (__BTDM_BROKER_ARRAY_SIZE(storage, __VA_ARGS__) & 0xFFFFU)),  \
        __VA_ARGS__})

#define BTDM_BROKER_ENTRY_DEF_FLASH(...) __BTDM_BROKER_ENTRY(const, __VA_ARGS__)
#define BTDM_BROKER_ENTRY_DEF_RAM(...) __BTDM_BROKER_ENTRY(, __VA_ARGS__)

#define BTDM_BROKER_NODE_DEF_FLASH(...)                                                            \
    {(const void *const)(__BTDM_BROKER_ARRAY_SIZE(const, __VA_ARGS__)), __VA_ARGS__}
#define BTDM_BROKER_NODE_DEF_RAM(...)                                                              \
    {(void *)(__BTDM_BROKER_ARRAY_SIZE(, __VA_ARGS__)), __VA_ARGS__}

/*
 ********************************************************************************
 * External Functions Declaration
 ********************************************************************************
 */
extern int r_btdm_broker_linear_selfcheck(const void *broker, bool is_ram);
extern int r_btdm_broker_indexed_selfcheck(const void *broker, bool is_ram);

extern int r_sym_bt_8fMePSgKBxkeyUSGQQcU(void *param);
extern int r_sym_bt_CXxFmLXrWP71jb0eVlzN(void *param);
extern int r_sym_bt_z2aLQW3Zvuoq7vASCpLB(void *param);
#if UC_BT_CTRL_BLE_IS_ENABLE
extern int brk_sym_acceptList_E2sQw1uVsjxxiP0PJWNT(void *param);
extern int brk_sym_acceptList_vvIIdyHCcpNAdAdzZrPN(void *param);
extern int brk_sym_acceptList_yfAo03VLjZhjCR2fnGvX(void *param);
extern int brk_sym_adv_S1K3QvkLgRcH2We9Z3KD(void *param);
extern int brk_sym_adv_k0eZcDthWb1sk0Kloemu(void *param);
extern int brk_sym_adv_xfHX4YH2Zb9de4JlBztj(void *param);
extern int brk_sym_arr_CG7zWaZkh7QOTatTjs2v(void *param);
extern int brk_sym_arr_SH7I2IviDsjnpqawpi0F(void *param);
extern int brk_sym_base_CdIdlqttqbZqCmaW9GAc(void *param);
extern int brk_sym_base_IdP4ncGWlpNArj2oW20N(void *param);
extern int brk_sym_base_NVN4AbeIskmtcV2PX3N1(void *param);
extern int brk_sym_base_QIE6fqExGJrsGUIGYNSf(void *param);
extern int brk_sym_base_uevmRkB8BjrlJh1c3VnZ(void *param);
extern int brk_sym_base_uuBMFaiuN71hyzVcBBth(void *param);
extern int brk_sym_coexHook_7UsddOHoZepnYRjYPawG(void *param);
extern int brk_sym_coexHook_WcEp3uxRHd6HYgB0pn0L(void *param);
extern int brk_sym_coexHook_wiWNhAUWlHyTZ7Z5ZC5Z(void *param);
extern int brk_sym_coexStack_hzrw4DdhxAw5xAlGFEwI(void *param);
extern int brk_sym_coexStack_sMPrrpveO5HOIb2qa5I8(void *param);
extern int brk_sym_extAdv_65JKSxhAHiLFV63DSxJk(void *param);
extern int brk_sym_extAdv_cxB4HBdDRajzTQg8DqKC(void *param);
extern int brk_sym_extAdv_j9EFs8yTWikhycyNL2tu(void *param);
extern int brk_sym_memMgmt_7tLw3T7RgI6l7DQc4dMI(void *param);
extern int brk_sym_memMgmt_PpI7zFiJWyV56U0OIURt(void *param);
extern int brk_sym_rand_6102wQhjz4grvtu5s8T3(void *param);
extern int brk_sym_rand_TnYdBCetsq1sndnhcROz(void *param);
extern int brk_sym_resolv_GWz7Tb1ymxsyLQ1208Jj(void *param);
extern int brk_sym_resolv_YX00BNf3Z2oJaizN1uK8(void *param);
extern int brk_sym_resolv_hQFDtxZqj5AP1IMR8s59(void *param);
extern int brk_sym_scan_0gIKmjMRFT37raN6hnDo(void *param);
extern int brk_sym_scan_1G8QO0lWyTS0nKOqvZOE(void *param);
extern int brk_sym_scan_Tfvee56pMZTiTxWnEWgP(void *param);
extern int brk_sym_scan_b8ogEC3hTjGmqL5oRuyw(void *param);
extern int brk_sym_scan_cgn6c03k4ZQqi2dJOtfm(void *param);
extern int brk_sym_scan_fd4yzdFac8vdFTlQ4BPA(void *param);
extern int brk_sym_scan_l1avG2cwTzJAlle4KLdU(void *param);
extern int brk_sym_scan_tdX6OLWyRKYxelkB8vZ1(void *param);
extern int brk_sym_scan_zmw2OczvfXFB4aIFaWkc(void *param);
extern int brk_sym_sched_MzeSZzbQ4ZKhWW6Wu5jV(void *param);
extern int brk_sym_sched_PNMeumLUZPcz8o4UrTLi(void *param);
extern int brk_sym_sched_eISDBv5SLWxSduVnTwzD(void *param);
extern int brk_sym_sched_hOsJ4rGq1XjiY7GjHycG(void *param);
extern int brk_sym_sched_wfZseauvjWnjfiu10yWN(void *param);
extern int brk_sym_sync_NOAs4m6LAUpamsUYHfqm(void *param);
extern int brk_sym_sync_dVvFbZ7ZwsleTM2nkzdV(void *param);
extern int brk_sym_sync_uoa5Xd4XcSSCUjyNsVGo(void *param);
#if UC_BLE_CTRL_CONN_ENABLED
extern int brk_sym_conn_Sw7sI0dtf78paJdn9Uhg(void *param);
extern int brk_sym_conn_eZ0plXo8e56VyJXdcbNn(void *param);
extern int brk_sym_conn_k5hEt7krj8McRSctLS7X(void *param);
extern int brk_sym_conn_nW2gyrsrYX980SM1JXUT(void *param);
extern int brk_sym_conn_sIcAPw7j0toiOGWivJ0e(void *param);
#endif /* UC_BLE_CTRL_CONN_ENABLED */
#if UC_BLE_CTRL_CONN_ENABLED && CONFIG_BT_LE_ERROR_SIM_ENABLED
extern int brk_sym_connSim_2RFEzu4Evi68Nnt4YowZ(void *param);
extern int brk_sym_connSim_3guOkwBVGmSgwrEIVUvL(void *param);
extern int brk_sym_connSim_EIv22SNth3fM1SPgJ9hO(void *param);
extern int brk_sym_connSim_SoaIlItcCgS4km7f1141(void *param);
extern int brk_sym_connSim_dytLpHTAgMtQWf4K3pGc(void *param);
extern int brk_sym_connSim_rsrSBeH3W8LE1yGVrtBc(void *param);
#endif /* UC_BLE_CTRL_CONN_ENABLED && CONFIG_BT_LE_ERROR_SIM_ENABLED */
#if UC_BLE_CTRL_PAWR_BCAST_SUPPORTED
extern int brk_sym_pawrBcast_02YCihGkfXcpLt0zh1uM(void *param);
extern int brk_sym_pawrBcast_0PNup0d8oI1288mfULNW(void *param);
extern int brk_sym_pawrBcast_0Wq1DOQE0eOWEGV2grxM(void *param);
extern int brk_sym_pawrBcast_1y9l4P0a3qWUSydU3cDY(void *param);
extern int brk_sym_pawrBcast_7jK56jlcfafMtoQgLg6t(void *param);
extern int brk_sym_pawrBcast_7qGF1F9weQ363Ml7Gj0n(void *param);
extern int brk_sym_pawrBcast_JHmbFJCpV155hIYyDVQ8(void *param);
extern int brk_sym_pawrBcast_JPGtn7AltqyvIRT8iPtF(void *param);
extern int brk_sym_pawrBcast_Jrj3qAmZkOoWxL1jFdmP(void *param);
extern int brk_sym_pawrBcast_KBuKVEBVXwxouWjaJUva(void *param);
extern int brk_sym_pawrBcast_OEzQrCBnW7rhUIi6H6gM(void *param);
extern int brk_sym_pawrBcast_SB1LBCzybPoDd0XYyLM8(void *param);
extern int brk_sym_pawrBcast_WNd7Y6n56cZrTlhBjFRU(void *param);
extern int brk_sym_pawrBcast_eBH7HrPT8y2Fj9382el3(void *param);
extern int brk_sym_pawrBcast_g6t1IjxRbcv5GUq1uXIA(void *param);
extern int brk_sym_pawrBcast_lk2Wpy4u04ukjtC7Ijcb(void *param);
extern int brk_sym_pawrBcast_on08yxBjym4I0iV3tlPo(void *param);
extern int brk_sym_pawrBcast_sSbQFGKMJszbA895ddLO(void *param);
extern int brk_sym_pawrBcast_w9Kcjl02BFd1e11Ic3A0(void *param);
#endif /* UC_BLE_CTRL_PAWR_BCAST_SUPPORTED */
#if UC_BLE_CTRL_PAWR_SYNC_SUPPORTED
extern int brk_sym_pawrSync_1cW8RtQRnVveTqFg9OSR(void *param);
extern int brk_sym_pawrSync_2gh2xSEYHokuhQ7UohkM(void *param);
extern int brk_sym_pawrSync_7bchpvHsUM8DXHyaXC7f(void *param);
extern int brk_sym_pawrSync_9XvRcZs9CUOgHNbMhe5G(void *param);
extern int brk_sym_pawrSync_LO7YyFM93srewqVpETY2(void *param);
extern int brk_sym_pawrSync_Qifv1iec7w57n2pBwucX(void *param);
extern int brk_sym_pawrSync_bbizSGqM6wZUOT0fQF1M(void *param);
extern int brk_sym_pawrSync_fn4PoAB0qFmYpbwqTO2H(void *param);
extern int brk_sym_pawrSync_k4txvaSWHJBXXqtE62MG(void *param);
extern int brk_sym_pawrSync_oBwpWArFlWxUJNCnHvsJ(void *param);
extern int brk_sym_pawrSync_pbTRKlIHoevVfNidtKZb(void *param);
extern int brk_sym_pawrSync_vAN2YoeCsSKafv2NdPVC(void *param);
extern int brk_sym_pawrSync_vBce2UGY5s0PhM3gRF8p(void *param);
extern int brk_sym_pawrSync_zuG0pqdRWK2zt5S09fTS(void *param);
#endif /* UC_BLE_CTRL_PAWR_SYNC_SUPPORTED */
#if UC_BT_CTRL_LE_BIS_BCAST_ENABLED
extern int brk_sym_bisBcast_2a3kgxHeabxbnBIsRh4s(void *param);
extern int brk_sym_bisBcast_2dWM4hoGuoEwy8loUGlI(void *param);
extern int brk_sym_bisBcast_2uiisGIdoGa6SNUZCE2m(void *param);
extern int brk_sym_bisBcast_BcOOZfvM58VUQK6uC2ew(void *param);
extern int brk_sym_bisBcast_EtJ8xQ5EIWS6IjqpsD1U(void *param);
extern int brk_sym_bisBcast_Pe7M3vC0UOSbZWC7Bm8D(void *param);
extern int brk_sym_bisBcast_vATQ5EU18M2xnmNZ84fY(void *param);
extern int brk_sym_bisBcast_vKPkmtQzLc0ZU4R3Mpa1(void *param);
extern int brk_sym_bisBcast_zlcnIBGOIfWlPC3WJ66f(void *param);
#endif /* UC_BT_CTRL_LE_BIS_BCAST_ENABLED */
#if UC_BT_CTRL_LE_BIS_SYNC_ENABLED
extern int brk_sym_bisSync_0xkTwhqVWK4bwJa6dDH6(void *param);
extern int brk_sym_bisSync_3FCcFQdSowAGd7aAejeg(void *param);
extern int brk_sym_bisSync_BO46u3KvbiXYsPrQplUT(void *param);
extern int brk_sym_bisSync_DuQGVcBNN6l6ggZp4Y8J(void *param);
extern int brk_sym_bisSync_EaKbiRZI30mvH3KWkMY5(void *param);
extern int brk_sym_bisSync_cB4uyzKmrYjduUbWqXdb(void *param);
extern int brk_sym_bisSync_qLbiM85U90F0BvzUDhgt(void *param);
#endif /* UC_BT_CTRL_LE_BIS_SYNC_ENABLED */
#if UC_BT_CTRL_LE_CIS_CENT_ENABLED
extern int brk_sym_cisCent_9P0KC1xZQpumJC2psVXO(void *param);
extern int brk_sym_cisCent_FX6J62IzYLkcQNdl7BqH(void *param);
extern int brk_sym_cisCent_GSToGcnDAoUIQsFaBPr7(void *param);
extern int brk_sym_cisCent_JUzUk54A9UNHtsUAHT8F(void *param);
extern int brk_sym_cisCent_jfaA90JGHwEKv1YkKIUO(void *param);
extern int brk_sym_cisCent_wt1P0oioDDyNz026HxsK(void *param);
extern int brk_sym_cisCent_y03W49d1NFScSUUO32Yn(void *param);
#endif /* UC_BT_CTRL_LE_CIS_CENT_ENABLED */
#if UC_BT_CTRL_LE_CIS_ENABLED
extern int brk_sym_cis_3DmpJ8V6gGsnSNopbei0(void *param);
extern int brk_sym_cis_5PzTPmH8LVt7wMSC7YnP(void *param);
extern int brk_sym_cis_Bmj5ai8K8ZKFKqWA8X8B(void *param);
extern int brk_sym_cis_Guzdzme9CFLxrbjG6HEJ(void *param);
extern int brk_sym_cis_QxmCCIxcxCfrWCKZmT3X(void *param);
extern int brk_sym_cis_UBu7crU2pIczpNc5cEcP(void *param);
extern int brk_sym_cis_WICEvILg2pK6JZvhry3j(void *param);
extern int brk_sym_cis_eeNELMi6IB5X3UF8MMvQ(void *param);
extern int brk_sym_cis_f2SkEM3LCeOCgMzDRq24(void *param);
extern int brk_sym_cis_i4bfG8waTBMteoJaWD40(void *param);
extern int brk_sym_cis_j0GHq32oPR9AN7YHYxEI(void *param);
extern int brk_sym_cis_mcjvVuw8ttAldZnobGxJ(void *param);
extern int brk_sym_cis_sHcp2gf28ZC99shUpnis(void *param);
extern int brk_sym_cis_vY92iDVk3ph4SrNh5ccy(void *param);
extern int brk_sym_cis_wQtv7EbxMlyUdVE0TQ9T(void *param);
extern int brk_sym_cis_zGkqwv9urnJ8DbX0JZAr(void *param);
#endif /* UC_BT_CTRL_LE_CIS_ENABLED */
#if UC_BT_CTRL_LE_CIS_PRPH_ENABLED
extern int brk_sym_cisPrph_6ORekVL63tenTy0sCT2O(void *param);
extern int brk_sym_cisPrph_Kb3o0Y7oUCdHJ1SXcoqj(void *param);
extern int brk_sym_cisPrph_bU9vRMdklv6bdZ52yCI5(void *param);
extern int brk_sym_cisPrph_xR7NheKHstTWkDAuZROL(void *param);
extern int brk_sym_cisPrph_zr8KQ3aEASRV2ZptTSnX(void *param);
#endif /* UC_BT_CTRL_LE_CIS_PRPH_ENABLED */
#if UC_BT_CTRL_LE_CTE_CAL_ENABLED
extern int brk_sym_cteCal_Lxc6TR3r9tNnhSe66bmE(void *param);
extern int brk_sym_cteCal_NKbWd6AypEyH2B338dAx(void *param);
extern int brk_sym_cteCal_ixMlNDshScz4WVTbiOKl(void *param);
extern int brk_sym_cteCal_j700khui6oomGLu22Rgb(void *param);
extern int brk_sym_cteCal_uWSKv9wSYw0nIY0GhuZF(void *param);
#endif /* UC_BT_CTRL_LE_CTE_CAL_ENABLED */
#if UC_BT_CTRL_LE_CTE_ENABLED
extern int brk_sym_cte_NjXx2EHiiDoBXhq2trhx(void *param);
extern int brk_sym_cte_P7A06WogxnY1JlnvQaGe(void *param);
extern int brk_sym_cte_aMtk23FR89BNAXNthzRu(void *param);
extern int brk_sym_cte_onkmOjIkv3QCPJVJQuUB(void *param);
extern int brk_sym_cte_vx3COozMkhCXFbq2ZjHK(void *param);
#endif /* UC_BT_CTRL_LE_CTE_ENABLED */
#if UC_BT_CTRL_LE_DTM_ENABLED
extern int brk_sym_dtm_8YKzWpU1Xe3NGGMtUeY6(void *param);
extern int brk_sym_dtm_Er3Vd8hhSABNIkl9x3At(void *param);
extern int brk_sym_dtm_wW4wnzkRa1kpb5NfwZVi(void *param);
#endif /* UC_BT_CTRL_LE_DTM_ENABLED */
#if UC_BT_CTRL_LE_ISO_ENABLED
extern int brk_sym_coexIsoHal_RVw8jrRQybFIlwvrONo9(void *param);
extern int brk_sym_iso_6SFIpEwXw1SsCdgfyN88(void *param);
extern int brk_sym_iso_8HHndXTSYyYcW1EhDzAN(void *param);
extern int brk_sym_iso_WJJo4fn6Hkvt9xXAvziZ(void *param);
extern int brk_sym_iso_Ywq26gNcDI3MQQVQbDEF(void *param);
extern int brk_sym_iso_gk1uwrtqhb5WsUZoPUJq(void *param);
extern int brk_sym_iso_qYKZbEBVIsH27HGf5VCS(void *param);
extern int brk_sym_iso_xew5NrC4aBwCavwqySPp(void *param);
#endif /* UC_BT_CTRL_LE_ISO_ENABLED */
#if UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED && CONFIG_BT_CTRL_SLEEP_ENABLE
extern int brk_sym_sleep_Ayr8PrlSNd7F6hhsmt13(void *param);
#endif /* UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED && CONFIG_BT_CTRL_SLEEP_ENABLE */
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
extern int btdm_common_sched_bredr_on_sched_check_remaining(void *param);
extern int btdm_common_sched_bredr_on_sched_hw_list_done(void *param);
extern int hci_tl_bredr_on_rx_cmd_c2h_num_pkt(void *param);
extern int hci_tl_bredr_on_rx_cmd_set_c2h_flow_ctrl(void *param);
extern int odm_afh_on_coex_wifi_channel_change(void *param);
extern int olc_intc_on_hal_exit_isr(void *param);
extern int olc_sleep_on_sched_actual_time_get(void *param);
extern int olc_sleep_on_sched_get_earlist_ticks(void *param);
extern int orca_ctrl_on_task_disable(void *param);
extern int orca_ctrl_on_task_enable(void *param);
extern int orca_ctrl_on_task_reset(void *param);
#endif /* UC_BT_CTRL_BR_EDR_IS_ENABLE */

/*
 ********************************************************************************
 * Linear Broker Definitions
 ********************************************************************************
 */
#if UC_BT_CTRL_BLE_IS_ENABLE
const void * const _adv_linear_broker_flash[] = BTDM_BROKER_NODE_DEF_FLASH(
#if UC_BLE_CTRL_PAWR_BCAST_SUPPORTED
    [1] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_0PNup0d8oI1288mfULNW,
          ),
    [2] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_02YCihGkfXcpLt0zh1uM,
          ),
    [3] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_g6t1IjxRbcv5GUq1uXIA,
          ),
    [4] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_7jK56jlcfafMtoQgLg6t,
          ),
    [5] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_w9Kcjl02BFd1e11Ic3A0,
          ),
    [6] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_0Wq1DOQE0eOWEGV2grxM,
          ),
    [7] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_JPGtn7AltqyvIRT8iPtF,
          ),
    [8] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_1y9l4P0a3qWUSydU3cDY,
          ),
    [9] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_lk2Wpy4u04ukjtC7Ijcb,
          ),
    [10] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_OEzQrCBnW7rhUIi6H6gM,
          ),
    [11] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_KBuKVEBVXwxouWjaJUva,
          ),
    [12] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_sSbQFGKMJszbA895ddLO,
          ),
    [13] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_WNd7Y6n56cZrTlhBjFRU,
          ),
#endif /* UC_BLE_CTRL_PAWR_BCAST_SUPPORTED */
);
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */

#if UC_BT_CTRL_BLE_IS_ENABLE
const void * const _base_linear_broker_flash[] = BTDM_BROKER_NODE_DEF_FLASH(
    [1] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_acceptList_E2sQw1uVsjxxiP0PJWNT,
              brk_sym_adv_k0eZcDthWb1sk0Kloemu,
#if UC_BT_CTRL_LE_BIS_BCAST_ENABLED
              brk_sym_bisBcast_Pe7M3vC0UOSbZWC7Bm8D,
#endif /* UC_BT_CTRL_LE_BIS_BCAST_ENABLED */
#if UC_BT_CTRL_LE_BIS_SYNC_ENABLED
              brk_sym_bisSync_BO46u3KvbiXYsPrQplUT,
#endif /* UC_BT_CTRL_LE_BIS_SYNC_ENABLED */
#if UC_BT_CTRL_LE_CIS_ENABLED
              brk_sym_cis_5PzTPmH8LVt7wMSC7YnP,
#endif /* UC_BT_CTRL_LE_CIS_ENABLED */
              brk_sym_coexStack_hzrw4DdhxAw5xAlGFEwI,
#if UC_BLE_CTRL_CONN_ENABLED && CONFIG_BT_LE_ERROR_SIM_ENABLED
              brk_sym_connSim_dytLpHTAgMtQWf4K3pGc,
#endif /* UC_BLE_CTRL_CONN_ENABLED && CONFIG_BT_LE_ERROR_SIM_ENABLED */
#if UC_BLE_CTRL_CONN_ENABLED
              brk_sym_conn_k5hEt7krj8McRSctLS7X,
#endif /* UC_BLE_CTRL_CONN_ENABLED */
#if UC_BT_CTRL_LE_CTE_CAL_ENABLED
              brk_sym_cteCal_j700khui6oomGLu22Rgb,
#endif /* UC_BT_CTRL_LE_CTE_CAL_ENABLED */
#if UC_BT_CTRL_LE_CTE_ENABLED
              brk_sym_cte_aMtk23FR89BNAXNthzRu,
#endif /* UC_BT_CTRL_LE_CTE_ENABLED */
#if UC_BT_CTRL_LE_DTM_ENABLED
              brk_sym_dtm_Er3Vd8hhSABNIkl9x3At,
#endif /* UC_BT_CTRL_LE_DTM_ENABLED */
              brk_sym_extAdv_65JKSxhAHiLFV63DSxJk,
#if UC_BT_CTRL_LE_ISO_ENABLED
              brk_sym_iso_gk1uwrtqhb5WsUZoPUJq,
#endif /* UC_BT_CTRL_LE_ISO_ENABLED */
#if UC_BLE_CTRL_PAWR_BCAST_SUPPORTED
              brk_sym_pawrBcast_7qGF1F9weQ363Ml7Gj0n,
#endif /* UC_BLE_CTRL_PAWR_BCAST_SUPPORTED */
#if UC_BLE_CTRL_PAWR_SYNC_SUPPORTED
              brk_sym_pawrSync_vBce2UGY5s0PhM3gRF8p,
#endif /* UC_BLE_CTRL_PAWR_SYNC_SUPPORTED */
              brk_sym_rand_TnYdBCetsq1sndnhcROz,
              brk_sym_resolv_YX00BNf3Z2oJaizN1uK8,
              brk_sym_scan_Tfvee56pMZTiTxWnEWgP,
              brk_sym_sync_uoa5Xd4XcSSCUjyNsVGo,
          ),
    [2] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_acceptList_yfAo03VLjZhjCR2fnGvX,
              brk_sym_adv_S1K3QvkLgRcH2We9Z3KD,
#if UC_BT_CTRL_LE_BIS_BCAST_ENABLED
              brk_sym_bisBcast_vKPkmtQzLc0ZU4R3Mpa1,
#endif /* UC_BT_CTRL_LE_BIS_BCAST_ENABLED */
#if UC_BT_CTRL_LE_BIS_SYNC_ENABLED
              brk_sym_bisSync_qLbiM85U90F0BvzUDhgt,
#endif /* UC_BT_CTRL_LE_BIS_SYNC_ENABLED */
#if UC_BT_CTRL_LE_CIS_ENABLED
              brk_sym_cis_j0GHq32oPR9AN7YHYxEI,
#endif /* UC_BT_CTRL_LE_CIS_ENABLED */
              brk_sym_coexStack_sMPrrpveO5HOIb2qa5I8,
#if UC_BLE_CTRL_CONN_ENABLED && CONFIG_BT_LE_ERROR_SIM_ENABLED
              brk_sym_connSim_rsrSBeH3W8LE1yGVrtBc,
#endif /* UC_BLE_CTRL_CONN_ENABLED && CONFIG_BT_LE_ERROR_SIM_ENABLED */
#if UC_BLE_CTRL_CONN_ENABLED
              brk_sym_conn_nW2gyrsrYX980SM1JXUT,
#endif /* UC_BLE_CTRL_CONN_ENABLED */
#if UC_BT_CTRL_LE_CTE_CAL_ENABLED
              brk_sym_cteCal_Lxc6TR3r9tNnhSe66bmE,
#endif /* UC_BT_CTRL_LE_CTE_CAL_ENABLED */
#if UC_BT_CTRL_LE_CTE_ENABLED
              brk_sym_cte_onkmOjIkv3QCPJVJQuUB,
#endif /* UC_BT_CTRL_LE_CTE_ENABLED */
#if UC_BT_CTRL_LE_DTM_ENABLED
              brk_sym_dtm_8YKzWpU1Xe3NGGMtUeY6,
#endif /* UC_BT_CTRL_LE_DTM_ENABLED */
              brk_sym_extAdv_cxB4HBdDRajzTQg8DqKC,
#if UC_BT_CTRL_LE_ISO_ENABLED
              brk_sym_iso_xew5NrC4aBwCavwqySPp,
#endif /* UC_BT_CTRL_LE_ISO_ENABLED */
#if UC_BLE_CTRL_PAWR_BCAST_SUPPORTED
              brk_sym_pawrBcast_SB1LBCzybPoDd0XYyLM8,
#endif /* UC_BLE_CTRL_PAWR_BCAST_SUPPORTED */
#if UC_BLE_CTRL_PAWR_SYNC_SUPPORTED
              brk_sym_pawrSync_1cW8RtQRnVveTqFg9OSR,
#endif /* UC_BLE_CTRL_PAWR_SYNC_SUPPORTED */
              brk_sym_rand_6102wQhjz4grvtu5s8T3,
              brk_sym_resolv_hQFDtxZqj5AP1IMR8s59,
              brk_sym_scan_cgn6c03k4ZQqi2dJOtfm,
              brk_sym_sync_dVvFbZ7ZwsleTM2nkzdV,
          ),
#if UC_BT_CTRL_LE_CIS_ENABLED
    [3] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_cis_vY92iDVk3ph4SrNh5ccy,
          ),
#endif /* UC_BT_CTRL_LE_CIS_ENABLED */
#if UC_BT_CTRL_LE_BIS_BCAST_ENABLED
    [4] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_bisBcast_BcOOZfvM58VUQK6uC2ew,
          ),
#endif /* UC_BT_CTRL_LE_BIS_BCAST_ENABLED */
);
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */

const void * const _btdm_coex_linear_broker_flash[] = BTDM_BROKER_NODE_DEF_FLASH(
#if UC_BT_CTRL_BLE_IS_ENABLE
    [1] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_coexHook_WcEp3uxRHd6HYgB0pn0L,
          ),
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
    [2] = BTDM_BROKER_ENTRY_DEF_FLASH(
#if UC_BT_CTRL_BLE_IS_ENABLE
              brk_sym_coexHook_wiWNhAUWlHyTZ7Z5ZC5Z,
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
              odm_afh_on_coex_wifi_channel_change,
#endif /* UC_BT_CTRL_BR_EDR_IS_ENABLE */
          ),
);

void * _btdm_hal_linear_broker_ram[] = BTDM_BROKER_NODE_DEF_RAM(
#if UC_BT_CTRL_BLE_IS_ENABLE
    [1] = BTDM_BROKER_ENTRY_DEF_RAM(
              brk_sym_base_QIE6fqExGJrsGUIGYNSf,
#if UC_BT_CTRL_LE_ISO_ENABLED
              brk_sym_coexIsoHal_RVw8jrRQybFIlwvrONo9,
              brk_sym_iso_6SFIpEwXw1SsCdgfyN88,
#endif /* UC_BT_CTRL_LE_ISO_ENABLED */
          ),
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
    [2] = BTDM_BROKER_ENTRY_DEF_RAM(
              r_sym_bt_z2aLQW3Zvuoq7vASCpLB,
#if UC_BT_CTRL_BLE_IS_ENABLE
              brk_sym_base_NVN4AbeIskmtcV2PX3N1,
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
              olc_intc_on_hal_exit_isr,
#endif /* UC_BT_CTRL_BR_EDR_IS_ENABLE */
          ),
);

const void * const _btdm_hci_linear_broker_flash[] = BTDM_BROKER_NODE_DEF_FLASH(
    [1] = BTDM_BROKER_ENTRY_DEF_FLASH(
#if UC_BT_CTRL_BLE_IS_ENABLE
#if UC_BLE_CTRL_CONN_ENABLED
              brk_sym_conn_eZ0plXo8e56VyJXdcbNn,
#endif /* UC_BLE_CTRL_CONN_ENABLED */
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
              hci_tl_bredr_on_rx_cmd_c2h_num_pkt,
#endif /* UC_BT_CTRL_BR_EDR_IS_ENABLE */
          ),
    [3] = BTDM_BROKER_ENTRY_DEF_FLASH(
#if UC_BT_CTRL_BLE_IS_ENABLE
#if UC_BLE_CTRL_CONN_ENABLED
              brk_sym_conn_Sw7sI0dtf78paJdn9Uhg,
#endif /* UC_BLE_CTRL_CONN_ENABLED */
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
              hci_tl_bredr_on_rx_cmd_set_c2h_flow_ctrl,
#endif /* UC_BT_CTRL_BR_EDR_IS_ENABLE */
          ),
);

const void * const _btdm_sched_linear_broker_flash[] = BTDM_BROKER_NODE_DEF_FLASH(
#if UC_BT_CTRL_BLE_IS_ENABLE
    [1] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_memMgmt_7tLw3T7RgI6l7DQc4dMI,
              brk_sym_sched_eISDBv5SLWxSduVnTwzD,
          ),
    [2] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_memMgmt_PpI7zFiJWyV56U0OIURt,
          ),
    [3] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_scan_0gIKmjMRFT37raN6hnDo,
          ),
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
    [6] = BTDM_BROKER_ENTRY_DEF_FLASH(
#if UC_BT_CTRL_BLE_IS_ENABLE
              brk_sym_sched_MzeSZzbQ4ZKhWW6Wu5jV,
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
              btdm_common_sched_bredr_on_sched_hw_list_done,
#endif /* UC_BT_CTRL_BR_EDR_IS_ENABLE */
          ),
    [7] = BTDM_BROKER_ENTRY_DEF_FLASH(
#if UC_BT_CTRL_BLE_IS_ENABLE
              brk_sym_sched_wfZseauvjWnjfiu10yWN,
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
              olc_sleep_on_sched_get_earlist_ticks,
#endif /* UC_BT_CTRL_BR_EDR_IS_ENABLE */
          ),
);

void * _btdm_sched_linear_broker_ram[] = BTDM_BROKER_NODE_DEF_RAM(
    [1] = BTDM_BROKER_ENTRY_DEF_RAM(
#if UC_BT_CTRL_BLE_IS_ENABLE
#if UC_BT_CTRL_LE_ISO_ENABLED
              brk_sym_iso_qYKZbEBVIsH27HGf5VCS,
#endif /* UC_BT_CTRL_LE_ISO_ENABLED */
              brk_sym_sched_PNMeumLUZPcz8o4UrTLi,
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
              olc_sleep_on_sched_actual_time_get,
#endif /* UC_BT_CTRL_BR_EDR_IS_ENABLE */
          ),
#if UC_BT_CTRL_BLE_IS_ENABLE
    [2] = BTDM_BROKER_ENTRY_DEF_RAM(
              brk_sym_scan_b8ogEC3hTjGmqL5oRuyw,
          ),
    [3] = BTDM_BROKER_ENTRY_DEF_RAM(
              brk_sym_scan_fd4yzdFac8vdFTlQ4BPA,
          ),
    [4] = BTDM_BROKER_ENTRY_DEF_RAM(
              brk_sym_scan_tdX6OLWyRKYxelkB8vZ1,
          ),
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
    [6] = BTDM_BROKER_ENTRY_DEF_RAM(
#if UC_BT_CTRL_BLE_IS_ENABLE
              brk_sym_sched_hOsJ4rGq1XjiY7GjHycG,
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
              btdm_common_sched_bredr_on_sched_check_remaining,
#endif /* UC_BT_CTRL_BR_EDR_IS_ENABLE */
          ),
);

const void * const _btdm_sleep_linear_broker_flash[] = BTDM_BROKER_NODE_DEF_FLASH(
#if UC_BT_CTRL_BLE_IS_ENABLE
#if UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED && CONFIG_BT_CTRL_SLEEP_ENABLE
    [1] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_sleep_Ayr8PrlSNd7F6hhsmt13,
          ),
#endif /* UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED && CONFIG_BT_CTRL_SLEEP_ENABLE */
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
);

void * _btdm_sleep_linear_broker_ram[] = BTDM_BROKER_NODE_DEF_RAM(
    [3] = BTDM_BROKER_ENTRY_DEF_RAM(
              r_sym_bt_CXxFmLXrWP71jb0eVlzN,
          ),
);

const void * const _btdm_task_linear_broker_flash[] = BTDM_BROKER_NODE_DEF_FLASH(
    [1] = BTDM_BROKER_ENTRY_DEF_FLASH(
#if UC_BT_CTRL_BLE_IS_ENABLE
              brk_sym_arr_CG7zWaZkh7QOTatTjs2v,
              brk_sym_base_uevmRkB8BjrlJh1c3VnZ,
#if UC_BT_CTRL_LE_BIS_BCAST_ENABLED
              brk_sym_bisBcast_2dWM4hoGuoEwy8loUGlI,
#endif /* UC_BT_CTRL_LE_BIS_BCAST_ENABLED */
#if UC_BT_CTRL_LE_BIS_SYNC_ENABLED
              brk_sym_bisSync_cB4uyzKmrYjduUbWqXdb,
#endif /* UC_BT_CTRL_LE_BIS_SYNC_ENABLED */
#if UC_BT_CTRL_LE_CIS_ENABLED
              brk_sym_cis_f2SkEM3LCeOCgMzDRq24,
#endif /* UC_BT_CTRL_LE_CIS_ENABLED */
#if UC_BLE_CTRL_CONN_ENABLED && CONFIG_BT_LE_ERROR_SIM_ENABLED
              brk_sym_connSim_EIv22SNth3fM1SPgJ9hO,
#endif /* UC_BLE_CTRL_CONN_ENABLED && CONFIG_BT_LE_ERROR_SIM_ENABLED */
#if UC_BT_CTRL_LE_CTE_CAL_ENABLED
              brk_sym_cteCal_NKbWd6AypEyH2B338dAx,
#endif /* UC_BT_CTRL_LE_CTE_CAL_ENABLED */
#if UC_BT_CTRL_LE_CTE_ENABLED
              brk_sym_cte_NjXx2EHiiDoBXhq2trhx,
#endif /* UC_BT_CTRL_LE_CTE_ENABLED */
              brk_sym_extAdv_j9EFs8yTWikhycyNL2tu,
#if UC_BT_CTRL_LE_ISO_ENABLED
              brk_sym_iso_8HHndXTSYyYcW1EhDzAN,
#endif /* UC_BT_CTRL_LE_ISO_ENABLED */
#if UC_BLE_CTRL_PAWR_BCAST_SUPPORTED
              brk_sym_pawrBcast_Jrj3qAmZkOoWxL1jFdmP,
#endif /* UC_BLE_CTRL_PAWR_BCAST_SUPPORTED */
#if UC_BLE_CTRL_PAWR_SYNC_SUPPORTED
              brk_sym_pawrSync_k4txvaSWHJBXXqtE62MG,
#endif /* UC_BLE_CTRL_PAWR_SYNC_SUPPORTED */
              brk_sym_scan_1G8QO0lWyTS0nKOqvZOE,
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
              orca_ctrl_on_task_enable,
#endif /* UC_BT_CTRL_BR_EDR_IS_ENABLE */
          ),
    [2] = BTDM_BROKER_ENTRY_DEF_FLASH(
#if UC_BT_CTRL_BLE_IS_ENABLE
              brk_sym_arr_SH7I2IviDsjnpqawpi0F,
              brk_sym_base_uuBMFaiuN71hyzVcBBth,
#if UC_BT_CTRL_LE_BIS_BCAST_ENABLED
              brk_sym_bisBcast_vATQ5EU18M2xnmNZ84fY,
#endif /* UC_BT_CTRL_LE_BIS_BCAST_ENABLED */
#if UC_BT_CTRL_LE_BIS_SYNC_ENABLED
              brk_sym_bisSync_DuQGVcBNN6l6ggZp4Y8J,
#endif /* UC_BT_CTRL_LE_BIS_SYNC_ENABLED */
#if UC_BT_CTRL_LE_CIS_ENABLED
              brk_sym_cis_UBu7crU2pIczpNc5cEcP,
#endif /* UC_BT_CTRL_LE_CIS_ENABLED */
#if UC_BT_CTRL_LE_CTE_CAL_ENABLED
              brk_sym_cteCal_uWSKv9wSYw0nIY0GhuZF,
#endif /* UC_BT_CTRL_LE_CTE_CAL_ENABLED */
#if UC_BT_CTRL_LE_CTE_ENABLED
              brk_sym_cte_vx3COozMkhCXFbq2ZjHK,
#endif /* UC_BT_CTRL_LE_CTE_ENABLED */
#if UC_BT_CTRL_LE_ISO_ENABLED
              brk_sym_iso_Ywq26gNcDI3MQQVQbDEF,
#endif /* UC_BT_CTRL_LE_ISO_ENABLED */
#if UC_BLE_CTRL_PAWR_BCAST_SUPPORTED
              brk_sym_pawrBcast_JHmbFJCpV155hIYyDVQ8,
#endif /* UC_BLE_CTRL_PAWR_BCAST_SUPPORTED */
#if UC_BLE_CTRL_PAWR_SYNC_SUPPORTED
              brk_sym_pawrSync_2gh2xSEYHokuhQ7UohkM,
#endif /* UC_BLE_CTRL_PAWR_SYNC_SUPPORTED */
              brk_sym_scan_zmw2OczvfXFB4aIFaWkc,
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
              orca_ctrl_on_task_disable,
#endif /* UC_BT_CTRL_BR_EDR_IS_ENABLE */
          ),
#if UC_BT_CTRL_BLE_IS_ENABLE
    [3] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_base_IdP4ncGWlpNArj2oW20N,
              brk_sym_coexHook_7UsddOHoZepnYRjYPawG,
          ),
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
    [4] = BTDM_BROKER_ENTRY_DEF_FLASH(
              r_sym_bt_8fMePSgKBxkeyUSGQQcU,
#if UC_BT_CTRL_BLE_IS_ENABLE
              brk_sym_acceptList_vvIIdyHCcpNAdAdzZrPN,
              brk_sym_adv_xfHX4YH2Zb9de4JlBztj,
              brk_sym_base_CdIdlqttqbZqCmaW9GAc,
#if UC_BT_CTRL_LE_BIS_BCAST_ENABLED
              brk_sym_bisBcast_zlcnIBGOIfWlPC3WJ66f,
#endif /* UC_BT_CTRL_LE_BIS_BCAST_ENABLED */
#if UC_BT_CTRL_LE_BIS_SYNC_ENABLED
              brk_sym_bisSync_EaKbiRZI30mvH3KWkMY5,
#endif /* UC_BT_CTRL_LE_BIS_SYNC_ENABLED */
#if UC_BT_CTRL_LE_CIS_ENABLED
              brk_sym_cis_QxmCCIxcxCfrWCKZmT3X,
#endif /* UC_BT_CTRL_LE_CIS_ENABLED */
#if UC_BLE_CTRL_CONN_ENABLED && CONFIG_BT_LE_ERROR_SIM_ENABLED
              brk_sym_connSim_3guOkwBVGmSgwrEIVUvL,
#endif /* UC_BLE_CTRL_CONN_ENABLED && CONFIG_BT_LE_ERROR_SIM_ENABLED */
#if UC_BLE_CTRL_CONN_ENABLED
              brk_sym_conn_sIcAPw7j0toiOGWivJ0e,
#endif /* UC_BLE_CTRL_CONN_ENABLED */
#if UC_BT_CTRL_LE_CTE_CAL_ENABLED
              brk_sym_cteCal_ixMlNDshScz4WVTbiOKl,
#endif /* UC_BT_CTRL_LE_CTE_CAL_ENABLED */
#if UC_BT_CTRL_LE_CTE_ENABLED
              brk_sym_cte_P7A06WogxnY1JlnvQaGe,
#endif /* UC_BT_CTRL_LE_CTE_ENABLED */
#if UC_BT_CTRL_LE_DTM_ENABLED
              brk_sym_dtm_wW4wnzkRa1kpb5NfwZVi,
#endif /* UC_BT_CTRL_LE_DTM_ENABLED */
#if UC_BT_CTRL_LE_ISO_ENABLED
              brk_sym_iso_WJJo4fn6Hkvt9xXAvziZ,
#endif /* UC_BT_CTRL_LE_ISO_ENABLED */
#if UC_BLE_CTRL_PAWR_BCAST_SUPPORTED
              brk_sym_pawrBcast_eBH7HrPT8y2Fj9382el3,
#endif /* UC_BLE_CTRL_PAWR_BCAST_SUPPORTED */
              brk_sym_resolv_GWz7Tb1ymxsyLQ1208Jj,
              brk_sym_scan_l1avG2cwTzJAlle4KLdU,
              brk_sym_sync_NOAs4m6LAUpamsUYHfqm,
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
              orca_ctrl_on_task_reset,
#endif /* UC_BT_CTRL_BR_EDR_IS_ENABLE */
          ),
);

#if UC_BT_CTRL_BLE_IS_ENABLE && UC_BLE_CTRL_CONN_ENABLED
const void * const _conn_linear_broker_flash[] = BTDM_BROKER_NODE_DEF_FLASH(
#if UC_BLE_CTRL_CONN_ENABLED && CONFIG_BT_LE_ERROR_SIM_ENABLED
    [1] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_connSim_2RFEzu4Evi68Nnt4YowZ,
          ),
    [2] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_connSim_SoaIlItcCgS4km7f1141,
          ),
#endif /* UC_BLE_CTRL_CONN_ENABLED && CONFIG_BT_LE_ERROR_SIM_ENABLED */
#if UC_BLE_CTRL_PAWR_SYNC_SUPPORTED
    [3] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrSync_pbTRKlIHoevVfNidtKZb,
          ),
    [4] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrSync_fn4PoAB0qFmYpbwqTO2H,
          ),
#endif /* UC_BLE_CTRL_PAWR_SYNC_SUPPORTED */
#if UC_BLE_CTRL_PAWR_BCAST_SUPPORTED
    [5] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrBcast_on08yxBjym4I0iV3tlPo,
          ),
#endif /* UC_BLE_CTRL_PAWR_BCAST_SUPPORTED */
    [6] = BTDM_BROKER_ENTRY_DEF_FLASH(
#if UC_BT_CTRL_LE_CIS_CENT_ENABLED
              brk_sym_cisCent_9P0KC1xZQpumJC2psVXO,
#endif /* UC_BT_CTRL_LE_CIS_CENT_ENABLED */
#if UC_BT_CTRL_LE_CIS_PRPH_ENABLED
              brk_sym_cisPrph_6ORekVL63tenTy0sCT2O,
#endif /* UC_BT_CTRL_LE_CIS_PRPH_ENABLED */
#if UC_BT_CTRL_LE_CIS_ENABLED
              brk_sym_cis_wQtv7EbxMlyUdVE0TQ9T,
#endif /* UC_BT_CTRL_LE_CIS_ENABLED */
          ),
    [7] = BTDM_BROKER_ENTRY_DEF_FLASH(
#if UC_BT_CTRL_LE_CIS_CENT_ENABLED
              brk_sym_cisCent_GSToGcnDAoUIQsFaBPr7,
#endif /* UC_BT_CTRL_LE_CIS_CENT_ENABLED */
#if UC_BT_CTRL_LE_CIS_PRPH_ENABLED
              brk_sym_cisPrph_zr8KQ3aEASRV2ZptTSnX,
#endif /* UC_BT_CTRL_LE_CIS_PRPH_ENABLED */
          ),
    [8] = BTDM_BROKER_ENTRY_DEF_FLASH(
#if UC_BT_CTRL_LE_CIS_CENT_ENABLED
              brk_sym_cisCent_JUzUk54A9UNHtsUAHT8F,
#endif /* UC_BT_CTRL_LE_CIS_CENT_ENABLED */
#if UC_BT_CTRL_LE_CIS_PRPH_ENABLED
              brk_sym_cisPrph_xR7NheKHstTWkDAuZROL,
#endif /* UC_BT_CTRL_LE_CIS_PRPH_ENABLED */
          ),
#if UC_BT_CTRL_LE_CIS_ENABLED
    [9] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_cis_eeNELMi6IB5X3UF8MMvQ,
          ),
#endif /* UC_BT_CTRL_LE_CIS_ENABLED */
    [10] = BTDM_BROKER_ENTRY_DEF_FLASH(
#if UC_BT_CTRL_LE_CIS_CENT_ENABLED
              brk_sym_cisCent_wt1P0oioDDyNz026HxsK,
#endif /* UC_BT_CTRL_LE_CIS_CENT_ENABLED */
#if UC_BT_CTRL_LE_CIS_PRPH_ENABLED
              brk_sym_cisPrph_Kb3o0Y7oUCdHJ1SXcoqj,
#endif /* UC_BT_CTRL_LE_CIS_PRPH_ENABLED */
#if UC_BT_CTRL_LE_CIS_ENABLED
              brk_sym_cis_i4bfG8waTBMteoJaWD40,
#endif /* UC_BT_CTRL_LE_CIS_ENABLED */
          ),
#if UC_BT_CTRL_LE_CIS_ENABLED
    [12] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_cis_sHcp2gf28ZC99shUpnis,
          ),
    [13] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_cis_3DmpJ8V6gGsnSNopbei0,
          ),
    [14] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_cis_WICEvILg2pK6JZvhry3j,
          ),
    [15] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_cis_zGkqwv9urnJ8DbX0JZAr,
          ),
    [16] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_cis_Bmj5ai8K8ZKFKqWA8X8B,
          ),
#endif /* UC_BT_CTRL_LE_CIS_ENABLED */
    [17] = BTDM_BROKER_ENTRY_DEF_FLASH(
#if UC_BT_CTRL_LE_CIS_CENT_ENABLED
              brk_sym_cisCent_jfaA90JGHwEKv1YkKIUO,
#endif /* UC_BT_CTRL_LE_CIS_CENT_ENABLED */
#if UC_BT_CTRL_LE_CIS_ENABLED
              brk_sym_cis_mcjvVuw8ttAldZnobGxJ,
#endif /* UC_BT_CTRL_LE_CIS_ENABLED */
          ),
#if UC_BT_CTRL_LE_CIS_ENABLED
    [18] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_cis_Guzdzme9CFLxrbjG6HEJ,
          ),
#endif /* UC_BT_CTRL_LE_CIS_ENABLED */
#if UC_BT_CTRL_LE_CIS_CENT_ENABLED
    [19] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_cisCent_y03W49d1NFScSUUO32Yn,
          ),
#endif /* UC_BT_CTRL_LE_CIS_CENT_ENABLED */
    [20] = BTDM_BROKER_ENTRY_DEF_FLASH(
#if UC_BT_CTRL_LE_CIS_CENT_ENABLED
              brk_sym_cisCent_FX6J62IzYLkcQNdl7BqH,
#endif /* UC_BT_CTRL_LE_CIS_CENT_ENABLED */
#if UC_BT_CTRL_LE_CIS_PRPH_ENABLED
              brk_sym_cisPrph_bU9vRMdklv6bdZ52yCI5,
#endif /* UC_BT_CTRL_LE_CIS_PRPH_ENABLED */
          ),
);
#endif /* UC_BT_CTRL_BLE_IS_ENABLE && UC_BLE_CTRL_CONN_ENABLED */

#if UC_BT_CTRL_BLE_IS_ENABLE
const void * const _nrtIsr_linear_broker_flash[] = BTDM_BROKER_NODE_DEF_FLASH(
);
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */

#if UC_BT_CTRL_BLE_IS_ENABLE
const void * const _perAdv_linear_broker_flash[] = BTDM_BROKER_NODE_DEF_FLASH(
#if UC_BT_CTRL_LE_BIS_BCAST_ENABLED
    [1] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_bisBcast_2a3kgxHeabxbnBIsRh4s,
          ),
    [2] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_bisBcast_EtJ8xQ5EIWS6IjqpsD1U,
          ),
    [3] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_bisBcast_2uiisGIdoGa6SNUZCE2m,
          ),
#endif /* UC_BT_CTRL_LE_BIS_BCAST_ENABLED */
);
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */

#if UC_BT_CTRL_BLE_IS_ENABLE
const void * const _sync_linear_broker_flash[] = BTDM_BROKER_NODE_DEF_FLASH(
#if UC_BLE_CTRL_PAWR_SYNC_SUPPORTED
    [1] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrSync_Qifv1iec7w57n2pBwucX,
          ),
    [2] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrSync_bbizSGqM6wZUOT0fQF1M,
          ),
    [3] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrSync_oBwpWArFlWxUJNCnHvsJ,
          ),
    [4] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrSync_vAN2YoeCsSKafv2NdPVC,
          ),
    [5] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrSync_9XvRcZs9CUOgHNbMhe5G,
          ),
    [6] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrSync_zuG0pqdRWK2zt5S09fTS,
          ),
    [7] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrSync_LO7YyFM93srewqVpETY2,
          ),
    [8] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_pawrSync_7bchpvHsUM8DXHyaXC7f,
          ),
#endif /* UC_BLE_CTRL_PAWR_SYNC_SUPPORTED */
#if UC_BT_CTRL_LE_BIS_SYNC_ENABLED
    [9] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_bisSync_3FCcFQdSowAGd7aAejeg,
          ),
    [10] = BTDM_BROKER_ENTRY_DEF_FLASH(
              brk_sym_bisSync_0xkTwhqVWK4bwJa6dDH6,
          ),
#endif /* UC_BT_CTRL_LE_BIS_SYNC_ENABLED */
);
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */

int
btdm_broker_init(void)
{
#if UC_BT_CTRL_BLE_IS_ENABLE
    if (r_btdm_broker_linear_selfcheck(_adv_linear_broker_flash, false)) {
        return -1;
    }
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */

#if UC_BT_CTRL_BLE_IS_ENABLE
    if (r_btdm_broker_linear_selfcheck(_base_linear_broker_flash, false)) {
        return -2;
    }
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */

    if (r_btdm_broker_linear_selfcheck(_btdm_coex_linear_broker_flash, false)) {
        return -3;
    }

    if (r_btdm_broker_linear_selfcheck(_btdm_hal_linear_broker_ram, true)) {
        return -4;
    }

    if (r_btdm_broker_linear_selfcheck(_btdm_hci_linear_broker_flash, false)) {
        return -5;
    }

    if (r_btdm_broker_linear_selfcheck(_btdm_sched_linear_broker_flash, false)) {
        return -6;
    }

    if (r_btdm_broker_linear_selfcheck(_btdm_sched_linear_broker_ram, true)) {
        return -7;
    }

    if (r_btdm_broker_linear_selfcheck(_btdm_sleep_linear_broker_flash, false)) {
        return -8;
    }

    if (r_btdm_broker_linear_selfcheck(_btdm_sleep_linear_broker_ram, true)) {
        return -9;
    }

    if (r_btdm_broker_linear_selfcheck(_btdm_task_linear_broker_flash, false)) {
        return -10;
    }

#if UC_BT_CTRL_BLE_IS_ENABLE && UC_BLE_CTRL_CONN_ENABLED
    if (r_btdm_broker_linear_selfcheck(_conn_linear_broker_flash, false)) {
        return -11;
    }
#endif /* UC_BT_CTRL_BLE_IS_ENABLE && UC_BLE_CTRL_CONN_ENABLED */

#if UC_BT_CTRL_BLE_IS_ENABLE
    if (r_btdm_broker_linear_selfcheck(_nrtIsr_linear_broker_flash, false)) {
        return -12;
    }
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */

#if UC_BT_CTRL_BLE_IS_ENABLE
    if (r_btdm_broker_linear_selfcheck(_perAdv_linear_broker_flash, false)) {
        return -13;
    }
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */

#if UC_BT_CTRL_BLE_IS_ENABLE
    if (r_btdm_broker_linear_selfcheck(_sync_linear_broker_flash, false)) {
        return -14;
    }
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */


    return 0;
}

void
btdm_broker_deinit(void)
{

}
