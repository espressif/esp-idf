/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c2.rom.rvfp.ld esp32c2
 *
 *
 * Generated from ./interface-esp32c2.yml md5sum c679b6ed5e9f0a9c3e7b93e5e0f2a1a3
 *
 * Compatible with ROM where ECO version equal or greater to 1.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rvfplib
 ***************************************/

/* Functions */
__adddf3 = 0x40000a18;
__addsf3 = 0x40000a1c;
__divsf3 = 0x40000a20;
__eqdf2 = 0x40000a24;
__eqsf2 = 0x40000a28;
__extendsfdf2 = 0x40000a2c;
__fixdfdi = 0x40000a30;
__fixdfsi = 0x40000a34;
__fixsfdi = 0x40000a38;
__fixsfsi = 0x40000a3c;
__fixunsdfsi = 0x40000a40;
__fixunssfdi = 0x40000a44;
__fixunssfsi = 0x40000a48;
__floatdidf = 0x40000a4c;
__floatdisf = 0x40000a50;
__floatsidf = 0x40000a54;
__floatsisf = 0x40000a58;
__floatundidf = 0x40000a5c;
__floatundisf = 0x40000a60;
__floatunsidf = 0x40000a64;
__floatunsisf = 0x40000a68;
__gedf2 = 0x40000a6c;
__gesf2 = 0x40000a70;
__gtdf2 = 0x40000a74;
__gtsf2 = 0x40000a78;
__ledf2 = 0x40000a7c;
__lesf2 = 0x40000a80;
__ltdf2 = 0x40000a84;
__ltsf2 = 0x40000a88;
__muldf3 = 0x40000a8c;
__mulsf3 = 0x40000a90;
__nedf2 = 0x40000a94;
__nesf2 = 0x40000a98;
__subdf3 = 0x40000a9c;
__subsf3 = 0x40000aa0;
__truncdfsf2 = 0x40000aa4;

/***************************************
 Group libgcc
***************************************/

/* Functions */
__absvdi2 = 0x400008a8;
__absvsi2 = 0x400008ac;
__addvdi3 = 0x400008b8;
__addvsi3 = 0x400008bc;
__ashldi3 = 0x400008c0;
__ashrdi3 = 0x400008c4;
__bswapdi2 = 0x400008c8;
__bswapsi2 = 0x400008cc;
__clear_cache = 0x400008d0;
__clrsbdi2 = 0x400008d4;
__clrsbsi2 = 0x400008d8;
__clzdi2 = 0x400008dc;
__clzsi2 = 0x400008e0;
__cmpdi2 = 0x400008e4;
__ctzdi2 = 0x400008e8;
__ctzsi2 = 0x400008ec;
__divdc3 = 0x400008f0;
__divdf3 = 0x400008f4;
__divdi3 = 0x400008f8;
__divsc3 = 0x400008fc;
__divsi3 = 0x40000904;
__ffsdi2 = 0x40000914;
__ffssi2 = 0x40000918;
__gcc_bcmp = 0x40000958;
__lshrdi3 = 0x40000974;
__moddi3 = 0x40000980;
__modsi3 = 0x40000984;
__muldc3 = 0x40000988;
__muldi3 = 0x40000990;
__mulsc3 = 0x40000994;
__mulsi3 = 0x4000099c;
__mulvdi3 = 0x400009a0;
__mulvsi3 = 0x400009a4;
__negdf2 = 0x400009ac;
__negdi2 = 0x400009b0;
__negsf2 = 0x400009b4;
__negvdi2 = 0x400009b8;
__negvsi2 = 0x400009bc;
__paritysi2 = 0x400009c4;
__popcountdi2 = 0x400009c8;
__popcountsi2 = 0x400009cc;
__powidf2 = 0x400009d0;
__powisf2 = 0x400009d4;
__subvdi3 = 0x400009e0;
__subvsi3 = 0x400009e4;
__ucmpdi2 = 0x400009ec;
__udivdi3 = 0x400009f0;
__udivmoddi4 = 0x400009f4;
__udivsi3 = 0x400009f8;
__udiv_w_sdiv = 0x400009fc;
__umoddi3 = 0x40000a00;
__umodsi3 = 0x40000a04;
__unorddf2 = 0x40000a08;
__unordsf2 = 0x40000a0c;
__extenddftf2 = 0x40000a10;
__trunctfdf2 = 0x40000a14;
