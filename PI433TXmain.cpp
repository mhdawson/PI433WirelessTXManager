// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#include "PI433TX.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define DEVICE 0x32
     
int main(int argc, char *argv[]) {
   int PIN = 0;

   // validate we got the required parameters 
   if (2 > argc) {
      printf("Missing mqttBroker parameter - exiting\n");
      return -1;
   }

   if(wiringPiSetup() == -1) {
      return 0;
   }
   
   PI433TX transmitter = PI433TX(PIN, DEVICE, argv[1]);

   if ((argc > 1) && (0 == strcmp(argv[1], "register"))) {
      printf("Registering device: %x\n", DEVICE);
      transmitter.doRegister();
   } else { 
      transmitter.listenForMessages((char*)"house/nexxtech/temp");
   }
}

