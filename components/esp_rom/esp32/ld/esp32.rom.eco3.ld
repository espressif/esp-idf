/*
ESP32 ECO3 ROM address table
Secure Boot Version 2 API's imported from the ROM
*/
PROVIDE ( ets_secure_boot_verify_signature = 0x4006543c);
PROVIDE ( ets_secure_boot_verify_boot_bootloader = 0x400655ec);
PROVIDE ( ets_use_secure_boot_v2 = 0x4000f8d4);
PROVIDE ( ets_rsa_pss_verify = 0x40065310);
PROVIDE ( ets_mgf1_sha256 = 0x400651a8);
PROVIDE ( ets_emsa_pss_verify = 0x4006520c);
