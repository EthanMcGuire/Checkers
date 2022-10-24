all: compile link

compile: checkers.hh
	g++ -c main.cpp -I"C:\SFML-2.5.1\include" -DSFML_STATIC

link:
	g++ main.o -o main -L"C:\SFML-2.5.1\lib" -lsfml-graphics-s -lsfml-window-s -lsfml-audio-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lwinmm -lgdi32 -mwindows -lopenal32 -lflac -lvorbisenc -lvorbisfile -lvorbis -logg -lsfml-main

clean:	
	rm -f main *.o