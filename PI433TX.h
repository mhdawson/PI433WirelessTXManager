// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#ifndef _PI433TX
#define _PI433TX

#include <wiringPi.h>
#include <time.h>
#include "Device.h"

#define CA_CERT_FILE     "ca.cert"
#define CLIENT_CERT_FILE "client.cert"
#define CLIENT_KEY_FILE  "client.key"

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
      PI433TX(int interrupt, char* mqttBroker, char* certsDir);
      void listenForMessages();
      bool registerDevice(Device* newDevice);

   private:
      int _txpin;
      char* _mqttBroker;
      DeviceListEntry* _devices; 
      char* _certsDir;

      MQTTClient _myClient;
      MQTTClient_connectOptions _mqttOptions;
      MQTTClient_SSLOptions _sslOptions;

      static int messageArrived(void *context, char* topicName, int topicLen, MQTTClient_message *message);
      static void connectionLost(void *context, char *cause);
      void subscribeToTopics(void);
};

#endif
