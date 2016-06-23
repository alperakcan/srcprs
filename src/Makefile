CC = gcc

CFLAGS = -Wall -pipe -O2 -g
YFLAGS = -d

TARGET = ../srcprs

obj = dir.o function.o list.o main.o parse.o parser_c.o

all: $(obj) $(TARGET)

$(TARGET): $(obj)
	gcc -o $(TARGET) $(obj)

clean:
	rm -rf $(obj) $(TARGET)
