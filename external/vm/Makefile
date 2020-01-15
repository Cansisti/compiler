FLAGS = -W -pedantic -std=c++17 -O3

.PHONY = all clean cleanall

all: maszyna-wirtualna maszyna-wirtualna-cln

maszyna-wirtualna: lexer.o parser.o mw.o main.o
	$(CXX) $^ -o $@ -I../../spdlog/include
	strip $@
	cp maszyna-wirtualna ../../build/vm

maszyna-wirtualna-debug: lexer.o parser.o mw.o
	$(CXX) $(FLAGS) -c main.cc -I../../spdlog/include -DMY_VM_DEBUG
	$(CXX) $^ main.o -o $@
	strip $@
	cp maszyna-wirtualna-debug ../../build/vmd

maszyna-wirtualna-cln: lexer.o parser.o mw-cln.o main.o
	$(CXX) $^ -o $@ -l cln
	strip $@

%.o: %.cc
	$(CXX) $(FLAGS) -c $^ -I../../spdlog/include

lexer.cc: lexer.l parser.hh
	flex -o $@ $<

parser.cc parser.hh: parser.y
	bison -Wall -d -o parser.cc $^

clean:
	rm -f *.o parser.cc parser.hh lexer.cc

cleanall: clean
	rm -f maszyna-wirtualna maszyna-wirtualna-cln
