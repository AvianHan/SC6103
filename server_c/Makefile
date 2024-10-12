# make file
# push new txt test
# 用于编译服务器代码的Makefile
CC = gcc
CFLAGS = -Wall -g
OBJ = server.o flight_service.o callback_handler.o data_storage.o thread_pool.o udp_utils.o

all: server

server: $(OBJ)
	$(CC) $(CFLAGS) -o server $(OBJ)

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

flight_service.o: flight_service.c flight_service.h
	$(CC) $(CFLAGS) -c flight_service.c

callback_handler.o: callback_handler.c callback_handler.h
	$(CC) $(CFLAGS) -c callback_handler.c

data_storage.o: data_storage.c data_storage.h
	$(CC) $(CFLAGS) -c data_storage.c

thread_pool.o: thread_pool.c thread_pool.h
	$(CC) $(CFLAGS) -c thread_pool.c

udp_utils.o: ../common/udp_utils.c ../common/udp_utils.h
	$(CC) $(CFLAGS) -c ../common/udp_utils.c

clean:
	rm -f *.o server