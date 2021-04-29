import re

# regex matches an potential PC value (0x4xxxxxxx)
MATCH_PCADDR = re.compile(r'0x4[0-9a-f]{7}', re.IGNORECASE)

DEFAULT_TOOLCHAIN_PREFIX = 'xtensa-esp32-elf-'

DEFAULT_PRINT_FILTER = ''

# coredump related messages
COREDUMP_UART_START = b'================= CORE DUMP START ================='
COREDUMP_UART_END = b'================= CORE DUMP END ================='
COREDUMP_UART_PROMPT = b'Press Enter to print core dump to UART...'

# coredump states
COREDUMP_IDLE = 0
COREDUMP_READING = 1
COREDUMP_DONE = 2

# coredump decoding options
COREDUMP_DECODE_DISABLE = 'disable'
COREDUMP_DECODE_INFO = 'info'

# panic handler related messages
PANIC_START = r'Core \s*\d+ register dump:'
PANIC_END = b'ELF file SHA256:'
PANIC_STACK_DUMP = b'Stack memory:'

# panic handler decoding states
PANIC_IDLE = 0
PANIC_READING = 1

# panic handler decoding options
PANIC_DECODE_DISABLE = 'disable'
PANIC_DECODE_BACKTRACE = 'backtrace'
