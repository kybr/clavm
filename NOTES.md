

Change Keyboard latency settings on macOS

    defaults write -g InitialKeyRepeat -int 10 # normal minimum is 15 (225 ms)
    defaults write -g KeyRepeat -int 1 # normal minimum is 2 (30 ms)


Measure TCP setup latency on localhost:

    socat TCP-LISTEN:8001,fork -
    pip3 install tcp-latency
    tcp-latency -p 8001 localhost

