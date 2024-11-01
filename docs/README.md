# GT

## Installing on Windows
This project uses a Makefile to build the library. To execute the Makefile on Windows, you must install a version of GNU make,
which is available in toolkits such as MSYS2, Cygwin, Git Bash, MinGW, or through the Windows Subsystem for Linux (WSL).

- MSYS2

Before installing the packages, make sure you're running the MinGW 64-bit shell.

   ```
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-SDL2
   pacman -S mingw-w64-x86_64-opengl
   pacman -S make
   git clone https://github.com/dev-harbour/gt.git
   ```
- To build the static library, execute:

   ```
   make
   ```

- To build and run samples and test code, navigate to the tests directory and execute:

   ```
   cd tests
   make
   ```
   or single build:
   ```
   gcc window.c -o window -I ../include -L ../lib -lmingw32 -lgt -lSDL2main -lSDL2 -lopengl32
   ```
## Installing on Linux

- Debian

   ```
   sudo apt update
   sudo apt install libsdl2-dev
   git clone https://github.com/dev-harbour/gt.git
   ```
- To build the static library, execute:

   ```
   make
   ```

- To build and run samples and test code, navigate to the examples directory and execute:

   ```
   cd tests
   make
   ```
   or single build:
   ```
   gcc window.c -o window -I ../include -L ../lib -lgt -lSDL2 -lGL
   ```
---
