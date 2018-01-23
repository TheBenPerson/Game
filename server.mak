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
sclient.so: $(SB)/sclient.so
$(SB)/sclient.so: $(addprefix $(SB)/, server.so net.so world.so)
$(SB)/sclient.so: src/server/client/client.cc

.PHONY: sworld.so
sworld.so: $(SB)/world.so
$(SB)/world.so: $(addprefix $(SB)/, server.so net.so)
$(SB)/world.so: src/server/world/world.cc

$(SB)/%.so:
	$(eval CPATH := $(CPATH)$(shell find src/server -type d | tr '\n' ':'))
	$(eval export CPATH)
	@setterm --foreground green
	# Compiling server module: '$(shell basename $@)'...
	@setterm --default

	gcc $(CF) -Isrc/server $(CFA) -shared -fpic $^ $(LF) $(LFA) -o $@
