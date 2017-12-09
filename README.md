# Game: a game

### Description:

I'm making a game called Game.
Game is built with mods in mind.
Because it is module oriented, so adding mods is as easy as dragging and dropping files.

*** Game is a work in progress. Don't expect it to work. ***

![thumbnail.png](https://github.com/thebenperson/game/blob/doc/dev/res/thumbnail.png)

### Dependancies:

- GCC, or sad Windows imitation (via [MinGW](http://mingw.org/), [Cygwin](https://cygwin.com/), [Bash for Windows](https://msdn.microsoft.com/en-us/commandline/wsl/about), etc.)
- libpng
- XCB

### Compiling:

To compile Game, run `make` in the top level directory.
The launcher will be located in bin/.
Client modules will be located in bin/client/.
Server modules will be located in bin/server/.

Ignore the other directories and their contents. (Or don't. I don't care.)

### Running:

Run `./exec.sh` in the top level directory.
