/* Produces a minimal ELF file for espcoredump tests */

typedef struct {
    char stuff[TASK_NAME_OFFSET];
    char pcTaskName[16];
} TCB_t;

TCB_t foo;

int main(void)
{
    return 0;
}
