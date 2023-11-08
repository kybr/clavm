

Change Keyboard latency settings on macOS

    defaults write -g InitialKeyRepeat -int 10 # normal minimum is 15 (225 ms)
    defaults write -g KeyRepeat -int 1 # normal minimum is 2 (30 ms)


Measure TCP setup latency on localhost:

    socat TCP-LISTEN:8001,fork -
    pip3 install tcp-latency
    tcp-latency -p 8001 localhost

2023-11-06..

The CLAVM API:

On the user code side, we have C as implemented by TinyCC. There are several functions that we can expect:

- For controlling execution:
  * `void play(void)` ~ called at the sample rate
  * `void compiled(void)` ~ called after the code is compiled
- For accessing state memory:
  * `float* _float(int)`
  * `double* _double(int)`
  * `int* _int(int)`
- For input/output and parameters
  * `void _out(int,float)` ~ output the given float on the given channel
  * `float _in(int)` ~ return the float on a given channel
  * `float _cc(int)` or `float _p(int)` and `int _i(int)`
- For other things
  * `double _time(void)` ~ second since the audio server started
  * `double _samplerate(void)` ~ the current sample rate
- What else? Perhaps a set of builtins, a library that might include...
  * `float Phasor(float)`
  * `float Cycle(float)`
  * `float Tri(float)`
  * `float Pulse(float,float)`
  * `float Onepole(float,float)`
  * `float History(float)`
  * `float Delay(float,float)`
  * `float Dust(float)`
  * `float Uniform(float)`
  * `float Normal(float)`

On the back end, we have C++ and C as implemented by clang (MSVC on Windows). The functions above are implemented on this backend in the C (flat) namespace---That is to say *no* namespace. Most of these functions depend on a single contiguous block of memory to operate. This memory should be shared etween the two compiler processes.

Idea: get rid of OSC. The bridge from Vim (Neovim) to the audio server is OSC, implemented in the luajit that is built into Neovim. What if, instead of OSC, we could use a shared memory interface to pass the text from the vim buffer to the audio server? I think this would be some C + FFI where the lua calls an FFI function `string send(string)` that connects to the shared memory, copies the string in, and posts the semaphore that signals the write is done.

submit.c (knows how to send clavm a string and get back a string)
clavm.cpp (server to Neovim, client to TCC processes)
server.cpp (pair of TCC processes that share memory)

submit might need to be a shared library so that lua can do FFI.




