#!/bin/bash

if test -d /home/pi/openFrameworks/addons/ofxPixileComms; then
  cd /home/pi/openFrameworks/addons/ofxPixileComms
  git pull
else
  cd /home/pi/openFrameworks/addons/
  git clone https://github.com/studioENESS/ofxPixileComms
fi

if test -d /home/pi/openFrameworks/addons/ofxPixelEyes; then
  cd /home/pi/openFrameworks/addons/ofxPixelEyes
  git pull
else
  cd /home/pi/openFrameworks/addons/
  git clone clone https://github.com/studioENESS/ofxPixelEyes
fi

if test -d /home/pi/openFrameworks/addons/ofxImGui; then
  cd /home/pi/openFrameworks/addons/ofxImGui
  git pull
else
  cd /home/pi/openFrameworks/addons/
  git clone clone -b develop https://github.com/ofxyz/ofxImGui
fi

if test -d /home/pi/openFrameworks/apps/2358_ModernGuru_ofMosquitto; then
  cd /home/pi/openFrameworks/apps/2358_ModernGuru_ofMosquitto/ofMosquitto
  git pull
  make clean && make
fi
