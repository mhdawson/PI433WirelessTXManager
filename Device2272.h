// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#ifndef _DEVICE2272
#define _DEVICE2272

#include "Device.h"

class Device2272 : public Device {
   public:
      Device2272(char* topic, int pulseWidth);
      int sendMessage(char* message);

   private:
      int sendTristate(char tristateValue);
      int _pulseWidth;
};

#endif
