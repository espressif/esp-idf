/*
 * ESP32S2 ROM address table
 * Generated for ROM with MD5sum: f054d40c5f6b9207d3827460a6f5748c
*/
PROVIDE ( ets_update_cpu_frequency = 0x4000d954 );

/* ToDo: Following address may need modification */
PROVIDE ( MD5Final = 0x4005db1c );
PROVIDE ( MD5Init = 0x4005da7c );
PROVIDE ( MD5Update = 0x4005da9c );
/* bootloader will use following functions from xtensa hal library */
xthal_get_ccount = 0x4000c050;
xthal_get_ccompare = 0x4000c078;
xthal_set_ccompare = 0x4000c058;
