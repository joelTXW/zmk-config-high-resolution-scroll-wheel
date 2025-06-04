# High Resolution Scroll Wheel

A [ZMK](https://github.com/zmkfirmware/zmk) configuration for a "High Resolution Scroll Wheel" using an [ams AS5600](https://ams-osram.com/products/sensor-solutions/position-sensors/ams-as5600-position-sensor) magnetic rotary position sensor.  
Inspired by the video [Wireless High Resolution Scrolling is Amazing](https://www.youtube.com/watch?v=FSy9G6bNuKA) by [Engineer Bo](https://www.youtube.com/@engineerbo), this project should be an open source alternative using [ZMK](https://github.com/zmkfirmware/zmk) keyboard firmware.

At the moment it's more of an initial proof of concept, and not fully implemented.

## Hardware

- XIAO nRF52840
- ams AS5600

## Dependencies

- [ams AS5600 driver](https://github.com/adolto/zmk-driver-ams-as5600)

## TODO

- [ ] Add status LEDs for selected profile and output (USB/BLE)
- [ ] Improve behaviour to select/clear of BLE profiles
- [ ] Design PCB
- [ ] Design proper case
