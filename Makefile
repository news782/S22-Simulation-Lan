CC = gcc
CFLAGS = -Wall -Wextra -g
OBJ = main.o network_structures.o network_display.o network_config.o ethernet_frame.o switch_logic.o

# Cible principale
all: main

main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)

main.o: main.c network_structures.h network_display.h network_config.h

network_structures.o: network_structures.c network_structures.h

network_display.o: network_display.c network_display.h network_structures.h

network_config.o: network_config.c network_config.h network_structures.h

ethernet_frame.o: ethernet_frame.c ethernet_frame.h network_structures.h

switch_logic.o: switch_logic.c switch_logic.h network_structures.h ethernet_frame.h

clean:
	rm -f *.o main

.PHONY: all clean