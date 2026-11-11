all: compile link

compile:
	g++ -c src/main.cpp -I/home/hericulum/Documents/libraries/SFML-3.1.0-linux-gcc-64-bit/SFML-3.1.0/include

link: 
	g++ main.o -o tetris \
		-L/home/hericulum/Documents/libraries/SFML-3.1.0-linux-gcc-64-bit/SFML-3.1.0/lib \
		-L/usr/lib/x86_64-linux-gnu \
		-Wl,-rpath,/usr/lib/x86_64-linux-gnu \
		-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio \
		-lFLAC -logg -lvorbis -lvorbisenc -lvorbisfile -lopenal

clean:
	rm -f tetris *.o