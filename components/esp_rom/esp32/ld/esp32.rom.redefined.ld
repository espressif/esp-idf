/* 
   ROM Functions defined in this file are not used in ESP-IDF as is,
   and different definitions for functions with the same names are provided.
   This file is not used when linking ESP-IDF and is intended for reference only
*/
  
PROVIDE ( abort = 0x4000bba4 );
PROVIDE ( aes_128_cbc_decrypt = 0x4005cc7c );
PROVIDE ( aes_128_cbc_encrypt = 0x4005cc18 );
PROVIDE ( aes_unwrap = 0x4005ccf0 );
PROVIDE ( base64_decode = 0x4005ced8 );
PROVIDE ( base64_encode = 0x4005cdbc );
PROVIDE ( ets_isr_mask = 0x400067fc );
PROVIDE ( ets_isr_unmask = 0x40006808 );
PROVIDE ( ets_timer_arm = 0x40008368 );
PROVIDE ( ets_timer_arm_us = 0x400083ac );
PROVIDE ( ets_timer_disarm = 0x400083ec );
PROVIDE ( ets_timer_done = 0x40008428 );
PROVIDE ( ets_timer_init = 0x400084e8 );
PROVIDE ( ets_timer_handler_isr = 0x40008454 );
PROVIDE ( ets_timer_setfn = 0x40008350 );
PROVIDE ( _free_r = 0x4000bbcc );
PROVIDE ( _getpid_r = 0x4000bcfc );
PROVIDE ( __getreent = 0x4000be8c );
PROVIDE ( _gettimeofday_r = 0x4000bc58 );
PROVIDE ( hmac_md5 = 0x4005d264 );
PROVIDE ( hmac_md5_vector = 0x4005d17c );
PROVIDE ( hmac_sha1 = 0x40060acc );
PROVIDE ( hmac_sha1_vector = 0x400609e4 );
PROVIDE ( hmac_sha256 = 0x40060d58 );
PROVIDE ( hmac_sha256_vector = 0x40060c84 );
PROVIDE ( _kill_r = 0x4000bd10 );
PROVIDE ( _lock_acquire = 0x4000be14 );
PROVIDE ( _lock_acquire_recursive = 0x4000be28 );
PROVIDE ( _lock_close = 0x4000bdec );
PROVIDE ( _lock_close_recursive = 0x4000be00 );
PROVIDE ( _lock_init = 0x4000bdc4 );
PROVIDE ( _lock_init_recursive = 0x4000bdd8 );
PROVIDE ( _lock_release = 0x4000be64 );
PROVIDE ( _lock_release_recursive = 0x4000be78 );
PROVIDE ( _lock_try_acquire = 0x4000be3c );
PROVIDE ( _lock_try_acquire_recursive = 0x4000be50 );
PROVIDE ( _malloc_r = 0x4000bbb4 );
PROVIDE ( MD5Final = 0x4005db1c );
PROVIDE ( MD5Init = 0x4005da7c );
PROVIDE ( MD5Update = 0x4005da9c );
PROVIDE ( md5_vector = 0x4005db80 );
PROVIDE ( pbkdf2_sha1 = 0x40060ba4 );
PROVIDE ( rc4_skip = 0x40060928 );
PROVIDE ( _raise_r = 0x4000bc70 );
PROVIDE ( _realloc_r = 0x4000bbe0 );
PROVIDE ( _sbrk_r = 0x4000bce4 );
PROVIDE ( sha1_prf = 0x40060ae8 );
PROVIDE ( sha1_vector = 0x40060b64 );
PROVIDE ( sha256_prf = 0x40060d70 );
PROVIDE ( sha256_vector = 0x40060e08 );
PROVIDE ( _system_r = 0x4000bc10 );
PROVIDE ( _times_r = 0x4000bc40 );
PROVIDE ( uart_tx_wait_idle = 0x40009278 );

