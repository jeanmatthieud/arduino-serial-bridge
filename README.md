# About
This project is a complement of the [old fashionned warning light](https://www.iot-experiments.com/esp32-warning-light/).

At first, the ESP32 (or ESP8266) was connected to an MQTT server.
Sadly, the corporate network is quite limited (bad WiFi connection, port closed, proxy...).

To make my project work, I use a small OrangePi Zero, configured to use the corporate proxy, and connected through an Ethernet port.
The ESP8266 is reprogrammed to read the serial port and respond to some user defined AT commands.
The ESP8266 is connected to the OrangePI UART1 port.
My electronic circuit is powering the ESP8266 (Wimos D1 mini at 5V) and the OrangePi, sharing a common GND.

# Orange PI Zero configuration
The Orange PI uses `Armbian Bionic with Linux 5.4.8-sunxi`.
Note: As I wasn't sure that the power of the circuit will e stale enough for the OrangePI Zero, I limited the CPU power at 4801Mhz, thanks to `armbian-config`.

I wanted to use the UART1 of the Orange PI Zero, located on pins PG06 / PG07.
I did it the hard way (see below). Easiest solution: use the command `armbian-config` and use `System > Toggle hardware configuration`.

To enable the serial port on armbian, I read the folowing file : `/boot/dtb/overlay/README.sun8i-h3-overlays`
Then, I edited `/boot/armbianEnv.txt` to add `uart1` to the `overlays` parameter.
After a reboot, the serial port `/dev/ttyS1` will be available!

To be able to use the serial port, the user has to be added to the `dialout` group:
`usermod -a -G dialout`

# AT Commands
The device can respond to AT Commands.

## Lamp state
```
AT+LAMP=1
AT+LAMP=0
```

## Neopixel color
```
AT+COLOR=0;0;0
AT+COLOR=255;0;0
AT+COLOR=0;255;0
AT+COLOR=0;0;255
```

## Counter values
```
AT+COUNTER1=13
AT+COUNTER2=9999
```