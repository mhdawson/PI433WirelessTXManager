// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <stdlib.h>
#include "PI433TX.h"
#include "Device2272.h"

#define MESSAGE_REPEAT_COUNT 15
#define MQTT_CONNECT_RETRY_SECONDS 5
#define QOS_AT_MOST_ONCE_0 0
#define TEMP_BUFFER_SIZE 1000
#define REGISTER_TRANSMITS 4

PI433TX::PI433TX(int pin, char deviceCode, char* mqttBroker) : _mqttOptions(MQTTClient_connectOptions_initializer) {
   this->_txpin = pin;
   this->_mqttBroker = mqttBroker;
   pinMode(this->_txpin, OUTPUT);
   this->_deviceCode = deviceCode;
   this->_myClient = NULL;
   this->_topic = NULL;

   this->_devices = (DeviceListEntry*) malloc(sizeof(DeviceListEntry));
   this->_devices->device = new Device2272((char*) "home/2272");
   this->_devices->device->setTxPin(this->_txpin);
   this->_devices->next = NULL;
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

   // Find which device the message is for
   DeviceListEntry* nextDevice = myThis->_devices;
   while(NULL != nextDevice) {
      if (nextDevice->device->topicMatches(topicName)) {
         printf("Sending %s\n",buffer);
         nextDevice->device->sendMessage(buffer);
         return 1;
      }
      nextDevice = nextDevice->next;
   }

   char* tempStart = strstr(buffer, ":");
   if (NULL != tempStart) {
      float temp = 0;
      if ( 1 == sscanf(tempStart + 1, "%f", &temp)) { 
         myThis->sendTemp(temp);   
      } 
   }

   MQTTClient_freeMessage(&message);

   return 1;
}

void PI433TX::doRegister(void) {
      for(int i=0;i < REGISTER_TRANSMITS; i++) {
         sendTemp(10);
      }
}

void PI433TX::connectionLost(void *context, char *cause) {
   PI433TX* myThis = (PI433TX*)context;

   while (true) {
      if (MQTTCLIENT_SUCCESS == MQTTClient_connect(myThis->_myClient, &(myThis->_mqttOptions))) {
         MQTTClient_subscribe(myThis->_myClient, myThis->_topic, QOS_AT_MOST_ONCE_0);
         break;
      }
      // wait 5 seconds and then try to reconnect
      usleep(MQTT_CONNECT_RETRY_SECONDS*1000*1000);
   }
}

void PI433TX::listenForMessages(char* topic) {
   this->_topic = (char*) malloc(strlen(topic) + 1);
   if (NULL == this->_topic) {
      printf("Error, failed to allocation string for topic\n");
      return;
   }
   strcpy(this->_topic, topic);

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

   MQTTClient_subscribe(_myClient, this->_topic, QOS_AT_MOST_ONCE_0);
   DeviceListEntry* nextDevice = this->_devices;
   while(NULL != nextDevice) {
      MQTTClient_subscribe(_myClient, nextDevice->device->getTopic(), QOS_AT_MOST_ONCE_0);
      nextDevice = nextDevice->next;
   }

   while(1) {
      usleep(1000 * 1000);
      MQTTClient_yield();
   }
}

void  PI433TX::sendSync(void) {
   for(int i=0;i<5;i++) { 
      sendPulse(1000,1000);
   }
}

void PI433TX::sendByte(char byte) {
   char mask = 0x80;
   for(int i=0;i<8;i++) { 
      if ((byte & mask) != 0) { 
         sendPulse(490,160);
      } else {
         sendPulse(160,490);
      }
      mask = (mask >> 1) & 0x7F;
   }
}

void PI433TX::sendMessage(char* bytes) {
   for (int i=0;i<MESSAGE_REPEAT_COUNT;i++){ 
      sendSync();
      for(int i=3;i>=0;i--) { 
         sendByte(bytes[i]);
      }
   }
}

void  PI433TX::sendPulse(int lowDuration, int highDuration) {
   // seems that output from transmitter is inverse of what pin is
   // set to, pulse is high followed by low 
   digitalWrite(_txpin, HIGH );
   usleep(lowDuration);
   digitalWrite(_txpin, LOW);
   usleep(highDuration);
}

void PI433TX::sendTemp(float temp) {
   char bytes[4];
   bytes[3] = _deviceCode;

   int value = ((temp*10)+500);
   bytes[1] = value & 0xFF;
   bytes[2] = ((value >> 8)& 0x0F) + 0x90; 

   addChecksum(bytes);

   sendMessage(bytes);
}

// to calculate the checksum first reverse the nibbles in each byte
// then add the 3 message bytes together, add 0x66 and the swap
// the nibbles to get the expected checksum which is stored in byte 0 
void PI433TX::addChecksum(char* bytes) {
   char calcChecksum = 0x66;

   for (int i = 1;i<4;i++) {
      calcChecksum += ((bytes[i] >> 4) & 0x0F) + ((bytes[i] & 0x0F) << 4);
   }

   // swap the nibbles around and store in the checksum byt
   bytes[0] = ((calcChecksum >> 4) & 0x0F) + ((calcChecksum & 0x0F) << 4);
}
