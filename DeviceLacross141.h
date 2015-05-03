// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#ifndef _DEVICELACROSS141
#define _DEVICELACROSS141

#include "Device.h"

class DeviceLacross141 : public Device {
   public:
      DeviceLacross141(char* topic, char deviceCode);
      int sendMessage(char* message);

   private:
      char _deviceCode;
      void sendSync(void);
      void sendByte(char byte);
      void sendTemp(float temp);
      void addChecksum(char* bytes);
      void sendTempMessage(char* bytes);
      void doRegister(void);
};

#endif
