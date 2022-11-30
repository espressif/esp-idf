/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32h2.rom.rvfp.ld for esp32h2
 *
 *
 * Generated from ./target/esp32h2/interface-esp32h2.yml md5sum 5d28ef7e188c1740008212a69e90de1e
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rvfplib
 ***************************************/

/* Functions */
__adddf3 = 0x400009e4;
__addsf3 = 0x400009e8;
__eqdf2 = 0x400009ec;
__eqsf2 = 0x400009f0;
__extendsfdf2 = 0x400009f4;
__fixdfdi = 0x400009f8;
__fixdfsi = 0x400009fc;
__fixsfdi = 0x40000a00;
__fixsfsi = 0x40000a04;
__fixunsdfsi = 0x40000a08;
__fixunssfdi = 0x40000a0c;
__fixunssfsi = 0x40000a10;
__floatdidf = 0x40000a14;
__floatdisf = 0x40000a18;
__floatsidf = 0x40000a1c;
__floatsisf = 0x40000a20;
__floatundidf = 0x40000a24;
__floatundisf = 0x40000a28;
__floatunsidf = 0x40000a2c;
__floatunsisf = 0x40000a30;
__gedf2 = 0x40000a34;
__gesf2 = 0x40000a38;
__gtdf2 = 0x40000a3c;
__gtsf2 = 0x40000a40;
__ledf2 = 0x40000a44;
__lesf2 = 0x40000a48;
__ltdf2 = 0x40000a4c;
__ltsf2 = 0x40000a50;
__muldf3 = 0x40000a54;
__mulsf3 = 0x40000a58;
__nedf2 = 0x40000a5c;
__nesf2 = 0x40000a60;
__subdf3 = 0x40000a64;
__subsf3 = 0x40000a68;
__truncdfsf2 = 0x40000a6c;

/***************************************
 Group libgcc
***************************************/

/* Functions */
__absvdi2 = 0x40000874;
__absvsi2 = 0x40000878;
__addvdi3 = 0x40000884;
__addvsi3 = 0x40000888;
__ashldi3 = 0x4000088c;
__ashrdi3 = 0x40000890;
__bswapdi2 = 0x40000894;
__bswapsi2 = 0x40000898;
__clear_cache = 0x4000089c;
__clrsbdi2 = 0x400008a0;
__clrsbsi2 = 0x400008a4;
__clzdi2 = 0x400008a8;
__clzsi2 = 0x400008ac;
__cmpdi2 = 0x400008b0;
__ctzdi2 = 0x400008b4;
__ctzsi2 = 0x400008b8;
__divdc3 = 0x400008bc;
__divdf3 = 0x400008c0;
__divdi3 = 0x400008c4;
__divsc3 = 0x400008c8;
__divsf3 = 0x400008cc;
__divsi3 = 0x400008d0;
__ffsdi2 = 0x400008e0;
__ffssi2 = 0x400008e4;
__gcc_bcmp = 0x40000924;
__lshrdi3 = 0x40000940;
__moddi3 = 0x4000094c;
__modsi3 = 0x40000950;
__muldc3 = 0x40000954;
__muldi3 = 0x4000095c;
__mulsc3 = 0x40000960;
__mulsi3 = 0x40000968;
__mulvdi3 = 0x4000096c;
__mulvsi3 = 0x40000970;
__negdf2 = 0x40000978;
__negdi2 = 0x4000097c;
__negsf2 = 0x40000980;
__negvdi2 = 0x40000984;
__negvsi2 = 0x40000988;
__paritysi2 = 0x40000990;
__popcountdi2 = 0x40000994;
__popcountsi2 = 0x40000998;
__powidf2 = 0x4000099c;
__powisf2 = 0x400009a0;
__subvdi3 = 0x400009ac;
__subvsi3 = 0x400009b0;
__ucmpdi2 = 0x400009b8;
__udivdi3 = 0x400009bc;
__udivmoddi4 = 0x400009c0;
__udivsi3 = 0x400009c4;
__udiv_w_sdiv = 0x400009c8;
__umoddi3 = 0x400009cc;
__umodsi3 = 0x400009d0;
__unorddf2 = 0x400009d4;
__unordsf2 = 0x400009d8;
__extenddftf2 = 0x400009dc;
__trunctfdf2 = 0x400009e0;
/***************************************
 Group libgcc
***************************************/

/* Functions */
