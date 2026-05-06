/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIB_AUDIO_H_
#define LIB_AUDIO_H_

#include <stdint.h>
#include <stdbool.h>

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

struct lib_ext_cfgs;
struct lib_ext_funcs;
struct lib_funcs;

extern int lib_ext_structs_check(const uint16_t *ext_structs, size_t size_structs);
extern int lib_ext_cfgs_set(const struct lib_ext_cfgs *ext_cfgs, size_t size_cfgs);
extern int lib_ext_funcs_set(const struct lib_ext_funcs *ext_funcs, size_t size_funcs);
extern int lib_funcs_set(const struct lib_funcs *funcs, size_t size_funcs);
extern const char *lib_ext_commit_get(void);

struct bt_aics;
struct bt_gatt_service;

extern struct bt_aics *lib_aics_insts_get(void);
extern void *lib_aics_svc_get(struct bt_aics *aics);
extern int lib_aics_init(void);
extern int lib_aics_client_init(void);

extern int lib_bap_base_init(void);
extern int lib_bap_iso_init(void);
extern int lib_bap_stream_init(void);

extern struct bt_gatt_service *lib_ascs_svc_get(void);
extern int lib_ascs_init(void);

extern struct bt_gatt_service *lib_bap_bass_svc_get(void);
extern int lib_bap_broadcast_assistant_init(void);
extern int lib_bap_broadcast_sink_init(void);
extern int lib_bap_broadcast_source_init(void);
extern int lib_bap_scan_delegator_init(void);
extern int lib_bap_unicast_client_init(void);
extern int lib_bap_unicast_server_init(void);

extern struct bt_gatt_service *lib_pacs_svc_get(void);
extern int lib_pacs_init(void);

extern struct bt_gatt_service *lib_cas_svc_get(void);

extern int lib_cap_common_init(void);
extern int lib_cap_stream_init(void);
extern int lib_cap_initiator_init(void);
extern int lib_cap_acceptor_init(void);
extern int lib_cap_commander_init(void);

extern int lib_ccid_init(void);

extern int lib_ccp_call_control_client_init(void);
extern int lib_ccp_call_control_server_init(void);

extern int lib_codec_init(void);

extern int lib_csip_set_coordinator_init(void);
extern void *lib_csip_set_member_svc_get(const void *svc_inst);
extern int lib_csip_set_member_init(void);

extern int lib_gmap_client_init(void);
extern struct bt_gatt_service *lib_gmas_svc_get(void);

extern struct bt_gatt_service *lib_has_svc_get(void);
extern int lib_has_init(void);
extern int lib_has_client_init(void);

extern int lib_mcc_init(void);
extern struct bt_gatt_service *lib_mcs_svc_get(void);
extern int lib_mcs_init(void);

extern int lib_media_proxy_init(void);

extern int lib_micp_mic_ctlr_init(void);
extern struct bt_gatt_service *lib_mics_svc_get(void);
extern int lib_micp_mic_dev_init(void);

extern int lib_mpl_init(void);

extern int lib_pbp_init(void);

extern int lib_tbs_client_init(void);
extern struct bt_gatt_service *lib_gtbs_svc_get(void);
extern struct bt_gatt_service *lib_tbs_server_list_get(void);
extern int lib_tbs_init(void);

extern struct bt_gatt_service *lib_tmas_svc_get(void);
extern int lib_tmap_init(void);

extern int lib_vcp_vol_ctlr_init(void);
extern struct bt_gatt_service *lib_vcs_svc_get(void);
extern int lib_vcp_vol_rend_init(void);

struct bt_vocs;
struct bt_vocs_server;

extern int lib_vocs_client_init(void);
extern struct bt_vocs_server *lib_vocs_insts_get(void);
extern void *lib_vocs_svc_get(struct bt_vocs *vocs);
extern int lib_vocs_init(void);

struct bt_conn;
struct bt_gatt_attr;
struct bt_gatt_discover_params;

struct bt_iso_recv_info;

struct bt_audio_codec_cfg;

