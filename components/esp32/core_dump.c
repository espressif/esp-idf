// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/uart_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "driver/gpio.h"
#include "rom/crc.h"

#include "esp_panic.h"
#include "esp_partition.h"

#if CONFIG_ESP32_ENABLE_COREDUMP
#define LOG_LOCAL_LEVEL CONFIG_ESP32_CORE_DUMP_LOG_LEVEL
#include "esp_log.h"
const static DRAM_ATTR char TAG[] = "esp_core_dump";

#define ESP_COREDUMP_LOG( level, format, ... )  if (LOG_LOCAL_LEVEL >= level)   { ets_printf(DRAM_STR(format), esp_log_early_timestamp(), (const char *)TAG, ##__VA_ARGS__); }
#define ESP_COREDUMP_LOGE( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_ERROR, LOG_FORMAT(E, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGW( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_WARN, LOG_FORMAT(W, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGI( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_INFO, LOG_FORMAT(I, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGD( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_DEBUG, LOG_FORMAT(D, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGV( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_VERBOSE, LOG_FORMAT(V, format), ##__VA_ARGS__)

#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH
#define ESP_COREDUMP_LOG_PROCESS( format, ... )  ESP_COREDUMP_LOGD(format, ##__VA_ARGS__)
#else
#define ESP_COREDUMP_LOG_PROCESS( format, ... )  do{/*(__VA_ARGS__);*/}while(0)
#endif

// TODO: allow user to set this in menuconfig or get tasks iteratively
#define COREDUMP_MAX_TASKS_NUM              32
#define COREDUMP_MAX_TASK_STACK_SIZE        (64*1024)

typedef esp_err_t (*esp_core_dump_write_prepare_t)(void *priv, uint32_t *data_len);
typedef esp_err_t (*esp_core_dump_write_start_t)(void *priv);
typedef esp_err_t (*esp_core_dump_write_end_t)(void *priv);
typedef esp_err_t (*esp_core_dump_flash_write_data_t)(void *priv, void * data, uint32_t data_len);

/** core dump emitter control structure */
typedef struct _core_dump_write_config_t
{
    // this function is called before core dump data writing
    // used for sanity checks
    esp_core_dump_write_prepare_t       prepare;
    // this function is called at the beginning of data writing
    esp_core_dump_write_start_t         start;
    // this function is called when all dump data are written
    esp_core_dump_write_end_t           end;
    // this function is called to write data chunk
    esp_core_dump_flash_write_data_t    write;
    // number of tasks with corrupted TCBs
    uint32_t                            bad_tasks_num;
    // pointer to data which are specific for particular core dump emitter
    void *                              priv;
} core_dump_write_config_t;

/** core dump data header */
typedef struct _core_dump_header_t
{
    uint32_t data_len;  // data length
    uint32_t tasks_num; // number of tasks
    uint32_t tcb_sz;    // size of TCB
} core_dump_header_t;

/** core dump task data header */
typedef struct _core_dump_task_header_t
{
    void *   tcb_addr;    // TCB address
    uint32_t stack_start; // stack start address
    uint32_t stack_end;   // stack end address
} core_dump_task_header_t;

static inline bool esp_task_stack_start_is_sane(uint32_t sp)
{
    return !(sp < 0x3ffae010UL || sp > 0x3fffffffUL);
}

static inline bool esp_tcb_addr_is_sane(uint32_t addr, uint32_t sz)
{
    //TODO: currently core dump supports TCBs in DRAM only, external SRAM not supported yet
    return !(addr < 0x3ffae000UL || (addr + sz) > 0x40000000UL);
}

