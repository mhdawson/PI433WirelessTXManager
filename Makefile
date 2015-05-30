all: PI433TXmain

PI433TXmain: Device.o Device2272.o DeviceLacross141.o  PI433TX.o PI433TXmain.o 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi -lpaho-mqtt3cs
	
clean:
	$(RM) *.o PI433TXmain
