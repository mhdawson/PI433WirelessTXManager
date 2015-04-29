// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#ifndef _DEVICE
#define _DEVICE
#include <wiringPi.h>
#include <time.h>
#include <string.h>

class Device {
   private:
      int _txpin; 
      char* _topic;
 
   protected:
      void sendPulseHighLow(int highDuration, int lowDuration);
      void sendPulseLowHigh(int lowDuration, int highDuration);

   public:
      Device(char* topic);
      Device() {};

      bool topicMatches(char* currentTopic) {
         return 0 == strncmp(_topic,currentTopic, strlen(_topic));
      }
   
      char* getTopic() {
         return _topic;
      }
   
      void setTxPin(int txpin) {
         _txpin = txpin;
      }

      // must be implemented by subclass
      virtual int sendMessage(char* message) {};
};

#endif
