/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Fixed key ids and eFuse block/id assignments baked into the committed
 * NVS fixture (fixtures/nvs_efuse_v1.bin). Bumping ANY of these values
 * means regenerating the fixture and bumping its filename's version.
 *
 * The eFuse block/id values are chosen to align with the existing
 * volatile tests on each driver's runner so the same physical eFuse
 * key the runner already has burned for the volatile test path serves
 * the persistent path too.
 */
#pragma once

/* Persistent PSA key ids (chosen arbitrarily, must be non-zero). */
#define ESP_PERSISTENT_FIXTURE_DS_KEY_ID     0x1ADA1U
#define ESP_PERSISTENT_FIXTURE_HMAC_KEY_ID   0x1ADA2U
#define ESP_PERSISTENT_FIXTURE_ECDSA_KEY_ID  0x1ADA3U

/* eFuse block / key-id assignments. The capture chip's import path uses
 * the dynamic-purpose wrap to satisfy validation; on the consume runners
 * these must match the eFuse blocks that runner has burned for the
 * relevant peripheral. */
/* RSA-DS uses a distinct block from HMAC so the two persistent keys
 * don't visually share an eFuse id. The DS HW is wrapped at op time, so
 * the runner doesn't actually need anything burned at this block — any
 * non-conflicting value works. */
#define ESP_PERSISTENT_FIXTURE_DS_EFUSE_KEY_ID    1  /* HMAC_KEY1; DS HW wrapped at op time */
#define ESP_PERSISTENT_FIXTURE_HMAC_EFUSE_KEY_ID  0  /* HMAC_KEY0 — matches the nvs_encr_hmac runner's real burned key */
#define ESP_PERSISTENT_FIXTURE_ECDSA_EFUSE_BLOCK  5  /* EFUSE_BLK_KEY1, matches SECP256R1_EFUSE_BLOCK in test_psa_ecdsa.c */
