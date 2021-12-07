all : tcm-timeslice

.PHONY : all
CXX = g++
CFLAGS = -lpthread -static-libstdc++ -std=c++11


tcm-timeslice : tcm-timeslice.o
	$(CXX) -o tcm-timeslice tcm-timeslice.o $(CFLAGS)


tcm-timeslice.o : main.cpp HashFunction.h TcmTimeslice.h TestFunctions.h
	$(CXX) -o tcm-timeslice.o -c main.cpp
	
.PHONY:clean
clean:
	-$(RM) tcm-timeslice
	-$(RM) tcm-timeslice.o