struct bt_aics;

struct bt_bap_ep;
struct bt_bap_stream;
enum bt_bap_ascs_reason;
enum bt_bap_ascs_rsp_code;
struct bt_bap_qos_cfg_pref;
struct bt_bap_unicast_client_cb;
struct bt_bap_scan_delegator_recv_state;

enum bt_cap_common_subproc_type;
enum bt_cap_common_proc_type;
struct bt_cap_stream;
struct bt_cap_broadcast_source;

struct bt_ots;
struct bt_ots_client;

struct bt_micp_mic_dev_register_param;

struct mpl_search;

struct bt_tbs_instance;

struct bt_vcp_vol_ctlr;
struct bt_vcp_included;
struct bt_vcp_vol_rend_register_param;

struct bt_vocs_client;

#if CONFIG_BT_AICS_CLIENT
extern int lib_aics_client_state_get(struct bt_aics *inst);
extern int lib_aics_client_gain_setting_get(struct bt_aics *inst);
extern int lib_aics_client_type_get(struct bt_aics *inst);
extern int lib_aics_client_status_get(struct bt_aics *inst);
extern int lib_aics_client_unmute(struct bt_aics *inst);
extern int lib_aics_client_mute(struct bt_aics *inst);
extern int lib_aics_client_manual_gain_set(struct bt_aics *inst);
extern int lib_aics_client_automatic_gain_set(struct bt_aics *inst);
extern int lib_aics_client_gain_set(struct bt_aics *inst, int8_t gain);
extern int lib_aics_client_description_get(struct bt_aics *inst);
extern int lib_aics_client_description_set(struct bt_aics *inst, const char *description);
#endif /* CONFIG_BT_AICS_CLIENT */

#if CONFIG_BT_BAP_BROADCAST_SINK
extern bool lib_bap_broadcast_sink_has_ep(const struct bt_bap_ep *ep);
#endif /* CONFIG_BT_BAP_BROADCAST_SINK */

#if CONFIG_BT_BAP_BROADCAST_SOURCE
extern bool lib_bap_broadcast_source_has_ep(const struct bt_bap_ep *ep);
#endif /* CONFIG_BT_BAP_BROADCAST_SOURCE */

#if CONFIG_BT_BAP_UNICAST_CLIENT
extern bool lib_bap_unicast_client_has_ep(const struct bt_bap_ep *ep);
extern int lib_bap_unicast_client_register_cb(struct bt_bap_unicast_client_cb *cb);
extern int lib_bap_unicast_client_config(struct bt_bap_stream *stream,
                                         const struct bt_audio_codec_cfg *codec_cfg);
extern int lib_bap_unicast_client_metadata(struct bt_bap_stream *stream,
                                           const uint8_t meta[], size_t meta_len);
extern int lib_bap_unicast_client_connect(struct bt_bap_stream *stream);
extern int lib_bap_unicast_client_start(struct bt_bap_stream *stream);
extern int lib_bap_unicast_client_disable(struct bt_bap_stream *stream);
extern int lib_bap_unicast_client_release(struct bt_bap_stream *stream);
#endif /* CONFIG_BT_BAP_UNICAST_CLIENT */

#if CONFIG_BT_BAP_UNICAST_SERVER
extern bool lib_bap_unicast_server_has_ep(const struct bt_bap_ep *ep);
extern int lib_bap_unicast_server_reconfig(struct bt_bap_stream *stream,
                                           const struct bt_audio_codec_cfg *codec_cfg);
extern int lib_bap_unicast_server_start(struct bt_bap_stream *stream);
extern int lib_bap_unicast_server_metadata(struct bt_bap_stream *stream,
                                           const uint8_t meta[], size_t meta_len);
extern int lib_bap_unicast_server_disable(struct bt_bap_stream *stream);
extern int lib_bap_unicast_server_release(struct bt_bap_stream *stream);
#endif /* CONFIG_BT_BAP_UNICAST_SERVER */

