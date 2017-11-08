
/* @brief macro to print IDF performance
 * @param mode :        performance item name. a string pointer.
 * @param value_fmt:    print format and unit of the value, for example: "%02fms", "%dKB"
 * @param value :       the performance value.
*/
#define IDF_LOG_PERFORMANCE(item, value_fmt, value) \
    printf("[Performance][%s]: "value_fmt"\n", item, value)


/* declare the performance here */
#define IDF_PERFORMANCE_MAX_HTTPS_REQUEST_BIN_SIZE                              610
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP                     200
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP_UNICORE             130
#define IDF_PERFORMANCE_MAX_ESP_TIMER_GET_TIME_PER_CALL                         1000
