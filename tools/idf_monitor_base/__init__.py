import re

from serial.tools import miniterm

from .console_parser import ConsoleParser
from .constants import CMD_STOP, CTRL_T
from .output_helpers import red_print

try:
    import queue  # noqa
except ImportError:
    import Queue as queue  # type: ignore  # noqa

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


def prompt_next_action(reason, console, console_parser, event_queue, cmd_queue):
    # type: (str, miniterm.Console, ConsoleParser, queue.Queue, queue.Queue) -> None
    console.setup()  # set up console to trap input characters
    try:
        red_print('--- {}'.format(reason))
        red_print(console_parser.get_next_action_text())

        k = CTRL_T  # ignore CTRL-T here, so people can muscle-memory Ctrl-T Ctrl-F, etc.
        while k == CTRL_T:
            k = console.getkey()
    finally:
        console.cleanup()
    ret = console_parser.parse_next_action_key(k)
    if ret is not None:
        cmd = ret[1]
        if cmd == CMD_STOP:
            # the stop command should be handled last
            event_queue.put(ret)
        else:
            cmd_queue.put(ret)
