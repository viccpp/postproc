###############################################################
# GNU Make Makefile for UNIX - GNU C++

CC = g++ -std=c++14 -pedantic-errors

OBJS := $(patsubst %.cpp,%.o,$(wildcard postproc/*.cpp) $(wildcard *.cpp))

.SUFFIXES:
.SUFFIXES: .cpp .o
.PHONY: clean

postprocess: $(OBJS)
	$(CC) -o $@ $^ -l__vic14 -lpcre

.cpp.o:
	$(CC) -c -I. -DPCRE_STATIC -g -Wall -MMD -o $@ $<

-include $(OBJS:.o=.d)

clean:
	rm -f *.o *.d postproc/*.o postproc/*.d postprocess postprocess.exe
