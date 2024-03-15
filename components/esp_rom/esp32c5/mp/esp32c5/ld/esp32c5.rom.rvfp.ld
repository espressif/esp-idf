/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.rvfp.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum 2f8bd1d8c95e398fe4f1ea496130ceb5
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rvfplibsf
 ***************************************/

/* Functions */
__addsf3 = 0x40000a18;
__eqsf2 = 0x40000a1c;
__extendsfdf2 = 0x40000a20;
__fixsfsi = 0x40000a24;
__fixunssfsi = 0x40000a28;
__floatdisf = 0x40000a2c;
__floatsisf = 0x40000a30;
__floatundisf = 0x40000a34;
__floatunsisf = 0x40000a38;
__gesf2 = 0x40000a3c;
__gtsf2 = 0x40000a40;
__lesf2 = 0x40000a44;
__ltsf2 = 0x40000a48;
__mulsf3 = 0x40000a4c;
__nesf2 = 0x40000a50;
__subsf3 = 0x40000a54;
__truncdfsf2 = 0x40000a58;


/***************************************
 Group rvfplibdf
 ***************************************/

/* Functions */
__adddf3 = 0x40000a5c;
__eqdf2 = 0x40000a60;
__fixdfdi = 0x40000a64;
__fixdfsi = 0x40000a68;
__fixsfdi = 0x40000a6c;
__fixunsdfsi = 0x40000a70;
__fixunssfdi = 0x40000a74;
__floatdidf = 0x40000a78;
__floatsidf = 0x40000a7c;
__floatundidf = 0x40000a80;
__floatunsidf = 0x40000a84;
__gedf2 = 0x40000a88;
__gtdf2 = 0x40000a8c;
__ledf2 = 0x40000a90;
__ltdf2 = 0x40000a94;
__muldf3 = 0x40000a98;
__nedf2 = 0x40000a9c;
__subdf3 = 0x40000aa0;

/***************************************
 Group libgcc
***************************************/

/* Functions */
__divsf3 = 0x400008ac;
__negsf2 = 0x400008d4;
__powisf2 = 0x400008dc;
__unordsf2 = 0x400008e8;
__absvdi2 = 0x400008ec;
__absvsi2 = 0x400008f0;
__addvdi3 = 0x400008f8;
__addvsi3 = 0x400008fc;
__ashldi3 = 0x40000900;
__ashrdi3 = 0x40000904;
__bswapdi2 = 0x40000908;
__bswapsi2 = 0x4000090c;
__clear_cache = 0x40000910;
__clrsbdi2 = 0x40000914;
__clrsbsi2 = 0x40000918;
__clzdi2 = 0x4000091c;
__clzsi2 = 0x40000920;
__cmpdi2 = 0x40000924;
__ctzdi2 = 0x40000928;
__ctzsi2 = 0x4000092c;
__divdc3 = 0x40000930;
__divdf3 = 0x40000934;
__divdi3 = 0x40000938;
__divsc3 = 0x4000093c;
__divsi3 = 0x40000940;
__ffsdi2 = 0x4000094c;
__ffssi2 = 0x40000950;
__gcc_bcmp = 0x40000984;
__lshrdi3 = 0x40000994;
__moddi3 = 0x4000099c;
__modsi3 = 0x400009a0;
__muldc3 = 0x400009a4;
__muldi3 = 0x400009ac;
__mulsc3 = 0x400009b0;
__mulsi3 = 0x400009b4;
__mulvdi3 = 0x400009b8;
__mulvsi3 = 0x400009bc;
__negdf2 = 0x400009c4;
__negdi2 = 0x400009c8;
__negvdi2 = 0x400009cc;
__negvsi2 = 0x400009d0;
__paritysi2 = 0x400009d4;
__popcountdi2 = 0x400009d8;
__popcountsi2 = 0x400009dc;
__powidf2 = 0x400009e0;
__subvdi3 = 0x400009e8;
__subvsi3 = 0x400009ec;
__ucmpdi2 = 0x400009f0;
__udivdi3 = 0x400009f4;
__udivmoddi4 = 0x400009f8;
__udivsi3 = 0x400009fc;
__udiv_w_sdiv = 0x40000a00;
__umoddi3 = 0x40000a04;
__umodsi3 = 0x40000a08;
__unorddf2 = 0x40000a0c;
__extenddftf2 = 0x40000a10;
__trunctfdf2 = 0x40000a14;
