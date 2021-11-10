all : horae horae-compacted

.PHONY : all
CXX = g++
CFLAGS = -lpthread -static-libstdc++ -std=c++11


horae : horae.o
	$(CXX) -o horae horae.o $(CFLAGS)
horae-compacted : horae-compacted.o
	$(CXX) -o horae-compacted horae-compacted.o $(CFLAGS)


horae.o : main.cpp headers/Horae.h headers/QueryFunction.h headers/HashFunction.h headers/LayerHeaders.h headers/params.h
	$(CXX) -o horae.o -c main.cpp -D HINT
horae-compacted.o : main.cpp headers/Horae.h headers/QueryFunction.h headers/HashFunction.h headers/LayerHeaders.h headers/params.h
	$(CXX) -o horae-compacted.o -c main.cpp -D HINT -D MEM
	

.PHONY:clean
clean:
	-$(RM) horae horae-compacted
	-$(RM) horae.o horae-compacted.o