// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <stdlib.h>
#include "Device2272.h"

#define PULSE_WIDTH 350
#define SHORT 1*PULSE_WIDTH
#define LONG 3*PULSE_WIDTH
#define SYNC_LENGTH_LOW 31*PULSE_WIDTH
#define SYNC_LENGTH_HIGH 1*PULSE_WIDTH
#define REPEAT 10

Device2272::Device2272(char* topic) : Device(topic) {
}


int Device2272::sendMessage(char* message) {
   for(int j= 0;j<REPEAT;j++) { 
      // sync 
      sendPulseHighLow(SYNC_LENGTH_HIGH,SYNC_LENGTH_LOW);

      // message itself 
      int length = strlen(message);
      for(int i=0;i<length;i++) {
         if (0 != sendTristate(message[i])) { 
            return -1;
         } 
      }
   }
}

int Device2272::sendTristate(char tristateValue) {
   if ('0' == tristateValue) { 
      //  _     _
      // | |___| |___
      sendPulseHighLow(SHORT,LONG);
      sendPulseHighLow(SHORT,LONG);
   } else if ('1' == tristateValue) {
      //  ___   ___
      // |   |_|   |_
      sendPulseHighLow(LONG,SHORT);
      sendPulseHighLow(LONG,SHORT);
   } else if ('F' == tristateValue) {
      //  _     ___
      // | |___|   |_
      sendPulseHighLow(SHORT,LONG);
      sendPulseHighLow(LONG,SHORT);
   } else {
      return -1;
   }
   return 0;
}
