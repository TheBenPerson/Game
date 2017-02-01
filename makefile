CF := -Isrc

all: debug clean

.PHONY: debug
debug: LF += -g
debug: game

.PHONY: release
release: LF += -Ofast
release: game
	strip --unneeded bin/game lib/*

game: main client util
	gcc bin/main.o $(LF) -Llib -lclient -lstdc++ -lutil -o bin/$@

main: src/Main/main.cpp
	gcc $(CF) -c $^ -o bin/$@.o

client: $(shell find src/Client -name "*.cpp")
	gcc $(CF) $(LF) -Isrc/Client -shared -fpic -lGL -lpng -lX11 $^ -o lib/lib$@.so

server: $(shell find src/Server -name "*.cpp")
	gcc $(CF) $(LF) -Isrc/Server -shared -fpic $^ -o lib/lib$@.so

util: $(shell find src/Util -name "*.cpp")
	gcc $(CF) -Isrc/Util -shared -fpic $^ $(LF) -o lib/lib$@.so

.PHONY: clean
clean:
	rm bin/*.o
