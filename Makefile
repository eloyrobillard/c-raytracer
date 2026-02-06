LD=-L./lib/ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11


.PHONY: main
main:
	${CC} src/main.c -O1 -Wall -std=c99 -Wno-missing-braces -I ./include/ ${LD}
	./a.out

setup-raylib:
	git clone https://github.com/raysan5/raylib.git raylib
	cd raylib
	mkdir build && cd build
	cmake ..
	make
	cd ../..
	mkdir include
	mkdir lib
	cp raylib/build/raylib/libraylib.a lib/
	cp raylib/build/raylib/include/* include/