static void esp_core_dump_write(XtExcFrame *frame, core_dump_write_config_t *write_cfg)
{
    int cur_task_bad = 0;
    esp_err_t err;
    TaskSnapshot_t tasks[COREDUMP_MAX_TASKS_NUM];
    UBaseType_t tcb_sz, tcb_sz_padded, task_num;
    uint32_t data_len = 0, i, len;
    union
    {
        core_dump_header_t      hdr;
        core_dump_task_header_t task_hdr;
    } dump_data;

    task_num = uxTaskGetSnapshotAll(tasks, COREDUMP_MAX_TASKS_NUM, &tcb_sz);
    // take TCB padding into account, actual TCB size will be stored in header
    if (tcb_sz % sizeof(uint32_t))
        tcb_sz_padded = (tcb_sz / sizeof(uint32_t) + 1) * sizeof(uint32_t);
    else
        tcb_sz_padded = tcb_sz;
    // header + tasknum*(tcb + stack start/end + tcb addr)
    data_len = sizeof(core_dump_header_t) + task_num*(tcb_sz_padded + sizeof(core_dump_task_header_t));
    for (i = 0; i < task_num; i++) {
        if (!esp_tcb_addr_is_sane((uint32_t)tasks[i].pxTCB, tcb_sz)) {
            ESP_COREDUMP_LOG_PROCESS("Bad TCB addr %x!", tasks[i].pxTCB);
            write_cfg->bad_tasks_num++;
            continue;
        }
        if (tasks[i].pxTCB == xTaskGetCurrentTaskHandleForCPU(xPortGetCoreID())) {
            // set correct stack top for current task
            tasks[i].pxTopOfStack = (StackType_t *)frame;
            ESP_COREDUMP_LOG_PROCESS("Current task EXIT/PC/PS/A0/SP %x %x %x %x %x",
                frame->exit, frame->pc, frame->ps, frame->a0, frame->a1);
        }
        else {
            XtSolFrame *task_frame = (XtSolFrame *)tasks[i].pxTopOfStack;
            if (task_frame->exit == 0) {
                ESP_COREDUMP_LOG_PROCESS("Task EXIT/PC/PS/A0/SP %x %x %x %x %x",
                    task_frame->exit, task_frame->pc, task_frame->ps, task_frame->a0, task_frame->a1);
            }
            else {
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH
                XtExcFrame *task_frame2 = (XtExcFrame *)tasks[i].pxTopOfStack;
#endif
                ESP_COREDUMP_LOG_PROCESS("Task EXIT/PC/PS/A0/SP %x %x %x %x %x",
                    task_frame2->exit, task_frame2->pc, task_frame2->ps, task_frame2->a0, task_frame2->a1);
            }
        }
        len = (uint32_t)tasks[i].pxEndOfStack - (uint32_t)tasks[i].pxTopOfStack;
        // check task's stack
        if (!esp_stack_ptr_is_sane((uint32_t)tasks[i].pxTopOfStack) || !esp_task_stack_start_is_sane((uint32_t)tasks[i].pxEndOfStack)
            || len > COREDUMP_MAX_TASK_STACK_SIZE) {
            if (tasks[i].pxTCB == xTaskGetCurrentTaskHandleForCPU(xPortGetCoreID())) {
                cur_task_bad = 1;
            }
            ESP_COREDUMP_LOG_PROCESS("Corrupted TCB %x: stack len %lu, top %x, end %x!",
                tasks[i].pxTCB, len, tasks[i].pxTopOfStack, tasks[i].pxEndOfStack);
            tasks[i].pxTCB = 0; // make TCB addr invalid to skip it in dump
            write_cfg->bad_tasks_num++;
        } else {
            ESP_COREDUMP_LOG_PROCESS("Stack len = %lu (%x %x)", len, tasks[i].pxTopOfStack, tasks[i].pxEndOfStack);
            // take stack padding into account
            len = (len + sizeof(uint32_t) - 1) & ~(sizeof(uint32_t) - 1);
            data_len += len;
        }
    }
    data_len -= write_cfg->bad_tasks_num*(tcb_sz_padded + sizeof(core_dump_task_header_t));

    ESP_COREDUMP_LOG_PROCESS("Core dump len = %lu (%d %d)", data_len, task_num, write_cfg->bad_tasks_num);

    // prepare write
    if (write_cfg->prepare) {
        err = write_cfg->prepare(write_cfg->priv, &data_len);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to prepare core dump (%d)!", err);
            return;
        }
    }
    // write start
    if (write_cfg->start) {
        err = write_cfg->start(write_cfg->priv);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to start core dump (%d)!", err);
            return;
        }
    }
    // write header
    dump_data.hdr.data_len  = data_len;
    dump_data.hdr.tasks_num = task_num - write_cfg->bad_tasks_num;
    dump_data.hdr.tcb_sz    = tcb_sz;
    err = write_cfg->write(write_cfg->priv, &dump_data, sizeof(core_dump_header_t));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write core dump header (%d)!", err);
        return;
    }
    // write tasks
    for (i = 0; i < task_num; i++) {
        if (!esp_tcb_addr_is_sane((uint32_t)tasks[i].pxTCB, tcb_sz)) {
            ESP_COREDUMP_LOG_PROCESS("Skip TCB with bad addr %x!", tasks[i].pxTCB);
            continue;
        }
        ESP_COREDUMP_LOG_PROCESS("Dump task %x", tasks[i].pxTCB);
        // save TCB address, stack base and stack top addr
        dump_data.task_hdr.tcb_addr    = tasks[i].pxTCB;
        dump_data.task_hdr.stack_start = (uint32_t)tasks[i].pxTopOfStack;
        dump_data.task_hdr.stack_end   = (uint32_t)tasks[i].pxEndOfStack;
        err = write_cfg->write(write_cfg->priv, &dump_data, sizeof(core_dump_task_header_t));
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to write task header (%d)!", err);
            return;
        }
        // save TCB
        err = write_cfg->write(write_cfg->priv, tasks[i].pxTCB, tcb_sz);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to write TCB (%d)!", err);
            return;
        }
        // save task stack
        if (tasks[i].pxTopOfStack != 0 && tasks[i].pxEndOfStack != 0) {
            err = write_cfg->write(write_cfg->priv, tasks[i].pxTopOfStack,
                    (uint32_t)tasks[i].pxEndOfStack - (uint32_t)tasks[i].pxTopOfStack);
            if (err != ESP_OK) {
                ESP_COREDUMP_LOGE("Failed to write task stack (%d)!", err);
                return;
            }
        } else {
            ESP_COREDUMP_LOG_PROCESS("Skip corrupted task %x stack!", tasks[i].pxTCB);
        }
    }

    // write end
    if (write_cfg->end) {
        err = write_cfg->end(write_cfg->priv);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to end core dump (%d)!", err);
            return;
        }
    }
    if (write_cfg->bad_tasks_num) {
        ESP_COREDUMP_LOGE("Skipped %d tasks with bad TCB!", write_cfg->bad_tasks_num);
        if (cur_task_bad) {
            ESP_COREDUMP_LOGE("Crashed task has been skipped!");
        }
    }
}

