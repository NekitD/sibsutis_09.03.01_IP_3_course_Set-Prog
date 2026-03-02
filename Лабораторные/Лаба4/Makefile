CC = gcc 

CLI_SOURCE = src/client.c
SER_SOURCE = src/server.c

CLI_TARGET = obj/client.o
SER_TARGET = obj/server.o


all: $(CLI_TARGET) $(SER_TARGET)

$(CLI_TARGET): $(CLI_SOURCE)
	@ $(CC) $< -o $@

$(SER_TARGET): $(SER_SOURCE)
	@ $(CC) $< -o $@

client-run: $(CLI_TARGET)
	@ ./$(CLI_TARGET)

server-run: $(SER_TARGET)
	@ ./$(SER_TARGET)

clean:
	@ rm obj/*

.PHONY: client-run server-run clean