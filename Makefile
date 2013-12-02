LIB=lib
OBJ=obj
INC=include
CLIENT=src

CC=gcc
CCFLAGS=-I$(INC) -Wall -O
LDFLAGS=-L$(OBJ) -lemqtt
AR=ar

test: $(OBJ)/libemqtt.a $(OBJ)/test.o $(OBJ)/ad.o
	$(CC) $(OBJ)/test.o $(OBJ)/ad.o -o test $(LDFLAGS)

$(OBJ)/ad.o: $(CLIENT)/ad.c
	$(CC) $(CCFLAGS) -c $(CLIENT)/ad.c -o $(OBJ)/ad.o

$(OBJ)/test.o: $(CLIENT)/test.c $(INC)/libemqtt.h
	$(CC) $(CCFLAGS) -c $(CLIENT)/test.c -o $(OBJ)/test.o

$(OBJ)/libemqtt.a: $(OBJ)/libemqtt.o
	$(AR) rcs $(OBJ)/libemqtt.a $(OBJ)/libemqtt.o

$(OBJ)/libemqtt.o: $(LIB)/libemqtt.c $(INC)/libemqtt.h
	$(CC) $(CCFLAGS) -c $(LIB)/libemqtt.c -o $(OBJ)/libemqtt.o

all: test

clean:
	rm -f $(OBJ)/*.o $(OBJ)/libemqtt.a
	rm -f test
	
dist-clean: clean
	rm -f test
