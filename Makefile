CC := gcc
CFLAGS := -Wall
HEADERS := ASM.h
OBJECTS := SymbolTable.o Main.o
TARGET := arg010_ASM

all: $(TARGET)

$(TARGET): $(OBJECTS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(TARGET) *~
