all : gss-timeslice

.PHONY : all
CXX = g++
CFLAGS = -lpthread -static-libstdc++ -std=c++11


gss-timeslice : gss-timeslice.o
	$(CXX) -o gss-timeslice gss-timeslice.o $(CFLAGS)

gss-timeslice.o : main.cpp QueryFunction.h HashFunction.h GssTimeslice.h params.h
	$(CXX) -o gss-timeslice.o -c main.cpp -D HINT
	

.PHONY:clean
clean:
	-$(RM) gss-timeslice
	-$(RM) gss-timeslice.o