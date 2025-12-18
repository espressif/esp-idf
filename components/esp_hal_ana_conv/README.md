# ESP Hardware Abstraction Layer for Analog Conversion Peripherals

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_ana_conv` component provides a **Hardware Abstraction Layer** for analog conversion related peripherals across all ESP-IDF supported targets. This HAL currently supports Analog-to-Digital Converter (ADC), Digital-to-Analog Converter (DAC), Temperature Sensor and other analog conversion related peripherals. The HAL enables analog signal acquisition, generation, monitoring, and temperature measurement capabilities.

## Architecture

The analog conversion HAL is structured in two main sub-layers:

1. **HAL Layer (Upper)**: Defines the operational steps and data structures required to control analog conversion peripherals (e.g., initialization, channel configuration, conversion control, DMA setup, sensor reading).

2. **Low-Level Layer (Bottom)**: Serves as a translation layer between the HAL and the register files defined in the `soc` component, handling target-specific register configurations.

## Supported Controllers

### ADC Controllers

This HAL supports various ADC controller types depending on the ESP chip:

- **RTC Controller**: Single conversion mode for low-power applications, supports deep sleep operation
- **ULP Controller**: Ultra-low-power controller for ULP coprocessor control
- **Digital Controller (DIG)**: Continuous conversion mode with DMA support, high-performance multi-channel scanning
- **Arbiter Controller (ARB)**: Manages multiple controller access to ADC2 (ESP32-S2, ESP32-S3)
- **PWDET Controller**: Power detection controller for Wi-Fi power monitoring (ADC2 only)

### DAC Controllers

This HAL supports various DAC controller types depending on the ESP chip:

- **RTC Controller**: Direct voltage output control
- **DMA Controller**: High-speed data output via I2S or SPI DMA

### Temperature Sensor Controllers

This HAL supports temperature sensor functionality depending on the ESP chip:

- **Temperature Sensor**: On-chip temperature monitoring with automatic range management

## ADC Features

### Work Modes

- **Oneshot Mode**: Single conversion per trigger, suitable for polling-based applications
- **Continuous Mode**: Continuous conversion with DMA, supports high-speed multi-channel scanning
- **Low-Power Mode**: Ultra-low-power operation for battery-powered applications
- **PWDET Mode**: Power detection mode for Wi-Fi power monitoring

### Conversion Capabilities

- **Multi-Unit Support**: Support for ADC1 and ADC2 (where available)
- **Conversion Modes**:
  - Single unit mode (ADC1 or ADC2 only)
  - Both units mode (ADC1 and ADC2 simultaneously)
  - Alternate unit mode (ADC1 and ADC2 alternately)
- **Resolution Options**: 9-bit, 10-bit, 11-bit, 12-bit, 13-bit (chip-dependent)
- **Attenuation Options**: 0dB, 2.5dB, 6dB, 12dB for different input voltage ranges
- **Pattern Table**: Configurable scan sequence with up to 16 patterns per unit

### Signal Processing

- **IIR Filtering**: Digital IIR filters for noise reduction at high sampling rates
- **Monitor Function**: Hardware threshold monitoring with interrupt support
- **Data Inversion**: Optional data inversion for compatibility
- **Calibration**: Hardware and software calibration support for improved accuracy

### Clock and Timing

- **Flexible Clock Sources**: APB clock, APLL, XTAL, RC_FAST (chip-dependent)
- **Configurable Sampling Rate**: Adjustable via clock division and trigger interval
- **FSM Timing Control**: Configurable wait times for reset, start, and standby phases

## DAC Features

### Output Capabilities

- **Dual Channel Support**: Two independent DAC channels (DAC_CHAN_0, DAC_CHAN_1)
- **8-bit Resolution**: 256-level voltage output (0V to VDD3P3_RTC)
- **Direct Voltage Output**: Direct control of output voltage value

### Cosine Wave Generator

- **Frequency Control**: Configurable frequency from 130Hz to several MHz
- **Amplitude Control**: Adjustable attenuation (0dB, 6dB, 12dB, 18dB)
- **Phase Control**: 0° or 180° phase shift support
- **DC Offset**: Configurable DC component for signal offset

### Advanced Features

- **DMA Output**: High-speed data streaming via I2S, SPI or GDMA
- **ADC-DAC Synchronization**: Synchronized operation with ADC RTC controller
- **Power Management**: Independent power control per channel

## Temperature Sensor Features

### Measurement Capabilities

- **Temperature Range**: Typically -40°C to 125°C (chip-dependent)
- **Automatic Range Management**: Five configurable temperature ranges with optimized accuracy for each range
- **Raw Value Reading**: Direct access to raw temperature sensor values
- **Degree Conversion**: Automatic conversion from raw values to temperature in degrees Celsius

### Range Management

- **Multi-Range Support**: Five temperature ranges with different offset and error characteristics
- **Dynamic Range Switching**: Automatic range adjustment based on measured temperature for optimal accuracy
- **Range Tracking**: HAL layer maintains range state to minimize unnecessary range switches

### Advanced Features

- **Sleep Retention**: Register context retention during sleep modes (chip-dependent)
- **Clock Source Selection**: Flexible clock source configuration (XTAL, FOSC, etc., chip-dependent)
- **ETM Support**: Event Task Matrix integration for temperature monitoring events and tasks
- **Wake-up Functionality**: Temperature-based wake-up from sleep modes
- **Power Control**: Independent power and clock gating control

## Usage

The HAL functions primarily serve ESP-IDF peripheral drivers such as `esp_adc`, `esp_driver_dac`, and `esp_driver_tsens`.

Advanced developers can use these interfaces directly when implementing custom drivers, with the understanding that API stability is not guaranteed.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros

