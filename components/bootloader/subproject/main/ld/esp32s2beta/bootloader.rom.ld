/*
 * ESP32S2 ROM address table
 * Generated for ROM with MD5sum: f054d40c5f6b9207d3827460a6f5748c
*/
PROVIDE ( ets_update_cpu_frequency = 0x4000d954 );

/* ToDo: Following address may need modification */
PROVIDE ( MD5Final = 0x400056e8 );
PROVIDE ( MD5Init = 0x40005648 );
PROVIDE ( MD5Update = 0x40005668 );
/* bootloader will use following functions from xtensa hal library */
xthal_get_ccount = 0x40015cbc;
xthal_get_ccompare = 0x40015ce8;
xthal_set_ccompare = 0x40015cc4;
