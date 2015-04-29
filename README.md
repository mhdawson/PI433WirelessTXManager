# Wireless Transmit Manager for PI

This project receives messages from an mqtt broker and then
sends them wirelessly on a raspberry pi. It allows additional
devices to be easily integrated.


## To build:
TODO

## Currently supported devices
- Generic 2272 based device (work in progress)
- Lacross 141 temperature sensor (Needs to be refactored into device) 

## Adding a new device
TODO

## TODOs
- extract Lacross 141 support out into device class
- finish introduction of device registration

- add more info about build dependencies
- add more comments to the code
- add more general documenation
- add 433 wireless transmitter circuit for pi
- add more specific device info
- add more info about how to figure out protocol for a new device
- add info on topics used to receive messages for each device
- make topics more easily configurable
