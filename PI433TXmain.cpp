// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#include "PI433TX.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "Device2272.h"
#include "DeviceLacross141.h"

// define error codes
#define MISSING_BROKER             -1
#define MISSING_CERT_DIR           -2 
#define INVALID_CERT_DIR           -3 
#define MISSING_CERT_FILES         -4
#define FAILED_TO_SETUP_WIRING_PI  -5

#define NUM_REQUIRED_CERT_FILES 3 

int main(int argc, char *argv[]) {
   int PIN = 0;
   char* certsDir = NULL;

   // validate we got the required parameters 
   if (2 > argc) {
      printf("Usage: PI433TXMain mqtt_url <cert info dir>\n");
      return MISSING_BROKER;
   }
   if (strstr(argv[1], "ssl://") == argv[1]) {
      // ok we expect a third parameter which tells us where to look for the 
      // certificates and keys 
      // the names of the files in the directory specified must be client.cert client.key and ca.cert
      if (3 > argc) { 
         printf("Cert info dir required for connection to broker with ssl - exiting\n");
         printf("Usage: PI433TXMain mqtt_url <cert info dir>\n");
         return MISSING_CERT_DIR;
      }
 
      // validate the directory exists
      DIR* dirPtr = opendir(argv[2]);
      if(NULL == dirPtr) {
         printf("Cert info dir was invalid - exiting\n");
         return INVALID_CERT_DIR;
      } 
     
      struct dirent *nextEntry;
      int requiredFilesFound = 0;
      while ((nextEntry = readdir(dirPtr)) != NULL) {
         if ((strcmp(nextEntry->d_name, CA_CERT_FILE) == 0) ||
             (strcmp(nextEntry->d_name, CLIENT_CERT_FILE) == 0) ||
             (strcmp(nextEntry->d_name, CLIENT_KEY_FILE) == 0)) {
            requiredFilesFound++;
         }
      }
      closedir(dirPtr);

      if (requiredFilesFound != NUM_REQUIRED_CERT_FILES) { 
         printf("Missing cert info files, directory must contain client.key, client.cert and ca.cert\n");
         return MISSING_CERT_FILES;
      }

      certsDir = argv[2];
   }

   if(wiringPiSetup() == -1) {
      return FAILED_TO_SETUP_WIRING_PI;
   }
   
   // TODOs
   // Externalize configuration of devices
   // Make device part of message as opposed to 141 device
   // allow topics and pin to be configurable
   PI433TX transmitter = PI433TX(PIN, argv[1], argv[2]);
   transmitter.registerDevice(new Device2272((char*) "home/2272"));
   transmitter.registerDevice(new DeviceLacross141((char*) "house/nexxtech/temp", 0x32));
   transmitter.listenForMessages();
}

