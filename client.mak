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
client: button.so client.so input.so win.so gfx.so audio.so ui.so \
        net.so auth.so world.so entity.so explosion.so fireball.so eye.so human.so sign.so player.so

.PHONY: button.so
button.so: $(CB)/button.so

.PHONY: client.so
client.so: LF += $(CB)/button.so
client.so: $(CB)/client.so

.PHONY: input.so
input.so: $(CB)/input.so

.PHONY: win.so
win.so: $(CB)/win.so
$(CB)/win.so: LF += $(CB)/client.so $(CB)/input.so
$(CB)/win.so: LF += -lglfw -lGL

.PHONY: gfx.so
gfx.so: $(CB)/gfx.so
$(CB)/gfx.so: LF += $(CB)/win.so
$(CB)/gfx.so: LF += -lGL -lepoxy -lpng

.PHONY: audio.so
audio.so: $(CB)/audio.so
$(CB)/audio.so: LF += $(CB)/client.so
$(CB)/audio.so: LF += -lopenal -lvorbisfile -lm

.PHONY: ui.so
ui.so: $(CB)/ui.so
$(CB)/ui.so: LF := $(CB)/gfx.so $(CB)/audio.so

########################################################################

.PHONY: net.so
net.so: $(CB)/net.so

.PHONY: auth.so
auth.so: $(CB)/auth.so
$(CB)/auth.so: $(CB)/net.so

.PHONY: world.so
world.so: $(CB)/world.so
$(CB)/world.so: $(CB)/gfx.so $(CB)/net.so

.PHONY: entity.so
entity.so: $(CB)/entity.so
$(CB)/entity.so: $(CB)/net.so $(CB)/world.so

.PHONY: explosion.so
explosion.so: $(CB)/explosion.so
$(CB)/explosion.so: LF += $(CB)/audio.so
$(CB)/explosion.so: $(CB)/entity.so

.PHONY: fireball.so
fireball.so: $(CB)/fireball.so
$(CB)/fireball.so: $(CB)/entity.so

.PHONY: eye.so
eye.so: $(CB)/eye.so
$(CB)/eye.so: $(CB)/entity.so

.PHONY: human.so
human.so: $(CB)/human.so
$(CB)/human.so: $(CB)/entity.so

.PHONY: sign.so
sign.so: $(CB)/sign.so
$(CB)/sign.so: $(CB)/gfx.so $(CB)/net.so

.PHONY: player.so
player.so: $(CB)/player.so
$(CB)/player.so: $(addprefix $(CB)/, gfx.so world.so entity.so audio.so)
$(CB)/player.so: LF += -lm

$(CB)/%.so: CPATH := $(CPATH)$(shell find src/client -type d | tr '\n' ':')
$(CB)/%.so: src/client/%/*.cc
	@setterm --foreground green
	# Compiling client module: '$(shell basename $@)'...
	@setterm --default

	$(CC) $(CF) -shared -fpic $^ $(LF) -o $@
