CC=cc
CCFLAGS=-Wextra -Wall -Wpedantic -O2
LDFLAGS=

SRC=src/main.c
OUTDIR=bin
TARGET=$(OUTDIR)/pong

all: $(TARGET)

$(TARGET): $(OUTDIR)
	$(CC) -o $(TARGET) $(CCFLAGS) $(LDFLAGS) $(SRC)

run: $(TARGET)
	./$(TARGET)

$(OUTDIR):
	mkdir -p $(OUTDIR)

dev: $(TARGET) run

clean:
	rm -rf $(OUTDIR)
