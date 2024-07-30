#!/bin/bash

# Add the Static IP template

if grep -Fxq "interface wlan0" /etc/dhcpcd.conf
then
    echo "wlan0 IP allready defined. Update IP manually or update this script."
    exit 1
else
    # Prompt user to enter the last number of the IP address
    read -p "Enter last number of static IP for wlan0: " last_number
    last_number=${last_number:-1}

    # Validate if the entered number is within valid range (1-254)
    if ((last_number < 30)) || ((last_number > 254)); then
        echo "Invalid number. Please enter a number between 30 and 254."
        exit 1
    fi

    echo "" >> /etc/dhcpcd.conf
    echo "interface wlan0" >> /etc/dhcpcd.conf
    echo "static ip_address=192.168.0.${last_number}/24" >> /etc/dhcpcd.conf
    echo "static routers=192.168.0.255" >> /etc/dhcpcd.conf
    echo "static domain_name_servers=192.168.0.250 8.8.8.8" >> /etc/dhcpcd.conf
fi

echo "Done!"
