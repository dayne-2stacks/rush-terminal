# Makefile for the shell program

# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -g

# Define the target executable
TARGET = rush

# List of source files
SOURCES = rush.c utilities.c commands.c
# List of object files (replace .c extensions with .o)
OBJECTS = $(SOURCES:.c=.o)

# Default target: build the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Rule for object files, each .c file depends on its .o
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Clean target: remove object files and the executable
clean:
	rm -f $(TARGET) $(OBJECTS)

# Phony targets are not files
.PHONY: clean
