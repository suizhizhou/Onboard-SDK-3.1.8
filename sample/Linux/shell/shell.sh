#!/bin/bash

make clean
rm onboardSDK
arm-none-linux-gnueabi-gcc src/server.c -o objs/server.o -c -I./inc -pthread
make

arm-none-linux-gnueabi-g++ -o bin/onboardSDK --std=c++11 -I../../lib/inc -Iinc -pthread -lrt objs/main.o objs/LinuxThread.o objs/LinuxSerialDevice.o objs/LinuxSetup.o objs/LinuxCleanup.o objs/LinuxMobile.o objs/LinuxFlight.o objs/LinuxWaypoint.o objs/LinuxInteractive.o objs/ReadUserConfig.o objs/DJI_API.o objs/DJI_App.o objs/DJI_Camera.o objs/DJI_Codec.o objs/DJI_Flight.o objs/DJI_Follow.o objs/DJI_HardDriver.o objs/DJI_HotPoint.o objs/DJI_Link.o objs/DJI_Memory.o objs/DJI_Mission.o objs/DJI_VirtualRC.o objs/DJI_WayPoint.o objs/server.o

mv bin/onboardSDK ./

