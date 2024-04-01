# Espressif Arduino-FOC Fork

This is a fork of upstream [Arduino-FOC](https://github.com/simplefoc/Arduino-FOC) with integration into ESP-IDF build system.

This fork repository is wrapped as an ESP-IDF component, which offers the algorithm implementation of FOC (Field Oriented Control) for BLDC and stepper motors.

To drive the motors, you need to use this component together with [esp_simplefoc](https://github.com/espressif/esp-iot-solution/tree/master/components/motor/esp_simplefoc), which offers the hardware drivers for motors.

## Supported SoCs

The driver supports all ESP chips with `LEDC` or `MCPWM` peripherals, please refer to [esp_simplefoc](https://github.com/espressif/esp-iot-solution/tree/master/components/motor/esp_simplefoc) for details.

## How to use

### Use this component together with [esp_simplefoc](https://github.com/espressif/esp-iot-solution/tree/master/components/motor/esp_simplefoc).

Just add ``idf_component.yml`` to your main component with the following content::

```yaml
## IDF Component Manager Manifest File
dependencies:
  esp_simplefoc: "*"
```

Or simply run:

```
idf.py add-dependency "esp_simplefoc"
```

## API Documentation

1. Library introduction can be found on [README](https://github.com/simplefoc/Arduino-FOC/blob/master/README.md) from the upstream Arduino-FOC.
2. Full API code documentation as well as example projects and step by step guides can be found on [SimpleFOC Docs Website](https://docs.simplefoc.com/).
