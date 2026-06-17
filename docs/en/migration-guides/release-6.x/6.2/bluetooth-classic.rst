Bluetooth Classic
=================

:link_to_translation:`zh_CN:[中文]`

Bluedroid
---------

    The following Bluedroid APIs have been changed:

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_hf_defs.h`

        - Type ``esp_hf_wbs_config_t`` is replaced by ``esp_hf_codec_mode_t``. The new type identifies the negotiated codec (CVSD, mSBC, or LC3-SWB), not a Wideband Speech on/off flag. ``esp_hf_wbs_config_t`` is kept as a deprecated alias of ``esp_hf_codec_mode_t``.
        - Enumerators are renamed. Numeric values 0 / 1 / 2 are unchanged. The old ``ESP_HF_WBS_*`` names remain as macros that alias the new enumerators:

            .. list-table::
               :header-rows: 1
               :widths: 40 40 20

               * - Old enumerator
                 - New enumerator
                 - Value
               * - ``ESP_HF_WBS_NONE``
                 - ``ESP_HF_CODEC_NONE``
                 - 0
               * - ``ESP_HF_WBS_NO``
                 - ``ESP_HF_CODEC_CVSD``
                 - 1
               * - ``ESP_HF_WBS_YES``
                 - ``ESP_HF_CODEC_MSBC``
                 - 2
               * - —
                 - ``ESP_HF_CODEC_LC3``
                 - 3

        - Existing ``switch`` statements and comparisons that use ``ESP_HF_WBS_NO`` or ``ESP_HF_WBS_YES`` still compile. Prefer the new names in new code.

        - ``ESP_HF_CODEC_NONE`` means no codec has been negotiated, or the preference is reset to the stack default. It does not mean that CVSD is unavailable. ``ESP_HF_CODEC_CVSD`` is the selected narrowband codec, whether Wideband Speech is unsupported, disabled, or the link fell back from mSBC / LC3.

        - ``ESP_HF_CODEC_LC3`` is new. Code that indexes a three-entry table with the codec value must add an LC3 entry or a default branch.

        - Do not use ``esp_hf_codec_mode_t`` as a capability bitmap. Peer codec capabilities are reported separately as ``ESP_HF_CODEC_CAP_CVSD`` / ``ESP_HF_CODEC_CAP_MSBC`` / ``ESP_HF_CODEC_CAP_LC3`` in ``ESP_HF_BAC_RESPONSE_EVT``.

        - Audio connection state ``ESP_HF_AUDIO_STATE_CONNECTED_LC3`` is added for an established LC3-SWB link, alongside the existing ``ESP_HF_AUDIO_STATE_CONNECTED`` (CVSD) and ``ESP_HF_AUDIO_STATE_CONNECTED_MSBC`` values.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_hf_ag_api.h`

        - ``esp_hf_ag_set_codec()`` now takes ``esp_hf_codec_mode_t``. Pass ``ESP_HF_CODEC_CVSD``, ``ESP_HF_CODEC_MSBC``, ``ESP_HF_CODEC_LC3``, or ``ESP_HF_CODEC_NONE`` to restore the stack default.

        - ``ESP_HF_WBS_RESPONSE_EVT`` (``wbs_rep``): field ``codec`` is now ``esp_hf_codec_mode_t``. The field name ``codec`` is unchanged. A ``status`` field (``esp_bt_status_t``) is added to report whether the request succeeded.

        - ``ESP_HF_BCS_RESPONSE_EVT`` (``bcs_rep``): field ``mode`` is now ``esp_hf_codec_mode_t``. The field name ``mode`` is unchanged.

    Suggested replacements:

    .. code-block:: c

        // Before
        esp_hf_ag_set_codec(bda, ESP_HF_WBS_YES);
        if (param->bcs_rep.mode == ESP_HF_WBS_NO) { /* CVSD */ }

        // After
        esp_hf_ag_set_codec(bda, ESP_HF_CODEC_MSBC);
        if (param->bcs_rep.mode == ESP_HF_CODEC_CVSD) { /* CVSD */ }
        else if (param->bcs_rep.mode == ESP_HF_CODEC_LC3) { /* LC3-SWB */ }
