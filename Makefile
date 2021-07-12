all : horae horae-mem

.PHONY : all
CXX = g++
CFLAGS = -lpthread -static-libstdc++ -std=c++11

horae : horae.o
	$(CXX) -o horae horae.o $(CFLAGS)
horae-mem : horae-mem.o
	$(CXX) -o horae-mem horae-mem.o $(CFLAGS)


horae.o : main.cpp headers/Horae.h headers/QueryFunction.h headers/HashFunction.h headers/LayerHeaders.h headers/params.h
	$(CXX) -o horae.o -c main.cpp -D HINT -D MEM
horae-mem.o : main.cpp headers/Horae.h headers/QueryFunction.h headers/HashFunction.h headers/LayerHeaders.h headers/params.h
	$(CXX) -o horae-mem.o -c main.cpp -D HINT -D MEM
	

.PHONY:clean
clean:
	-$(RM) horae horae-mem
	-$(RM) horae.o horae-mem.o