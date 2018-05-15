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

.PHONY: client
client: client.so input.so win.so gfx.so ui.so net.so world.so entity.so eye.so fireball.so explosion.so sign.so person.so player.so

.PHONY: client.so
client.so: $(CB)/client.so
$(CB)/client.so: $(addprefix $(CB)/, button.so)
$(CB)/client.so: src/client/client.cc

.PHONY: input.so
input.so: $(CB)/input.so
$(CB)/input.so: src/client/input/input.cc

.PHONY: win.so
win.so: $(CB)/win.so
$(CB)/win.so: LFA := -lglfw -lGL
$(CB)/win.so: $(addprefix $(CB)/, client.so input.so)
$(CB)/win.so: src/client/win/win.cc

.PHONY: gfx.so
gfx.so: $(CB)/gfx.so
$(CB)/gfx.so: LFA := -lGL -lpng
$(CB)/gfx.so: $(addprefix $(CB)/, client.so win.so)
$(CB)/gfx.so: src/client/gfx/gfx.cc

.PHONY: audio.so
audio.so: $(CB)/audio.so
$(CB)/audio.so: LFA := -lvorbisfile -lasound
$(CB)/audio.so: $(addprefix $(CB)/, client.so button.so)
$(CB)/audio.so: src/client/audio/audio.cc

.PHONY: button.so
button.so: $(CB)/button.so
$(CB)/button.so: $(shell find src/client/button/ -name "*.cc")

.PHONY: ui.so
ui.so: $(CB)/ui.so
$(CB)/ui.so: $(addprefix $(CB)/, client.so input.so win.so gfx.so button.so audio.so)
$(CB)/ui.so: $(shell find src/client/ui/ -name "*.cc")

.PHONY: net.so
net.so: $(CB)/net.so
$(CB)/net.so: $(addprefix $(CB)/, client.so)
$(CB)/net.so: src/client/net/net.cc

.PHONY: world.so
world.so: $(CB)/world.so
$(CB)/world.so: $(addprefix $(CB)/, net.so input.so gfx.so)
$(CB)/world.so: src/client/world/world.cc

.PHONY: entity.so
entity.so: $(CB)/entity.so
$(CB)/entity.so: $(addprefix $(CB)/, client.so net.so world.so)
$(CB)/entity.so: src/client/entity/entity.cc

.PHONY: eye.so
eye.so: $(CB)/eye.so
$(CB)/eye.so: $(addprefix $(CB)/, net.so world.so entity.so gfx.so)
$(CB)/eye.so: src/client/eye/eye.cc

.PHONY: fireball.so
fireball.so: $(CB)/fireball.so
$(CB)/fireball.so: $(addprefix $(CB)/, net.so world.so entity.so gfx.so)
$(CB)/fireball.so: src/client/fireball/fireball.cc

.PHONY: explosion.so
explosion.so: $(CB)/explosion.so
$(CB)/explosion.so: $(addprefix $(CB)/, net.so world.so entity.so gfx.so)
$(CB)/explosion.so: src/client/explosion/explosion.cc

.PHONY: sign.so
sign.so: $(CB)/sign.so
$(CB)/sign.so: $(addprefix $(CB)/, net.so gfx.so input.so)
$(CB)/sign.so: src/client/sign/sign.cc

.PHONY: person.so
person.so: $(CB)/person.so
$(CB)/person.so: $(addprefix $(CB)/, net.so world.so entity.so gfx.so)
$(CB)/person.so: src/client/person/person.cc

.PHONY: player.so
player.so: $(CB)/player.so
$(CB)/player.so: LFA := -lm
$(CB)/player.so: $(addprefix $(CB)/, client.so input.so net.so win.so world.so gfx.so)
$(CB)/player.so: src/client/player/player.cc

$(CB)/%.so: CPATH := $(CPATH)$(shell find src/client -type d | tr '\n' ':')
$(CB)/%.so:
	@setterm --foreground green
	# Compiling client module: '$(shell basename $@)'...
	@setterm --default

	$(CC) $(CF) -Isrc/client -shared -fpic $^ $(LF) $(LFA) -o $@