#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH

// magic numbers to control core dump data consistency
#define COREDUMP_FLASH_MAGIC_START    0xE32C04EDUL
#define COREDUMP_FLASH_MAGIC_END      0xE32C04EDUL

typedef struct _core_dump_write_flash_data_t
{
    uint32_t    off;
} core_dump_write_flash_data_t;

typedef struct _core_dump_partition_t
{
    // core dump partition start
    uint32_t start;
    // core dump partition size
    uint32_t size;
} core_dump_partition_t;

typedef struct _core_dump_flash_config_t
{
    // core dump partition start
    core_dump_partition_t partition;
    // core dump partition size
    uint32_t              crc;
} core_dump_flash_config_t;

// core dump flash data
static core_dump_flash_config_t s_core_flash_config;

static uint32_t esp_core_dump_write_flash_padded(size_t off, uint8_t *data, uint32_t data_size)
{
    esp_err_t err;
    uint32_t data_len = 0, k, len;
    union
    {
        uint8_t    data8[4];
        uint32_t   data32;
    } rom_data;

    data_len = (data_size / sizeof(uint32_t)) * sizeof(uint32_t);

    assert(off >= s_core_flash_config.partition.start);
    assert((off + data_len + (data_size % sizeof(uint32_t) ? sizeof(uint32_t) : 0)) <=
        s_core_flash_config.partition.start + s_core_flash_config.partition.size);

    err = spi_flash_write(off, data, data_len);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write data to flash (%d)!", err);
        return 0;
    }

    len = data_size % sizeof(uint32_t);
    if (len) {
        // write last bytes with padding, actual TCB len can be retrieved by esptool from core dump header
        rom_data.data32 = 0;
        for (k = 0; k < len; k++)
            rom_data.data8[k] = *(data + data_len + k);
        err = spi_flash_write(off + data_len, &rom_data, sizeof(uint32_t));
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to finish write data to flash (%d)!", err);
            return 0;
        }
        data_len += sizeof(uint32_t);
    }

    return data_len;
}

