
OS := $(shell uname)

CXX = c++
CXX += -std=c++11
CXX += -g
CXX += -Wall
CXX += -I/opt/homebrew/include
#CXX += -O2


LINK_AUDIO =
ifeq "$(OS)" "Darwin"
LINK_AUDIO += -framework CoreAudio
LINK_AUDIO += -framework CoreFoundation
else
LINK_AUDIO += -lasound
endif

DEFINE_AUDIO =
ifeq "$(OS)" "Darwin"
DEFINE_AUDIO += -D__MACOSX_CORE__
DEFINE_AUDIO += -Wdeprecated-declarations
else
DEFINE_AUDIO += -D__LINUX_ALSA__
endif

#LINK += -ldl
#LINK += -lpthread

LINK_TCC =
LINK_TCC += -L static/lib
LINK_TCC += -ltcc
LINK_TCC += static/lib/libtcc.a
ifeq "$(OS)" "Darwin"
  LINK_TCC += -L/opt/homebrew/lib
else
LINK_TCC += -ldl
LINK_TCC += -lpthread
endif

all: clavm clavm-submit clavm-broker clavm-wav test

%.o : %.cpp
	$(CXX) -c $<

ifeq "$(OS)" "Darwin"
else
Utilities.o : Utilities.cpp
	$(CXX) -c $< -Wno-psabi
endif

TCC.o : TCC.cpp
	$(CXX) -I static/include -c $<

RtMidi.o : RtMidi.cpp
	$(CXX) -D__MACOSX_CORE__ -c $<

RtAudio.o : RtAudio.cpp
	$(CXX) $(DEFINE_AUDIO) -c $<

clavm : clavm.o TCC.o HotSwap.o RtAudio.o RtMidi.o
	$(CXX) $^ -o $@ $(LINK_TCC) $(LINK_AUDIO) -llo -framework CoreMidi

clavm-submit : clavm-submit.o TCC.o Utilities.o
	$(CXX) $^ -o $@ $(LINK_TCC) -llo

clavm-broker : clavm-broker.o TCC.o Utilities.o
	$(CXX) $^ -o $@ $(LINK_TCC) -llo

clavm-wav : clavm-wav.o TCC.o Utilities.o
	$(CXX) $^ -o $@ $(LINK_TCC)

test : test.o TCC.o HotSwap.o
	$(CXX) $^ -o $@ $(LINK_TCC)

clean:
	rm *.o test clavm clavm-submit clavm-wav clavm-broker
