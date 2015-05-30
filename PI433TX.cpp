// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <stdlib.h>
#include "PI433TX.h"

#define MQTT_CONNECT_RETRY_SECONDS 5
#define QOS_AT_MOST_ONCE_0 0
#define TEMP_BUFFER_SIZE 1000

#define SEPARATOR "/"

PI433TX::PI433TX(int pin, char* mqttBroker, char* certsDir) : _mqttOptions(MQTTClient_connectOptions_initializer),
                                                              _sslOptions(MQTTClient_SSLOptions_initializer) { 
   this->_txpin = pin;
   this->_mqttBroker = mqttBroker;
   pinMode(this->_txpin, OUTPUT);
   this->_myClient = NULL;
   this->_devices = NULL;

   _certsDir = certsDir;
   if (strstr(_mqttBroker, "ssl://") == _mqttBroker) {
      // ssl is enabled so setup the options
      _mqttOptions.ssl = &_sslOptions;
      _sslOptions.trustStore = (char*) malloc(strlen(_certsDir) + strlen(SEPARATOR) + strlen(CA_CERT_FILE) + 1);
      _sslOptions.keyStore   = (char*) malloc(strlen(_certsDir) + strlen(SEPARATOR) + strlen(CLIENT_CERT_FILE) + 1);
      _sslOptions.privateKey   = (char*) malloc(strlen(_certsDir) + strlen(SEPARATOR) + strlen(CLIENT_KEY_FILE) + 1);
      strcpy((char*) _sslOptions.trustStore, _certsDir);
      strcpy((char*) _sslOptions.keyStore, _certsDir);
      strcpy((char*) _sslOptions.privateKey, _certsDir);
      strcat((char*) _sslOptions.trustStore, SEPARATOR);
      strcat((char*) _sslOptions.keyStore, SEPARATOR);
      strcat((char*) _sslOptions.privateKey, SEPARATOR);
      strcat((char*) _sslOptions.trustStore, CA_CERT_FILE);
      strcat((char*) _sslOptions.keyStore, CLIENT_CERT_FILE);
      strcat((char*) _sslOptions.privateKey, CLIENT_KEY_FILE);

      _sslOptions.enabledCipherSuites = "TLSv1.2";
   } else {
      _mqttOptions.ssl = NULL;
   }
} 

bool PI433TX::registerDevice(Device* newDevice) {
  
   DeviceListEntry* deviceEntry = (DeviceListEntry*) malloc(sizeof(DeviceListEntry));
   if (NULL != deviceEntry) {
      newDevice->setTxPin(this->_txpin);
      deviceEntry->device = newDevice;
      deviceEntry->next = this->_devices;
      this->_devices = deviceEntry;
      return true;
   }
   return false;
}


int PI433TX::messageArrived(void *context, char* topicName, int topicLen, MQTTClient_message *message) {
   PI433TX* myThis = (PI433TX*)context;
   char buffer[TEMP_BUFFER_SIZE];
   int limit = TEMP_BUFFER_SIZE -1;
   if (message->payloadlen < limit) { 
      limit = message->payloadlen;
   }

   memset(buffer, 0, TEMP_BUFFER_SIZE);
   strncpy(buffer, (char*) message->payload, limit);

   // Find which device the message is for and allow it to process message
   DeviceListEntry* nextDevice = myThis->_devices;
   while(NULL != nextDevice) {
      if (nextDevice->device->topicMatches(topicName)) {
         nextDevice->device->sendMessage(buffer);
         break;
      }
      nextDevice = nextDevice->next;
   }

   MQTTClient_freeMessage(&message);
   return 1;
}

void PI433TX::connectionLost(void *context, char *cause) {
   PI433TX* myThis = (PI433TX*)context;

   while (true) {
      if (MQTTCLIENT_SUCCESS == MQTTClient_connect(myThis->_myClient, &(myThis->_mqttOptions))) {
         myThis->subscribeToTopics(); 
         break;
      }
      // wait 5 seconds and then try to reconnect
      usleep(MQTT_CONNECT_RETRY_SECONDS*1000*1000);
   }
}

void PI433TX::listenForMessages() {
   MQTTClient_create(&_myClient, _mqttBroker, "PI433TXManager", MQTTCLIENT_PERSISTENCE_NONE, (void*) this);
   MQTTClient_setCallbacks(_myClient, (void*) this, &(PI433TX::connectionLost), &(PI433TX::messageArrived), NULL);

   _mqttOptions.keepAliveInterval = 10;
   _mqttOptions.cleansession = 1;
   _mqttOptions.reliable = 0;

   int connectResult = MQTTClient_connect(_myClient, &_mqttOptions);
   if(MQTTCLIENT_SUCCESS != connectResult) {
      printf("Failed to connect to MQTT broker, exiting\n");
      return;
   }
   
   subscribeToTopics(); 

   while(1) {
      usleep(1000 * 1000);
      MQTTClient_yield();
   }
}

void PI433TX::subscribeToTopics(void) {
   DeviceListEntry* nextDevice = this->_devices;
   while(NULL != nextDevice) {
      MQTTClient_subscribe(_myClient, nextDevice->device->getTopic(), QOS_AT_MOST_ONCE_0);
      nextDevice = nextDevice->next;
   }
}

