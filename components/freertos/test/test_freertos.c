/*
 Test for multicore FreeRTOS. This test spins up threads, fiddles with queues etc.
*/

#include <esp_types.h>
#include <stdio.h>
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/uart_reg.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"


void ets_isr_unmask(uint32_t unmask);

static xQueueHandle myQueue;
static xQueueHandle uartRxQueue;

int ctr;

#if 1
//Idle-loop for delay. Tests involuntary yielding
static void cvTaskDelay(int dummy)
{
    volatile int i;
    for (i = 0; i < (1 << 17); i++);
}
#else
//Delay task execution using FreeRTOS methods. Tests voluntary yielding.
#define cvTaskDelay(x) vTaskDelay(x)
#endif

#if 0
static void dosegfault3(int i)
{
    volatile char *p = (volatile char *)0;
    *p = i;
}

static void dosegfault2(int i)
{
    if (i > 3) {
        dosegfault3(i);
    }
}

static void dosegfault(int i)
{
    if (i < 5) {
        dosegfault(i + 1);
    }
    dosegfault2(i);
}
#endif

static void queueSender(void *pvParameters)
{
    int myCtr = xPortGetCoreID() * 100000;
    while (1) {
        printf("Core %d: Send to queue: %d\n", xPortGetCoreID(), myCtr);
        xQueueSend(myQueue, (void *)(&myCtr), portMAX_DELAY);
        printf("Send to queue done.\n");
        cvTaskDelay(100);
        myCtr++;
    }
}

static void queueReceiver(void *pvParameters)
{
    int theCtr;
    while (1) {
        xQueueReceive(myQueue, &theCtr, portMAX_DELAY);
        printf("Core %d: Receive from queue: %d\n", xPortGetCoreID(), theCtr);
    }
}


static void tskone(void *pvParameters)
{
//  char *p=(char *)0;
    while (1) {
        ctr++;
//      if (ctr>60) dosegfault(3);
        printf("Task1, core %d, ctr=%d\n", xPortGetCoreID(), ctr);
        cvTaskDelay(500);
    }
}

static void tsktwo(void *pvParameters)
{
    while (1) {
        ctr++;
        printf("Task2, core %d, ctr=%d\n", xPortGetCoreID(), ctr);
        cvTaskDelay(500);
    }
}

static void tskthree(void *pvParameters)
{
    while (1) {
        ctr++;
        printf("Task3, core %d, ctr=%d\n", xPortGetCoreID(), ctr);
        cvTaskDelay(500);
    }
}

static void tskfour(void *pvParameters)
{
    while (1) {
        ctr++;
        printf("Task4, core %d, ctr=%d\n", xPortGetCoreID(), ctr);
        cvTaskDelay(500);
    }
}

static void tskfive(void *pvParameters)
{
    while (1) {
        ctr++;
        printf("Task5, core %d, ctr=%d\n", xPortGetCoreID(), ctr);
        cvTaskDelay(500);
    }
}

static void tskyield(void *pvParameters)
{
    while (1) {
        portYIELD();
    }
}

static void  tskUartRecv(void *pvParameters)
{
    char c;
    while (1) {
        xQueueReceive(uartRxQueue, &c, portMAX_DELAY);
        printf("Uart received %c!\n", c);
    }
}


static void uartIsrHdl(void *arg)
{
    char c;
    BaseType_t xHigherPriorityTaskWoken;
    SET_PERI_REG_MASK(UART_INT_CLR_REG(0), UART_RXFIFO_FULL_INT_CLR);
    while (READ_PERI_REG(UART_STATUS_REG(0)) & (UART_RXFIFO_CNT << UART_RXFIFO_CNT_S)) {
        c = READ_PERI_REG(UART_FIFO_REG(0));
        xQueueSendFromISR(uartRxQueue, &c, &xHigherPriorityTaskWoken);
        printf("ISR: %c\n", c);
    }
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

static void uartRxInit(xQueueHandle q)
{
    uint32_t reg_val;

    PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_U0RXD);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD_U0TXD);


    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_U0RXD);