#if CONFIG_BT_CAP_ACCEPTOR
extern bool lib_cap_acceptor_ccids_exist(const struct bt_conn *conn, const uint8_t ccids[],
                                         uint8_t ccid_cnt);
#endif /* CONFIG_BT_CAP_ACCEPTOR */

#if CONFIG_BT_CAP_INITIATOR_UNICAST
extern void lib_cap_common_set_subproc(enum bt_cap_common_subproc_type subproc_type);
extern bool lib_cap_common_proc_is_type(enum bt_cap_common_proc_type proc_type);
extern bool lib_cap_common_subproc_is_type(enum bt_cap_common_subproc_type subproc_type);
extern void lib_cap_initiator_cp_cb(struct bt_cap_stream *cap_stream,
                                    enum bt_bap_ascs_rsp_code rsp_code,
                                    enum bt_bap_ascs_reason reason);
extern void lib_cap_initiator_codec_configured(struct bt_cap_stream *cap_stream);
extern void lib_cap_initiator_qos_configured(struct bt_cap_stream *cap_stream);
extern void lib_cap_initiator_enabled(struct bt_cap_stream *cap_stream);
extern void lib_cap_initiator_connected(struct bt_cap_stream *cap_stream);
extern void lib_cap_initiator_started(struct bt_cap_stream *cap_stream);
extern void lib_cap_initiator_metadata_updated(struct bt_cap_stream *cap_stream);
extern void lib_cap_initiator_disabled(struct bt_cap_stream *cap_stream);
extern void lib_cap_initiator_stopped(struct bt_cap_stream *cap_stream);
extern void lib_cap_initiator_released(struct bt_cap_stream *cap_stream);
#endif /* CONFIG_BT_CAP_INITIATOR_UNICAST */

#if CONFIG_BT_CAP_HANDOVER
extern bool lib_cap_common_handover_is_active(void);
extern bool lib_cap_handover_is_handover_broadcast_source(const  struct bt_cap_broadcast_source *cap_broadcast_source);
extern void lib_cap_handover_complete(void);
extern void lib_cap_handover_unicast_proc_complete(void);
extern void lib_cap_handover_broadcast_source_stopped(uint8_t reason);
extern void lib_cap_handover_unicast_to_broadcast_reception_start(void);
extern int lib_cap_handover_broadcast_reception_stopped(void);
extern void lib_cap_handover_receive_state_updated(const struct bt_conn *conn,
                                                   const struct bt_bap_scan_delegator_recv_state *state);
#endif /* CONFIG_BT_CAP_HANDOVER */

#if CONFIG_BT_CAP && CONFIG_BT_BAP_UNICAST
extern void lib_cap_stream_configured_cb(struct bt_bap_stream *bap_stream,
                                         const struct bt_bap_qos_cfg_pref *pref);
extern void lib_cap_stream_qos_set_cb(struct bt_bap_stream *bap_stream);
extern void lib_cap_stream_enabled_cb(struct bt_bap_stream *bap_stream);
extern void lib_cap_stream_metadata_updated_cb(struct bt_bap_stream *bap_stream);
extern void lib_cap_stream_disabled_cb(struct bt_bap_stream *bap_stream);
extern void lib_cap_stream_released_cb(struct bt_bap_stream *bap_stream);
#endif /* CONFIG_BT_CAP && CONFIG_BT_BAP_UNICAST */

#if CONFIG_BT_CAP && CONFIG_BT_AUDIO_RX
extern void lib_cap_stream_recv_cb(struct bt_bap_stream *bap_stream,
                                   const struct bt_iso_recv_info *info,
                                   const uint8_t *data, uint16_t len);
#endif /* CONFIG_BT_CAP && CONFIG_BT_AUDIO_RX */

#if CONFIG_BT_CAP && CONFIG_BT_AUDIO_TX
extern void lib_cap_stream_sent_cb(struct bt_bap_stream *bap_stream, void *user_data);
#endif /* CONFIG_BT_CAP && CONFIG_BT_AUDIO_TX */

