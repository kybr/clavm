# Live coding C for music and sound


```c
float* _float(int); // grab one float of persistent state
void _out(float, float); // output sample (left, right)
float _rate(); // what is the sample rate?
double sin(double); // trigonometry!!

// round down
int floor(double x) {
  int xi = (int)x;
  return x < xi ? xi - 1 : xi;
}

// a ramp up from 0 to 1 repeatedly at the given frequency
// a "stateful" function; it remembers the previous value
float phasor(float hz) {
  float* phase = _float(1);
  *phase += hz / _rate();
  *phase -= floor(*phase);
  return *phase;
}

double pi = 6.28318530718;
void play() {
  // play two sine waves, slightly out of tune
  _out(0.1 * sin(pi * phasor(220)),
       0.1 * sin(pi * phasor(221)));
}

```




## Making CLAVM work on your computer

- Works on macOS and Linux
  - Help me make it work on Windows?





#### macOS

- Install [_Homebrew_][]
  - `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`
  - This should install the _Xcode_ command-line tools if they are not already installed
- Use `brew` to install _neovim_ and _liblo_
  - `brew install neovim liblo` (anything else?)
- Use `git` to get the CLAVM source code
  - `git clone https://github.com/kybr/clavm`
- From the _clavm_ folder, get the TCC (Tiny C Compiler) source
  - `cd clavm`
  - `git submodule update --init`
- Build TCC (Tiny C Compiler) by executing the `build-tcc` script
  - `./build-tcc`
- Build the _clavm_ programs with the `make` command
  - `make`



Here is the consolidated set of commands:

```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install neovim liblo
git clone https://github.com/kybr/clavm
cd clavm
git submodule update --init
./build-tcc
make
```



[_Homebrew_]: https://brew.sh/



## Linux



- Install some libraries and commands
  - `sudo apt update` (perhaps optional)
  - `sudo apt upgrade` (perhaps optional)
  - `sudo apt install build-essential git neovim liblo-dev libasound2-dev`
- Use `git` to get the CLAVM source code
  - `git clone https://github.com/kybr/clavm`
- From the _clavm_ folder, use `git` to get the TCC (Tiny C Compiler) source
  - `cd clavm`
  - `git submodule update --init`
- Build TCC (Tiny C Compiler) by executing the `build-tcc` script
  - `./build-tcc`
- Build the _clavm_ programs with the `make` command
  - `make`



Here is the consolidated set of commands:

```
sudo apt update # perhaps optional
sudo apt upgrade # perhaps optional
sudo apt install build-essential git neovim liblo-dev libasound2-dev
git clone https://github.com/kybr/clavm
cd clavm
git submodule update --init
./build-tcc
make
```




## Related

There is another live coding music in C:

"No input live coding"
<https://toplap.org/2012/10/01/clive-no-input-live-coding/>

