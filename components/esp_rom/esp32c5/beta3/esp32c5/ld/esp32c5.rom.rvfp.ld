/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.rvfp.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum 2476337377df636dda217b0b3c1a63db
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rvfplibsf
 ***************************************/

/* Functions */
__addsf3 = 0x400009e0;
__eqsf2 = 0x400009e4;
__extendsfdf2 = 0x400009e8;
__fixsfsi = 0x400009ec;
__fixunssfsi = 0x400009f0;
__floatdisf = 0x400009f4;
__floatsisf = 0x400009f8;
__floatundisf = 0x400009fc;
__floatunsisf = 0x40000a00;
__gesf2 = 0x40000a04;
__gtsf2 = 0x40000a08;
__lesf2 = 0x40000a0c;
__ltsf2 = 0x40000a10;
__mulsf3 = 0x40000a14;
__nesf2 = 0x40000a18;
__subsf3 = 0x40000a1c;
__truncdfsf2 = 0x40000a20;


/***************************************
 Group rvfplibdf
 ***************************************/

/* Functions */
__adddf3 = 0x40000a24;
__eqdf2 = 0x40000a28;
__fixdfdi = 0x40000a2c;
__fixdfsi = 0x40000a30;
__fixsfdi = 0x40000a34;
__fixunsdfsi = 0x40000a38;
__fixunssfdi = 0x40000a3c;
__floatdidf = 0x40000a40;
__floatsidf = 0x40000a44;
__floatundidf = 0x40000a48;
__floatunsidf = 0x40000a4c;
__gedf2 = 0x40000a50;
__gtdf2 = 0x40000a54;
__ledf2 = 0x40000a58;
__ltdf2 = 0x40000a5c;
__muldf3 = 0x40000a60;
__nedf2 = 0x40000a64;
__subdf3 = 0x40000a68;

/***************************************
 Group libgcc
***************************************/

/* Functions */
__divsf3 = 0x40000874;
__negsf2 = 0x4000089c;
__powisf2 = 0x400008a4;
__unordsf2 = 0x400008b0;
__absvdi2 = 0x400008b4;
__absvsi2 = 0x400008b8;
__addvdi3 = 0x400008c0;
__addvsi3 = 0x400008c4;
__ashldi3 = 0x400008c8;
__ashrdi3 = 0x400008cc;
__bswapdi2 = 0x400008d0;
__bswapsi2 = 0x400008d4;
__clear_cache = 0x400008d8;
__clrsbdi2 = 0x400008dc;
__clrsbsi2 = 0x400008e0;
__clzdi2 = 0x400008e4;
__clzsi2 = 0x400008e8;
__cmpdi2 = 0x400008ec;
__ctzdi2 = 0x400008f0;
__ctzsi2 = 0x400008f4;
__divdc3 = 0x400008f8;
__divdf3 = 0x400008fc;
__divdi3 = 0x40000900;
__divsc3 = 0x40000904;
__divsi3 = 0x40000908;
__ffsdi2 = 0x40000914;
__ffssi2 = 0x40000918;
__gcc_bcmp = 0x4000094c;
__lshrdi3 = 0x4000095c;
__moddi3 = 0x40000964;
__modsi3 = 0x40000968;
__muldc3 = 0x4000096c;
__muldi3 = 0x40000974;
__mulsc3 = 0x40000978;
__mulsi3 = 0x4000097c;
__mulvdi3 = 0x40000980;
__mulvsi3 = 0x40000984;
__negdf2 = 0x4000098c;
__negdi2 = 0x40000990;
__negvdi2 = 0x40000994;
__negvsi2 = 0x40000998;
__paritysi2 = 0x4000099c;
__popcountdi2 = 0x400009a0;
__popcountsi2 = 0x400009a4;
__powidf2 = 0x400009a8;
__subvdi3 = 0x400009b0;
__subvsi3 = 0x400009b4;
__ucmpdi2 = 0x400009b8;
__udivdi3 = 0x400009bc;
__udivmoddi4 = 0x400009c0;
__udivsi3 = 0x400009c4;
__udiv_w_sdiv = 0x400009c8;
__umoddi3 = 0x400009cc;
__umodsi3 = 0x400009d0;
__unorddf2 = 0x400009d4;
__extenddftf2 = 0x400009d8;
__trunctfdf2 = 0x400009dc;