#if CONFIG_BT_MCC_OTS
extern void lib_mcc_on_obj_selected(struct bt_ots_client *otc_inst,
                                    struct bt_conn *conn, int result);
extern int lib_mcc_on_icon_content(struct bt_ots_client *otc_inst,
                                   struct bt_conn *conn,
                                   uint32_t offset, uint32_t len,
                                   uint8_t *data_p, bool is_complete);
extern void lib_mcc_on_object_metadata(struct bt_ots_client *otc_inst,
                                       struct bt_conn *conn, int err,
                                       uint8_t metadata_read);
#endif /* CONFIG_BT_MCC_OTS */

#if CONFIG_BT_MCS && CONFIG_BT_OTS
extern struct bt_ots *lib_mcs_get_ots(void);
extern void lib_mcs_media_proxy_sctrl_current_track_id_cb(uint64_t id);
extern void lib_mcs_media_proxy_sctrl_next_track_id_cb(uint64_t id);
extern void lib_mcs_media_proxy_sctrl_parent_group_id_cb(uint64_t id);
extern void lib_mcs_media_proxy_sctrl_current_group_id_cb(uint64_t id);
extern void lib_mcs_media_proxy_sctrl_search_cb(uint8_t result_code);
extern void lib_mcs_media_proxy_sctrl_search_results_id_cb(uint64_t id);
extern void lib_mcs_svc_inc_init(void);
extern void lib_mcs_chrc_icon_obj_id_init(void);
extern void lib_mcs_chrc_track_segments_obj_id_init(void);
extern void lib_mcs_chrc_current_track_obj_id_init(void);
extern void lib_mcs_chrc_next_track_obj_id_init(void);
extern void lib_mcs_chrc_parent_group_obj_id_init(void);
extern void lib_mcs_chrc_current_group_obj_id_init(void);
extern void lib_mcs_chrc_search_control_point_init(void);
extern void lib_mcs_chrc_search_results_obj_id_init(void);
#endif /* CONFIG_BT_MCS && CONFIG_BT_OTS */

#if CONFIG_BT_MPL_OBJECTS
extern uint64_t lib_media_proxy_sctrl_get_icon_id(void);
extern uint64_t lib_media_proxy_sctrl_get_track_segments_id(void);
extern uint64_t lib_media_proxy_sctrl_get_current_track_id(void);
extern void lib_media_proxy_sctrl_set_current_track_id(uint64_t id);
extern uint64_t lib_media_proxy_sctrl_get_next_track_id(void);
extern void lib_media_proxy_sctrl_set_next_track_id(uint64_t id);
extern uint64_t lib_media_proxy_sctrl_get_parent_group_id(void);
extern uint64_t lib_media_proxy_sctrl_get_current_group_id(void);
extern void lib_media_proxy_sctrl_set_current_group_id(uint64_t id);
extern void lib_media_proxy_sctrl_send_search(const struct mpl_search *search);
extern uint64_t lib_media_proxy_sctrl_get_search_results_id(void);
extern void lib_media_proxy_pl_current_track_id_cb(uint64_t id);
extern void lib_media_proxy_pl_next_track_id_cb(uint64_t id);
extern void lib_media_proxy_pl_parent_group_id_cb(uint64_t id);
extern void lib_media_proxy_pl_current_group_id_cb(uint64_t id);
extern void lib_media_proxy_pl_search_cb(uint8_t result_code);
extern void lib_media_proxy_pl_search_results_id_cb(uint64_t id);
#endif /* CONFIG_BT_MPL_OBJECTS */

#if CONFIG_BT_MICP_MIC_CTLR_AICS
extern uint8_t lib_micp_discover_include_func(struct bt_conn *conn,
                                              const struct bt_gatt_attr *attr,
                                              struct bt_gatt_discover_params *params);
extern int lib_micp_mic_ctrl_aics_init(void);
#endif /* CONFIG_BT_MICP_MIC_CTLR_AICS */

