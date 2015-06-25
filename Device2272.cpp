// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <stdlib.h>
#include "Device2272.h"

#define SHORT 1*_pulseWidth
#define LONG 3*_pulseWidth
#define SYNC_LENGTH_LOW 1*_pulseWidth
#define SYNC_LENGTH_HIGH 31*_pulseWidth

#define NUM_CHARS_IN_MESSAGE 12

#define INVALID_NUM_CHARS_IN_MESSAGE  -1
#define INVALID_CHAR                  -2 

// The 2262/2272 protocol indicates a CODE frame is 4 CODE words so we need to 
// repeat at least 4 times 
#define REPEAT 10

Device2272::Device2272(char* topic, int pulseWidth) : Device(topic) {
   _pulseWidth = pulseWidth;
}

int Device2272::sendMessage(char* message) {
   for(int j= 0;j<REPEAT;j++) { 
      // TODO add validation of the length of the message
      int length = strlen(message);
      if (length != NUM_CHARS_IN_MESSAGE) {
         return INVALID_NUM_CHARS_IN_MESSAGE;
      }

      // protocol indicates that CODEWORD has 12 bits followed by sync
      // first 8 are the address of the device, the next 4 the data value, 
      // and the last the sync
      for(int i=0;i<length;i++) {
         if (0 != sendTristate(message[i])) { 
            return INVALID_CHAR;
         } 
      }
      // sync
      sendPulseLowHigh(SYNC_LENGTH_LOW,SYNC_LENGTH_HIGH);
   }
}

int Device2272::sendTristate(char tristateValue) {
   if ('0' == tristateValue) { 
      //  _     _
      // | |___| |___
      sendPulseLowHigh(SHORT,LONG);
      sendPulseLowHigh(SHORT,LONG);
   } else if ('1' == tristateValue) {
      //  ___   ___
      // |   |_|   |_
      sendPulseLowHigh(LONG,SHORT);
      sendPulseLowHigh(LONG,SHORT);
   } else if ('F' == tristateValue) {
      //  _     ___
      // | |___|   |_
      sendPulseLowHigh(SHORT,LONG);
      sendPulseLowHigh(LONG,SHORT);
   } else {
      return INVALID_CHAR;
   }
   return 0;
}
