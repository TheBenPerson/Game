# Game Development Build
# https://github.com/TheBenPerson/Game
#
# Copyright (C) 2016-2018 Ben Stockett <thebenstockett@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# always include files in src/common
export CPATH := src/:$(shell find src/common -type d | tr '\n' ':')

# flags
CC := g++
CF := -O0 -Wall -Wextra -Wpedantic -Wno-write-strings -Wno-conversion-null -Wno-vla -D NULL=0
LF := -g -fsanitize=address

# build directories
BIN := bin
CB := $(BIN)/client
SB := $(BIN)/server
OBJ := $(BIN)/obj

.PHONY: all
all:
	@setterm --foreground green
	# Starting make with parallel jobs enabled...
	# To disable, run 'make _all'
	@setterm --default
	make -j $(shell nproc) _all

.PHONY: _all
_all: launcher client server
	@setterm --foreground green
	# Done
	@setterm --default

.PHONY: release
release: LF := -O3 -s
release: all

# launcher
# -rdynamic flag for modules referencing things defined in main

.PHONY: launcher
launcher: $(BIN)/game
$(BIN)/game: $(OBJ)/main.o $(BIN)/common.so
	@setterm --foreground green
	# Compiling launcher: 'game'...
	@setterm --default
	$(CC) $(CF) $^ $(LF) -ldl -rdynamic -o $@

$(OBJ)/main.o: src/main.cc
	@setterm --foreground green
	# Compiling launcher: 'main.o'...
	@setterm --default
	$(CC) $(CF) -c $^ $(LF) -o $@

# common.so

.PHONY: common.so
common.so: $(BIN)/common.so
$(BIN)/common.so: $(shell find src/common -name "*.cc")
	@setterm --foreground green
	# Compiling library: 'common.so'...
	@setterm --default
	$(CC) $(CF) -shared -fpic $^ $(LF) -lpthread -lm -lconfuse -o $@

# client modules

include client.mak
include server.mak

.PHONY: clean
clean:
	-find bin -type f -name "[^.]*" | xargs rm 2> /dev/null
