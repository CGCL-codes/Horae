all : horae-1-addr-bmap horae-2x8-kick-bmap horae-2x8-no-kick-bmap horae-4x4-kick-bmap horae-4x4-no-kick-bmap \
	horae-2x8-kick-nmbp horae-2x8-no-kick-nmbp horae-4x4-kick-nmbp horae-4x4-no-kick-nmbp \
	horae-2x8-kick-nmnp horae-2x8-no-kick-nmnp horae-4x4-kick-nmnp horae-4x4-no-kick-nmnp

.PHONY : all
CXX = g++
CFLAGS = -lpthread -static-libstdc++ -std=c++11

horae-1-addr-bmap : horae-1-addr-bmap.o
	$(CXX) -o horae-1-addr-bmap horae-1-addr-bmap.o $(CFLAGS)
horae-2x8-kick-bmap : horae-2x8-kick-bmap.o
	$(CXX) -o horae-2x8-kick-bmap horae-2x8-kick-bmap.o $(CFLAGS)
horae-2x8-no-kick-bmap : horae-2x8-no-kick-bmap.o
	$(CXX) -o horae-2x8-no-kick-bmap horae-2x8-no-kick-bmap.o $(CFLAGS)
horae-4x4-kick-bmap : horae-4x4-kick-bmap.o
	$(CXX) -o horae-4x4-kick-bmap horae-4x4-kick-bmap.o $(CFLAGS)
horae-4x4-no-kick-bmap : horae-4x4-no-kick-bmap.o
	$(CXX) -o horae-4x4-no-kick-bmap horae-4x4-no-kick-bmap.o $(CFLAGS)

# horae-1-addr-nmbp : horae-1-addr-nmbp.o
# 	$(CXX) -o horae-1-addr-nmbp horae-1-addr-nmbp.o $(CFLAGS)
horae-2x8-kick-nmbp : horae-2x8-kick-nmbp.o
	$(CXX) -o horae-2x8-kick-nmbp horae-2x8-kick-nmbp.o $(CFLAGS)
horae-2x8-no-kick-nmbp : horae-2x8-no-kick-nmbp.o
	$(CXX) -o horae-2x8-no-kick-nmbp horae-2x8-no-kick-nmbp.o $(CFLAGS)
horae-4x4-kick-nmbp : horae-4x4-kick-nmbp.o
	$(CXX) -o horae-4x4-kick-nmbp horae-4x4-kick-nmbp.o $(CFLAGS)
horae-4x4-no-kick-nmbp : horae-4x4-no-kick-nmbp.o
	$(CXX) -o horae-4x4-no-kick-nmbp horae-4x4-no-kick-nmbp.o $(CFLAGS)

# horae-1-addr-nmnp : horae-1-addr-nmnp.o
# 	$(CXX) -o horae-1-addr-nmnp horae-1-addr-nmnp.o $(CFLAGS)
horae-2x8-kick-nmnp : horae-2x8-kick-nmnp.o
	$(CXX) -o horae-2x8-kick-nmnp horae-2x8-kick-nmnp.o $(CFLAGS)
horae-2x8-no-kick-nmnp : horae-2x8-no-kick-nmnp.o
	$(CXX) -o horae-2x8-no-kick-nmnp horae-2x8-no-kick-nmnp.o $(CFLAGS)
horae-4x4-kick-nmnp : horae-4x4-kick-nmnp.o
	$(CXX) -o horae-4x4-kick-nmnp horae-4x4-kick-nmnp.o $(CFLAGS)
horae-4x4-no-kick-nmnp : horae-4x4-no-kick-nmnp.o
	$(CXX) -o horae-4x4-no-kick-nmnp horae-4x4-no-kick-nmnp.o $(CFLAGS)




horae-1-addr-bmap.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-both-map-and-prelist/LAYER_ADDR_1.h
	$(CXX) -o horae-1-addr-bmap.o -c main.cpp -D HINT -D ADDR_1 -D SPD -D BMAP
horae-2x8-kick-bmap.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-both-map-and-prelist/LAYER_KICK_2x8.h
	$(CXX) -o horae-2x8-kick-bmap.o -c main.cpp -D HINT -D KICK_2x8 -D SPD -D BMAP
horae-2x8-no-kick-bmap.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-both-map-and-prelist/LAYER_NO_KICK_2x8.h
	$(CXX) -o horae-2x8-no-kick-bmap.o -c main.cpp -D HINT -D NO_KICK_2x8 -D SPD -D BMAP
