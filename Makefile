CC = gcc -Iinclude
BUILD_FLAGS = -O3 -w -s
DEBUG_FLAGS = -Wall -g
SDL_INCLUDE = C:\Dev\mingw-dev\include
SDL_LIB = C:\Dev\mingw-dev\lib
WINDOWS = -lmingw32 -I$(SDL_INCLUDE) -L$(SDL_LIB)
SDL_FLAGS = -lSDL2main -lSDL2 -lSDL2_ttf
exe_file = emu.exe

$(exe_file): src/*.c
	$(CC) $(BUILD_FLAGS) $(WINDOWS) src/*.c -o $(exe_file) $(SDL_FLAGS)

debug: src/*.c
	$(CC) $(DEBUG_FLAGS) $(WINDOWS) src/*.c -o $(exe_file) $(SDL_FLAGS)

cleanw:
	del /Q *.o $(exe_file)
