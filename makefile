# Game Development Build
# https://github.com/TheBenPerson/Game

# Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# flags
export CPATH := $(shell find src/common -type d | tr '\n' ':')
CF := -Wall -Wextra -Wno-conversion-null -Wno-write-strings
LF := -fsanitize=leak -g

# build directories
BIN := bin
CBIN := $(BIN)/client
SBIN := $(BIN)/server
OBJ := $(BIN)/obj

# default target
all: $(BIN)/game

.PHONY: release
release: LF := -Ofast
release: $(BIN)/game
	strip --strip-unneeded $(BIN)/common.so $(CBIN)/*

.phony: CMOD
CMOD:
	$(eval CPATH := $(CPATH)$(shell find src/client -type d | tr '\n' ':'))
	$(eval export CPATH)
	@setterm --foreground green
	@echo Compiling client module...
	@setterm --default

.phony: SMOD
SMOD:
	$(eval CPATH := $(CPATH)$(shell find src/server -type d | tr '\n' ':'))
	$(eval export CPATH)
	@setterm --foreground green
	@echo Compiling server module...
	@setterm --default

# launcher
$(BIN)/game: $(OBJ)/main.o $(BIN)/common.so
	gcc $^ -lstdc++ -ldl $(LF) -o $@

$(OBJ)/main.o: src/main.cpp
	$(eval export CPATH)
	gcc $(CF) -c $^ $(LF) -o $@

# client modules
client.so: CMOD $(CBIN)/client.so
$(CBIN)/client.so: src/client/client.cpp
	gcc $(CF) -shared -fpic -lGL -lpng -lX11 -lX11-xcb -lxcb $^ $(LF) -o $@

input.so: CMOD $(CBIN)/input.so
$(CBIN)/input.so: src/client/input/input.cpp
	gcc $(CF) -shared -fpic $^ $(LF) -o $@

win.so: CMOD $(CBIN)/win.so
$(CBIN)/win.so: $(CBIN)/client.so $(CBIN)/input.so src/client/win/win.cpp
	gcc $(CF) -shared -fpic $^ $(LF) -o $@

gfx.so: CMOD $(CBIN)/gfx.so
$(CBIN)/gfx.so: $(CBIN)/client.so $(CBIN)/win.so src/client/gfx/gfx.cpp
	gcc $(CF) -shared -fpic $^ $(LF) -o $@

ui.so: CMOD $(CBIN)/ui.so
$(CBIN)/ui.so: $(CBIN)/client.so $(CBIN)/gfx.so $(CBIN)/input.so $(CBIN)/win.so $(shell find src/client/ui -name "*.cpp")
	gcc $(CF) $(CBIN)/client.so -shared -fpic $^ $(LF) -o $@
	# change me 4 formatting plz

server.so: $(BIN)/server.so
$(BIN)/server.so: $(shell find src/server -name "*.cpp")
	gcc $(CF) -Isrc/server -shared -fpic $^ $(LF) -o $@

common.so: $(BIN)/common.so
$(BIN)/common.so: $(shell find src/common -name "*.cpp")
	gcc $(CF) -shared -fpic -pthread $^ $(LF) -o $@

.PHONY: clean
clean:
	-rm $(OBJ)/* $(BIN)/* 2> /dev/null
