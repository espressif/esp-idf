/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
extern void main();

/* Initialize lp core related system functions before calling user's main*/
void lp_core_startup()
{
    main();
}
