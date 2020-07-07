CXX = c++
CXX += -std=c++11
CXX += -g
CXX += -Wall
#CXX += -O2

LINK =
LINK += -L static/lib
LINK += -ltcc
LINK += -llo
#LINK += -ldl
#LINK += -lpthread
LINK += -framework CoreAudio
LINK += -framework CoreFoundation

_: clavm clavm-submit clavm-wav test

%.o : %.cpp
	$(CXX) -c $<

TCC.o : TCC.cpp
	$(CXX) -I static/include -c $<

RtAudio.o : RtAudio.cpp
	$(CXX) -D__MACOSX_CORE__ -c $<

clavm : clavm.o TCC.o HotSwap.o RtAudio.o
	$(CXX) $(LINK) $^ -o $@

clavm-submit : clavm-submit.o TCC.o
	$(CXX) -L static/lib -ltcc -llo $^ -o $@

clavm-wav : clavm-wav.o TCC.o
	$(CXX) -L static/lib -ltcc $^ -o $@

test : test.o TCC.o HotSwap.o
	$(CXX) -L static/lib -ltcc $^ -o $@

clean:
	rm *.o test clavm clavm-submit
