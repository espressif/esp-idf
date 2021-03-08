# eventfd example

The example demonstrates the use of `eventfd()` to collect events from other tasks and ISRs in a
`select()` based main loop.  The example starts two tasks and installs a timer interrupt handler:
1. The first task writes to the first `eventfd` periodically.
2. The timer interrupt handler writes to the second `eventfd`.
3. The second task collects the event from two fds with a `select()` loop.

See the README.md file in the upper level 'examples' directory for more information about examples.
