all : segtree

.PHONY : all
CXX = g++
CFLAGS = -lpthread -static-libstdc++ -std=c++11

segtree : segtree.o
	$(CXX) -o segtree segtree.o $(CFLAGS)

segtree.o : main.cpp QueryFunction.h HashFunction.h params.h SegmentTree.h DynamicSegTree.h TreeNode.h
	$(CXX) -o segtree.o -c main.cpp -D HINT
	
.PHONY:clean
clean:
	-$(RM) segtree
	-$(RM) segtree.o