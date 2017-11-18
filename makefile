# Game Development Build
# https://github.com/TheBenPerson/Game
#
# Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>
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
export CPATH := $(shell find src/common -type d | tr '\n' ':')

# compiler flags
CF := -Wall -Wextra -Wno-conversion-null -Wno-write-strings

# linker flags
LF := -fsanitize=leak -g

# build directories
BIN := bin
CBIN := $(BIN)/client
SBIN := $(BIN)/server
OBJ := $(BIN)/obj

all:
	@setterm --foreground green
	# Starting make with parallel jobs enabled...
	# To disable, run 'make _all'
	@setterm --default
	make -j $(shell nproc) _all

_all: game client
	@setterm --foreground green
	# Done
	@setterm --default

# launcher

.PHONY: game
game: $(BIN)/game
$(BIN)/game: $(OBJ)/main.o $(BIN)/common.so
	@setterm --foreground green
	# Compiling launcher: 'game'...
	@setterm --default
	gcc $(CF) $^ $(LF) -o $@

$(OBJ)/main.o: src/main.cpp
	@setterm --foreground green
	# Compiling launcher: 'main.o'...
	@setterm --default
	gcc $(CF) -shared -fpic $^ $(LF) -o $@

# common.so

.PHONY: common.so
common.so: $(BIN)/common.so
$(BIN)/common.so: $(shell find src/common -type f)
	@setterm --foreground green
	# Compiling library: 'common.so'...
	@setterm --default
	gcc $(CF) -shared -fpic $^ $(LF) -o $@

# client modules

.PHONY: client
client: client.so input.so win.so gfx.so ui.so

.PHONY: client.so
client.so: $(CBIN)/client.so
$(CBIN)/client.so: LFA := -lGL -lpng -lX11 -lX11-xcb -lxcb
$(CBIN)/client.so: src/client/client.cpp

.PHONY: input.so
input.so: $(CBIN)/input.so
$(CBIN)/input.so: src/client/input/input.cpp

.PHONY: win.so
win.so: $(CBIN)/win.so
$(CBIN)/win.so: $(addprefix $(CBIN)/, client.so input.so)
$(CBIN)/win.so: src/client/win/win.cpp

.PHONY: gfx.so
gfx.so: $(CBIN)/gfx.so
$(CBIN)/gfx.so: $(addprefix $(CBIN)/, client.so win.so)
$(CBIN)/gfx.so: src/client/gfx/gfx.cpp

.PHONY: ui.so
ui.so: $(CBIN)/ui.so
$(CBIN)/ui.so: $(addprefix $(CBIN)/, client.so input.so win.so gfx.so)
$(CBIN)/ui.so: $(shell find src/client/ui/ -name "*.cpp")

$(CBIN)/%.so:
	$(eval CPATH := $(CPATH)$(shell find src/client -type d | tr '\n' ':'))
	$(eval export CPATH)
	@setterm --foreground green
	# Compiling client module: '$(shell basename $@)'...
	@setterm --default

	gcc $(CF) -Isrc/client $(CFA) -shared -fpic $^ $(LF) $(LFA) -o $@

.PHONY:clean
clean:
	-rm $(CBIN)/*.so 2> /dev/null
	-rm $(SBIN)/*.so 2> /dev/null
	-rm $(BIN)/common.so 2> /dev/null
	-rm $(BIN)/game 2> /dev/null
	-rm $(OBJ)/*.o 2> /dev/null
