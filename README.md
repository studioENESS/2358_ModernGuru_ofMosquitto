# ofMosquitto

> 2358 Yarrila Arts Centre

## Hardware

  - Raspberry Pi Zero 2 W
  - PAM8302A (Mono Audio Amplifier)
  - adjusting the device tree overlay to change the PWM audio pins
  - 2 x 270 Ohms resistor
  - 2 x 150 Ohms resistor
  - 2 x 10µF capacitor
  - 2 x 33nF capacitor

### Audio

As the Raspberry Pi Zero has no audio output we need to adjust the device tree overlay and reroute the PWM audio pins PWM0 (GPIO40) and PWM1 (GPIO45) to GPIO18 (ALT5) and GPIO19 (ALT5). Then add some capacitors as outlined in this [Adafruit Tutorial](https://learn.adafruit.com/adding-basic-audio-ouput-to-raspberry-pi-zero/pi-zero-pwm-audio)

The simpler way is to add the following line to `config.txt`. It will reconfigure the pins at boot without any external software or services. The PWMO will be on the GPIO #18, and PWM1 on GPIO #13.

  > dtoverlay=pwm-2chan,pin=18,func=2,pin2=13,func2=4

## Software

  - Raspian v10 (Buster*, Enable SPI)

> *OpenFrameworks is using [OpenMAX](https://www.khronos.org/openmaxil) and needs the `lopenmaxil` library.

  - OpenFrameworks v0.12.0 Armv6
  - ofxGPIO

### Pin Mapping

  - GPIO 10 (SPI, MOSI, APA DATA)
  - GPIO 11 (SPI, CLK,  APA CLK)
  - GPIO 13 (PWM1, AUDIO L)
  - GPIO 18 (PWM0, AUDIO R)

![preview](img/eness_logo.png)
