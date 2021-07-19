CC=gcc
OBJFILES=$(wildcard ./*.c)
INTERMEDIARY=main
TARGET=demo

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) -I ../../src $(OBJFILES) -o $(INTERMEDIARY) -L../../ -lfsm

clean:
	rm $(TARGET)
