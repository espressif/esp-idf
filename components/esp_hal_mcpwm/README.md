# ESP Hardware Abstraction Layer for MCPWM Peripheral

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The ESP Hardware Abstraction Layer for Motor Control PWM Peripherals (`esp_hal_mcpwm`) provides a unified interface to interact with MCPWM (Motor Control Pulse Width Modulation) peripherals across all ESP chip families. This component abstracts the hardware-specific details of different MCPWM implementations, enabling consistent usage patterns regardless of the underlying hardware.

The component supports comprehensive MCPWM functionality including:
- PWM signal generation with precise timing control
- Dead-time insertion for motor control applications
- Fault detection and protection mechanisms
- Capture functionality for feedback systems
- Synchronization between multiple MCPWM units
- Event-driven operation modes