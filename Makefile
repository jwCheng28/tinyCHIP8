CC = gcc
BUILD_FLAGS = -O3 -w -s
DEBUG_FLAGS = -Wall -g
SDL_INCLUDE = C:\Dev\mingw-dev\include
SDL_LIB = C:\Dev\mingw-dev\lib
WINDOWS = -lmingw32 -I$(SDL_INCLUDE) -L$(SDL_LIB)
SDL_FLAGS = -lSDL2main -lSDL2 -lSDL2_ttf
exe_file = emu.exe

$(exe_file): *.c
	$(CC) $(BUILD_FLAGS) $(WINDOWS) *.c -o $(exe_file) $(SDL_FLAGS)

display: display.c
	$(CC) $(DEBUG_FLAGS) $(WINDOWS) display.c -o $(exe_file) $(SDL_FLAGS)

clean:
	rm -f *.o $(exe_file)

cleanw:
	del /Q *.o $(exe_file)
