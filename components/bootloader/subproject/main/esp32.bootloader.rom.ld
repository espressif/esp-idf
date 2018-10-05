PROVIDE ( ets_update_cpu_frequency = 0x40008550 ); /* Updates g_ticks_per_us on the current CPU only; not on the other core */
PROVIDE ( MD5Final = 0x4005db1c );
PROVIDE ( MD5Init = 0x4005da7c );
PROVIDE ( MD5Update = 0x4005da9c );
