# Wireless Transmit Manager for PI

This project receives messages from an mqtt broker and then
sends them wirelessly on a raspberry pi. It allows additional
devices to be easily integrated.

## Transmitter

The transmitter I used is a BX-TM01-433:

![picture receiver 1](pictures/transmitter-1.jpg?raw=yes)
![picture receiver 2](pictures/transmitter-2.jpg?raw=yes)


It works well from the basement to the second floor in my house and is 
available on ebay for about $3 Canadian: [BX-TM01-433](http://www.ebay.ca/itm/Very-Small-ASK-RF-Wireless-Transmitter-board-module-433MHZ-10mA-75KHZ-3kbs-/280909343896?hash=item4167806c98). While this one
works well for me, any other 433 transmitter should work with this project as well.


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