horae-4x4-kick-bmap.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-both-map-and-prelist/LAYER_KICK_4x4.h
	$(CXX) -o horae-4x4-kick-bmap.o -c main.cpp -D HINT -D KICK_4x4 -D SPD -D BMAP
horae-4x4-no-kick-bmap.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-both-map-and-prelist/LAYER_NO_KICK_4x4.h
	$(CXX) -o horae-4x4-no-kick-bmap.o -c main.cpp -D HINT -D NO_KICK_4x4 -D SPD -D BMAP

# horae-1-addr-nmbp.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-no-map-but-prelist/LAYER_ADDR_1.h
# 	$(CXX) -o horae-1-addr-nmbp.o -c main.cpp -D HINT -D ADDR_1 -D SPD -D NMBP
horae-2x8-kick-nmbp.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-no-map-but-prelist/LAYER_KICK_2x8.h
	$(CXX) -o horae-2x8-kick-nmbp.o -c main.cpp -D HINT -D KICK_2x8 -D SPD -D NMBP
horae-2x8-no-kick-nmbp.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-no-map-but-prelist/LAYER_NO_KICK_2x8.h
	$(CXX) -o horae-2x8-no-kick-nmbp.o -c main.cpp -D HINT -D NO_KICK_2x8 -D SPD -D NMBP
horae-4x4-kick-nmbp.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-no-map-but-prelist/LAYER_KICK_4x4.h
	$(CXX) -o horae-4x4-kick-nmbp.o -c main.cpp -D HINT -D KICK_4x4 -D SPD -D NMBP
horae-4x4-no-kick-nmbp.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-no-map-but-prelist/LAYER_NO_KICK_4x4.h
	$(CXX) -o horae-4x4-no-kick-nmbp.o -c main.cpp -D HINT -D NO_KICK_4x4 -D SPD -D NMBP

# horae-1-addr-nmnp.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-no-map-no-prelist/LAYER_ADDR_1.h
# 	$(CXX) -o horae-1-addr-nmnp.o -c main.cpp -D HINT -D ADDR_1 -D SPD -D NMNP
horae-2x8-kick-nmnp.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-no-map-no-prelist/LAYER_KICK_2x8.h
	$(CXX) -o horae-2x8-kick-nmnp.o -c main.cpp -D HINT -D KICK_2x8 -D SPD -D NMNP
horae-2x8-no-kick-nmnp.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-no-map-no-prelist/LAYER_NO_KICK_2x8.h
	$(CXX) -o horae-2x8-no-kick-nmnp.o -c main.cpp -D HINT -D NO_KICK_2x8 -D SPD -D NMNP
horae-4x4-kick-nmnp.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-no-map-no-prelist/LAYER_KICK_4x4.h
	$(CXX) -o horae-4x4-kick-nmnp.o -c main.cpp -D HINT -D KICK_4x4 -D SPD -D NMNP
horae-4x4-no-kick-nmnp.o : main.cpp Horae/HORAE_SPD.h QueryFunction.h HashFunction.h Layer/Layer-no-map-no-prelist/LAYER_NO_KICK_4x4.h
	$(CXX) -o horae-4x4-no-kick-nmnp.o -c main.cpp -D HINT -D NO_KICK_4x4 -D SPD -D NMNP


.PHONY:clean
clean:
	-$(RM) horae-1-addr-bmap horae-2x8-kick-bmap horae-2x8-no-kick-bmap horae-4x4-kick-bmap horae-4x4-no-kick-bmap 
	-$(RM) horae-1-addr-bmap.o horae-2x8-kick-bmap.o horae-2x8-no-kick-bmap.o horae-4x4-kick-bmap.o horae-4x4-no-kick-bmap.o

	-$(RM) horae-2x8-kick-nmbp horae-2x8-no-kick-nmbp horae-4x4-kick-nmbp horae-4x4-no-kick-nmbp 
	-$(RM) horae-2x8-kick-nmbp.o horae-2x8-no-kick-nmbp.o horae-4x4-kick-nmbp.o horae-4x4-no-kick-nmbp.o 

	-$(RM) horae-2x8-kick-nmnp horae-2x8-no-kick-nmnp horae-4x4-kick-nmnp horae-4x4-no-kick-nmnp 
	-$(RM) horae-2x8-kick-nmnp.o horae-2x8-no-kick-nmnp.o horae-4x4-kick-nmnp.o horae-4x4-no-kick-nmnp.o 
