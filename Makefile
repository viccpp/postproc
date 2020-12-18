###############################################################
# GNU Make Makefile for UNIX - GNU C++

CC = g++ -std=c++14 -pedantic-errors

OBJS := $(patsubst %.cpp,%.o,$(wildcard postproc/*.cpp) $(wildcard *.cpp))

.SUFFIXES:
.SUFFIXES: .cpp .o
.PHONY: clean

postprocess: $(OBJS)
	$(CC) -o $@ $^ -lmfisoft14 -lpcre

.cpp.o:
	$(CC) -c -I. -g -MMD -o $@ $<

-include $(OBJS:.o=.d)

clean:
	rm -f *.o *.d postproc/*.o postproc/*.d postprocess postprocess.exe
