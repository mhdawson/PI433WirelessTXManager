// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#include <unistd.h> 
#include "Device.h"

Device::Device(char* topic) {
   _topic = topic;
   _txpin = -1;
}

void Device::sendPulseLowHigh(int lowDuration, int highDuration) {
   // seems that output from transmitter is inverse of what pin is set to
   digitalWrite(_txpin, HIGH );
   usleep(lowDuration);
   digitalWrite(_txpin, LOW);
   usleep(highDuration);
}

void Device::sendPulseHighLow(int highDuration, int lowDuration) {
   // seems that output from transmitter is inverse of what pin is set to
   digitalWrite(_txpin, LOW );
   usleep(highDuration);
   digitalWrite(_txpin, HIGH);
   usleep(lowDuration);
}
