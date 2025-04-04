# Compilator
CC = gcc

# Opțiuni de compilare
CFLAGS = -Wall

# Biblioteci pentru linker
LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Numele executabilului
TARGET = run

# Fișierele sursă
SRCS = main.c game.c

# Fișierele obiect (generate automat)
OBJS = $(SRCS:.c=.o)

# Regula implicită
all: $(TARGET)

# Legarea executabilului
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDLIBS)

# Compilare fișiere sursă în obiecte
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Curățare
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean