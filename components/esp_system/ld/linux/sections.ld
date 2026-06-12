/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
SECTIONS
{
    .esp_sys_event :
    {
        . = ALIGN(4);
        PROVIDE(_esysev_shdn_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.esysev_shdn.*)))
        PROVIDE(_esysev_shdn_end = .);

        . = ALIGN(4);
        PROVIDE(_esysev_initc_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.esysev_initc.*)))
        PROVIDE(_esysev_initc_end = .);

        . = ALIGN(4);
        PROVIDE(_esysev_inits_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.esysev_inits.*)))
        PROVIDE(_esysev_inits_end = .);

        . = ALIGN(4);
        PROVIDE(_esysev_panic_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.esysev_panic.*)))
        PROVIDE(_esysev_panic_end = .);

        . = ALIGN(4);
        PROVIDE(_esysev_paneb_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.esysev_paneb.*)))
        PROVIDE(_esysev_paneb_end = .);
    }
}
INSERT AFTER .data;
