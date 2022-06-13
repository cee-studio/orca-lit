TARGET = test

CFLAGS = -c -v

VPATH = src
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) -o $@ $(OBJ) -lcurl -lssl -lcrypto

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f $(TARGET) $(TARGET).exe
	rm -f *.o *.obj
