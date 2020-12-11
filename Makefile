###############################################################
# GNU Make Makefile for UNIX - GNU C++

CC = g++ -std=c++98 -pedantic-errors -Wno-long-long

OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.cpp,%.o,$(wildcard postproc/*.cpp))

.SUFFIXES:
.SUFFIXES: .cpp .o
.PHONY: clean

postprocess: $(OBJS)
	$(CC) -o $@ $^ -ljanuary_tools -ljanuary -lpcre

.cpp.o:
	$(CC) -c -I. -MMD -o $@ $<

-include $(OBJS:.o=.d)

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) postprocess postprocess.exe
