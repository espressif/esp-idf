/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*
 * NAN ND-PMK Test Vector Header
 */

#ifndef NAN_PMK_TEST_H
#define NAN_PMK_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Run NAN ND-PMK derivation test vectors
 *
 * Tests the PBKDF2 derivation against official Wi-Fi Aware Specification
 * test vectors (Appendix M.1)
 */
void nan_pmk_run_test_vectors(void);

#ifdef __cplusplus
}
#endif

#endif /* NAN_PMK_TEST_H */
