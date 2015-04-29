// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#ifndef _PI433TX
#define _PI433TX

#include <wiringPi.h>
#include <time.h>
#include "Device.h"

typedef struct DeviceListEntry {
   Device* device;
   DeviceListEntry* next;
} DeviceListEntry;

extern "C" {
    #include "MQTTClient.h"
    #include "MQTTClientPersistence.h"
}

class PI433TX {
   public:
      PI433TX(int interrupt, char deviceCode, char* mqttBroker);
      void listenForMessages(char* topic);
      void doRegister(void);

      void sendTemp(float temp);

   private:
      int _txpin;
      char* _mqttBroker;
      DeviceListEntry* _devices; 

      char _deviceCode;
      char* _topic;
      MQTTClient _myClient;
      MQTTClient_connectOptions _mqttOptions;

      void sendMessage(char* bytes);
      void sendSync(void); 
      void sendByte(char byte);
      void sendPulse(int lowDuration, int highDuration);

      void addChecksum(char* bytes);

      static int messageArrived(void *context, char* topicName, int topicLen, MQTTClient_message *message);
      static void connectionLost(void *context, char *cause);
};

#endif
