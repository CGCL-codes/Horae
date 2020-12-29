all : horae-2x8-kick horae-2x8-no-kick horae-4x4-kick horae-4x4-no-kick

.PHONY : all
CXX = g++
CFLAGS = -lpthread -static-libstdc++ -std=c++11

horae-2x8-kick : horae-2x8-kick.o
	$(CXX) -o horae-2x8-kick horae-2x8-kick.o $(CFLAGS)
horae-2x8-no-kick : horae-2x8-no-kick.o
	$(CXX) -o horae-2x8-no-kick horae-2x8-no-kick.o $(CFLAGS)
horae-4x4-kick : horae-4x4-kick.o
	$(CXX) -o horae-4x4-kick horae-4x4-kick.o $(CFLAGS)
horae-4x4-no-kick : horae-4x4-no-kick.o
	$(CXX) -o horae-4x4-no-kick horae-4x4-no-kick.o $(CFLAGS)

horae-2x8-kick.o : main.cpp HORAE.h QueryFunction.h HashFunction.h Layer/LAYER_KICK_2x8.h
	$(CXX) -o horae-2x8-kick.o -c main.cpp -D HINT -D KICK_2x8
horae-2x8-no-kick.o : main.cpp HORAE.h QueryFunction.h HashFunction.h Layer/LAYER_NO_KICK_2x8.h
	$(CXX) -o horae-2x8-no-kick.o -c main.cpp -D HINT -D NO_KICK_2x8
horae-4x4-kick.o : main.cpp HORAE.h QueryFunction.h HashFunction.h Layer/LAYER_KICK_4x4.h
	$(CXX) -o horae-4x4-kick.o -c main.cpp -D HINT -D KICK_4x4
horae-4x4-no-kick.o : main.cpp HORAE.h QueryFunction.h HashFunction.h Layer/LAYER_NO_KICK_4x4.h
	$(CXX) -o horae-4x4-no-kick.o -c main.cpp -D HINT -D NO_KICK_4x4


.PHONY:clean
clean:
	-$(RM) horae-2x8-kick horae-2x8-no-kick horae-4x4-kick horae-4x4-no-kick 
	-$(RM) horae-2x8-kick.o horae-2x8-no-kick.o horae-4x4-kick.o horae-4x4-no-kick.o 
