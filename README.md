# ofMosquitto

> 2358 Yarrila Arts Centre

## Hardware

  - Raspberry Pi Zero 2 W  
  - Adafruit Speaker Bonnet  
  - 3W 4-8Ohm Speaker  
  - 2x ENESS Eye PCB  
  - 5VDC 6A PSU  
**Optional**  
  - DfRobot [microwave sensors]  

### Pin Mapping

The Speaker Bonnet uses 3 data pins (I2C) on pins:
  `18`, `19`, `21`

LEDs are conneced using 2 pins (SPI):
  `10` (MOSI, DATA), `11` (CLK)

## Microwave Sensors

The [microwave sensors] pulse pin is connected to RPI GPIO `23` (Left/Front), `24` (Right/Back). We use a voltage devider to step down the triggers.

    `5V -> 2K2 | 3K3 -> GND`

**Detection Angle**: The angle of detection is 72 degrees with the antenna in a parallel direction (azimuth) The vertical (pitch) direction of the antenna is 36 degrees.

## Software

  - Raspian v10 (Buster*, Enable SPI and I2C)

> *OpenFrameworks is using [OpenMAX](https://www.khronos.org/openmaxil) and needs the `lopenmaxil` library.

  - OpenFrameworks v0.12.0 Armv6
  - ofxGPIO
  - ofxPixileComms
  - ofxPixelEyes

## Config Autostart

    mkdir /home/pi/.config/autostart
    nano /home/pi/.config/autostart/mosquitto.desktop

Write the following into the mosquitto.desktop file.

    [Desktop Entry]
    Type=Application
    Name=Mosquitto
    Exec=/usr/bin/bash /home/pi/openFrameworks/apps/2358_ModernGuru_ofMosquitto/scripts/client.sh

## Config Static IP for WiFi

> Start IP for Mosquittos is 30

    interface wlan0
    static ip_address=192.168.0.30/24
    static routers=192.168.0.255
    static domain_name_servers=192.168.0.255


![preview](img/eness_logo.png)


[microwave sensors]: http://groups.google.com/group/celluloid-ruby](https://www.dfrobot.com/product-1403.html
