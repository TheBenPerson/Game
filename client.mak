.PHONY: client
client: client.so input.so win.so gfx.so ui.so net.so world.so

.PHONY: client.so
client.so: $(CB)/client.so
$(CB)/client.so: $(addprefix $(CB)/, button.so)
$(CB)/client.so: src/client/client.cc

.PHONY: input.so
input.so: $(CB)/input.so
$(CB)/input.so: src/client/input/input.cc

.PHONY: win.so
win.so: $(CB)/win.so
$(CB)/win.so: LFA := -lxcb -lX11-xcb
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

$(CB)/%.so:
	$(eval CPATH := $(CPATH)$(shell find src/client -type d | tr '\n' ':'))
	$(eval export CPATH)
	@setterm --foreground green
	# Compiling client module: '$(shell basename $@)'...
	@setterm --default

	gcc $(CF) -Isrc/client $(CFA) -shared -fpic $^ $(LF) $(LFA) -o $@
