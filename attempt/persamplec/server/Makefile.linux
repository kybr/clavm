.SUFFIXES:

CXX =
CXX += g++
CXX += -std=c++11

COMPILER =
COMPILER += -Wall
COMPILER += -g
COMPILER += -D__LINUX_ALSA__ 

LINKER =
LINKER += -ltcc
LINKER += -ldl
LINKER += -lpthread
LINKER += -llo
LINKER += -lasound

_: per-sample-c-server

%.o: %.cpp
	$(CXX) $^ -c $(COMPILER)

per-sample-c-server: per-sample-c-server.o compiler.o RtAudio.o
	$(CXX) $^ -o $@ $(LINKER)
	cp per-sample-c-server ../play

clean:
	rm -f per-sample-c-server
	rm -f RtAudio.o compiler.o per-sample-c-server.o
