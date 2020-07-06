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

_: cavm cavm-submit test

%.o : %.cpp
	$(CXX) -c $<

TCC.o : TCC.cpp
	$(CXX) -I static/include -c $<

RtAudio.o : RtAudio.cpp
	$(CXX) -D__MACOSX_CORE__ -c $<

cavm : cavm.o TCC.o HotSwap.o RtAudio.o
	$(CXX) $(LINK) $^ -o $@

cavm-submit : cavm-submit.o TCC.o
	$(CXX) -L static/lib -ltcc -llo $^ -o $@

test : test.o TCC.o HotSwap.o
	$(CXX) -L static/lib -ltcc $^ -o $@

clean:
	rm *.o test cavm cavm-submit