static esp_err_t esp_core_dump_flash_write_prepare(void *priv, uint32_t *data_len)
{
    esp_err_t err;
    uint32_t sec_num;
    core_dump_write_flash_data_t *wr_data = (core_dump_write_flash_data_t *)priv;

    // check for available space in partition
    // add space for 2 magics. TODO: change to CRC
    if ((*data_len + 2*sizeof(uint32_t)) > s_core_flash_config.partition.size) {
        ESP_COREDUMP_LOGE("Not enough space to save core dump!");
        return ESP_ERR_NO_MEM;
    }
    *data_len += 2*sizeof(uint32_t);

    wr_data->off = 0;

    sec_num = *data_len / SPI_FLASH_SEC_SIZE;
    if (*data_len % SPI_FLASH_SEC_SIZE)
        sec_num++;
    assert(sec_num * SPI_FLASH_SEC_SIZE <= s_core_flash_config.partition.size);
    err = spi_flash_erase_range(s_core_flash_config.partition.start + 0, sec_num * SPI_FLASH_SEC_SIZE);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to erase flash (%d)!", err);
        return err;
    }

    return err;
}

static esp_err_t esp_core_dump_flash_write_word(core_dump_write_flash_data_t *wr_data, uint32_t word)
{
    esp_err_t err = ESP_OK;
    uint32_t  data32 = word;

    assert(wr_data->off + sizeof(uint32_t) <= s_core_flash_config.partition.size);
    err = spi_flash_write(s_core_flash_config.partition.start + wr_data->off, &data32, sizeof(uint32_t));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write to flash (%d)!", err);
        return err;
    }
    wr_data->off += sizeof(uint32_t);

    return err;
}

static esp_err_t esp_core_dump_flash_write_start(void *priv)
{
    core_dump_write_flash_data_t *wr_data = (core_dump_write_flash_data_t *)priv;
    // save magic 1
    return esp_core_dump_flash_write_word(wr_data, COREDUMP_FLASH_MAGIC_START);
}

static esp_err_t esp_core_dump_flash_write_end(void *priv)
{
    core_dump_write_flash_data_t *wr_data = (core_dump_write_flash_data_t *)priv;
#if LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG
    union
    {
        uint8_t    data8[16];
        uint32_t   data32[4];
    } rom_data;

    esp_err_t err = spi_flash_read(s_core_flash_config.partition.start + 0, &rom_data, sizeof(rom_data));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to read flash (%d)!", err);
        return err;
    }
    else {
        ESP_COREDUMP_LOG_PROCESS("Data from flash:");
        for (uint32_t i = 0; i < sizeof(rom_data)/sizeof(rom_data.data32[0]); i++) {
            ESP_COREDUMP_LOG_PROCESS("%x", rom_data.data32[i]);
        }
    }
#endif

    // save magic 2
    return esp_core_dump_flash_write_word(wr_data, COREDUMP_FLASH_MAGIC_END);
}

static esp_err_t esp_core_dump_flash_write_data(void *priv, void * data, uint32_t data_len)
{
    esp_err_t err = ESP_OK;
    core_dump_write_flash_data_t *wr_data = (core_dump_write_flash_data_t *)priv;

    uint32_t len = esp_core_dump_write_flash_padded(s_core_flash_config.partition.start + wr_data->off, data, data_len);
    if (len != data_len)
        return ESP_FAIL;

    wr_data->off += len;

    return err;
}

void esp_core_dump_to_flash(XtExcFrame *frame)
{
    core_dump_write_config_t wr_cfg;
    core_dump_write_flash_data_t wr_data;

    uint32_t crc = crc32_le(UINT32_MAX, (uint8_t const *)&s_core_flash_config.partition,
                            sizeof(s_core_flash_config.partition));
    if (s_core_flash_config.crc != crc) {
        ESP_COREDUMP_LOGE("Core dump flash config is corrupted! CRC=0x%x instead of 0x%x", crc, s_core_flash_config.crc);
        return;
    }

    /* init non-OS flash access critical section */
    spi_flash_guard_set(&g_flash_guard_no_os_ops);

    memset(&wr_cfg, 0, sizeof(wr_cfg));
    wr_cfg.prepare = esp_core_dump_flash_write_prepare;
    wr_cfg.start = esp_core_dump_flash_write_start;
    wr_cfg.end = esp_core_dump_flash_write_end;
    wr_cfg.write = esp_core_dump_flash_write_data;
    wr_cfg.priv = &wr_data;

    ESP_COREDUMP_LOGI("Save core dump to flash...");
    esp_core_dump_write(frame, &wr_cfg);
    ESP_COREDUMP_LOGI("Core dump has been saved to flash.");
}
#endif

#if CONFIG_ESP32_ENABLE_COREDUMP_TO_UART

