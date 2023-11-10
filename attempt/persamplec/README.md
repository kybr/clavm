# Per-Sample C: a live-coding environment for audio programming

This project depends on the Tiny C Compiler so it supports C only. For compiling
C++, we refer you to the Per-Sample C++ project which uses clang and the LLVM
compiler infrastructure.





## TODO

We should add a bunch of built-in functions like those offered by GLSL,
GenExpr, Puredata, Max, and SuperCollider.

- test on Windows
- make plugins for atom, vscode, and sublime
- need a way to include standard math library functions
- need error reporting in the editor
- pi + dac + arduino with knobs

Ideas:

- client shortcut compile
  + report compile error immediately
  + don't send to server
- remove clang-format dependency
- store each program for playback
- maintain a session recording on the server
- command line arguments for server
  + audio device
  + sample rate
  + include path
  + report status 
- expose server configuration on OSC
- switch to oscpkt (drop liblo)

ꛘ

## Why?

Several motivating questions drive this project. Its direction, features and
limitations result from asking (and not necessarily answering):

- What would an "audio shader" look like? GLSL for sound?
- How might we make teaching DSP and C/C++ easy/fun?
- What if someone put a gun to my head and said "Write music in pure C."
- What would make a good discovery environment for ByteBeat?
- What if we think of music a just the result of a bunch of calculations?
- How did the ancients write music code? (C64, Mario Brothers, Zelda)
