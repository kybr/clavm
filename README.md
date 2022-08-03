# C.L.A.V.M ~ C Language Audio Virtual Machine

This system enables a user to write little C programs that make sound. A program is compiles and run _immediately_ very quickly (usually less than 3 milliseconds). Right now I use it to make music, but my aim is to use it to 1) teach signals (DSP) and 2) serve as the audio runtime (VM) for music composition and performance with a high-level language like Raku.



# Live coding C for music and sound

First, live coding music in C is not my idea! Claude Heiland-Allen (mathr) has been live-coding with C since at least 2012 and his music is great. Please check out clive and mathr. (People have been writing sound and music in C for a very long time.) I learned of clive after I got my system off the ground. The goals, approatches, architectures, and musical outputs of clavm and clive are significantly different.

Second, live-coding sound in C is **physically dangerous** (I am dead serious). Do not live-code in C. Don't. But, if you do decide to live-code in C, then at least, please **do not wear headphones or earbuds or use powerful speakers**. It is very easy to make very loud sounds by making a typo or causing a crash or any of a hundred things. Use your computer speakers or small speakers or keep your monitors at a low level. You were warned.

Third, this system is not production level software. It can crash. There is no polish. It is not easy to use. Many things are incomplete or unimplemented. I have many features and additions on my todo list. In fact, I am still designing it and I may totally change the way it works. You would be a _alpha_ tester if you use it.

Ok. Now let's look at an example program:


```c
// C•L•A•V•M
//
// stereo beats ~ play two pure tones slightly out of tune
//

// {{{ prelude
// this prelude contains function declarations and definitions
// that we will use later. you can hide it with your code 
// editor so you can focus on your sounds

//
// functions built into CLAVM
//
double* _double(int); // grabs one double of persistent memory
void _out(float, float); // outputs a sample, both left and right
float _rate(); // reminds us of the current sample rate

//
// functions defined here in the prelude
//

int floor(double x) {
  // rounds down to the nearest integer
  int xi = (int)x;
  return x < xi ? xi - 1 : xi;
}

double phasor(double hz) {
  // ramps up from 0 to 1 repeatedly at the given frequency
  // a "stateful" function; it remembers the previous phase
  double* phase = _double(1);
  *phase += hz / _rate();
  *phase -= floor(*phase);
  return *phase;
}

double sine(double x) {
  // Taylor series expansion of sin; expects x on (0, 1)
  x = 2 * x - 1;
  double xx = x * x;
  return x * (3.138982 + xx * (-5.133625 + xx * (2.428288 - xx * 0.433645f)));
}

// }}} end of prelude

void play() {
  // the play function is called thousands of times per second. each time we
  // provide two numbers each between -1 and 1 that are essentially commands
  // for our speakers to move to forward or move back from their resting
  // position. call _out() to output samples.

  // loudness         frequency
  //    |                 |
  _out(0.1 * sine(phasor(220)),  // left output sample
       0.1 * sine(phasor(221))); // right output sample
  //        /           \
  //  sine function   phasor function

  // we hear "beats" at 1 Hz; read about this phenomenon here:
  // ...
}

```




## Making CLAVM work on your computer

It only works on macOS and Linux. Help me make it work on Windows?



### Get some dependencies

#### macOS

- Install [_Homebrew_][]
  - `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`
  - This should install the _Xcode_ command-line tools if they are not already installed
- Use `brew` to install _neovim_ (not vim) and _liblo_
  - `brew install neovim liblo`

Here is the consolidated set of commands:

```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install neovim liblo
```

[_Homebrew_]: https://brew.sh/



## Linux

```
sudo apt update # perhaps optional
sudo apt upgrade # perhaps optional
sudo apt install build-essential git neovim liblo-dev libasound2-dev
```

Get _neovim_ specifically, vim will not work.





### Build and start the system

- Use `git` to get the CLAVM source code
  - `git clone https://github.com/kybr/clavm`
- From the _clavm_ folder, get the TCC (Tiny C Compiler) source
  - `cd clavm`
  - `git submodule update --init`
- Build TCC by executing the `build-tinycc` script
  - `./build-tinycc`
- Build the _clavm_ programs with the `make` command
  - `make`
- Start a couple programs
  - In one terminal: `./clavm`
  - In another terminal: `./clavm-broker`
- In a third terminal, open an example in neovim
  - `nvim play/another.c`
- In neovim, "source" our configuration file
  - `:source .lvimrc`
- Then take off your headphones; Remove your earbuds. Set the levels on your modest speaker system to a reasonable volume.
- Now just add a space or change something in the file and you should here some sounds

```
git clone https://github.com/kybr/clavm
cd clavm
git submodule update --init
./build-tinycc
make
./clavm # in a terminal
./clavm-broker # in another terminal
nvim play/another.c # in a third terminal
:source .lvimrc # in neovim
```
