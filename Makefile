CC=cc
CCFLAGS=-Wextra -Wall -Wpedantic -O2
LDFLAGS=$(shell pkg-config --libs sdl2)

BIN_DIR=bin
OBJ_DIR=obj

SRCS=$(shell find src -name *.c)
OBJS=$(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRCS))
TARGET=$(BIN_DIR)/pong

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $(TARGET)  $(LDFLAGS)

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@
	
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

run: $(TARGET)
	./$(TARGET)

dev: $(TARGET) run

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
