#Copyright (C) 2016-2017 Ben Stockett

#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:

#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.

#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.

CF := -Isrc
LF :=

#GDB := -g
#LEAK_DETECTION := -fsanitize=leak

all: debug clean

.PHONY: debug
debug: LF += $(GDB) $(LEAK_DETECTION)
debug: game

.PHONY: release
release: LF += -Ofast
release: game
	strip --strip-unneeded bin/game lib/*

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
