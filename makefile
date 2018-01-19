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
export CPATH := src/:$(shell find src/common -type d | tr '\n' ':')

# compiler flags
CF := -Wall -Wextra -Wpedantic -Wno-conversion-null -Wno-implicit-fallthrough -Wno-write-strings -fpermissive

# linker flags
LF := -fsanitize=address -g

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

_all: launcher client
	@setterm --foreground green
	# Done
	@setterm --default

release: LF := -O2 -s
release: all

# launcher
# -rdynamic flag for modules referencing things defined in

.PHONY: launcher
launcher: $(BIN)/game
$(BIN)/game: $(OBJ)/main.o $(BIN)/common.so
	@setterm --foreground green
	# Compiling launcher: 'game'...
	@setterm --default
	gcc $(CF) $^ $(LF) -ldl -lstdc++ -rdynamic -o $@

$(OBJ)/main.o: src/main.cc
	@setterm --foreground green
	# Compiling launcher: 'main.o'...
	@setterm --default
	gcc $(CF) -c $^ $(LF) -o $@

# common.so

.PHONY: common.so
common.so: $(BIN)/common.so
$(BIN)/common.so: $(shell find src/common -type f)
	@setterm --foreground green
	# Compiling library: 'common.so'...
	@setterm --default
	gcc $(CF) -shared -fpic $^ $(LF) -lpthread -o $@

# client modules

.PHONY: client
client: client.so input.so win.so gfx.so ui.so

.PHONY: client.so
client.so: $(CBIN)/client.so
$(CBIN)/client.so: LFA := -lGL -lpng -lX11 -lX11-xcb -lxcb
$(CBIN)/client.so: $(addprefix $(CBIN)/, button.so)
$(CBIN)/client.so: src/client/client.cc

.PHONY: input.so
input.so: $(CBIN)/input.so
$(CBIN)/input.so: src/client/input/input.cc

.PHONY: win.so
win.so: $(CBIN)/win.so
$(CBIN)/win.so: $(addprefix $(CBIN)/, client.so input.so)
$(CBIN)/win.so: src/client/win/win.cc

.PHONY: gfx.so
gfx.so: $(CBIN)/gfx.so
$(CBIN)/gfx.so: $(addprefix $(CBIN)/, client.so win.so)
$(CBIN)/gfx.so: src/client/gfx/gfx.cc

.PHONY: audio.so
audio.so: $(CBIN)/audio.so
$(CBIN)/audio.so: LFA := -lvorbisfile -lasound
$(CBIN)/audio.so: $(addprefix $(CBIN)/, client.so button.so)
$(CBIN)/audio.so: src/client/audio/audio.cc

.PHONY: button.so
button.so: $(CBIN)/button.so
$(CBIN)/button.so: $(shell find src/client/button/ -name "*.cc")

.PHONY: ui.so
ui.so: $(CBIN)/ui.so
$(CBIN)/ui.so: $(addprefix $(CBIN)/, client.so input.so win.so gfx.so button.so audio.so)
$(CBIN)/ui.so: $(shell find src/client/ui/ -name "*.cc")

.PHONY: net.so
net.so: $(CBIN)/net.so
$(CBIN)/net.so: $(addprefix $(CBIN)/, client.so)
$(CBIN)/net.so: src/client/net/net.cc

.PHONY: world.so
world.so: $(CBIN)/world.so
$(CBIN)/world.so: $(addprefix $(CBIN)/, net.so input.so gfx.so)
$(CBIN)/world.so: src/client/world/world.cc

$(CBIN)/%.so:
	$(eval CPATH := $(CPATH)$(shell find src/client -type d | tr '\n' ':'))
	$(eval export CPATH)
	@setterm --foreground green
	# Compiling client module: '$(shell basename $@)'...
	@setterm --default

	gcc $(CF) -Isrc/client $(CFA) -shared -fpic $^ $(LF) $(LFA) -o $@

.PHONY: server
server: sclient.so server.so

.PHONY: sclient.so
sclient.so: $(SBIN)/sclient.so
$(SBIN)/sclient.so: $(addprefix $(SBIN)/, server.so net.so world.so)
$(SBIN)/sclient.so: src/server/client/client.cc

.PHONY: sworld.so
sworld.so: $(SBIN)/world.so
$(SBIN)/world.so: $(SBIN)/server.so
$(SBIN)/world.so: src/server/world/world.cc

.PHONY: server.so
server.so: $(SBIN)/server.so
$(SBIN)/server.so: src/server/server.cc

.PHONY: snet.so
snet.so: $(SBIN)/net.so
$(SBIN)/net.so: $(SBIN)/server.so
$(SBIN)/net.so: src/server/net/net.cc

$(SBIN)/%.so:
	$(eval CPATH := $(CPATH)$(shell find src/server -type d | tr '\n' ':'))
	$(eval export CPATH)
	@setterm --foreground green
	# Compiling server module: '$(shell basename $@)'...
	@setterm --default

	gcc $(CF) -Isrc/server $(CFA) -shared -fpic $^ $(LF) $(LFA) -o $@

tmp/client.cc.tags: DIRS := $(shell find src/common src/client -type d)
tmp/client.cc.tags: FILES := $(shell find src/common src/client -type f)
tmp/client.cc.tags: CFLAGS := "-Isrc/ $(shell echo $(DIRS) | sed 's/src/-Isrc/g')"
tmp/client.cc.tags:
	CFLAGS=$(CFLAGS) geany -g $@ $(FILES)

.PHONY:clean
clean:
	-rm $(CBIN)/*.so 2> /dev/null
	-rm $(SBIN)/*.so 2> /dev/null
	-rm $(BIN)/common.so 2> /dev/null
	-rm $(BIN)/game 2> /dev/null
	-rm $(OBJ)/*.o 2> /dev/null