#if CONFIG_BT_MICP_MIC_DEV_AICS
extern int lib_micp_mic_dev_prepare_aics_inst(struct bt_micp_mic_dev_register_param *param);
#endif /* CONFIG_BT_MICP_MIC_DEV_AICS */

#if CONFIG_BT_MPL_OBJECTS
extern uint64_t lib_mpl_get_icon_id(void);
extern uint64_t lib_mpl_get_track_segments_id(void);
extern uint64_t lib_mpl_get_current_track_id(void);
extern void lib_mpl_set_current_track_id(uint64_t id);
extern uint64_t lib_mpl_get_next_track_id(void);
extern void lib_mpl_set_next_track_id(uint64_t id);
extern uint64_t lib_mpl_get_parent_group_id(void);
extern uint64_t lib_mpl_get_current_group_id(void);
extern void lib_mpl_set_current_group_id(uint64_t id);
extern void lib_mpl_send_search(const struct mpl_search *search);
extern uint64_t lib_mpl_get_search_results_id(void);
#endif /* CONFIG_BT_MPL_OBJECTS */

#if CONFIG_BT_TBS_CLIENT_TBS
extern int lib_tbs_client_primary_discover_tbs(struct bt_conn *conn);
#endif /* CONFIG_BT_TBS_CLIENT_TBS */

#if CONFIG_BT_TBS_CLIENT_GTBS
extern int lib_tbs_client_primary_discover_gtbs(struct bt_conn *conn);
#endif /* CONFIG_BT_TBS_CLIENT_GTBS */

#if CONFIG_BT_TBS_CLIENT_CCID
extern struct bt_tbs_instance *lib_tbs_client_get_by_ccid(const struct bt_conn *conn,
                                                          uint8_t ccid);
#endif /* CONFIG_BT_TBS_CLIENT_CCID */

#if CONFIG_BT_VCP_VOL_CTLR_AICS
extern void lib_vcp_vol_ctlr_aics_init(void);
#endif /* CONFIG_BT_VCP_VOL_CTLR_AICS */

#if CONFIG_BT_VCP_VOL_CTLR_VOCS
extern void lib_vcp_vol_ctlr_vocs_init(void);
#endif /* CONFIG_BT_VCP_VOL_CTLR_VOCS */

#if CONFIG_BT_VCP_VOL_CTLR_AICS || CONFIG_BT_VCP_VOL_CTLR_VOCS
extern uint8_t lib_vcp_vol_ctlr_vcs_discover_include_func(struct bt_conn *conn,
                                                          const struct bt_gatt_attr *attr,
                                                          struct bt_gatt_discover_params *params);

extern int lib_vcp_vol_ctlr_included_get(struct bt_vcp_vol_ctlr *vol_ctlr,
                                         struct bt_vcp_included *included);
#endif /* CONFIG_BT_VCP_VOL_CTLR_AICS || CONFIG_BT_VCP_VOL_CTLR_VOCS */

#if (CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT > 0)
extern int lib_vcp_vol_rend_prepare_aics_inst(struct bt_vcp_vol_rend_register_param *param);
#endif /* (CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT > 0) */

#if (CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT > 0)
extern int lib_vcp_vol_rend_prepare_vocs_inst(struct bt_vcp_vol_rend_register_param *param);
#endif /* (CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT > 0) */

#if CONFIG_BT_VOCS_CLIENT
extern int lib_vocs_client_state_get(struct bt_vocs_client *inst);
extern int lib_vocs_client_state_set(struct bt_vocs_client *inst, int16_t offset);
extern int lib_vocs_client_location_get(struct bt_vocs_client *inst);
extern int lib_vocs_client_location_set(struct bt_vocs_client *inst, uint32_t location);
extern int lib_vocs_client_description_get(struct bt_vocs_client *inst);
extern int lib_vocs_client_description_set(struct bt_vocs_client *inst, const char *description);
#endif /* CONFIG_BT_VOCS_CLIENT */

#ifdef __cplusplus
}
#endif

#endif /* LIB_AUDIO_H_ */