static void esp_core_dump_b64_encode(const uint8_t *src, uint32_t src_len, uint8_t *dst) {
    const static DRAM_ATTR char b64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int i, j, a, b, c;

    for (i = j = 0; i < src_len; i += 3) {
        a = src[i];
        b = i + 1 >= src_len ? 0 : src[i + 1];
        c = i + 2 >= src_len ? 0 : src[i + 2];

        dst[j++] = b64[a >> 2];
        dst[j++] = b64[((a & 3) << 4) | (b >> 4)];
        if (i + 1 < src_len) {
            dst[j++] = b64[(b & 0x0F) << 2 | (c >> 6)];
        }
        if (i + 2 < src_len) {
            dst[j++] = b64[c & 0x3F];
        }
    }
    while (j % 4 != 0) {
        dst[j++] = '=';
    }
    dst[j++] = '\0';
}

static esp_err_t esp_core_dump_uart_write_start(void *priv)
{
    esp_err_t err = ESP_OK;
    ets_printf(DRAM_STR("================= CORE DUMP START =================\r\n"));
    return err;
}

static esp_err_t esp_core_dump_uart_write_end(void *priv)
{
    esp_err_t err = ESP_OK;
    ets_printf(DRAM_STR("================= CORE DUMP END =================\r\n"));
    return err;
}

static esp_err_t esp_core_dump_uart_write_data(void *priv, void * data, uint32_t data_len)
{
    esp_err_t err = ESP_OK;
    char buf[64 + 4], *addr = data;
    char *end = addr + data_len;

    while (addr < end) {
        size_t len = end - addr;
        if (len > 48) len = 48;
        /* Copy to stack to avoid alignment restrictions. */
        char *tmp = buf + (sizeof(buf) - len);
        memcpy(tmp, addr, len);
        esp_core_dump_b64_encode((const uint8_t *)tmp, len, (uint8_t *)buf);
        addr += len;
        ets_printf(DRAM_STR("%s\r\n"), buf);
    }

    return err;
}

static int esp_core_dump_uart_get_char() {
    int i;
    uint32_t reg = (READ_PERI_REG(UART_STATUS_REG(0)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT;
    if (reg)
        i = READ_PERI_REG(UART_FIFO_REG(0));
    else
        i = -1;
    return i;
}

void esp_core_dump_to_uart(XtExcFrame *frame)
{
    core_dump_write_config_t wr_cfg;
    uint32_t tm_end, tm_cur;
    int ch;

    memset(&wr_cfg, 0, sizeof(wr_cfg));
    wr_cfg.prepare = NULL;
    wr_cfg.start = esp_core_dump_uart_write_start;
    wr_cfg.end = esp_core_dump_uart_write_end;
    wr_cfg.write = esp_core_dump_uart_write_data;
    wr_cfg.priv = NULL;

    //Make sure txd/rxd are enabled
    // use direct reg access instead of gpio_pullup_dis which can cause exception when flash cache is disabled
    REG_CLR_BIT(GPIO_PIN_REG_1, FUN_PU);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_U0RXD);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD_U0TXD);

    ESP_COREDUMP_LOGI("Press Enter to print core dump to UART...");
    tm_end = xthal_get_ccount() / (XT_CLOCK_FREQ / 1000) + CONFIG_ESP32_CORE_DUMP_UART_DELAY;
    ch = esp_core_dump_uart_get_char();
    while (!(ch == '\n' || ch == '\r')) {
        tm_cur = xthal_get_ccount() / (XT_CLOCK_FREQ / 1000);
        if (tm_cur >= tm_end)
            break;
        ch = esp_core_dump_uart_get_char();
    }
    ESP_COREDUMP_LOGI("Print core dump to uart...");
    esp_core_dump_write(frame, &wr_cfg);
    ESP_COREDUMP_LOGI("Core dump has been written to uart.");
}
#endif

void esp_core_dump_init()
{
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH
    const esp_partition_t *core_part;

    ESP_COREDUMP_LOGI("Init core dump to flash");
    core_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, NULL);
    if (!core_part) {
        ESP_COREDUMP_LOGE("No core dump partition found!");
        return;
    }
    ESP_COREDUMP_LOGI("Found partition '%s' @ %x %d bytes", core_part->label, core_part->address, core_part->size);
    s_core_flash_config.partition.start = core_part->address;
    s_core_flash_config.partition.size  = core_part->size;
    s_core_flash_config.crc             = crc32_le(UINT32_MAX, (uint8_t const *)&s_core_flash_config.partition,
                                                    sizeof(s_core_flash_config.partition));
#endif
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_UART
    ESP_COREDUMP_LOGI("Init core dump to UART");
#endif
}

#endif

