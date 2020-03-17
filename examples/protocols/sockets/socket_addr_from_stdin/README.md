# Socket address input

This directory contains a common component for socket client examples which read IP address from stdin (if configured).
This option is typically used in the CI, but could be enabled in the project configuration.
In that case this component is used to receive a string that is evaluated and processed to output
socket structures to open a connection.
