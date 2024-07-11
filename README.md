# ofMosquitto

> 2358 Yarrila Arts Centre

## Hardware

  - Raspberry Pi Zero 2 W
  - Adafruit Speaker Bonnet
  - 3W 4-8Ohm Speaker
  - 2x ENESS Eye PCB
  - 5VDC 6A PSU

### Pin Mapping

The Speaker Bonnet uses 3 data pins (I2C) on pins:
  `18`, `19`, `21`

LEDs are conneced using 2 pins (SPI):
  `10` (MOSI, DATA), `11` (CLK)

## Software

  - Raspian v10 (Buster*, Enable SPI and I2C)

> *OpenFrameworks is using [OpenMAX](https://www.khronos.org/openmaxil) and needs the `lopenmaxil` library.

  - OpenFrameworks v0.12.0 Armv6
  - ofxGPIO
  - ofxPixileComms
  - ofxPixelEyes


![preview](img/eness_logo.png)
