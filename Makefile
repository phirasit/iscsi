CC=gcc
CFLAGS=-pthread -g
INCLUDE_DIR=.

TARGET=iscsi
TARGET_DIR=bin

SRC_DIR=./ ./request
SRC=$(wildcard *.c) $(wildcard request/*.c)
OBJ=$(addprefix ./$(TARGET_DIR)/,$(SRC:.c=.o))

all: pre-build target
	
pre-build:
	mkdir -p $(TARGET_DIR)
	mkdir -p $(addprefix ./$(TARGET_DIR)/,$(SRC_DIR))

target: $(OBJ)
	$(CC) -I $(INCLUDE_DIR) $(CFLAGS) $(OBJ) -o ./$(TARGET)

$(TARGET_DIR)/%.o: %.c
	$(CC) -I $(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

clean:
	rm -rf ./$(TARGET_DIR)
	rm -f $(TARGET)

.PHONY: all clean

