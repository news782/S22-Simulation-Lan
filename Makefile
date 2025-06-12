CC = gcc
CFLAGS = -Wall -Wextra -g

# Liste des fichiers source
SRCS = main.c network_structures.c ethernet_frame.c switch_logic.c stp.c network_config.c network_display.c
OBJS = $(SRCS:.c=.o)

# Nom de ton exécutable
EXEC = simulateur_reseau

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

	
.PHONY: all clean