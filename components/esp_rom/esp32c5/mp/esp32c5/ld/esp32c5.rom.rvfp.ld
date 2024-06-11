/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.rvfp.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum f5c146321f24f88ad1f27234da5aed11
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rvfplibsf
 ***************************************/

/* Functions */
__addsf3 = 0x40000a2c;
__eqsf2 = 0x40000a30;
__extendsfdf2 = 0x40000a34;
__fixsfsi = 0x40000a38;
__fixunssfsi = 0x40000a3c;
__floatdisf = 0x40000a40;
__floatsisf = 0x40000a44;
__floatundisf = 0x40000a48;
__floatunsisf = 0x40000a4c;
__gesf2 = 0x40000a50;
__gtsf2 = 0x40000a54;
__lesf2 = 0x40000a58;
__ltsf2 = 0x40000a5c;
__mulsf3 = 0x40000a60;
__nesf2 = 0x40000a64;
__subsf3 = 0x40000a68;
__truncdfsf2 = 0x40000a6c;


/***************************************
 Group rvfplibdf
 ***************************************/

/* Functions */
__adddf3 = 0x40000a70;
__eqdf2 = 0x40000a74;
__fixdfdi = 0x40000a78;
__fixdfsi = 0x40000a7c;
__fixsfdi = 0x40000a80;
__fixunsdfsi = 0x40000a84;
__fixunssfdi = 0x40000a88;
__floatdidf = 0x40000a8c;
__floatsidf = 0x40000a90;
__floatundidf = 0x40000a94;
__floatunsidf = 0x40000a98;
__gedf2 = 0x40000a9c;
__gtdf2 = 0x40000aa0;
__ledf2 = 0x40000aa4;
__ltdf2 = 0x40000aa8;
__muldf3 = 0x40000aac;
__nedf2 = 0x40000ab0;
__subdf3 = 0x40000ab4;

/***************************************
 Group libgcc
***************************************/

/* Functions */
__divsf3 = 0x400008c0;
__negsf2 = 0x400008e8;
__powisf2 = 0x400008f0;
__unordsf2 = 0x400008fc;
__absvdi2 = 0x40000900;
__absvsi2 = 0x40000904;
__addvdi3 = 0x4000090c;
__addvsi3 = 0x40000910;
__ashldi3 = 0x40000914;
__ashrdi3 = 0x40000918;
__bswapdi2 = 0x4000091c;
__bswapsi2 = 0x40000920;
__clear_cache = 0x40000924;
__clrsbdi2 = 0x40000928;
__clrsbsi2 = 0x4000092c;
__clzdi2 = 0x40000930;
__clzsi2 = 0x40000934;
__cmpdi2 = 0x40000938;
__ctzdi2 = 0x4000093c;
__ctzsi2 = 0x40000940;
__divdc3 = 0x40000944;
__divdf3 = 0x40000948;
__divdi3 = 0x4000094c;
__divsc3 = 0x40000950;
__divsi3 = 0x40000954;
__ffsdi2 = 0x40000960;
__ffssi2 = 0x40000964;
__gcc_bcmp = 0x40000998;
__lshrdi3 = 0x400009a8;
__moddi3 = 0x400009b0;
__modsi3 = 0x400009b4;
__muldc3 = 0x400009b8;
__muldi3 = 0x400009c0;
__mulsc3 = 0x400009c4;
__mulsi3 = 0x400009c8;
__mulvdi3 = 0x400009cc;
__mulvsi3 = 0x400009d0;
__negdf2 = 0x400009d8;
__negdi2 = 0x400009dc;
__negvdi2 = 0x400009e0;
__negvsi2 = 0x400009e4;
__paritysi2 = 0x400009e8;
__popcountdi2 = 0x400009ec;
__popcountsi2 = 0x400009f0;
__powidf2 = 0x400009f4;
__subvdi3 = 0x400009fc;
__subvsi3 = 0x40000a00;
__ucmpdi2 = 0x40000a04;
__udivdi3 = 0x40000a08;
__udivmoddi4 = 0x40000a0c;
__udivsi3 = 0x40000a10;
__udiv_w_sdiv = 0x40000a14;
__umoddi3 = 0x40000a18;
__umodsi3 = 0x40000a1c;
__unorddf2 = 0x40000a20;
__extenddftf2 = 0x40000a24;
__trunctfdf2 = 0x40000a28;
