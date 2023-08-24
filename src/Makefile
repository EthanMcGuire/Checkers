mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

ROOT_DIR = $(mkfile_dir)

ROOT_DIR = C:/Users/Ethan McGuire/Desktop/VSCode/CheckersCode/Checkers/src/

all: compile checkers link

compile: checkers.hh
	g++ -c main.cpp -I"${ROOT_DIR}SFML/SFML-2.5.1/include" -DSFML_STATIC

checkers: checkers.hh
	g++ -c checkers.cpp -I"${ROOT_DIR}SFML/SFML-2.5.1/include" -DSFML_STATIC

link:
	g++ main.o checkers.o -o main -L"${ROOT_DIR}SFML/Build/lib" -lsfml-graphics-s -lsfml-window-s -lsfml-audio-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lwinmm -lgdi32 -lopenal32 -lflac -lvorbisenc -lvorbisfile -lvorbis -logg -lsfml-main

clean:	
	del -f main.exe *.o