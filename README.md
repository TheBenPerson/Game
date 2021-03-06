# Game: a game

### Description:

I'm making a game called Game.
Game is built with mods in mind.
Because it is module oriented, adding mods is as easy as dragging and dropping files.

*** Game is a work in progress. Don't expect it to work. ***

[![thumbnail.png](https://github.com/thebenperson/game/blob/doc/res/thumbnail.png)](https://raw.githubusercontent.com/thebenperson/game/doc/res/video.mp4)

### Dependancies:

- *NIX setup, or sad Windows imitation (via [MinGW](http://mingw.org/), [Cygwin](https://cygwin.com/), [Bash for Windows](https://msdn.microsoft.com/en-us/commandline/wsl/about), etc.)
- [libpng](http://libpng.org/pub/png/libpng.html)
- [GLFW](http://glfw.org/)

### Compiling:

To compile Game, run `make` in the top level directory.
The launcher will be located in bin/.
Client modules will be located in bin/client/.
Server modules will be located in bin/server/.

Ignore the other directories and their contents. (Or don't. I don't care.)

### Running:

Run `./exec.sh` in the top level directory.
