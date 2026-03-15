# High Resolution Scroll Wheel

A [ZMK](https://github.com/zmkfirmware/zmk) configuration for a "High Resolution Scroll Wheel" using an [ams AS5600](https://ams-osram.com/products/sensor-solutions/position-sensors/ams-as5600-position-sensor) magnetic rotary position sensor.  
Inspired by the video [Wireless High Resolution Scrolling is Amazing](https://www.youtube.com/watch?v=FSy9G6bNuKA) by [Engineer Bo](https://www.youtube.com/@engineerbo), this project should be an open source alternative using [ZMK](https://github.com/zmkfirmware/zmk) keyboard firmware.

At the moment it's more of an initial proof of concept, and not fully implemented.

## Hardware

- XIAO nRF52840
- ams AS5600

## Dependencies

- [ams AS5600 driver](https://github.com/adolto/zmk-driver-ams-as5600)

## Tuning scroll resolution

Scroll resolution is controlled through HID Resolution Multiplier (usage `0x48`) with smooth scrolling enabled.

Tune these in `config/high-resolution-scroll-wheel.conf`:

- `CONFIG_ZMK_POINTING_SMOOTH_SCROLLING=y`
- `CONFIG_ZMK_INPUT_AMS_AS5600_SET_HID_RESOLUTION_MULTIPLIER=y`
- `CONFIG_ZMK_INPUT_AMS_AS5600_HID_WHEEL_RESOLUTION=<0..15>`

Higher values increase the reported wheel resolution (`15` is highest).

## TODO

- [ ] Add status LEDs for selected profile and output (USB/BLE)
- [ ] Improve behaviour to select/clear of BLE profiles
- [ ] Design PCB
- [ ] Design proper case
