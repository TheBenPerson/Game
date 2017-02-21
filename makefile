CF := -Isrc
LF :=

#LEAK_DETECTION := -fsanitize=leak

all: debug clean

.PHONY: debug
debug: LF += -g $(LEAK_DETECTION)
debug: game

.PHONY: release
release: LF += -Ofast
release: game
	strip --unneeded bin/game lib/*

game: main client util
	gcc bin/main.o $(LF) -Llib -ldl -lstdc++ -lutil -o bin/$@

main: src/Main/main.cpp
	gcc $(CF) -c $^ $(LF) -o bin/$@.o

client: $(shell find src/Client -name "*.cpp")
	gcc $(CF) -Isrc/Client -shared -fpic -lGL -lpng -lX11 -lX11-xcb -lxcb $^ $(LF) -o lib/lib$@.so

server: $(shell find src/Server -name "*.cpp")
	gcc $(CF) -Isrc/Server -shared -fpic $^ -o $(LF) lib/lib$@.so

util: $(shell find src/Util -name "*.cpp")
	gcc $(CF) -Isrc/Util -shared -fpic $^ $(LF) -o lib/lib$@.so

.PHONY: clean
clean:
	rm bin/*.o
