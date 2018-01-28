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

.PHONY: server
server: server.so snet.so sclient.so sworld.so

.PHONY: server.so
server.so: $(SB)/server.so
$(SB)/server.so: src/server/server.cc

.PHONY: snet.so
snet.so: $(SB)/net.so
$(SB)/net.so: $(SB)/server.so
$(SB)/net.so: src/server/net/net.cc

.PHONY: sclient.so
sclient.so: $(SB)/client.so
$(SB)/client.so: $(addprefix $(SB)/, server.so net.so world.so)
$(SB)/client.so: src/server/client/client.cc

.PHONY: sworld.so
sworld.so: $(SB)/world.so
$(SB)/world.so: $(addprefix $(SB)/, server.so net.so)
$(SB)/world.so: src/server/world/world.cc

.PHONY: sentity.so
sentity.so: $(SB)/entity.so
$(SB)/entity.so: $(addprefix $(SB)/, server.so net.so world.so)
$(SB)/entity.so: src/server/entity/entity.cc

.PHONY: seye.so
seye.so: $(SB)/eye.so
$(SB)/eye.so: $(addprefix $(SB)/, server.so net.so client.so world.so entity.so)
$(SB)/eye.so: src/server/eye/eye.cc

$(SB)/%.so:
	$(eval CPATH := $(CPATH)$(shell find src/server -type d | tr '\n' ':'))
	$(eval export CPATH)
	@setterm --foreground green
	# Compiling server module: '$(shell basename $@)'...
	@setterm --default

	gcc $(CF) -Isrc/server $(CFA) -shared -fpic $^ $(LF) $(LFA) -o $@
