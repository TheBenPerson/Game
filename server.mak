# Game Development Build
# https://github.com/TheBenhuman/Game
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

.PHONY: server
server: snet.so tile.so sclient.so sworld.so sentity.so sexplosion.so sfireball.so \
seye.so shuman.so door.so ssign.so splayer.so sauth.so

.PHONY: snet.so
snet.so: $(SB)/net.so

.PHONY: tile.so
tile.so: $(SB)/tile.so

.PHONY: sclient.so
sclient.so: $(SB)/client.so
$(SB)/client.so: $(SB)/net.so

.PHONY: sworld.so
sworld.so: $(SB)/world.so
$(SB)/world.so: $(SB)/tile.so $(SB)/client.so

.PHONY: sentity.so
sentity.so: $(SB)/entity.so
$(SB)/entity.so: $(SB)/net.so $(SB)/world.so

.PHONY: sexplosion.so
sexplosion.so: $(SB)/explosion.so
$(SB)/explosion.so: $(SB)/entity.so $(SB)/fireball.so

.PHONY: sfireball.so
sfireball.so: $(SB)/fireball.so
$(SB)/fireball.so: $(SB)/entity.so
$(SB)/fireball.so: LF += -lm

.PHONY: seye.so
seye.so: $(SB)/eye.so
$(SB)/eye.so: $(SB)/entity.so $(SB)/fireball.so
$(SB)/eye.so: LF += -lm

.PHONY: shuman.so
shuman.so: $(SB)/human.so
$(SB)/human.so: $(SB)/entity.so

.PHONY: sauth.so
sauth.so: $(SB)/auth.so
$(SB)/auth.so: $(SB)/net.so $(SB)/world.so

.PHONY: door.so
door.so: $(SB)/door.so
$(SB)/door.so: $(addprefix $(SB)/, tile.so client.so auth.so entity.so world.so)

.PHONY: ssign.so
ssign.so: $(SB)/sign.so
$(SB)/sign.so: $(SB)/net.so $(SB)/tile.so

.PHONY: splayer.so
splayer.so: $(SB)/player.so
$(SB)/player.so: $(addprefix $(SB)/, entity.so human.so auth.so sign.so)

$(SB)/%.so: CPATH := $(CPATH)$(shell find src/server -type d | tr '\n' ':')
$(SB)/%.so: src/server/*/%.cc
	@setterm --foreground green
	# Compiling server module: '$(shell basename $@)'...
	@setterm --default

	$(CC) $(CF) -shared -fpic $^ $(LF) $(LFA) -o $@
