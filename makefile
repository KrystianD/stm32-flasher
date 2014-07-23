flasher: main.cpp proto.cpp uart.cpp devices.cpp uart.h
	g++ -O2 main.cpp proto.cpp uart.cpp devices.cpp -o flasher -fpermissive
