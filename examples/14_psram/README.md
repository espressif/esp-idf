简介：
	本工程主要演示psram的初始化以及读写操作
注意事项：
	psram进行读写操作软件应用层面需要做原子保护
	
	
###Brief:
This example shows how to 

* init psram: call psram_enable() API
* PSRAM_CACHE_F40M_S40M mean flash cache 40Mhz and sram cache 40Mhz, which is the recommended option in current version.
* read & write psram: Must add lock for read/write operations in this version.
*  We need to initialize mmu in set_cache_and_start_app() of bootloader_start.c, refer to bootloader_start.c in the demo folder. 

	```
//32KBytes for each page, totally 128 pages, that is 4MBytes.
cache_sram_mmu_set( 0, 0, 0x3f800000, 0, 32, 128 );
```

###Restrictions:

1. GPIO16, GPIO17 and GPIO20 are occupied, can not be used in app code.
2. GPIO matrix signal 224 and 225 are occupied, can not be used in app code. 
3. SPI1 is used to initialize the psram chip.
4. Must add lock for psram read/write operations in this version.
5. ~~For now, we have some confilcts between spi flash read/write and psram driver, we will resolve it soon.~~

   
###update on 161206: Item No.5 has been fixed
 

