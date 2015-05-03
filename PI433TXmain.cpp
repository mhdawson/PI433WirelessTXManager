// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#include "PI433TX.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "Device2272.h"
#include "DeviceLacross141.h"

#define MISSING_BROKER  -1;
#define FAILED_TO_SETUP_WIRING_PI  -2;

int main(int argc, char *argv[]) {
   int PIN = 0;

   // validate we got the required parameters 
   if (2 > argc) {
      printf("Missing mqttBroker parameter - exiting\n");
      return MISSING_BROKER;
   }

   if(wiringPiSetup() == -1) {
      return FAILED_TO_SETUP_WIRING_PI;
   }
   
   // TODOs
   // Externalize configuration of devices
   // Make device part of message as opposed to 141 device
   // allow topics and pin to be configurable
   PI433TX transmitter = PI433TX(PIN, argv[1]);
   transmitter.registerDevice(new Device2272((char*) "home/2272"));
   transmitter.registerDevice(new DeviceLacross141((char*) "house/nexxtech/temp", 0x32));
   transmitter.listenForMessages();
}

