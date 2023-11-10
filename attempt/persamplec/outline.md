













**Karl Yerkes:**

- Artist / musician / engineer / educator; easily distracted, can't decide
- BS in Computer Engineering, University of Washington Seattle
- PhD in Media Arts and Technology, University of California Santa Barbara
- Artist in Residence at the SETI Institute; Made [SOMNIUM](https://www.youtube.com/watch?v=rNEHmrmx7EI)
- Direct an electroacoustic performance group, the [CREATE ensemble](https://soundcloud.com/create-ensemble)
- Lecturer in Meda Arts and Technology, teaching 6 courses a year
- [Raku](https://www.raku.org) user and contributor 🦋





(I studied computers because I wanted to know how synthesiser keyboards worked.)















# Per-Sample C



...is a C++ compiler/environment for new students of audio programming. Based on LLVM/clang and the Tiny C Compiler, it compiles a students short C/C++ program each time it changes, generating audio and visualizing the waveforms and analysis of the resulting signals. The immediacy of the system satisfies the curiosity of the impatient student that might not otherwise choose to learn C++.





(This is a work in progress. It is pre-alpha software.  It will probably crash during this talk.)

















## Why?

Several motivating questions drive this project. Its direction, features and
limitations result from asking (and not necessarily answering):

- How might we make teaching DSP and C/C++ easy/fun?
- What if I could only write music in C/C++? How might I do that?
- What would make a good discovery environment for [bytebeat](http://canonical.org/~kragen/bytebeat/) music?
- What if we thought of music a just the result of a bunch of calculations?
- How did the ancients write music code? (see [demoscene](https://en.wikipedia.org/wiki/Demoscene))
- We want to release a "album" on a teensy (arduino)
- What would an "audio shader" look like? GLSL for sound?
- How do compilers and virtual machines work?





(I am inspired and influenced by the work of my friends and colleagues [Charlie Roberts](https://charlie-roberts.com) and [Graham Wakefield](https://ampd.yorku.ca/profile/graham-wakefield/).)






















## Orientation



- this is a "live" compiler
- recompiles each time the text changes
- errors show up immediately
  + syntax errors
  + run-time errors (crashes)
    * (we cannot catch **all** run-time errors)
- valid programs make it to the server where `process` is called to generate each output sample
- assign to `o[]` to command the speakers





```C
void process(double d, float* i, float* o) { //
  o[0] = o[1] = 0; // change 0 to 1 to 0 to 1
}
```

```C
void process(double d, float* i, float* o) { //
  this is a compile error
  o[0] = o[1] = 0;
}
```

```C
void process(double d, float* i, float* o) { //
  0 / 0; // this is a run-time error
  o[0] = o[1] = 0;
}
```

```C
void _process(double d, float* i, float* o) { //
  o[0] = o[1] = 0;
}
```

















## Time

- `double d` is the number of seconds since the server started
- use it to generate signals, envelopes, or to schedule events
- make alternate timelines by doing arithmetic on copies of `d`

```C
void process(double d, float* i, float* o) { //
  d -= (int)d; // turn a line into a repeating ramp on (0, 1)
  o[0] = o[1] = d;
}
```

```C
void process(double d, float* i, float* o) { //
  d *= 2; // try 1, 2, 10, 100, 1000, 10000
  d -= (int)d;
  // d = 1 - d; // make a down-ramp out of an up ramp
  o[0] = o[1] = d;
}
```

```C
// remove the integer part of a number
//
float frac(double d) { //
  return d - (int)d;
}

void process(double d, float* i, float* o) { //
  // d *= 2; // try 1, 2, 10, 100, 1000, 10000
  // d -= (int)d; // turn a line into a repeating ramp on (0, 1)
  float f = frac(d * 2);
  o[0] = o[1] = f;
}
```



















## Sine

- use `sin` to make a smooth sound; sine is the purest timbre

```C
#include "math.h"

float frac(double d) {  //
  return d - (int)d;
}

void process(double d, float* i, float* o) {  //
  float phase = frac(220 * d);
  o[0] = o[1] = sin(M_PI * 2 * phase) * 0.3;
}
```

```C
#include "math.h"

float frac(double d) {  //
  return d - (int)d;
}

void process(double d, float* i, float* o) {  //
  float p1 = frac(224 * d);
  float p2 = frac(220 * d);
  o[0] = sin(M_PI * 2 * p1) * 0.3;
  o[1] = sin(M_PI * 2 * p2) * 0.3;
  o[2] = o[1] + o[0];
}
```

















## Phasor

- a ramp from 0 to 1 repeating at a given frequency
- a common object in computer music systems



```C
// an upward ramp from 0 to 1 repeating at a given frequency
//
float phasor(float hertz) {

  // static variables remember their value from last time 
  static float phase = 0;
  
  float value = phase;
  
  phase += hertz / SAMPLE_RATE;
  if (phase > 1) //
    phase -= 1;
  
  return value;
}

void process(double d, float* i, float* o) { //
  o[0] = phasor(2); // we can use this to make sine tones too
  o[1] = 0;
  // o[1] = phasor(2); // but, call phasor a second time ruins phase!
}
```



(We borrow words and concepts from the lexicon of Cycling'74's Max and SuperCollider.)

















## Global memory trick

- using a global block of "memory" and an index, we can make things like `phasor` reusable
- rules: 1) increment `index` whenever you use it, and 2) reset `index` on each process call

```C
#include "math.h"

float memory[10000];
int index = 0;

float phasor(float hertz) {
  float value = memory[index];

  memory[index] += hertz / SAMPLE_RATE;
  if (memory[index] > 1)  //
    memory[index] -= 1;

  // a new phase for each call of the phasor function
  index++;

  return value;
}

void process(double d, float* i, float* o) {
  index = 0; // don't forget to reset!

  o[0] = phasor(2);
  o[1] = phasor(2.03);
}
```











## `#define` and `#undef` 

- we can make this code a little more readable with the help of the pre-processor

```C
#include "math.h"

float memory[10000];
int index = 0;

#define PHASE (memory[index])
float phasor(float hertz) {
  float value = PHASE;

  PHASE += hertz / SAMPLE_RATE;
  if (PHASE > 1)  //
    PHASE -= 1;

  index++;  // a new phase for each call of the phasor function
  return value;
}
#undef PHASE

void process(double d, float* i, float* o) {  //
  index = 0; // reset global memory index

  o[0] = 0.2 * sin(2 * M_PI * phasor(110));
  o[1] = 0.2 * sin(2 * M_PI * phasor(111));
}
```















## Frequency Modulation

- our synthesis library is composed from a set of things like `phasor` above

```C
#include "synth.h"

// begin is called by the framework right after a recompile
// but before it calls process
void begin() {
  synth_begin(); // this call sets up the system
}

void process(double d, float* i, float* o) {
  synth_process(); // must call

  float f = sine(phasor(220 + 220 * sine(phasor(1))));

  o[0] = 0.2 * f;
  o[1] = 0.2 * f;
}
```





















## Envelope and  control signals

- use `frac` to make envelopes to control stuff

```C
#include "synth.h"

void begin() { synth_begin(); }

void process(double d, float* i, float* o) {
  synth_process();

  float e = frac(.1 * d);
  o[2] = e;

  float f = sine(phasor(220 + e * 220 * sine(phasor(220))));

  o[0] = 0.2 * f;
  o[1] = 0.2 * f;

  o[3] = sine(phasor(2 + e * 2 * sine(phasor(2))));
}
```



















## Phase distortion synthesis

- used on Casio keyboards such as the CZ-1

```C
#include "synth.h"

void begin() { synth_begin(); }

void process(double d, float* i, float* o) {
  synth_process();

  float e = phasor(0.1) * 0.5;
  o[2] = e;

  float phase = pd(phasor(220), e, 0.5);

  float f = sine(phase);

  o[0] = 0.2 * f;
  o[1] = 0.2 * f;

  o[2] = pd(phasor(220 / 100.0), e, 0.5);
  o[3] = sine(o[2]);
}
```















## Sequencing 

- we can change frequency in steps using an array and an integer
- `mtof()` converts midi "note" values to frequency in Hertz 

```C
#include "synth.h"

void begin() { synth_begin(); }

void process(double d, float* i, float* o) {
  synth_process();

  float e = frac(d * 1) * 0.5;
  o[2] = e;

  float hz = mtof((float[]){46, 62, 23, 38}[(int)d % 4]);

  float phase = pd(phasor(hz), e, 0.5);

  float f = sine(phase);

  o[0] = 0.2 * f;
  o[1] = 0.2 * f;
}
```





















## More synth offerings

- `ms20` is a virtualization of the famous Korg analog filter
- `quasi` is an FM-based anti-aliased sawtooth-sounding synth

```C
#include "synth.h"

void begin() { synth_begin(); }

void process(double d, float* i, float* o) {
  synth_process();

  d *= 2.9;

  float e = frac(d * 1) * 0.5;
  o[2] = e;

  float hz = mtof((float[]){46, 62, 24, 39, 65, 34, 56, 67}[(int)d % 8]);

  float f = quasi(hz, 0.99 * e);

  o[3] = 1 - frac(d * 2);
  f = ms20(f, hz * 8 * o[3], 1.2);

  o[0] = 0.2 * f;
  o[1] = 0.2 * f;

  o[5] = quasi(hz / 100, 0.99 * e);
}
```

























## bytebeat 

- we can support bytebeat music very easily 

```C
void process(double d, float* i, float* o) {
  // simulate 8000 Hz playback rate
  d *= 8000.0;
  int t = d;
  char r = t * 5 & (t >> 7) | t * 3 & (t * 4 >> 10);
  o[0] = o[1] = r / 128.0 * 0.3;
}
```

























## Misc



```C

// tick randomly
//

int rand();

// random on (0, 1)
float r() {
  // this is the number on macOS; it might be different on Linux or Windows
  return rand() / 2147483647.0f;
}

void process(double t, float* input, float* o) {
  for (int i = 0; i < 8; i++) {
    o[i] = 0;
  }

  o[0] = o[1] = 0;
  if (r() < 0.0001)  //
    o[0] = o[1] = 1.0;
}

```





















## Conclusion / end notes



- MAT 240A/B/C series sometimes accepts undergraduates
- MAT program offers an MS in Media Arts and Technology
- Curtis Roads is a living legend; He teaches at UCSB
- Questions





