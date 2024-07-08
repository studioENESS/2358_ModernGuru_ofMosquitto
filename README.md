# ofMosquitto

> 2358 Yarrila Arts Centre

## Hardware

  - Raspberry Pi Zero 2 W
  - PAM8302A (Mono Audio Amplifier)

### Audio

As the Raspberry Pi Zero has no audio output we need to adjust the device tree overlay and reroute the PWM audio pins PWM0 (GPIO40) and PWM1 (GPIO45) to GPIO18 (ALT5) and GPIO19 (ALT5). Then add some capacitors as outlined in this [Adafruit Tutorial](https://learn.adafruit.com/adding-basic-audio-ouput-to-raspberry-pi-zero/pi-zero-pwm-audio)

## Software

  - Raspian v10 (Buster)
  - OpenFrameworks v0.12.0 Armv6
  - ofxGPIO

### Pin Mapping

  - GPIO 10 (SPI, MOSI, APA DATA)
  - GPIO 11 (SPI, CLK,  APA CLK)

![preview](img/eness_logo.png)