//  reg_val = READ_PERI_REG(UART_CONF1(0));
    reg_val = (1 << UART_RXFIFO_FULL_THRHD_S);
    WRITE_PERI_REG(UART_CONF1_REG(0), reg_val);
    CLEAR_PERI_REG_MASK(UART_INT_ENA_REG(0), UART_TXFIFO_EMPTY_INT_ENA | UART_RXFIFO_TOUT_INT_ENA);
    SET_PERI_REG_MASK(UART_INT_ENA_REG(0), UART_RXFIFO_FULL_INT_ENA);

    printf("Enabling int %d\n", ETS_UART0_INUM);
    DPORT_REG_SET_FIELD(DPORT_PRO_UART_INTR_MAP_REG, DPORT_PRO_UART_INTR_MAP, ETS_UART0_INUM);
    DPORT_REG_SET_FIELD(DPORT_PRO_UART1_INTR_MAP_REG, DPORT_PRO_UART1_INTR_MAP, ETS_UART0_INUM);

    xt_set_interrupt_handler(ETS_UART0_INUM, uartIsrHdl, NULL);
    xt_ints_on(1 << ETS_UART0_INUM);

}

// TODO: split this thing into separate orthogonal tests
TEST_CASE("Bunch of FreeRTOS tests", "[freertos][ignore]")
{
    char *tst;
    TaskHandle_t th[12];
    int i;
    printf("%s\n", __FUNCTION__);
    tst = pvPortMalloc(16);
    printf("Test malloc returns addr %p\n", tst);
    printf("Free heap: %u\n", xPortGetFreeHeapSize());
    myQueue = xQueueCreate(10, sizeof(int));
    uartRxQueue = xQueueCreate(256, sizeof(char));
    printf("Free heap: %u\n", xPortGetFreeHeapSize());

    printf("Creating tasks\n");
    xTaskCreatePinnedToCore(tskyield  , "tskyield1"  , 2048, NULL, 3, &th[0], 0);
    xTaskCreatePinnedToCore(tskyield  , "tskyield2"  , 2048, NULL, 3, &th[1], 1);

    xTaskCreatePinnedToCore(tskone  , "tskone"  , 2048, NULL, 3, &th[2], 0);
    xTaskCreatePinnedToCore(tsktwo  , "tsktwo"  , 2048, NULL, 3, &th[3], 1);
    xTaskCreatePinnedToCore(tskthree, "tskthree", 2048, NULL, 3, &th[4], 0);
    xTaskCreatePinnedToCore(tskfour , "tskfour" , 2048, NULL, 3, &th[5], tskNO_AFFINITY);
    xTaskCreatePinnedToCore(tskfive , "tskfive" , 2048, NULL, 3, &th[6], tskNO_AFFINITY);
    xTaskCreatePinnedToCore(queueSender , "qsend1" , 2048, NULL, 3, &th[7], 0);
    xTaskCreatePinnedToCore(queueSender , "qsend2" , 2048, NULL, 3, &th[8], 1);
    xTaskCreatePinnedToCore(queueReceiver , "qrecv1" , 2048, NULL, 3, &th[9], 1);
    xTaskCreatePinnedToCore(queueReceiver , "qrecv2" , 2048, NULL, 3, &th[10], 0);
    xTaskCreatePinnedToCore(tskUartRecv , "tskuart" , 2048, NULL, 4, &th[11], 1);
    printf("Free heap: %u\n", xPortGetFreeHeapSize());
    uartRxInit(uartRxQueue);

    // Let stuff run for 20s
    vTaskDelay(20000 / portTICK_PERIOD_MS);

    //Shut down all the tasks
    for (i = 0; i < 12; i++) {
        vTaskDelete(th[i]);
    }
    xt_ints_off(1 << ETS_UART0_INUM);
}

