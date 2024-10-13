# make file
# push new txt test
# 用于编译服务器代码的Makefile
# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Object files
OBJ = server.o flight_service.o callback_handler.o data_storage.o thread_pool.o marshalling.o unmarshalling.o

# Target: Build the server executable
all: server

# Link object files to create the final executable
server: $(OBJ)
	$(CC) $(CFLAGS) -o server $(OBJ) -lpthread -lws2_32

# Compile each .c file into an object file
server.o: server.c server.h
	$(CC) $(CFLAGS) -c server.c

flight_service.o: flight_service.c server.h
	$(CC) $(CFLAGS) -c flight_service.c

callback_handler.o: callback_handler.c server.h
	$(CC) $(CFLAGS) -c callback_handler.c

data_storage.o: data_storage.c server.h
	$(CC) $(CFLAGS) -c data_storage.c

thread_pool.o: thread_pool.c server.h
	$(CC) $(CFLAGS) -c thread_pool.c

marshalling.o: marshalling.c communication.h
	$(CC) $(CFLAGS) -c marshalling.c

unmarshalling.o: unmarshalling.c communication.h
	$(CC) $(CFLAGS) -c unmarshalling.c

# Clean object files and the executable
clean:
	rm -f *.o server.exe
