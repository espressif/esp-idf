/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* The interrupt API functions are implemented in ROM under the deprecated names, define the aliases */
PROVIDE ( esprv_int_set_priority = esprv_intc_int_set_priority );
PROVIDE ( esprv_int_set_threshold = esprv_intc_int_set_threshold );
PROVIDE ( esprv_int_enable = esprv_intc_int_enable );
PROVIDE ( esprv_int_disable = esprv_intc_int_disable );
PROVIDE ( esprv_int_set_type = esprv_intc_int_set_type );
