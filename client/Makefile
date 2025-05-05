CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

SDL_SRC = client.c
TEST_SRC = test_client.c

SDL_TARGET = client
TEST_TARGET = test_client

.PHONY: all clean

all: $(SDL_TARGET) $(TEST_TARGET)

$(SDL_TARGET): $(SDL_SRC)
	$(CC) $(CFLAGS) -o $(SDL_TARGET) $(SDL_SRC) $(LDFLAGS)

$(TEST_TARGET): $(TEST_SRC)
	$(CC) -Wall -Wextra -g -o $(TEST_TARGET) $(TEST_SRC)

clean:
	rm -f $(SDL_TARGET) $(TEST_TARGET)
