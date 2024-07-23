#!/bin/bash

cd /home/pi/openFrameworks/addons/ofxPixileComms
git pull
cd /home/pi/openFrameworks/addons/ofxPixelEyes
git pull
cd /home/pi/openFrameworks/apps/2358_ModernGuru_ofMosquitto/ofMosquitto
git pull
make clean && make
