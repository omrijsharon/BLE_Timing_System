# BLE_Timing_System
A timing system for races based on ESP32 and uses Bluetooth Low Energy (BLE).

## How to use?
* Connect the ground station to 5V power supply and position it in the finish gate.
* Connect the beacon module to 5V (from the flight controller or using [this BEC](https://s.click.aliexpress.com/e/_DddLQzr) from the LiPo balance)
* Start racing!

## Features:
* *Easy to use:* Plug and Play. No soldering is needed. Unplug when not needed.
* *Pass-By Detection:* The ground station scans for beacons and counts a lap when the beacon's RSSI is maximal.
* *Real Time Race Info on your smartphone:* The ground station is also an access point with a webpage. In the webpage you have access to all the relevant race information of each competitor.
* *Easy editing:* in the webpage you can change each beacon name, and also reset and delete competitors.
* *LED lights:* The ground station outputs PWM signal proportional to the beacon's RSSI that is being detected at the moment.
* *Each beacon is unique:* the beacons identifier is its MAC Address. This mean that even though you flash the same hex file to all your beacons- they will all be distinguished.

## Hardware
Seeed Studio XIAO ESP32C3: 5-8 USD.


