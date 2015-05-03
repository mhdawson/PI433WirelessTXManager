// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <stdlib.h>
#include "DeviceLacross141.h"

#define MESSAGE_REPEAT_COUNT 15
#define REGISTER_TRANSMITS 4

DeviceLacross141::DeviceLacross141(char* topic, char deviceCode) : Device(topic) {
   _deviceCode = deviceCode;
}

void  DeviceLacross141::sendSync(void) {
   for(int i=0;i<5;i++) {
      sendPulseLowHigh(1000,1000);
   }
}

void DeviceLacross141::sendByte(char byte) {
   char mask = 0x80;
   for(int i=0;i<8;i++) {
      if ((byte & mask) != 0) {
         sendPulseLowHigh(490,160);
      } else {
         sendPulseLowHigh(160,490);
      }
      mask = (mask >> 1) & 0x7F;
   }
}

int DeviceLacross141::sendMessage(char* buffer) {
   char* tempStart = strstr(buffer , ":");
   if (NULL != tempStart) {
      float temp = 0;
      if ( 1 == sscanf(tempStart + 1, "%f", &temp)) {
         this->sendTemp(temp);
      } else {
         return -1;
      }
   } else {
      doRegister();
   }
   return 0;
}

void DeviceLacross141::sendTemp(float temp) {
   char bytes[4];
   bytes[3] = _deviceCode;

   int value = ((temp*10)+500);
   bytes[1] = value & 0xFF;
   bytes[2] = ((value >> 8)& 0x0F) + 0x90;

   addChecksum(bytes);

   sendTempMessage(bytes);
}

void DeviceLacross141::sendTempMessage(char* bytes) {
   for (int i=0;i<MESSAGE_REPEAT_COUNT;i++){ 
      sendSync();
      for(int i=3;i>=0;i--) { 
         sendByte(bytes[i]);
      }
   }
}

// to calculate the checksum first reverse the nibbles in each byte
// then add the 3 message bytes together, add 0x66 and the swap
// the nibbles to get the expected checksum which is stored in byte 0
void DeviceLacross141::addChecksum(char* bytes) {
   char calcChecksum = 0x66;

   for (int i = 1;i<4;i++) {
      calcChecksum += ((bytes[i] >> 4) & 0x0F) + ((bytes[i] & 0x0F) << 4);
   }

   // swap the nibbles around and store in the checksum byt
   bytes[0] = ((calcChecksum >> 4) & 0x0F) + ((calcChecksum & 0x0F) << 4);
}

void DeviceLacross141::doRegister(void) {
   for(int i=0;i < REGISTER_TRANSMITS; i++) {
      sendTemp(10);
   }
}

