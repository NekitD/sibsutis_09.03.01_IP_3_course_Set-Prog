CC = g++
CFLAGS = -Wall -g -I./include -std=c++17

CFLAGS += -I/usr/include/mariadb
CPPFLAGS = -MP -MMD

LDFLAGS = -L/usr/lib64 -lmariadb
LDLIBS = -lpqxx -lpq -pthread -L/usr/lib64 -lsodium

BIN = bin
SRC = src
OBJ = obj
INCLUDE = include

CONTEXT_DIR = context
LOBBY_DIR = lobby
GAME_DIR = game
SET_DIR = set

CONTEXT_SRC = $(SRC)/$(CONTEXT_DIR)/dbcontext.cpp
LOBBY_SRC = $(SRC)/$(LOBBY_DIR)/lobby.cpp
GAME_SRC = $(SRC)/$(GAME_DIR)/game.cpp
CLIENT_SRC = $(SRC)/$(SET_DIR)/client.cpp
SERVER_SRC = $(SRC)/$(SET_DIR)/server.cpp

CONTEXT_OBJ_FILE = $(OBJ)/$(CONTEXT_DIR)/dbcontext.o
LOBBY_OBJ_FILE = $(OBJ)/$(LOBBY_DIR)/lobby.o
GAME_OBJ_FILE = $(OBJ)/$(GAME_DIR)/game.o
CLIENT_OBJ = $(OBJ)/$(SET_DIR)/client.o
SERVER_OBJ = $(OBJ)/$(SET_DIR)/server.o

CONTEXT_DEPS = $(OBJ)/$(CONTEXT_DIR)/dbcontext.d
LOBBY_DEPS = $(OBJ)/$(LOBBY_DIR)/lobby.d
GAME_DEPS = $(OBJ)/$(GAME_DIR)/game.d
CLIENT_DEPS = $(OBJ)/$(SET_DIR)/client.d
SERVER_DEPS = $(OBJ)/$(SET_DIR)/server.d

CLIENT_TARGET = $(BIN)/client
SERVER_TARGET = $(BIN)/server
GAME_LIB = $(OBJ)/$(GAME_DIR)/libgame.a
LOBBY_LIB = $(OBJ)/$(LOBBY_DIR)/liblobby.a
CONTEXT_LIB = $(OBJ)/$(CONTEXT_DIR)/libcontext.a

DIRS = $(OBJ)/$(GAME_DIR) $(OBJ)/$(SET_DIR) $(OBJ)/$(LOBBY_DIR) $(OBJ)/$(CONTEXT_DIR)
$(shell mkdir -p $(DIRS))

.PHONY: all clean game player server client

all: $(GAME_LIB) $(LOBBY_LIB) $(CONTEXT_LIB) $(CLIENT_TARGET) $(SERVER_TARGET)

$(GAME_LIB): $(GAME_OBJ_FILE)
	@ar rcs $@ $^

$(LOBBY_LIB): $(LOBBY_OBJ_FILE)
	@ar rcs $@ $^

$(CONTEXT_LIB): $(CONTEXT_OBJ_FILE)
	@ar rcs $@ $^

$(GAME_OBJ_FILE): $(GAME_SRC)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(LOBBY_OBJ_FILE): $(LOBBY_SRC)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(CONTEXT_OBJ_FILE): $(CONTEXT_SRC)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(CLIENT_OBJ): $(CLIENT_SRC)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(SERVER_OBJ): $(SERVER_SRC)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(SERVER_TARGET): $(SERVER_OBJ) $(LOBBY_LIB) $(GAME_LIB) $(CONTEXT_LIB)
	@mkdir -p $(BIN)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(CLIENT_TARGET): $(CLIENT_OBJ) $(GAME_LIB) $(CONTEXT_LIB)
	@mkdir -p $(BIN)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)


client: $(CLIENT_TARGET)

server: $(SERVER_TARGET)

run-ser:
	@./$(SERVER_TARGET)

run-cli:
	@./$(CLIENT_TARGET)

-include $(GAME_DEPS) $(LOBBY_DEPS) $(CLIENT_DEPS) $(SERVER_DEPS) $(CONTEXT_DEPS)

clean:
	@rm -rf $(OBJ)/$(GAME_DIR)/* $(OBJ)/$(LOBBY_DIR)/* $(OBJ)/$(CONTEXT_DIR)/* $(OBJ)/$(SET_DIR)/* $(CLIENT_TARGET) $(SERVER_TARGET)