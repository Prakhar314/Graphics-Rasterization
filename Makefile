CC = g++
CFLAGS  = -Wall `pkg-config --cflags --libs sdl2 SDL2_image`
 
TARGET = main

all: $(TARGET)

run: $(TARGET)
	SDL_VIDEODRIVER=dummy ./$(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp

clean:
	$(RM) -f $(TARGET) out.png